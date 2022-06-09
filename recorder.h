#ifndef RECORDER_H
#define RECORDER_H

#include <QMainWindow>
#include <QMediaRecorder>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class Recorder;
}

class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;

QT_END_NAMESPACE

class AudioLevel;

class Recorder : public QMainWindow
{
    Q_OBJECT

public:
    explicit Recorder(QWidget *parent = nullptr);
    ~Recorder();

public slots:
    void processBuffer(const QAudioBuffer&);

private slots:
    void setOutputLocation();
    void togglePause();
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

    void on_outputButton_clicked();
    void on_recordButton_clicked();
    void on_pauseButton_clicked();

    void on_pushButtonBack_clicked();

private:
    void clearAudioLevels();

    Ui::Recorder *ui;

    QAudioRecorder *audioRecorder = nullptr;
    QAudioProbe *probe = nullptr;
    QList<AudioLevel*> audioLevels;
    bool outputLocationSet = false;

};

#endif // RECORDER_H
