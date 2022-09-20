#ifndef LEVELDIALOG_H
#define LEVELDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QPainter>
#include <QCoreApplication>

#include "game.h"

class LevelDialog : public QDialog
{
    Q_OBJECT

public:
    LevelDialog(Game *gamePtr, QWidget *parent = nullptr);

public slots:
    void easyButtonClicked();
    void mediumButtonClicked();
    void hardButtonClicked();
    void expertButtonClicked();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Game *game;
    QPushButton *easyButton;
    QPushButton *mediumButton;
    QPushButton *hardButton;
    QPushButton *expertButton;

signals:
    void gameStart();
};

#endif // LEVELDIALOG_H
