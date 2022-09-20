#include "game.h"

#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <iostream>
#include <QDebug>

Game::Game(QObject *parent):QObject(parent)
{
    status = OFF;
    emptyCounter = 0;
    wrongTimes = 0;
    tipsTimes = 3;
    HintFlag = false;
    seconds = 0;
    NoteButton = 0;
    GameMode = 4;
    timer = new QTimer(this);
    for (int i = 0; i < 15; i++) {
        thread[i] = new generateThread(this);
        QObject::connect(this, SIGNAL(sendGameMode(int, int)), thread[i], SLOT(reciveGameMode(int, int)));
    }
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
    curPos.setX(-1); curPos.setY(-1);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            KnownGrid[i][j] = 0;
            grid[i][j] = 0;
            EmptyGrid[i][j] = 0;
        }
    }
    for (int i = 0; i < 4; i++) {
        PageNum[i] = 0;
    }
    readRecordData();
    readSettingsData();
}

Game::~Game()
{
    for (int i = 0; i < 15; i++) {
        thread[i]->wait();
        thread[i]->quit();
    }
}

void Game::handleTimeOut()
{
    if (status == ON) {
        seconds++;
        TimeChanged();
    }
}

void Game::initBook()
{
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            book.block[i][j] = book.col[i][j] = book.row[i][j] = 0;
        }
    }
}

void Game::initGame()
{
    emptyCounter = 0;
    wrongTimes = 0;
    tipsTimes = 3;
    seconds = 0;
    NoteButton = 0;
    HintFlag = false;
    curPos.setX(-1); curPos.setY(-1);

    while (!Actions.empty()) {
        Actions.pop();
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            KnownGrid[i][j] = 0;
            grid[i][j] = 0;
            EmptyGrid[i][j] = 0;
        }
    }
    for (int i = 0; i < 9; i++) {
        num[i] = 9;
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 10; k++) {
                NoteGrid[i][j][k] = 0;
            }
        }
    }
}

void Game::initNoteGrid(int i, int j)
{
    for (int k = 0; k < 10; k++) {
        NoteGrid[i][j][k] = 0;
    }
}

void Game::eraseNoteGrid(int i, int j, int number, Action & act)
{
    for (int k = 0; k < 9; k++) {
        if (NoteGrid[i][k][9]) {
            if (NoteGrid[i][k][number]) {
                NoteGrid[i][k][number] = 0;
                act.v.push_back(NoteGridPos(i, k));
                NoteGrid[i][k][9]--;
            }
        }
    }
    for (int k = 0; k < 9; k++) {
        if (NoteGrid[k][j][9]) {
            if (NoteGrid[k][j][number]) {
                NoteGrid[k][j][number] = 0;
                act.v.push_back(NoteGridPos(k, j));
                NoteGrid[k][j][9]--;
            }
        }
    }
    int m = (i / 3) * 3;
    int n = (j / 3) * 3;
    for (int k = m; k < m + 3; k++) {
        for (int l = n; l < n + 3; l++) {
            if (NoteGrid[k][l][9]) {
                if (NoteGrid[k][l][number]) {
                    NoteGrid[k][l][number] = 0;
                    act.v.push_back(NoteGridPos(k, l));
                    NoteGrid[k][l][9]--;
                }
            }
        }
    }
}

void Game::fillNoteGrid(Action & act)
{
    while (!act.v.empty()) {
        NoteGridPos pos = act.v.back();
        NoteGrid[pos.i][pos.j][act.number] = 1;
        NoteGrid[pos.i][pos.j][9]++;
        act.v.pop_back();
    }
}

void Game::rowFilledTest(int row, int col)
{
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] != KnownGrid[row][i]) {
            return;
        }
    }

    emit rowFilledSignal(row, col);
}

void Game::colFilledTest(int row, int col)
{
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] != KnownGrid[i][col]) {
            return;
        }
    }
    emit colFilledSignal(col, row);
}

void Game::setGridNum(int num)
{
    int i = curPos.y();
    int j = curPos.x();
    Action act;
    if (!grid[i][j]) {
        if (NoteButton && HintFlag == false) {
            if (!NoteGrid[i][j][num] && book.isNotOne(i + 1, j + 1, num + 1))
            {
                NoteGrid[i][j][num] = 1;
                NoteGrid[i][j][9]++;
                act.i = i; act.j = j; act.number = num;
                act.flag = -1;
                Actions.push(act);
            }
        } else {
            grid[i][j] = num + 1;
            act.i = i; act.j = j;
            act.flag = NoteGrid[i][j][9]; act.number = num;
            NoteGrid[i][j][9] = 0;

            if (settingsFlag[1] && grid[i][j] != KnownGrid[i][j]) {
                wrongTimes++;
                if (wrongTimes == 3) {
                    WinFlag = false;
                    GameOver();
                }
            } else {
                emptyCounter--;
                this->num[grid[i][j] - 1]--;
                book.setVal(i + 1, j + 1, num + 1, 1);
                rowFilledTest(i, j);
                colFilledTest(i, j);
                if (settingsFlag[3]) {
                    eraseNoteGrid(i, j, num, act);
                }
                if (!emptyCounter) {
                    WinFlag = true;
                    GameOver();
                }
            }
            Actions.push(act);
            if (HintFlag == true) {
                HintFlag = false;
            }
        }
    }
}

