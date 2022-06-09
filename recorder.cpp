#include "recorder.h"
#include "ui_recorder.h"

#include "audiolevel.h"
#include "mainwindow.h"

#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QFileDialog>
#include <QMediaRecorder>
#include <QStandardPaths>

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

Recorder::Recorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Recorder)
{
    ui->setupUi(this);
    audioRecorder = new QAudioRecorder(this);
    probe = new QAudioProbe(this);

    connect(probe, &QAudioProbe::audioBufferProbed,
            this, &Recorder::processBuffer);
    probe->setSource(audioRecorder);

    //audio devices
    ui->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &device: audioRecorder->audioInputs()) {
        ui->audioDeviceBox->addItem(device, QVariant(device));
    }

    //audio codecs
    ui->audioCodecBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &codecName: audioRecorder->supportedAudioCodecs()) {
        ui->audioCodecBox->addItem(codecName, QVariant(codecName));
    }

    //containers
    ui->containerBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &containerName: audioRecorder->supportedContainers()) {
        ui->containerBox->addItem(containerName, QVariant(containerName));
    }

    //sample rate
    ui->sampleRateBox->addItem(tr("Default"), QVariant(0));
    for (int sampleRate: audioRecorder->supportedAudioSampleRates()) {
        ui->sampleRateBox->addItem(QString::number(sampleRate), QVariant(
                sampleRate));
    }

    //channels
    ui->channelsBox->addItem(tr("Default"), QVariant(-1));
    ui->channelsBox->addItem(QStringLiteral("1"), QVariant(1));
    ui->channelsBox->addItem(QStringLiteral("2"), QVariant(2));
    ui->channelsBox->addItem(QStringLiteral("4"), QVariant(4));

    connect(audioRecorder, &QAudioRecorder::durationChanged, this, &Recorder::updateProgress);
    connect(audioRecorder, &QAudioRecorder::statusChanged, this, &Recorder::updateStatus);
    connect(audioRecorder, &QAudioRecorder::stateChanged, this, &Recorder::onStateChanged);
    connect(audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this,
            &Recorder::displayErrorMessage);
}

Recorder::~Recorder()
{
    delete ui;
}

void Recorder::updateProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    ui->statusBar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void Recorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
    default:
        break;
    }

    if (audioRecorder->error() == QMediaRecorder::NoError)
        ui->statusBar->showMessage(statusMessage);
}

void Recorder::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {

    case QMediaRecorder::RecordingState:
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Pause"));
        break;

    case QMediaRecorder::PausedState:
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Resume"));
        break;

    case QMediaRecorder::StoppedState:
        ui->recordButton->setText(tr("Record"));
        ui->pauseButton->setText(tr("Pause"));
        break;

    }

    ui->pauseButton->setEnabled(audioRecorder->state() != QMediaRecorder::StoppedState);
}

static QVariant boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Recorder::toggleRecord()
{
    if (audioRecorder->state() == QMediaRecorder::StoppedState) {
        audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());

        QAudioEncoderSettings settings;
        settings.setCodec(boxValue(ui->audioCodecBox).toString());
        settings.setSampleRate(boxValue(ui->sampleRateBox).toInt());
        settings.setChannelCount(boxValue(ui->channelsBox).toInt());

        QString container = boxValue(ui->containerBox).toString();

        audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        audioRecorder->record();
    }
    else {
        audioRecorder->stop();
    }
}

void Recorder::togglePause()
{
    if (audioRecorder->state() != QMediaRecorder::PausedState)
        audioRecorder->pause();
    else
        audioRecorder->record();
}

void Recorder::setOutputLocation()
{

#ifdef Q_OS_WINRT
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QDir().mkpath(cacheDir)) {
        qWarning() << "Failed to create cache directory";
        return;
    }
    QString fileName = cacheDir + QLatin1String("/output.wav");
#else
    QString fileName = QFileDialog::getSaveFileName();
#endif
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    outputLocationSet = true;
}

void Recorder::displayErrorMessage()
{
    ui->statusBar->showMessage(audioRecorder->errorString());
}

void Recorder::clearAudioLevels()
{
    for (int i = 0; i < audioLevels.size(); ++i)
        audioLevels.at(i)->setLevel(0);
}


qreal getPeakValue(const QAudioFormat& format)
{

    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32)
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}


QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

template <class T>
QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

void Recorder::processBuffer(const QAudioBuffer& buffer)
{
    if (audioLevels.count() != buffer.format().channelCount()) {
        qDeleteAll(audioLevels);
        audioLevels.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            AudioLevel *level = new AudioLevel(ui->centralwidget);
            audioLevels.append(level);
            ui->levelsLayout->addWidget(level);
        }
    }

    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        audioLevels.at(i)->setLevel(levels.at(i));
}

void Recorder::on_outputButton_clicked()
{
    setOutputLocation();
}

void Recorder::on_recordButton_clicked()
{
    toggleRecord();
}

void Recorder::on_pauseButton_clicked()
{
    togglePause();
}

void Recorder::on_pushButtonBack_clicked()
{
    MainWindow *mainWindow;
    mainWindow = new MainWindow();
    this->hide();
    mainWindow->show();
}
