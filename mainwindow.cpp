#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "notepad.h"
#include "painter.h"
#include "recorder.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonPainter_clicked()
{
//    Open painter window
    Painter * painter = new Painter();
    if (!painter->isVisible()) {
        this->hide();
        painter->show();
    }
}

void MainWindow::on_pushButtonRecorder_clicked()
{
//    Open recorder window
    Recorder * recorder = new Recorder();
    if (!recorder->isVisible()) {
        this->hide();
        recorder->show();
    }
}

void MainWindow::on_pushButtonNotepad_clicked()
{
//    Open notepad window
    Notepad * notepad = new Notepad();
    if (!notepad->isVisible()) {
        this->hide();
        notepad->show();
    }
}
