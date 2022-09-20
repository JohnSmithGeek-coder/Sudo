#ifndef GAME_H
#define GAME_H

#include <QPoint>
#include <QPushButton>
#include <QFile>
#include <vector>
#include <stack>
#include <QDataStream>
#include <QTextStream>
#include <QObject>
#include <QTimer>

#include "generatethread.h"

class Game : public QObject
{
    Q_OBJECT

public:
    struct NoteGridPos{
        int i, j;
        NoteGridPos(int x, int y) :
            i(x), j(y) { }
    };

    struct Action{
        int i, j;
        int flag; int number;
        int NoteGridStatus[10];
        std::vector<NoteGridPos> v;
    };

    enum GAME_STATUS{ON, OFF, SUSPEND, OVER};

public:
    int grid[9][9];
    int KnownGrid[9][9];
    int EmptyGrid[9][9];
    int NoteGrid[9][9][10];
    int num[9];
    int GameMode;  //1,2,3,4分别代表简单、中等、困难、专家
    int seconds;
    int NoteButton;

    int startTimes[4];
    int winTimes[4];
    int shortestTime[4];
    int longestTime[4];
    int averageTime[4];

    bool settingsFlag[4];

public:
    Game(QObject *parent = nullptr);
    ~Game();

    void ReadGridData();

    void readRecordData();
    void saveRecordData();

    void readSettingsData();
    void saveSettingsData();

    GAME_STATUS getGameStatus() { return status; }
    void  setGameStatus(GAME_STATUS s) { status = s; }
    QPoint getCurrentPos() { return curPos; }
    void setCurrentPos(QPoint newPos) { curPos = newPos; }
    int  getWrongTimes() { return wrongTimes; }
    bool getWinFlag() { return WinFlag; }
    int  getTipsTimes() { return tipsTimes; }

    void setGridNum(int num);

    void GameStart();
    void GameSuspend();
    void GameRestart();
    void GameOver();

    void Undo();
    void Erase();
    void handleNote();
    void Hint();

    void SaveGame(QDataStream & out);
    void RecoverGame(QDataStream & in);

private:
    struct Position{
        int row_num;
        int col_num;
        int block_num;
        Position(int i, int j) {
            row_num = i;
            col_num = j;
            block_num = (i / 3) * 3 + j / 3;
        }
    };

    struct Book{
        int col[10][10];
        int row[10][10];
        int block[10][10];
        void setVal(int i, int j, int number, int val) {
            row[i][number] = val;
            col[j][number] = val;
            int k = ((i - 1) / 3) * 3 + (j - 1) / 3 + 1;
            block[k][number] = val;
        }
        bool isNotOne(int i, int j, int number) {
            int k = ((i - 1) / 3) * 3 + (j - 1) / 3 + 1;
            if (row[i][number] != 1 && col[j][number] != 1 && block[k][number] != 1) {
                return true;
            }
            return false;
        }
    };

private:
    GAME_STATUS status;
    int wrongTimes;
    int tipsTimes;
    int emptyCounter;

    QPoint curPos;

    int PageNum[4];
    bool WinFlag;
    bool HintFlag;

    QTimer *timer;
    generateThread *thread[15];

    Book book;

    std::stack<Action> Actions;

    void initBook();
    void initGame();
    void initNoteGrid(int i, int j);
    void eraseNoteGrid(int i, int j, int number, Action & act);
    void fillNoteGrid(Action & act);
    void rowFilledTest(int row, int col);
    void colFilledTest(int row, int col);

signals:
    void GameOverSignal();
    void GameOnSignal();
    void TimeChanged();
    void sendGameMode(int, int);
    void rowFilledSignal(int, int);
    void colFilledSignal(int, int);

public slots:
    void handleTimeOut();
};

#endif // GAME_H