void Game::ReadGridData()
{
    QString path[4];
    path[0] = "./data/easygriddata";
    path[1] = "./data/mediumgriddata";
    path[2] = "./data/hardgriddata";
    path[3] = "./data/expertgriddata";
    int index = GameMode - 1;
    int t = PageNum[index];
    path[index].push_back(PageNum[index] + 48);
    PageNum[index] = (++PageNum[index]) % ((int)std::pow(2, index));
    QFile file;
    file.setFileName(path[index]);

    int n = (int)std::pow(2, index) + t - 1;

    while (fileMutex[n].tryLock() == false) {

        path[index].chop(1);
        path[index].push_back(PageNum[index] + 48);
        t = PageNum[index];
        PageNum[index] = (++PageNum[index]) % ((int)std::pow(2, index));
        file.setFileName(path[index]);
        n = std::pow(2, index) + t - 1;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        std::cout << "open error" << std::endl;
    } else {
        file.seek(0);
        QDataStream in(&file);
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                in >> grid[i][j] >> KnownGrid[i][j];
            }
        }
        file.close();
    }

    fileMutex[n].unlock();

    emit sendGameMode(GameMode, t);
    thread[n]->start();

    initBook();
    emptyCounter = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                book.setVal(i + 1, j + 1, grid[i][j], 1);
                num[grid[i][j] - 1]--;
            } else {
                emptyCounter++;
                EmptyGrid[i][j] = 1;
            }
        }
    }
}

void Game::readRecordData()
{
    QFile file("data/record");
    file.open(QIODevice::ReadOnly);
    if (file.atEnd()) {
        for (int i = 0; i < 4; i++) {
            startTimes[i] = 0;
            winTimes[i] = 0;
            shortestTime[i] = 0;
            longestTime[i] = 0;
            averageTime[i] = 0;
        }
        file.close();
        return;
    }
    file.seek(0);
    QDataStream in(&file);
    for (int i = 0; i < 4; i++) {
        in >> startTimes[i];
        in >> winTimes[i];
        in >> shortestTime[i];
        in >> longestTime[i];
        in >> averageTime[i];
    }
    file.close();
}

void Game::saveRecordData()
{
    QFile file("data/record");
    file.open(QIODevice::WriteOnly);
    file.seek(0);
    QDataStream out(&file);
    for (int i = 0; i < 4; i++) {
        out << startTimes[i];
        out << winTimes[i];
        out << shortestTime[i];
        out << longestTime[i];
        out << averageTime[i];
    }
    file.close();
}

void Game::readSettingsData()
{
    QFile file("data/settings");
    file.open(QIODevice::ReadOnly);
    if (file.atEnd()) {
        for (int i = 0; i < 4; i++) {
            settingsFlag[i] = true;
        }
        file.close();
        return;
    }
    file.seek(0);
    QDataStream in(&file);
    for (int i = 0; i < 4; i++) {
        in >> settingsFlag[i];
    }
    file.close();
}

void Game::saveSettingsData()
{
    QFile file("data/settings");
    file.open(QIODevice::WriteOnly);
    file.seek(0);
    QDataStream out(&file);
    for (int i = 0; i < 4; i++) {
        out << settingsFlag[i];
    }
    file.close();
}

void Game::GameStart()
{
    initGame();
    ReadGridData();
    status = ON;
    timer->start(1000);
    startTimes[GameMode - 1]++;
    GameOnSignal();
}

void Game::GameSuspend()
{
    timer->stop();
    status = SUSPEND;
}

void Game::GameRestart()
{
    timer->start(1000);
    status = ON;
}

void Game::GameOver()
{
    status = OVER;
    timer->stop();
    int index = GameMode - 1;
    if (WinFlag == true) {
        if (shortestTime[index] == 0 || seconds < shortestTime[index]) {
            shortestTime[index] = seconds;
        }
        if (seconds > longestTime[index]) {
            longestTime[index] = seconds;
        }
        averageTime[index] = (averageTime[index] * winTimes[index] + seconds) / (winTimes[index] + 1);
        winTimes[index]++;
    }
    GameOverSignal();
}



