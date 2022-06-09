#include "notepad.h"
#include "ui_notepad.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif // QT_CONFIG(printdialog)
#include <QPrinter>
#endif // QT_CONFIG(printer)
#endif // QT_PRINTSUPPORT_LIB
#include <QFont>
#include <QFontDialog>

#include "mainwindow.h"

Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::on_pushButtonNew_clicked()
{
    if (ui->textEdit->toPlainText() != "") {
        currentFile.clear();
        ui->textEdit->setText(QString());
    } else {
        QMessageBox::warning(this, "Warning", "Already opened new text file");
    }

}

void Notepad::on_pushButtonOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
}

void Notepad::on_pushButtonSave_clicked()
{
    if (ui->textEdit->toPlainText() != "") {
        QString fileName;

        if (currentFile.isEmpty()) {
            fileName = QFileDialog::getSaveFileName(this, "Save");
            currentFile = fileName;
        } else {
            fileName = currentFile;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
            return;
        }

        setWindowTitle(fileName);
        QTextStream out(&file);
        QString text = ui->textEdit->toPlainText();
        out << text;
        file.close();

    } else {
        QMessageBox::warning(this, "Warning", "Cannot save. Text file is empty!");
    }

}

void Notepad::on_pushButtonSaveAs_clicked()
{
    if (ui->textEdit->toPlainText() != "") {
        QString fileName = QFileDialog::getSaveFileName(this, "Save as");
        QFile file(fileName);

        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
            return;
        }

        currentFile = fileName;
        setWindowTitle(fileName);
        QTextStream out(&file);
        QString text = ui->textEdit->toPlainText();
        out << text;
        file.close();

    } else {
        QMessageBox::warning(this, "Warning", "Cannot save. Text file is empty!");
    }

}

void Notepad::on_pushButtonPrint_clicked()
{
    if (ui->textEdit->toPlainText() != "") {
        #if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
            QPrinter printDev;
        #if QT_CONFIG(printdialog)
            QPrintDialog dialog(&printDev, this);
            if (dialog.exec() == QDialog::Rejected)
                return;
        #endif
            ui->textEdit->print(&printDev);
        #endif

    } else {
        QMessageBox::warning(this, "Warning", "Cannot print. Text file is empty!");
    }

}

void Notepad::on_pushButtonCopy_clicked()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->copy();
#endif
}

void Notepad::on_pushButtonCut_clicked()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->cut();
#endif
}

void Notepad::on_pushButtonPaste_clicked()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->paste();
#endif
}

void Notepad::on_pushButtonUndo_clicked()
{
    ui->textEdit->undo();
}

void Notepad::on_pushButtonRedo_clicked()
{
    ui->textEdit->redo();
}

void Notepad::on_pushButtonFont_clicked()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        ui->textEdit->setFont(font);
}

void Notepad::on_pushButtonBold_clicked()
{
    if (isBold == true){
        ui->textEdit->setFontWeight(QFont::Normal);
        isBold = false;
    } else {
        ui->textEdit->setFontWeight(QFont::Bold);
        isBold = true;
    }
}

void Notepad::on_pushButtonItalic_clicked()
{
    if (isItalic == true){
        ui->textEdit->setFontItalic(false);
        isItalic = false;
    } else {
        ui->textEdit->setFontItalic(true);
        isItalic = true;
    }
}

void Notepad::on_pushButtonUnderscore_clicked()
{
    if (isUnderine == true){
        ui->textEdit->setFontUnderline(false);
        isUnderine = false;
    } else {
        ui->textEdit->setFontUnderline(true);
        isUnderine = true;
    }
}

void Notepad::on_pushButtonExit_clicked()
{
    QCoreApplication::quit();
}

void Notepad::on_pushButtonBackToMenu_clicked()
{
    MainWindow *mainWindow;
    mainWindow = new MainWindow();
    this->hide();
    mainWindow->show();
}
