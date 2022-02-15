#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // �������·��
    QDir::setCurrent(a.applicationDirPath());
    // ���ط����ļ�
    QTranslator translator;
    translator.load("widgets_zh_cn.qm");
    a.installTranslator(&translator);
    // �򿪴���
    MainWindow w;
    w.show();
    return a.exec();
}
