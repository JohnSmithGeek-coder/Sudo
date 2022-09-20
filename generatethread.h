#ifndef GENERATETHREAD_H
#define GENERATETHREAD_H

#include <QThread>
#include <QFile>
#include <vector>
#include <QMutex>

extern QMutex fileMutex[15];

class generateThread : public QThread
{
    Q_OBJECT

public:
    generateThread(QObject *parent = nullptr);
    void run();

public slots:
    void reciveGameMode(int GameMode, int PageNum_);

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

    Book book;
    std::vector<Position> V;

    int grid[9][9];
    int KnownGrid[9][9];
    int mode;
    int PageNum;

    void initBook();
    int  dfs(unsigned int i);
    int  checkDfs(unsigned int i);
    int  checkEmptyGrid(int i, int j);
    void generateKnownGrid();
    void generateGrid(int mode);
};

#endif // GENERATETHREAD_H
