#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget *desktop=QApplication::desktop();
    //获取桌面宽度
    int win_w=desktop->width();
    //获取桌面高度
    int win_h=desktop->height();
    MainWindow w;
    w.move((win_w-w.width())/2,(win_h-w.height())/2-35);//居中显示
    w.show();
    return a.exec();
}
