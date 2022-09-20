#ifndef GAMERULEDIALOG_H
#define GAMERULEDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QIcon>

class GameRuleDialog : public QDialog
{
    Q_OBJECT
public:
    GameRuleDialog(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *);
};

#endif // GAMERULEDIALOG_H
