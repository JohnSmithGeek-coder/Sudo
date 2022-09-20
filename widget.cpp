#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QIODevice>
#include <QDataStream>
#include <QGraphicsDropShadowEffect>

#include <synchapi.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/prefix1/image/mainicon.png"));

    game = new Game(this);
    QObject::connect(game, SIGNAL(GameOverSignal()), this, SLOT(handleGameOver()));
    QObject::connect(game, SIGNAL(TimeChanged()), this, SLOT(handleTimeChanged()));
    QObject::connect(game, SIGNAL(GameOnSignal()), this, SLOT(handleGameOn()));
    QObject::connect(game, SIGNAL(rowFilledSignal(int, int)), this, SLOT(handleRowFilled(int, int)));
    QObject::connect(game, SIGNAL(colFilledSignal(int, int)), this, SLOT(handleColFilled(int, int)));

    bottomNumButton = -1;
    toolsButton = -1;

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(8);        // 阴影圆角的大小
    effect->setColor(Qt::gray);      //阴影的颜色
    effect->setOffset(3,3);

    setFixedSize(1000, 700);
    setStyleSheet("background-color:white");
    setWindowTitle(" 数独");
    setFocusPolicy(Qt::StrongFocus);

    QFile file("./data/suspend.txt");

    file.open(QIODevice::ReadOnly);
    if (!file.atEnd()) {
        QDataStream in(&file);
        game->RecoverGame(in);
    }
    file.close();

    if (game->getGameStatus() == Game::SUSPEND) {
        PlayButton = new QPushButton(tr("继续游戏"), this);
    } else {
        PlayButton = new QPushButton(tr("开始游戏"), this);
    }
    PlayNewGameButton = new QPushButton(tr("开始新游戏"), this);
    PlayNewGameButton->setGraphicsEffect(effect);
    SettingsButton = new QPushButton(tr("设置⚙"), this);
    DataButton = new QPushButton(tr("统计📊"), this);
    PlayButton->setStyleSheet("QPushButton{"
                              "background-color:rgb(73, 144, 226);"
                              "color:white;"
                              "font-family:Microsoft Yahei;"
                              "font-size:20pt;"
                              "border-radius:8px;"
                              "}"
                              "QPushButton:hover{"
                              "background-color:rgb(44 , 137 , 255);"
                              "}"
                              "QPushButton:pressed{"
                              "background-color:rgb(14 , 135 , 228);"
                              "padding-left:3px;"
                              "padding-top:3px;"
                              "}");
    PlayNewGameButton->setStyleSheet("QPushButton{"
                                     "background-color:white;"
                                     "color:rgb(73, 144, 226);"
                                     "font-family:Microsoft Yahei;"
                                     "font-size:20pt;"
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
    SettingsButton->setStyleSheet("QPushButton{"
                                  "background-color:rgb(73, 144, 226);"
                                  "color:white;"
                                  "font-family:Microsoft Yahei;"
                                  "font-size:15pt;"
                                  "border-radius:8px;"
                                  "}"
                                  "QPushButton:hover{"
                                  "background-color:rgb(44 , 137 , 255);"
                                  "}"
                                  "QPushButton:pressed{"
                                  "background-color:rgb(14 , 135 , 228);"
                                  "padding-left:3px;"
                                  "padding-top:3px;"
                                  "}");
    DataButton->setStyleSheet("QPushButton{"
                                  "background-color:rgb(73, 144, 226);"
                                  "color:white;"
                                  "font-family:Microsoft Yahei;"
                                  "font-size:15pt;"
                                  "border-radius:8px;"
                                  "}"
                                  "QPushButton:hover{"
                                  "background-color:rgb(44 , 137 , 255);"
                                  "}"
                                  "QPushButton:pressed{"
                                  "background-color:rgb(14 , 135 , 228);"
                                  "padding-left:3px;"
                                  "padding-top:3px;"
                                  "}");
    PlayButton->setGeometry(630, 360, 280, 80);
    PlayNewGameButton->setGeometry(630, 460, 280, 80);
    PlayNewGameButton->setVisible(false);
    DataButton->setGeometry(630, 460, 130, 70);
    SettingsButton->setGeometry(780, 460, 130, 70);
    if (game->getGameStatus() == Game::SUSPEND) {
        PlayNewGameButton->setVisible(true);
        DataButton->setGeometry(630, 560, 130, 70);
        SettingsButton->setGeometry(780, 560, 130, 70);
    }
    QObject::connect(PlayButton, SIGNAL(clicked()), this, SLOT(PlayButtonClicked()));
    QObject::connect(DataButton, SIGNAL(clicked()), this, SLOT(DataButtonClicked()));
    QObject::connect(SettingsButton, SIGNAL(clicked()), this, SLOT(SettingsButtonClicked()));
    QObject::connect(PlayNewGameButton, SIGNAL(clicked()), this, SLOT(PlayNewGameButtonClicked()));

    settingsDialog = new SettingDialog(game, this);
    statisticDialog = new StatisticDialog(game, this);
    levelDailog = new LevelDialog(game, this);
    QObject::connect(levelDailog, SIGNAL(gameStart()), this, SLOT(handleGameStart()));

    startAnimationTimer = new QTimer(this);
    QObject::connect(startAnimationTimer, SIGNAL(timeout()), this, SLOT(handleStartAnimationTimeOut()));

    rowFilledAnimationTimer = new QTimer(this);
    QObject::connect(rowFilledAnimationTimer, SIGNAL(timeout()), this, SLOT(handleRowFilledAnimationTimeOut()));

    colFilledAnimationTimer = new QTimer(this);
    QObject::connect(colFilledAnimationTimer, SIGNAL(timeout()), this, SLOT(handlecolFilledAniamtionTimeOut()));

    yPos = 550;
    cIndex = -1; auxcIndex = -1;
    rIndex = -1; auxrIndex = -1;
    rmoveIndex = 0; cmoveIndex = 0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::PlayNewGameButtonClicked()
{
    PlayButton->setText(tr("继续游戏"));
    game->GameSuspend();
    levelDailog->show();
    update();
}

void Widget::PlayButtonClicked()
{
    if (game->getGameStatus() == Game::OFF || game->getGameStatus() == Game::OVER) {
        levelDailog->show();
    }
    else if (game->getGameStatus() == Game::SUSPEND) {
        PlayButton->setText((tr("暂停游戏")));
        game->GameRestart();
    }
    else if (game->getGameStatus() == Game::ON) {
        PlayButton->setText(tr("继续游戏"));
        game->GameSuspend();
    }
    update();
}

void Widget::SettingsButtonClicked()
{
    if (game->getGameStatus() == Game::ON) {
        PlayButton->setText(tr("继续游戏"));
        game->GameSuspend();
        update();
    }
    settingsDialog->setWindowTitle("设置");
    settingsDialog->show();
}

void Widget::DataButtonClicked()
{
    if (game->getGameStatus() == Game::ON) {
        PlayButton->setText(tr("继续游戏"));
        game->GameSuspend();
        update();
    }
    statisticDialog->setData(statisticDialog->currentLevel);
    statisticDialog->show();
}

void Widget::handleTimeChanged()
{
    update();
}

void Widget::handleStartAnimationTimeOut()
{
    if (yPos != 550) {
        yPos += 50;
        if (yPos > 500) {
            startAnimationTimer->stop();
        }
    }
    update();
}

void Widget::handleRowFilledAnimationTimeOut()
{
    if (rIndex != -1) {
        cmoveIndex++;
        if (auxcIndex + cmoveIndex > 8 && auxcIndex - cmoveIndex < 0) {
            rowFilledAnimationTimer->stop();
            rIndex = -1; auxcIndex = -1; cmoveIndex = 0;
        }
    }
    update();
}

void Widget::handlecolFilledAniamtionTimeOut()
{
    if (cIndex != -1) {
        rmoveIndex++;
        if (auxrIndex + rmoveIndex > 8 && auxrIndex - rmoveIndex < 0) {
            colFilledAnimationTimer->stop();
            cIndex = -1; auxrIndex = -1; rmoveIndex = 0;
        }
    }
    update();
}

void Widget::handleGameStart()
{
    startAnimationTimer->start(60);
    yPos = 50;
}

void Widget::handleGameOver()
{
    PlayButton->setText(tr("开始游戏"));
    PlayNewGameButton->setVisible(false);
    DataButton->setGeometry(630, 460, 130, 70);
    SettingsButton->setGeometry(780, 460, 130, 70);
}

void Widget::handleGameOn()
{
    PlayNewGameButton->setVisible(true);
    DataButton->setGeometry(630, 560, 130, 70);
    SettingsButton->setGeometry(780, 560, 130, 70);
    PlayButton->setText(tr("暂停游戏"));
}

void Widget::handleRowFilled(int row, int col)
{
    rIndex = row; auxcIndex = col;
    rowFilledAnimationTimer->start(40);
}

void Widget::handleColFilled(int col, int row)
{
    cIndex = col; auxrIndex = row;
    colFilledAnimationTimer->start(40);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    if (x >= 50 && x <= 500 && y >= 50 && y <= 500) {
        if (game->getGameStatus() == Game::ON) {
            handleGridMouse(x, y);
        }
    }
    int num;
    if (x >= 50 && x <= 500 && y >= 630 && y <= 680) {
        if (game->getGameStatus() == Game::ON) {
            num = bottomNumButton;
            bottomNumButton = -1;
            if (num != -1 && game->num[num] > 0) {
                game->setGridNum(num);
            }
        }
    }

    if (x >= 80 && x <= 450 && y >= 520 && y <= 560) {
        if ((x - 80) % 110 <= 40) {
            num = toolsButton;
            toolsButton = -1;
            switch (num) {
            case 0:
                game->Undo();
                break;
            case 1:
                game->Erase();
                break;
            case 2:
                game->handleNote();
                break;
            case 3:
                game->Hint();
                break;
            }
        }
    }
    update();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    int num;
    if (x >= 50 && x <= 500 && y >= 630 && y <= 680) {
        if (game->getGameStatus() == Game::ON) {
            num = (x - 50) / 50;
            if (game->num[num]) {
                bottomNumButton = num;
            }
        }
    }
    if (x >= 80 && x <= 450 && y >= 520 && y <= 560) {
        if (game->getGameStatus() == Game::ON) {
            if ((x - 80) % 110 <= 40) {
                num = (x - 80) / 110;
                toolsButton = num;
            }
        }
    }
    update();
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    int number = event->key();
    if (49 <=  number && 57 >= number) {
        if (game->getGameStatus() == Game::ON) {
            int num = bottomNumButton;
            bottomNumButton = -1;
            if (num != -1 && game->num[num] > 0) {
                game->setGridNum(num);
            }
        }
    }
    if (number >= 16777234 && number <= 16777237) {
        QPoint point = game->getCurrentPos();
        int x = point.x(); int y = point.y();
        switch (number) {
        case Qt::Key_Down:
            if (y < 8) {
                game->setCurrentPos(QPoint(x, ++y));
            }
            break;
        case Qt::Key_Up:
            if (y > 0) {
                game->setCurrentPos(QPoint(x, --y));
            }
            break;
        case Qt::Key_Left:
            if (x > 0) {
                game->setCurrentPos(QPoint(--x, y));
            }
            break;
        case Qt::Key_Right:
            if (x < 8) {
                game->setCurrentPos(QPoint(++x, y));
            }
            break;
        }

    }
    if (number == Qt::Key_N) {
        game->handleNote();
    }
    if (number == Qt::Key_Backspace) {
        toolsButton = -1;
        game->Undo();
    }
    if (number == Qt::Key_Space) {
        PlayButtonClicked();
    }
    update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    int number = event->key();
    if (49 <=  number && 57 >= number) {
        number -= 49;
        if (game->getGameStatus() == Game::ON) {
            if (game->num[number]) {
                bottomNumButton = number;
            }
        }
    }
    if (number == Qt::Key_Backspace) {
        toolsButton = 0;
    }
    update();
}

void Widget::handleGridMouse(int x, int y)
{
    int i = (x - 50) / 50;
    int j = (y - 50) / 50;
    QPoint pos(i, j);
    game->setCurrentPos(pos);
}

void Widget::paintEvent(QPaintEvent * )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    drawBackColor(painter);
    drawTools(painter);
    drawBottomNum(painter);
    drawGrid(painter);
    drawGridNum(painter);
    drawGameInfo(painter);
    drawIllustration(painter);
}

