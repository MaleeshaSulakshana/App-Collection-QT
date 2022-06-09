#ifndef PAINTER_H
#define PAINTER_H

#include <QMainWindow>
#include <QList>

class scribblearea;

class Painter : public QMainWindow
{
    Q_OBJECT

public:
    Painter();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void open();
    void save();
    void backToMainMenu();
    void penColor();
    void penWidth();
    void about();

private:
    void createActions();
    void createMenus();

    bool maybeSave();

    bool saveFile(const QByteArray &fileFormat);

    scribblearea *scribbleArea;
    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;

    QAction *openAct;

    QList<QAction *> saveAsActs;
    QAction *exitAct;
    QAction *backToMenu;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

};

#endif // PAINTER_H
