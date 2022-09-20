#include "generatethread.h"

#include <ctime>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <QDataStream>


QMutex fileMutex[15];

generateThread::generateThread(QObject *parent)
    :QThread(parent)
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = 0;
            KnownGrid[i][j] = 0;
        }
    }
}

void generateThread::initBook()
{
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            book.block[i][j] = book.col[i][j] = book.row[i][j] = 0;
        }
    }
    V.clear();
}

int  generateThread::dfs(unsigned int i)
{
    if (i == V.size()) {
        return 1;
    }
    int row, col;
    row = V[i].row_num + 1;
    col = V[i].col_num + 1;
    for (int k = 1; k < 10; k++) {
        if (book.isNotOne(row, col, k)) {
            book.setVal(row, col, k, 1);
            if (dfs(i + 1)) {
                KnownGrid[row - 1][col - 1] = k;
                return 1;
            } else {
                book.setVal(row, col, k, 0);
            }
        }
    }
    return 0;
}

int  generateThread::checkDfs(unsigned int i)
{
    if (i == V.size()) {
        return 1;
    }
    int row, col, block;
    row = V[i].row_num + 1;
    col = V[i].col_num + 1;
    block = V[i].block_num + 1;
    for (int k = 1; k < 10; k++) {
        if (book.isNotOne(row, col, k)) {
            book.setVal(row, col, k, 1);
            if (checkDfs(i + 1)) {
                book.setVal(row, col, k, 0);
                return 1;
            } else {
                book.setVal(row, col, k, 0);
            }
        }
    }
    return 0;
}

int  generateThread::checkEmptyGrid(int i, int j)
{
    int row, col;
    row = i + 1;
    col = j + 1;
    for (int k = 1; k < 10; k++) {
        if (k != grid[i][j]) {
            if (book.isNotOne(row, col, k)) {
                book.setVal(row, col, k, 1);
                if (checkDfs(0)) {
                    book.setVal(row, col, k, 0);
                    return 0;
                } else {
                    book.setVal(row, col, k, 0);
                }
            }
        }
    }
    return 1;
}

void generateThread::generateKnownGrid()
{

    initBook();

    srand((unsigned)time(NULL));

    int col, index;
    std::vector<int> number;
    for (int i = 1; i < 10; i++) {
        number.push_back(i);
    }
    for (int i = 0; i < 9; i++) {
        col = rand() % 9;
        index = rand() % number.size();
        KnownGrid[i][col] = number[index];
        number.erase(number.begin() + index);
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (KnownGrid[i][j] != 0) {
                book.setVal(i + 1, j + 1, KnownGrid[i][j], 1);
            } else {
                V.push_back(Position(i, j));
            }
        }
    }
    dfs(0);
}
void generateThread::generateGrid(int mode)
{
    srand((unsigned)time(NULL));
    int bit;

    initBook();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = KnownGrid[i][j];
            book.setVal(i + 1, j + 1, grid[i][j], 1);
        }
    }

    int percent;
    switch (mode) {
    case 1:
        percent = 500; break;
    case 2:
        percent = 7500; break;
    case 3:
        percent = 8500; break;
    case 4:
        percent = 9650; break;

    }
    int emptyCounter = 0;

    std::vector<int> row;
    for (int i = 0; i < 9; i++) {
        row.push_back(i);
    }
    int index, i;
    while (row.size() != 0) {
        index = rand() % row.size();
        i = row[index];
        for (int j = 0; j < 9; j++) {
            bit = rand() % 10000;
            if (bit < percent) {
                book.setVal(i + 1, j + 1, grid[i][j], 0);
                if (emptyCounter < 4 || checkEmptyGrid(i, j)) {
                    grid[i][j] = 0;
                    emptyCounter++;
                    V.push_back(Position(i, j));
                } else {
                    book.setVal(i + 1, j + 1, grid[i][j], 1);
                }
            }
        }
        row.erase(row.begin() + index);
    }
}

void generateThread::run()
{
    int index = mode - 1;
    int PN = PageNum;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = 0;
            KnownGrid[i][j] = 0;
        }
    }
    QString path[4];
    path[0] = "./data/easygriddata";
    path[1] = "./data/mediumgriddata";
    path[2] = "./data/hardgriddata";
    path[3] = "./data/expertgriddata";

    path[index].push_back(PN + 48);

    QFile file(path[index]);

    int n = std::pow(2, index) + PN - 1;

    fileMutex[n].lock();
    generateKnownGrid();
    generateGrid(mode);
    file.open(QIODevice::WriteOnly);
    file.seek(0);
    QDataStream out(&file);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            out << grid[i][j] << KnownGrid[i][j];
        }
    }

    file.close();

    fileMutex[n].unlock();
}

void generateThread::reciveGameMode(int GameMode, int PageNum_)
{
    mode = GameMode;
    PageNum = PageNum_;
}