void Widget::closeEvent(QCloseEvent * )
{
    QFile file("./data/suspend.txt");
    file.open(QIODevice::WriteOnly);
    file.seek(0);
    QDataStream out(&file);
    game->SaveGame(out);
    file.close();
    game->saveRecordData();
    game->saveSettingsData();
}

void Widget::drawBackColor(QPainter &painter)
{
    if (game->getGameStatus() != Game::ON) return;
    QPoint pos = game->getCurrentPos();
    int x = pos.x();
    int y = pos.y();
    QBrush brush(QColor(226, 231, 237, 255), Qt::SolidPattern);
    painter.setBrush(brush);
    if (x != -1 && y != -1) {
        int draw_x = 50 + 50 * x;
        int draw_y = 50 + 50 * y;
        int width = 50;
        int height = 450;
        painter.drawRect(50, draw_y, height, width);
        painter.drawRect(draw_x, 50, width, height);

        int g_x = (x / 3) * 150 + 50;
        int g_y = (y / 3) * 150 + 50;
        painter.drawRect(g_x, g_y, 150, 150);

        brush.setColor(QColor(200, 208, 221, 255));
        painter.setBrush(brush);

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (game->grid[i][j] != 0 && game->grid[i][j] == game->grid[y][x]) {
                    draw_x = 50 + 50 * j;
                    draw_y = 50 + 50 * i;
                    painter.drawRect(draw_x, draw_y, 50, 50);
                }
            }
        }

        draw_x = 50 + 50 * x;
        draw_y = 50 + 50 * y;
        brush.setColor(QColor(187, 222, 250, 255));
        painter.setBrush(brush);
        painter.drawRect(draw_x, draw_y, 50, 50);
    }

    if (yPos >= 50 && yPos < 500) {
        brush.setColor(QColor(200, 208, 221, 255));
        painter.setBrush(brush);
        painter.drawRect(QRect(50, yPos, 450, 50));
        brush.setColor(QColor(226, 231, 237, 255));
        painter.setBrush(brush);
        painter.drawRect(QRect(50, yPos - 50, 450, 50));
        painter.drawRect(QRect(50, yPos + 50, 450, 50));
    }

    if (rIndex != -1) {
        brush.setColor(QColor(200, 208, 221, 255));
        painter.setBrush(brush);
        if (auxcIndex + cmoveIndex < 9) {
            painter.drawRect(QRect(50 + 50 * (auxcIndex + cmoveIndex), 50 + 50 * rIndex, 50, 50));
        }
        if (auxcIndex - cmoveIndex >= 0) {
            painter.drawRect(QRect(50 + 50 * (auxcIndex - cmoveIndex), 50 + 50 * rIndex, 50, 50));
        }
        if (auxcIndex + cmoveIndex + 1 < 9) {
            painter.drawRect(QRect(50 + 50 * (auxcIndex + cmoveIndex + 1), 50 + 50 * rIndex, 50, 50));
        }
        if (auxcIndex - cmoveIndex - 1 >= 0) {
            painter.drawRect(QRect(50 + 50 * (auxcIndex - cmoveIndex - 1), 50 + 50 * rIndex, 50, 50));
        }
    }

    if (cIndex != -1) {
        brush.setColor(QColor(200, 208, 221, 255));
        painter.setBrush(brush);
        if (auxrIndex + rmoveIndex < 9) {
            painter.drawRect(QRect(50 + 50 * cIndex, 50 + 50 * (auxrIndex + rmoveIndex), 50, 50));
        }
        if (auxrIndex - rmoveIndex >= 0) {
            painter.drawRect(QRect(50 + 50 * cIndex, 50 + 50 * (auxrIndex - rmoveIndex), 50, 50));
        }
        if (auxrIndex + rmoveIndex + 1 < 9) {
            painter.drawRect(QRect(50 + 50 * cIndex, 50 + 50 * (auxrIndex + rmoveIndex + 1), 50, 50));
        }
        if (auxrIndex - rmoveIndex - 1 >= 0) {
            painter.drawRect(QRect(50 + 50 * cIndex, 50 + 50 * (auxrIndex - rmoveIndex - 1), 50, 50));
        }
    }

    brush.setColor(QColor(187, 222, 250, 255));
    painter.setBrush(brush);
    if (bottomNumButton != -1) {
        int b_x = 50 + bottomNumButton * 50;
        painter.drawRect(b_x, 630, 50, 50);
    }

    if (toolsButton != -1) {
        int t_x = 80 + 110 * toolsButton;
        painter.drawRect(t_x, 520, 40, 40);
    }
}

