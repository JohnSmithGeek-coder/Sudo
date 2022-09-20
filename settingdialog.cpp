#include "settingdialog.h"

#include <QMessageBox>

SettingDialog::SettingDialog(Game *gamePtr, QWidget * parent)
    :QDialog(parent)
{
    game = gamePtr;
    setBaseSize(190, 240);
    setMaximumSize(300, 480);
    setMinimumSize(190, 240);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/new/prefix1/image/settings-icon.png"));

    setStyleSheet("QPushButton{"
                  "background-color:white;"
                  "color:rgb(0, 0, 0);"
                  "font-family:Microsoft Yahei;"
                  "font-size:12pt;"
                  "text-align:left;"
                  "border-radius:8px;"
                  "}"
                  "QPushButton:hover{"
                  "background-color:rgb(248, 249, 252);"
                  "}"
                  "QPushButton:pressed{"
                  "background-color:white;"
                  "padding-left:3px;"
                  "padding-top:3px;"
                  "}");
    int width = 60;
    int begin = 20;
    int height = 150;
    int iconSize = 20;
    gameSettings = new QPushButton(tr("  游戏设置"), this);
    gameSettings->setGeometry(begin, 0, height, width);
    gameSettings->setIcon(QIcon(":/new/prefix1/image/settings.png"));
    gameSettings->setIconSize(QSize(iconSize, iconSize));
    QObject::connect(gameSettings, SIGNAL(clicked()), this, SLOT(gameSettingsClicked()));

    gameRule = new QPushButton(tr("  游戏规则"), this);
    gameRule->setGeometry(begin, width, height, width);
    gameRule->setIcon(QIcon(":/new/prefix1/image/compliant.png"));
    gameRule->setIconSize(QSize(iconSize, iconSize));
    QObject::connect(gameRule, SIGNAL(clicked()), this, SLOT(gameRuleClicked()));

    help = new QPushButton(tr("  帮助"), this);
    help->setGeometry(begin, 2 * width, height, width);
    help->setIcon(QIcon(":/new/prefix1/image/help.png"));
    help->setIconSize(QSize(iconSize, iconSize));
    QObject::connect(help, SIGNAL(clicked()), this, SLOT(helpClicked()));

    about = new QPushButton(tr("  关于"), this);
    about->setGeometry(begin, 3 * width, height, width);
    about->setIcon(QIcon(":/new/prefix1/image/info.png"));
    about->setIconSize(QSize(iconSize, iconSize));
    QObject::connect(about, SIGNAL(clicked()), this, SLOT(aboutClicked()));
    gameSettingsFlag = false;

    gameRuleDialog = new GameRuleDialog(this);
    aboutDialog = new AboutDialog(this);
}

void SettingDialog::paintEvent(QPaintEvent * )
{
    if (gameSettingsFlag) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QPen pen;
        pen.setWidth(3);
        painter.setPen(pen);
        QFont font = painter.font();
        font.setFamily("Microsoft Yahei");
        font.setPixelSize(18);
        painter.setFont(font);
        int x_start = 60;
        int y_start = 80;
        int width = 180;
        int height = 60;
        QString settings[4] = {"游戏音效",
                               "错误上限",
                               "隐藏已使用数字",
                               "自动移除笔记"};
        for (int i = 0; i < 4; i++) {
            painter.drawText(QRect(x_start, y_start, width, height), settings[i]);
            y_start += 60;
        }

        width = 40; height = 20;
        x_start = 240; y_start = 80;
        QImage on_image(":/new/prefix1/image/button_on.png");
        QImage off_image(":/new/prefix1/image/button_off.png");
        on_image = on_image.scaled(40, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        off_image = off_image.scaled(40, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        for (int i = 0; i < 4; i++) {
            if (game->settingsFlag[i]) {
                painter.drawImage(QRect(x_start, y_start, width, height), on_image);
            } else {
                painter.drawImage(QRect(x_start, y_start, width, height), off_image);
            }
            y_start += 60;
        }
    }
}

void SettingDialog::closeEvent(QCloseEvent *)
{
    resize(190, 240);
    int begin = 60;
    gameRule->setGeometry(20, begin, 150, 60);
    help->setGeometry(20, begin + 60, 150, 60);
    about->setGeometry(20, begin + 120, 150, 60);
    gameSettingsFlag = false;
    update();
}

void SettingDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (gameSettingsFlag == false) return;
    int x = event->x();
    int y = event->y();
    if (x >= 240 && x <= 280 && y >= 80 && y < 320) {
        if ((y - 80) % 60 > 20) {
            return;
        }
        int i = (y - 80) / 60;
        if (game->settingsFlag[i]) {
            game->settingsFlag[i] = false;
        } else {
            game->settingsFlag[i] = true;
        }
    }
    update();
}

void SettingDialog::gameSettingsClicked()
{
    if (gameSettingsFlag) {
        resize(190, 240);
        int begin = 60;
        gameRule->setGeometry(20, begin, 150, 60);
        help->setGeometry(20, begin + 60, 150, 60);
        about->setGeometry(20, begin + 120, 150, 60);
        gameSettingsFlag = false;
        update();
    } else {
        resize(300, 480);
        int begin = 300;
        gameRule->setGeometry(20, begin, 150, 60);
        help->setGeometry(20, begin + 60, 150, 60);
        about->setGeometry(20, begin + 120, 150, 60);
        gameSettingsFlag = true;
        update();
    }

}

void SettingDialog::gameRuleClicked()
{
    gameRuleDialog->show();
}

void SettingDialog::helpClicked()
{

}

void SettingDialog::aboutClicked()
{
    aboutDialog->show();
}
