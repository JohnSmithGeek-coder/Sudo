#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QPainter>
#include <QMouseEvent>

#include "game.h"
#include "gameruledialog.h"
#include "aboutdialog.h"

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    SettingDialog(Game * gamePtr, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void gameSettingsClicked();
    void gameRuleClicked();
    void helpClicked();
    void aboutClicked();

private:
    bool gameSettingsFlag;
    Game *game;
    QPushButton *gameSettings;
    QPushButton *gameRule;
    QPushButton *help;
    QPushButton *about;
    GameRuleDialog *gameRuleDialog;
    AboutDialog *aboutDialog;

};

#endif // SETTINGDIALOG_H
