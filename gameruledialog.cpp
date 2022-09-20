#include "gameruledialog.h"

GameRuleDialog::GameRuleDialog(QWidget *parent)
    :QDialog(parent)
{
    setFixedSize(980, 500);
    setWindowIcon(QIcon(":/new/prefix1/image/compliant.png"));
    setWindowTitle("  游戏规则");
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

void GameRuleDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QString path[3] = {":/new/prefix1/image/rule1.png",
                       ":/new/prefix1/image/rule2.png",
                       ":/new/prefix1/image/rule3.png"};
    QImage image[3] = {QImage(path[0]), QImage(path[1]), QImage(path[2])};
    int x_start = 20;
    int y_start = 10;
    int width = 300;
    int height = 480;
    for (int i = 0; i < 3; i++) {
        image[i] = image[i].scaled(300, 480, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawImage(QRect(x_start, y_start, width, height), image[i]);
        x_start += width + 20;
    }
}