void Game::Undo()
{
    if (Actions.empty()) return;
    Action act = Actions.top();
    if (act.flag > -1) {
        if (grid[act.i][act.j] == KnownGrid[act.i][act.j]) {
            grid[act.i][act.j] = 0;
            NoteGrid[act.i][act.j][9] = act.flag;
            book.setVal(act.i + 1, act.j + 1, act.number + 1, 0);
            if (!act.v.empty()) {
                fillNoteGrid(act);
            }
            emptyCounter++;
            num[act.number]++;
        } else {
            grid[act.i][act.j] = 0;
            NoteGrid[act.i][act.j][9] = act.flag;
        }
    }
    else if (act.flag == -1){
        NoteGrid[act.i][act.j][act.number] = 0;
    }
    else if (act.flag == -2) {
        for (int k = 0; k < 10; k++) {
            NoteGrid[act.i][act.j][k] = act.NoteGridStatus[k];
        }
    }
    setCurrentPos(QPoint(act.j, act.i));
    Actions.pop();
}

void Game::Erase()
{
    int j = curPos.x();
    int i = curPos.y();
    if (NoteGrid[i][j][9] > 0) {
        Action act; act.i = i; act.j = j;
        act.flag = -2;
        for (int k = 0; k < 10; k++) {
            act.NoteGridStatus[k] = NoteGrid[i][j][k];
            NoteGrid[i][j][k] = 0;
        }
        Actions.push(act);
    }
}

void Game::handleNote()
{
    if (!NoteButton) {
        NoteButton = 1;
    } else {
        NoteButton = 0;
    }
}

void Game::Hint()
{
    int i = curPos.y();
    int j = curPos.x();
    if (i == -1 && j == -1) return;
    if (tipsTimes > 0 && grid[i][j] == 0) {
        HintFlag = true;
        setGridNum(KnownGrid[i][j] - 1);
        tipsTimes--;
    }
}

void Game::SaveGame(QDataStream & out)
{
    /*    record.dat文件结构
     *   length  |  type    |  name         |  meaning
     * ----------+----------+---------------+--------------------------------
     *      1    |  int     |  flag         |  表示程序关闭时是否处于游戏中状态
     *     9*9   |  int     |  grid         |  当前数独板各个位置数字
     *     9*9   |  int     |  KnownGrid    |  当前数独板已知的各个位置的数字
     *    9*9*10 |  int     |  NoteGrid     |  当前数独板各个位置的笔记信息
     *     9*9   |  int     |  EmptyGrid    |  当前游戏被遮住格子的信息
     *      1    |  int     |  len          |  当前游戏的操作记录Actions的长度
     *     ...   |  action  |  Actions      |  当前游戏的操作记录
     *      1    |  int     |  GameMode     |  当前游戏的模式
     *      1    |  int     |  WrongTimes   |  当前游戏的错误次数
     *      1    |  int     |  TipsTimes    |  当前游戏剩余提示数
     *      1    |  int     |  seconds      |  当前游戏用时
     *      1    |  int     |  emptyCounter |  当前空格数量
     *     1+1   |  int     |  curPos       |  当前位置信息
     *      1    |  int     |  NoteButton   |  笔记开关信息
     */
    int flag;
    if (status == OFF || status == OVER) {
        flag = 0;  out << flag;
        return;
    }
    flag = 1;
    out << flag;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            out << grid[i][j];
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            out << KnownGrid[i][j];
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 10; k++) {
                out << NoteGrid[i][j][k];
            }
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            out << EmptyGrid[i][j];
        }
    }
    int len = Actions.size();
    out << len;
    Action act;
    while (!Actions.empty()) {
        act = Actions.top();
        out << act.flag << act.i << act.j << act.number;
        Actions.pop();
    }
    out << GameMode;
    out << wrongTimes;
    out << tipsTimes;
    out << seconds;
    out << emptyCounter;
    out << curPos.x() << curPos.y();
    out << NoteButton;
    for (int i = 0; i < 4; i++) {
        out << PageNum[i];
    }
}

void Game::RecoverGame(QDataStream & in)
{
    int flag;
    in >> flag;
    if (flag == 0) {
        return;
    }
    status = SUSPEND;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> grid[i][j];
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> KnownGrid[i][j];
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 10; k++) {
                in >> NoteGrid[i][j][k];
            }
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> EmptyGrid[i][j];
        }
    }
    int len;
    in >> len;
    Action act;
    std::vector<Action> tmp;
    for (int i = 0; i < len; i++) {
        in >> act.flag >> act.i >> act.j >> act.number;
        tmp.push_back(act);
    }
    while (!tmp.empty()) {
        Actions.push(tmp.back());
        tmp.pop_back();
    }
    in >> GameMode;
    in >> wrongTimes;
    in >> tipsTimes;
    in >> seconds;
    in >> emptyCounter;
    HintFlag = false;
    int x, y;
    in >> x >> y;
    setCurrentPos(QPoint(x, y));
    in >> NoteButton;
    for (int i = 0; i < 9; i++) {
        num[i] = 9;
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                num[grid[i][j] - 1]--;
                book.setVal(i + 1, j + 1, grid[i][j], 1);
            }
        }
    }
}
