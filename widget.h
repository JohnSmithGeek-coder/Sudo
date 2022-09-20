#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QPainter>
#include <QPoint>
#include <QTimer>
#include <QFile>
#include <QMessageBox>

#include "game.h"
#include "settingdialog.h"
#include "statisticdialog.h"
#include "leveldialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void PlayButtonClicked();
    void PlayNewGameButtonClicked();
    void SettingsButtonClicked();
    void DataButtonClicked();
    void handleGameOver();
    void handleGameOn();
    void handleTimeChanged();
    void handleStartAnimationTimeOut();
    void handleRowFilledAnimationTimeOut();
    void handlecolFilledAniamtionTimeOut();
    void handleGameStart();
    void handleRowFilled(int, int);
    void handleColFilled(int, int);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    Ui::Widget *ui;
    QPushButton *PlayButton;
    QPushButton *PlayNewGameButton;
    QPushButton *SettingsButton;
    QPushButton *DataButton;
    Game *game;
    QDialog *settingsDialog;
    StatisticDialog *statisticDialog;
    LevelDialog *levelDailog;

    QTimer *startAnimationTimer;
    QTimer *rowFilledAnimationTimer;
    QTimer *colFilledAnimationTimer;

    //animation
    int yPos;
    int rIndex, cIndex;
    int auxrIndex, auxcIndex;
    int rmoveIndex, cmoveIndex;

    int bottomNumButton;
    int toolsButton;

    void drawBackColor(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawGridNum(QPainter &painter);
    void drawBottomNum(QPainter &painter);
    void drawTools(QPainter &painter);
    void drawGameInfo(QPainter &painter);
    void drawIllustration(QPainter &painter);

    void handleGridMouse(int x, int y);
};
#endif // WIDGET_H
