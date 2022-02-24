#pragma once
#pragma execution_character_set("utf-8")    // 声明文件使用的编码格式

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QCloseEvent>

#define SETTING_FILENAME        "setting.ini"  // 配置文件名
#define PORT_UPDATE_INTERVAL    1000           // 串口更新间隔

typedef struct
{
    int BaudRateIndex;      // 波特率下拉框索引
    int DataBitsIndex;      // 数据位下拉框索引
    int ParityIndex;        // 校验位下拉框索引
    int StopBitsIndex;      // 停止位下拉框索引
    int FlowControlIndex;   // 流控下拉框索引
    bool RxHex;             // 接收设置为HEX
    bool RxWordWrap;        // 接收自动换行    
    bool RxShowTime;        // 显示时间
    bool TxHex;             // 发送设置为HEX
    bool ShowTx;          // 显示发送
    bool TxCrc;             // 发送添加CRC校验
    int TxCrcModel;         // CRC计算模型下拉框索引
} SettingInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private:
    Ui::MainWindowClass ui;
    QTimer* timerUpdatePort;
    QTimer* timerRepeat;
    void UpdatePortList();
    void AddHistory(QString text);
    void SendDatas(QString text);

protected:
    void closeEvent(QCloseEvent* event); // 重载窗口关闭事件

private slots:
    void on_pause_triggered(bool checked);
    void on_start_triggered(bool checked);
    void on_stop_triggered(bool checked);
    void on_clean_triggered();
    void on_btnSend_clicked();
    void slot_PortReceive();
    void slot_UpdatePort();
    void slot_timerRepeat();
    void on_cmbSendHistory_activated(const QString& arg1);
    void on_cbxRepeat_clicked(bool state);
    void on_radioTxAscii_clicked(bool state);
    void on_radioTxHex_clicked(bool state);
};
