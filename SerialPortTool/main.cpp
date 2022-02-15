#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置相对路径
    QDir::setCurrent(a.applicationDirPath());
    // 加载翻译文件
    QTranslator translator;
    translator.load("widgets_zh_cn.qm");
    a.installTranslator(&translator);
    // 打开窗口
    MainWindow w;
    w.show();
    return a.exec();
}
