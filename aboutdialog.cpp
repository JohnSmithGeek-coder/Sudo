#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    :QDialog(parent)
{
    setFixedSize(600, 400);
    setWindowIcon(QIcon(":/new/prefix1/image/info.png"));
    setWindowTitle("  关于");
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

void AboutDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QImage icon(":/new/prefix1/image/mainicon.png");
    painter.drawImage(QRect(250, 50, 100, 100), icon);
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setFamily("Microsoft Yahei");
    font.setPixelSize(20);
    painter.setFont(font);
    painter.drawText(QRect(240, 150, 100, 50), Qt::AlignCenter, "数独");
    painter.drawText(QRect(210, 200, 160, 50), Qt::AlignCenter, "版本号 : beta");
    painter.drawText(QRect(190, 250, 200, 50), Qt::AlignCenter, "开发者 : 无聊滴小辰");
    pen.setColor(QColor(0, 131, 192, 255));
    painter.setPen(pen);
    font.setPixelSize(16);
    painter.setFont(font);
    painter.drawText(QRect(95, 320, 400, 50), Qt::AlignCenter, "如有任何问题或建议请发邮件至2986565765@qq.com");
}
