#pragma once
#pragma execution_character_set("utf-8")    // 声明文件使用的编码格式

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private:
    Ui::MainWindowClass ui;
    QTimer* timerUpdatePort;
    void UpdatePortList();

private slots:
    void on_pause_triggered(bool checked);
    void on_start_triggered(bool checked);
    void on_stop_triggered(bool checked);
    void on_clean_triggered();
    void on_btnSend_clicked();
    void slot_PortReceive();
    void slot_UpdatePort();
};
