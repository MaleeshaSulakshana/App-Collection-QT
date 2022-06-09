#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>

namespace Ui {
class Notepad;
}

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Notepad(QWidget *parent = nullptr);
    ~Notepad();

private slots:
    void on_pushButtonNew_clicked();

    void on_pushButtonOpen_clicked();

    void on_pushButtonSave_clicked();

    void on_pushButtonSaveAs_clicked();

    void on_pushButtonPrint_clicked();

    void on_pushButtonCopy_clicked();

    void on_pushButtonCut_clicked();

    void on_pushButtonPaste_clicked();

    void on_pushButtonUndo_clicked();

    void on_pushButtonRedo_clicked();

    void on_pushButtonFont_clicked();

    void on_pushButtonBold_clicked();

    void on_pushButtonItalic_clicked();

    void on_pushButtonUnderscore_clicked();

    void on_pushButtonExit_clicked();

    void on_pushButtonBackToMenu_clicked();

private:
    Ui::Notepad *ui;

    QString currentFile;
    bool isItalic=false;
    bool isBold=false;
    bool isUnderine=false;

};

#endif // NOTEPAD_H
