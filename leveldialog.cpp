#include "leveldialog.h"

LevelDialog::LevelDialog(Game *gamePtr, QWidget *parent)
    :QDialog(parent)
{
    game = gamePtr;
    setFixedSize(400, 450);
    setWindowTitle(" 难度选择");
    setWindowIcon(QIcon(":/new/prefix1/image/volume.png"));

    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    setStyleSheet("QPushButton{"
                  "background-color:white;"
                  "color:rgb(73, 144, 226);"
                  "font-family:Microsoft Yahei;"
                  "font-size:20pt;"
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

    easyButton = new QPushButton(QIcon(":/new/prefix1/image/easy.png"), "        简单", this);
    easyButton->setStyleSheet("color:rgb(38, 198, 218)");
    mediumButton = new QPushButton(QIcon(":/new/prefix1/image/medium.png"), "        中等", this);
    mediumButton->setStyleSheet("color:rgb(255, 210, 0)");
    hardButton = new QPushButton(QIcon(":/new/prefix1/image/hard.png"), "        困难", this);
    hardButton->setStyleSheet("color:rgb(190, 99, 249)");
    expertButton = new QPushButton(QIcon(":/new/prefix1/image/expert.png"), "        专家", this);
    expertButton->setStyleSheet("color:rgb(252, 87, 59)");
    easyButton->setIconSize(QSize(60, 60));
    easyButton->setGeometry(70, 75, 280, 68);
    mediumButton->setIconSize(QSize(60, 60));
    mediumButton->setGeometry(70, 150, 280, 68);
    hardButton->setIconSize(QSize(60, 60));
    hardButton->setGeometry(70, 225, 280, 68);
    expertButton->setIconSize(QSize(60, 60));
    expertButton->setGeometry(70, 300, 280, 68);

    QObject::connect(easyButton, SIGNAL(clicked()), this, SLOT(easyButtonClicked()));
    QObject::connect(mediumButton, SIGNAL(clicked()), this, SLOT(mediumButtonClicked()));
    QObject::connect(hardButton, SIGNAL(clicked()), this, SLOT(hardButtonClicked()));
    QObject::connect(expertButton, SIGNAL(clicked()), this, SLOT(expertButtonClicked()));
}

void LevelDialog::paintEvent(QPaintEvent *)
{
    if (game->getGameStatus() != Game::ON && game->getGameStatus() != Game::SUSPEND) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setFamily("Microsoft Yahei");
    font.setPixelSize(20);
    painter.setFont(font);
    painter.drawText(QRect(0, 10, 400, 40), Qt::AlignCenter, "⚠警告：当前游戏进度即将丢失！");
}

void LevelDialog::easyButtonClicked()
{
    game->GameMode = 1;
    game->GameStart();
    emit gameStart();
    close();
}

void LevelDialog::mediumButtonClicked()
{
    game->GameMode = 2;
    game->GameStart();
    emit gameStart();
    close();
}

void LevelDialog::hardButtonClicked()
{
    game->GameMode = 3;
    game->GameStart();
    emit gameStart();
    close();
}

void LevelDialog::expertButtonClicked()
{
    game->GameMode = 4;
    game->GameStart();
    emit gameStart();
    close();
}