void Widget::drawGrid(QPainter &painter)
{
    if (game->getGameStatus() == Game::OFF || game->getGameStatus() == Game::OVER) return;
    QPen pen(Qt::black, 4);
    painter.setPen(pen);
    int Start = 50;
    int End = 500;
    int Hor = Start;
    int Ver = Start;
    for (int i = 0; i < 4; i++) {
        painter.drawLine(Start, Ver, End, Ver);
        painter.drawLine(Hor, Start, Hor, End);
        Hor += 150; Ver += 150;
    }
    pen.setWidth(2);
    painter.setPen(pen);
    Ver = 100;
    Hor = 100;
    int i = 0;
    while (i < 6) {
        painter.drawLine(Start, Ver, End, Ver);
        painter.drawLine(Hor, Start, Hor, End);
        i++;
        if (i % 2 == 0) {
            Hor += 100; Ver += 100;
        } else {
            Hor += 50;  Ver += 50;
        }
    }
}

void Widget::drawGridNum(QPainter &painter)
{
    if (game->getGameStatus() != Game::ON) return;
    QPen pen;
    QFont font = painter.font();
    pen.setWidth(3); //pen.setColor(Qt::black);
    painter.setPen(pen);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(36);
    painter.setFont(font);
    int x, y;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game->grid[i][j]) {
                y = 50 + i * 50;
                x = 50 + j * 50;
                if (y < yPos) {
                    QRect rect(x, y, 50, 50);
                    QString s; s.push_back(game->grid[i][j] + 48);

                    if (game->grid[i][j] == game->KnownGrid[i][j]) {
                        if (!game->EmptyGrid[i][j]) {
                            pen.setColor(Qt::black); painter.setPen(pen);
                            painter.drawText(rect, Qt::AlignCenter, s);
                        } else {
                            pen.setColor(QColor(14, 71, 161, 255)); painter.setPen(pen);
                            painter.drawText(rect, Qt::AlignCenter, s);
                        }

                    }
                    else {
                        pen.setColor(Qt::red); painter.setPen(pen);
                        painter.drawText(rect, Qt::AlignCenter, s);
                    }
                }
            }
        }
    }
    for (int i = 0; i < 9; i++ ) {
        for (int j = 0; j < 9; j++) {
            if (game->grid[i][j] == 0) {
                if (game->NoteGrid[i][j][9] > 0) {
                    for (int k = 0; k < 9; k++) {
                        if (game->NoteGrid[i][j][k]) {
                            pen.setColor(Qt::black); painter.setPen(pen);
                            font.setPixelSize(14);   painter.setFont(font);
                            y = 50 + i * 50 + k / 3 * 16;
                            x = 50 + j * 50 + k % 3 * 16;
                            if (y < yPos) {
                                QRect rect(x, y, 16, 16);
                                QString s1; s1.push_back(k + 1 + 48);
                                painter.drawText(rect, Qt::AlignCenter, s1);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Widget::drawBottomNum(QPainter &painter)
{
    if (game->getGameStatus() == Game::OFF || game->getGameStatus() == Game::OVER) return;
    QPen pen;
    QFont font = painter.font();
    pen.setWidth(3);
    if (!game->NoteButton) {
        pen.setColor(QColor(14, 71, 161, 255));
    } else {
        pen.setColor(QColor(97, 98, 102, 255));
    }

    painter.setPen(pen);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(48);
    painter.setFont(font);
    int pos = 50;
    if (game->settingsFlag[2]) {
        for (int i = 0; i < 9; i++) {
            if (game->num[i] > 0) {
                QString s; s.push_back(i + 49);
                painter.drawText(pos, 630, 50, 50, Qt::AlignCenter, s);
            }
            pos += 50;
        }
    } else {
        for (int i = 0; i < 9; i++) {
            QString s; s.push_back(i + 49);
            painter.drawText(pos, 630, 50, 50, Qt::AlignCenter, s);
            pos += 50;
        }
    }
}

void Widget::drawTools(QPainter &painter)
{
    if (game->getGameStatus() == Game::OFF || game->getGameStatus() == Game::OVER) return;
    QImage image1(":/new/prefix1/image/undo.png");
    QImage image2(":/new/prefix1/image/erase.png");
    QImage image3(":/new/prefix1/image/note.png");
    QImage image4(":/new/prefix1/image/tips.png");
    QImage image5(":/new/prefix1/image/pen.png");
    image1 = image1.scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image2 = image2.scaled(35, 35, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image3 = image3.scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image4 = image4.scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image5 = image5.scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(QPoint(80, 520), image1);
    painter.drawImage(QPoint(190, 520), image2);
    if (game->NoteButton) {
        painter.drawImage(QPoint(300, 520), image5);
        QPen pen;
        pen.setWidth(3); pen.setColor(QColor(18, 150, 219, 255));
        painter.setPen(pen);
        QFont font1 = painter.font();
        font1.setFamily("Microsoft YaHei");
        font1.setPixelSize(14);
        font1.setBold(true);
        painter.setFont(font1);
        QRect rect0(340, 520, 23, 23);
        painter.drawText(rect0, "ON");
    } else {
        painter.drawImage(QPoint(300, 520), image3);
    }
    painter.drawImage(QPoint(410, 520), image4);
    QPen pen;
    pen.setWidth(3); pen.setColor(QColor(0, 0, 0, 255));
    painter.setPen(pen);
    QFont font = painter.font();
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(22);
    font.setBold(false);
    painter.setFont(font);
    int infoPos = 590;
    painter.drawText(80, infoPos, tr("撤消"));
    painter.drawText(190, infoPos, tr("擦除"));
    painter.drawText(300, infoPos, tr("笔记"));
    painter.drawText(410, infoPos, tr("提示"));
    int number = game->getTipsTimes();
    if (number > 0) {
        pen.setColor(QColor(255, 255, 255, 255));
        pen.setWidth(3);
        painter.setPen(pen);

        font.setPixelSize(16);
        painter.setFont(font);

        QBrush brush(QColor(18, 150, 219, 255), Qt::SolidPattern);
        painter.setBrush(brush);
        QRect rect(440, 520, 23, 23);
        painter.drawEllipse(rect);
        painter.drawText(rect, Qt::AlignCenter, QString::number(number));
    }
}

void Widget::drawGameInfo(QPainter &painter)
{
    if (game->getGameStatus() == Game::OFF || game->getGameStatus() == Game::OVER) return;
    QPen pen;
    QFont font = painter.font();
    pen.setWidth(3); pen.setColor(QColor(110, 110, 110, 255));
    painter.setPen(pen);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(18);
    painter.setFont(font);
    switch (game->GameMode) {
    case 1:
        painter.drawText(50, 15, 60, 30, Qt::AlignLeft, tr("简单"));
        break;
    case 2:
        painter.drawText(50, 15, 60, 30, Qt::AlignLeft, tr("中等"));
        break;
    case 3:
        painter.drawText(50, 15, 60, 30, Qt::AlignLeft, tr("困难"));
        break;
    case 4:
        painter.drawText(50, 15, 60, 30, Qt::AlignLeft, tr("专家"));
        break;
    }

    if (game->settingsFlag[1]) {
        painter.drawText(230, 15, 60, 30, Qt::AlignLeft, tr("错误"));
        QString s;
        s.push_back(game->getWrongTimes() + 48);
        s.push_back('/'); s.push_back('3');
        painter.drawText(270, 15, 60, 30, Qt::AlignLeft, s);
    }

    QString s1;
    s1.append("⏱️ ");
    int seconds = game->seconds;
    int high = seconds / 60;
    s1.push_back(high / 10 + 48);
    s1.push_back(high % 10 + 48);
    s1.push_back(':');
    int low = seconds % 60;
    s1.push_back(low / 10 + 48);
    s1.push_back(low % 10 + 48);
    painter.drawText(410, 15, 90, 30, Qt::AlignRight, s1);
}

void Widget::drawIllustration(QPainter &painter)
{
    QImage image5(":/new/prefix1/image/board.png");
    image5 = image5.scaled(250, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(QPoint(635, 50), image5);

    if (game->getGameStatus() == Game::SUSPEND) {
        QImage image6(":/new/prefix1/image/play.png");
        image6 = image6.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawImage(QPoint(225, 225), image6);
    }

    QPen pen;
    pen.setWidth(3); pen.setColor(QColor(254, 135, 1, 255));
    painter.setPen(pen);
    QFont font = painter.font();
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(22);
    painter.setFont(font);

    if (game->getGameStatus() == Game::OFF) {
        QImage image7(":/new/prefix1/image/wait.png");
        painter.drawImage(QPoint(180, 180), image7);
        painter.drawText(120, 450, 400, 40, Qt::AlignCenter, tr("等待开始游戏中<(￣︶￣)↗[GO!]"));
    }

    if (game->getGameStatus() == Game::OVER) {
        if (game->getWinFlag()) {
            QImage image8(":/new/prefix1/image/ok.png");
            painter.drawImage(QPoint(180, 180), image8);
            painter.drawText(120, 450, 400, 40, Qt::AlignCenter, tr("恭喜成功完成游戏d=====(￣▽￣*)b"));
        } else {
            QImage image9(":/new/prefix1/image/lose.png");
            painter.drawImage(QPoint(180, 180), image9);
            painter.drawText(120, 450, 400, 40, Qt::AlignCenter, tr("游戏失败，再接再厉哦( •̀ ω •́ )✧"));
        }
    }

    pen.setWidth(3); pen.setColor(Qt::black);
    painter.setPen(pen);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(36);
    painter.setFont(font);
    painter.drawText(730, 320, tr("数独"));
}
