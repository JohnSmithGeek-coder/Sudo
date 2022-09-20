#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QIcon>
#include <QPainter>

class AboutDialog : public QDialog
{
public:
    AboutDialog(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // ABOUTDIALOG_H
