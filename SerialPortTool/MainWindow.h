#pragma once
#pragma execution_character_set("utf-8")    // �����ļ�ʹ�õı����ʽ

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QCloseEvent>

#define SETTING_FILENAME        "setting.ini"  // �����ļ���
#define PORT_UPDATE_INTERVAL    1000           // ���ڸ��¼��

typedef struct
{
    int BaudRateIndex;      // ����������������
    int DataBitsIndex;      // ����λ����������
    int ParityIndex;        // У��λ����������
    int StopBitsIndex;      // ֹͣλ����������
    int FlowControlIndex;   // ��������������
    bool RxHex;             // ��������ΪHEX
    bool RxWordWrap;        // �����Զ�����    
    bool RxShowTime;        // ��ʾʱ��
    bool TxHex;             // ��������ΪHEX
    bool ShowTx;          // ��ʾ����
    bool TxCrc;             // �������CRCУ��
    int TxCrcModel;         // CRC����ģ������������
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
    void closeEvent(QCloseEvent* event); // ���ش��ڹر��¼�

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
