#include "mainwindow.h"
#include <QApplication>

int lineno=0;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qRegisterMetaType<QVector<QPair<int,int>>>("QVector<QPair<int,int>>");
    qRegisterMetaType<QSharedPointer<TreeNode>>("QSharedPointer<TreeNode>");
    return a.exec();
}
