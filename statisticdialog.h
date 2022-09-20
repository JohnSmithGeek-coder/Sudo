#ifndef STATISTICDIALOG_H
#define STATISTICDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

#include "game.h"

class StatisticDialog : public QDialog
{
    Q_OBJECT
public:
    int currentLevel;
    StatisticDialog(Game *gamePtr, QWidget *parent = nullptr);
    void setData(int level);

public slots:
    void handleTimeOut();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Game *game;
    QTimer *timer;

    int startTimes;
    int winTimes;
    float winRate;
    int shortestTime;
    int longestTime;
    int averageTime;

    int xpos;
    int moveStatus;

    int curPos;
    QString timeTransform(int time);
};

#endif // STATISTICDIALOG_H
