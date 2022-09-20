#include "statisticdialog.h"

#include <iostream>

StatisticDialog::StatisticDialog(Game *gamePtr, QWidget *parent)
    :QDialog(parent)
{
    game = gamePtr;
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
    xpos = 45;
    moveStatus = 0;
    setFixedSize(400, 600);
    setWindowTitle("  统计信息");
    setWindowIcon(QIcon(":/new/prefix1/image/trend.png"));
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    currentLevel = 0;
    curPos = currentLevel;
    setData(currentLevel);
    setMouseTracking(true);
}

void StatisticDialog::handleTimeOut()
{
    int distance = 50;

    if (moveStatus > 1) {
        distance = 50;
    } else {
        distance = 25;
    }
    if (xpos < 100 * curPos + 45) {
        xpos += distance;
    } else if (xpos > 100 * curPos + 45) {
        xpos -= distance;
    } else {
        timer->stop();
    }
    update();
}

void StatisticDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setFamily("Microsoft Yahei");
    font.setPixelSize(24);
    painter.setFont(font);
    QString level[4] = {"简单", "中等", "困难", "专家"};
    for (int pos = 0; pos < 400; pos += 100) {
        if (pos / 100 == currentLevel || pos / 100 == curPos) {
            pen.setColor(QColor(18, 150, 219, 255));
        } else {
            pen.setColor(QColor(0, 0, 0, 255));
        }
        painter.setPen(pen);
        painter.drawText(pos, 10, 100, 60, Qt::AlignCenter, level[pos / 100]);
    }
    pen.setColor(QColor(18, 150, 219, 255));
    pen.setWidth(4); //pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawLine(xpos - 20, 65, xpos + 30, 65);

    QBrush brush(QColor(18, 150, 219, 255), Qt::SolidPattern);
    brush.setColor(QColor(255, 255, 255, 255));
    painter.setBrush(brush);
    pen.setColor(QColor(73, 144, 226, 255)); pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRoundedRect(50, 135, 300, 180, 10, 10);
    painter.drawRoundedRect(50, 378, 300, 180, 10, 10);

    pen.setColor(QColor(75, 75, 75, 255));
    painter.setPen(pen);
    font.setPixelSize(20);
    painter.setFont(font);
    painter.drawText(30, 90, 50, 50, Qt::AlignLeft, "游戏");

    QString GameInfo[3] = {"游戏开始", "游戏获胜", "胜率"};
    QString GameImagePath[3] = {":/new/prefix1/image/begin.png",
                            ":/new/prefix1/image/win.png",
                            ":/new/prefix1/image/winrate.png"};
    QString GameData[3] = {QString::number(startTimes),
                           QString::number(winTimes),
                           QString::number(winRate, 'f', 2)};
    GameData[2].push_back('%');
    int pos = 150;
    for (int i = 0; i < 3; i++) {
        painter.drawImage(QRect(60, pos, 30, 30), QImage(GameImagePath[i]));
        painter.drawText(120, pos, 150, 60, Qt::AlignLeft, GameInfo[i]);
        painter.drawText(250, pos, 80, 60, Qt::AlignRight, GameData[i]);
        pos += 60;
    }

    painter.drawText(30, 330, 50, 50, Qt::AlignLeft, "时间");

    QString TimeInfo[3] = {"最短用时", "最长用时", "平均用时"};
    QString TimeImagePath[3] = {":/new/prefix1/image/shortestTime.png",
                            ":/new/prefix1/image/longestTime.png",
                            ":/new/prefix1/image/averageTime.png"};
    QString TimeData[3];
    TimeData[0] = timeTransform(shortestTime);
    TimeData[1] = timeTransform(longestTime);
    TimeData[2] = timeTransform(averageTime);
    QImage image[3];
    for (int i = 0; i < 3; i++) {
        image[i].load(TimeImagePath[i]);
        image[i] = image[i].scaled(30, 30, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    pos = 390;
    for (int i = 0; i < 3; i++) {
        painter.drawImage(QPoint(60, pos), image[i]);
        painter.drawText(120, pos, 150, 60, Qt::AlignLeft, TimeInfo[i]);
        painter.drawText(250, pos, 80, 60, Qt::AlignRight, TimeData[i]);
        pos += 60;
    }
}

void StatisticDialog::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    if (x >= 0 && x < 400 && y >= 20 && y <= 60) {
        currentLevel = x / 100;
    }
    setData(currentLevel);
    update();
}

void StatisticDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (timer->isActive()) return;
    int frontPos = curPos;
    int x = event->x();
    int y = event->y();

    if (x > 0 && x < 400 && y >= 20 && y <= 60) {
        curPos = x / 100;
        if (curPos == frontPos) return;
    } else {
        if (curPos == currentLevel) return;
        curPos = currentLevel;
    }
    int t = frontPos - curPos;
    moveStatus = (t >= 0 ? t : (-t));
    timer->start(1);
    update();
}

void StatisticDialog::setData(int level)
{
    startTimes = game->startTimes[level];
    winTimes = game->winTimes[level];
    if (startTimes == 0) {
        winRate = 0;
    } else {
       winRate = (winTimes * 1.0 / startTimes) * 100;
    }
    shortestTime = game->shortestTime[level];
    longestTime = game->longestTime[level];
    averageTime = game->averageTime[level];
}

QString StatisticDialog::timeTransform(int time)
{
    QString s1;
    int seconds = time;
    if (seconds == 0) {
        s1 = "--:--";
        return s1;
    }
    int high = seconds / 60;
    s1.push_back(high / 10 + 48);
    s1.push_back(high % 10 + 48);
    s1.push_back(':');
    int low = seconds % 60;
    s1.push_back(low / 10 + 48);
    s1.push_back(low % 10 + 48);
    return s1;
}
