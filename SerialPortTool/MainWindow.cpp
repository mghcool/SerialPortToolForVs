#include "MainWindow.h"
#include "CrcCheck.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QDomDocument>
#include <QDateTime>
#include <QStandardItemModel>
#include "VersionUpdate.h"


QSerialPort serial;             //串口对象
QList<QSerialPortInfo> portList;//串口列表
QLabel* lblStatus;             //状态栏状态标签
QLabel* lblRxByte;             //状态栏接收字节标签
QLabel* lblTxByte;             //状态栏发送字节标签
CrcCheck crcObj;
QSettings* qSettings;
SettingInfo settingInfo;
QFile apacheFile;
QDomDocument apacheDoc;
QDomElement apacheDocRoot;

int RxdCount;       // 接收字节计数
int TxdCount;       // 发送字节计数

// 重载窗口关闭事件
void MainWindow::closeEvent(QCloseEvent* e)
{

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    // 设置窗口标题版本
    this->setWindowTitle(this->windowTitle() + " v" + VersionUpdate::GetVersion());
    // 获取标准用户配置文件夹
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    // 读取配置
    QString settingPath = configDir.absoluteFilePath(SETTING_FILENAME);    // 配置文件全路径
    QFileInfo info(settingPath);
    qSettings = new QSettings(settingPath, QSettings::IniFormat);  // 载入配置文件
    qSettings->setIniCodec("UTF-8");                          // 设置配置文件编码
    if (info.exists())
    {
        // 如果配置文件存在，就读取配置文件
        // 串口设置
        settingInfo.BaudRateIndex = qSettings->value("SerialPort/BaudRate").toInt();
        settingInfo.DataBitsIndex = qSettings->value("SerialPort/DataBits").toInt();
        settingInfo.ParityIndex = qSettings->value("SerialPort/Parity").toInt();
        settingInfo.StopBitsIndex = qSettings->value("SerialPort/StopBits").toInt();
        settingInfo.FlowControlIndex = qSettings->value("SerialPort/FlowControl").toInt();
        // 接收设置
        settingInfo.RxHex = qSettings->value("RxdSetting/Hex").toBool();
        settingInfo.RxWordWrap = qSettings->value("RxdSetting/WordWrap").toBool();        
        settingInfo.RxShowTime = qSettings->value("RxdSetting/ShowTime").toBool();
        // 发送设置
        settingInfo.TxHex = qSettings->value("TxdSetting/Hex").toBool();
        settingInfo.ShowTx = qSettings->value("RxdSetting/ShowTx").toBool();
        settingInfo.TxCrc = qSettings->value("TxdSetting/Crc").toBool();
        settingInfo.TxCrcModel = qSettings->value("TxdSetting/CrcModel").toInt(); 
    }
    else// 如果配置文件不存在，就初始化配置文件
    {
        // 串口设置
        qSettings->beginGroup("SerialPort");
        qSettings->setValue("BaudRate", 3);       // 波特率
        qSettings->setValue("DataBits", 3);       // 数据位
        qSettings->setValue("Parity", 0);         // 校验位
        qSettings->setValue("StopBits", 0);       // 停止位
        qSettings->setValue("FlowControl", 0);    // 流控
        qSettings->endGroup();
        // 接收设置
        qSettings->beginGroup("RxdSetting");
        qSettings->setValue("Hex", false);        // 是否16进制
        qSettings->setValue("WordWrap", false);   // 自动换行
        qSettings->setValue("ShowTime", false);   // 显示时间
        qSettings->endGroup();
        // 发送设置
        qSettings->beginGroup("TxdSetting");
        qSettings->setValue("Hex", false);        // 是否16进制
        qSettings->setValue("ShowTx", false);     // 显示发送
        qSettings->setValue("Crc", false);        // 启用CRC校验
        qSettings->setValue("CrcModel", 0);       // CRC计算模型
        qSettings->endGroup();
        // 发送历史
        qSettings->beginGroup("History");
        qSettings->setValue("Line0", "");
        qSettings->setValue("Line1", "");
        qSettings->setValue("Line2", "");
        qSettings->setValue("Line4", "");
        qSettings->setValue("Line5", "");
        qSettings->setValue("Line6", "");
        qSettings->setValue("Line7", "");
        qSettings->setValue("Line8", "");
        qSettings->setValue("Line9", "");
        qSettings->endGroup();
        // 设置配置变量
        settingInfo = { 3,3 };
    }
    //创建状态标签
    lblStatus = new QLabel();
    ui.statusbar->addWidget(lblStatus);
    lblStatus->setMinimumWidth(500);
    lblStatus->setMaximumWidth(500);
    lblStatus->setStyleSheet("color:red;");
    lblStatus->setText(" 未打开串口");
    //创建接收和发送字节数标签
    lblRxByte = new QLabel();
    ui.statusbar->addWidget(lblRxByte);
    lblRxByte->setMinimumWidth(100);
    lblRxByte->setText("Rx: 0 Bytes");
    lblTxByte = new QLabel();
    ui.statusbar->addWidget(lblTxByte);
    lblTxByte->setMinimumWidth(100);
    lblTxByte->setText("Tx: 0 Bytes");
    //添加crc选项
    for (int i = 0; i < crcObj.modelListSize; i++)
    {
        ui.cmbCRCType->addItem(crcObj.modelList[i].name);
    }
    //设置默认选项
    ui.comBaudRate->setCurrentIndex(settingInfo.BaudRateIndex);         // 波特率
    ui.comDataBits->setCurrentIndex(settingInfo.DataBitsIndex);         // 数据位
    ui.comParity->setCurrentIndex(settingInfo.ParityIndex);             // 校验位
    ui.comStopBits->setCurrentIndex(settingInfo.StopBitsIndex);         // 停止位
    ui.comFlowControl->setCurrentIndex(settingInfo.FlowControlIndex);   // 流控
    ui.radioRxHex->setChecked(settingInfo.RxHex);                       // 是否16进制
    ui.cbxWordWrap->setChecked(settingInfo.RxWordWrap);                 // 自动换行
    ui.cbxShowTime->setChecked(settingInfo.RxShowTime);                 // 显示时间
    ui.radioTxHex->setChecked(settingInfo.TxHex);                       // 是否16进制
    ui.cbxShowSend->setChecked(settingInfo.ShowTx);                     // 显示发送
    ui.cbxCRC->setChecked(settingInfo.TxCrc);                           // 启用CRC校验
    ui.cmbCRCType->setCurrentIndex(settingInfo.TxCrcModel);             // CRC计算模型
    ui.spinBoxRepeat->setValue(1000);                                   // 重复发送间隔
    ui.cbxCRC->setEnabled(settingInfo.TxHex);                           // 是否启用CRC
    //更新串口列表
    UpdatePortList();
    // 读取缓存
    apacheFile.setFileName(configDir.absoluteFilePath("cache.xml"));
    if (apacheFile.exists())
    {
        apacheFile.open(QFile::ReadOnly);
        apacheDoc.setContent(&apacheFile);
        apacheFile.close();
        apacheDocRoot = apacheDoc.documentElement();
        QDomNode historyNode = apacheDocRoot.lastChild(); //获得第最后一个子节点History

        if (!historyNode.isNull())
        {
            QDomNodeList list = historyNode.childNodes();
            for (int i = 0; i < list.count(); i++)
            {
                QString val = list.at(i).attributes().item(0).nodeValue();
                ui.cmbSendHistory->insertItem(i, val);
            }
        }
    }
    else
    {
        apacheFile.open(QFile::WriteOnly | QFile::Truncate);
        // 创建xml头部格式
        QDomProcessingInstruction instruction;
        instruction = apacheDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        apacheDoc.appendChild(instruction);
        // 创建root节点
        apacheDocRoot = apacheDoc.createElement("root");
        apacheDocRoot.appendChild(apacheDoc.createElement("CMD"));
        apacheDocRoot.appendChild(apacheDoc.createElement("History"));
        apacheDoc.appendChild(apacheDocRoot);
        // 保存文件
        QTextStream stream(&apacheFile);
        apacheDoc.save(stream, 4);  //4 缩进字符
        apacheFile.close();
    }
    //定义串口更新定时器
    timerUpdatePort = new QTimer(this);
    timerUpdatePort->start(PORT_UPDATE_INTERVAL);
    //连接信号槽
    connect(&serial, SIGNAL(readyRead()), this, SLOT(slot_PortReceive()));
    connect(timerUpdatePort, SIGNAL(timeout()), this, SLOT(slot_UpdatePort()));
    // 重复发送定时器
    timerRepeat = new QTimer(this);
    connect(timerRepeat, SIGNAL(timeout()), this, SLOT(slot_timerRepeat()));
}

//窗体析构
MainWindow::~MainWindow()
{
    // 保存串口设置
    qSettings->setValue("SerialPort/BaudRate", ui.comBaudRate->currentIndex());         // 波特率
    qSettings->setValue("SerialPort/DataBits", ui.comDataBits->currentIndex());         // 数据位
    qSettings->setValue("SerialPort/Parity", ui.comParity->currentIndex());             // 校验位
    qSettings->setValue("SerialPort/StopBits", ui.comStopBits->currentIndex());         // 停止位
    qSettings->setValue("SerialPort/FlowControl", ui.comFlowControl->currentIndex());   // 流控
    // 保存接受设置
    qSettings->setValue("RxdSetting/Hex", ui.radioRxHex->isChecked());                  // 是否16进制
    qSettings->setValue("RxdSetting/WordWrap", ui.cbxWordWrap->isChecked());            // 自动换行
    qSettings->setValue("RxdSetting/ShowTime", ui.cbxShowTime->isChecked());            // 显示时间
    // 保存发送设置
    qSettings->setValue("TxdSetting/Hex", ui.radioTxHex->isChecked());                  // 是否16进制
    qSettings->setValue("RxdSetting/ShowTx", ui.cbxShowSend->isChecked());              // 显示发送
    qSettings->setValue("TxdSetting/Crc", ui.cbxCRC->isChecked());                      // 启用CRC校验
    qSettings->setValue("TxdSetting/CrcModel", ui.cmbCRCType->currentIndex());          // CRC计算模型

    // 释放资源
    serial.close();
    timerUpdatePort->stop();
    delete lblStatus;
    delete lblRxByte;
    delete lblTxByte;
    delete timerUpdatePort;
    delete qSettings;

    // 保存缓存文件
    apacheFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream stream(&apacheFile);
    apacheDoc.save(stream, 4);  //4 缩进字符
    apacheFile.close();
}

//更新串口列表
void MainWindow::UpdatePortList()
{
    QList<QSerialPortInfo> newList = QSerialPortInfo::availablePorts();
    if (portList.size() != newList.size())
    {
        portList = newList;
        ui.cmbSerialPort->clear();
        QStandardItemModel* model = new QStandardItemModel();
        QStandardItem* item = new QStandardItem();//设置下拉框选择提示
        foreach(const QSerialPortInfo & info, portList)
        {
            QString portInfo = info.portName() + " " + info.description();
            item = new QStandardItem(portInfo);     // 下拉框显示文本
            item->setToolTip(portInfo);             // 设置提示类容
            model->appendRow(item);                 // 将item添加到model中
        }
        ui.cmbSerialPort->setModel(model);          // 下拉框设置model
    }
}

//串口更新定时器触发
void MainWindow::slot_UpdatePort()
{
    UpdatePortList();
}

//打开串口
void MainWindow::on_start_triggered(bool checked)
{
    ui.start->setChecked(true);
    ui.pause->setChecked(false);
    ui.stop->setChecked(false);

    if (portList.isEmpty())
    {
        ui.start->setChecked(false);
        ui.pause->setChecked(false);
        ui.stop->setChecked(true);
        QMessageBox::warning(this, "错误", "没有相应的串口！", QMessageBox::Ok);
        return;
    }

    if (checked)
    {
        //设置串口名
        serial.setPortName(portList[ui.cmbSerialPort->currentIndex()].portName());
        //设置波特率
        serial.setBaudRate(ui.comBaudRate->currentText().toInt());
        //设置数据位数
        serial.setDataBits((QSerialPort::DataBits)ui.comDataBits->currentText().toInt());
        //设置奇偶校验
        switch (ui.comParity->currentIndex())
        {
        case 0: serial.setParity(QSerialPort::NoParity);   break;
        case 1: serial.setParity(QSerialPort::EvenParity); break;
        case 2: serial.setParity(QSerialPort::OddParity);  break;
        case 3: serial.setParity(QSerialPort::SpaceParity); break;
        case 4: serial.setParity(QSerialPort::MarkParity); break;
        }
        //设置停止位
        switch (ui.comStopBits->currentIndex())
        {
        case 0: serial.setStopBits(QSerialPort::OneStop);       break;
        case 1: serial.setStopBits(QSerialPort::OneAndHalfStop); break;
        case 2: serial.setStopBits(QSerialPort::TwoStop);       break;
        }
        //设置流控制
        switch (ui.comFlowControl->currentIndex())
        {
        case 0: serial.setFlowControl(QSerialPort::NoFlowControl);  break;
        case 1: serial.setFlowControl(QSerialPort::HardwareControl); break;
        case 2: serial.setFlowControl(QSerialPort::SoftwareControl); break;
        }
        //打开串口
        if (serial.open(QIODevice::ReadWrite))
        {
            QString text = " 串口打开: " 
                + ui.cmbSerialPort->currentText() 
                + " " 
                + ui.comBaudRate->currentText();
            lblStatus->setStyleSheet("color:green;");
            lblStatus->setText(text);
        }
        else
        {
            switch (serial.error()) {
                case QSerialPort::PermissionError:
                    QMessageBox::warning(this, "错误", "串口被占用！", QMessageBox::Ok);
                    break;
                case QSerialPort::OpenError:
                    QMessageBox::warning(this, "错误", "无法打开串口", QMessageBox::Ok);
                    break;
                default:
                    QMessageBox::warning(this, "错误", "打开串口错误!", QMessageBox::Ok);
            }
            serial.clearError();
            ui.start->setChecked(false);
            ui.pause->setChecked(false);
            ui.stop->setChecked(true);
        }
    }
}

//暂停串口
void MainWindow::on_pause_triggered(bool checked)
{
    ui.start->setChecked(false);
    ui.pause->setChecked(true);
    ui.stop->setChecked(false);
    if (checked)
    {
        qDebug() << "pause";
    }
}

//停止串口
void MainWindow::on_stop_triggered(bool checked)
{
    ui.start->setChecked(false);
    ui.pause->setChecked(false);
    ui.stop->setChecked(true);
    if (checked)
    {
        serial.close();
        lblStatus->setStyleSheet("color:red;");
        lblStatus->setText(tr(" 未打开串口"));
    }
}

//添加一条历史记录
void MainWindow::AddHistory(QString text)
{
    if (ui.cmbSendHistory->itemText(0) != text)
    {
        ui.cmbSendHistory->insertItem(0, text);
        ui.cmbSendHistory->setCurrentIndex(0);

        int historyCount = ui.cmbSendHistory->count();
        QDomNode historyNode = apacheDocRoot.lastChild(); //获得第最后一个子节点History
        QDomElement val = apacheDoc.createElement("text");
        val.setAttribute("val", text);
        QDomNode first = historyNode.firstChild();
        historyNode.insertBefore(val, first);
    }
}

// 选中历史记录
void MainWindow::on_cmbSendHistory_activated(const QString& arg1)
{
    ui.textEditTx->setText(arg1);
}

// 发送数据
void MainWindow::SendDatas(QString text)
{
    // 换行符转换
    text = text.replace("\n", "\r\n");
    //转换数据
    QByteArray sendData;
    if (ui.radioTxAscii->isChecked()) sendData = text.toUtf8();   //按Ascii发送
    else sendData = QByteArray::fromHex(text.toLatin1().data()); //按Hex发送

    //CRC校验
    if (ui.cbxCRC->isChecked() && ui.radioTxHex->isChecked())
    {
        quint32 crcVal = crcObj.computeCrcVal(sendData, ui.cmbCRCType->currentIndex());
        sendData.append(crcVal & 0x00FF);
        sendData.append(crcVal >> 8);
    }

    // 显示发送
    if (ui.cbxShowSend->isChecked())
    {
        if (ui.cbxShowTime->isChecked())
        {
            QDateTime nowDataTime = QDateTime::currentDateTime();
            QString timeStr = nowDataTime.toString("[hh:mm:ss.zzz] ");
            ui.textShowRx->append(timeStr + text);
        }
        else
        {
            ui.textShowRx->append(text);
        }
    }
    // 写入发送缓存区
    qDebug() << sendData;
    serial.write(sendData);
    TxdCount += sendData.count();
    lblTxByte->setText("Tx: " + QString::number(TxdCount) + " Bytes");

    //添加到历史发送
    this->AddHistory(text);
}

//发送一条信息
void MainWindow::on_btnSend_clicked()
{
    //如果串口没有打开，那就打开串口
    if (!ui.start->isChecked())
    {
        on_start_triggered(true);
        return;
    }
    //获取输入窗口sendData的数据
    QString inputText = ui.textEditTx->toPlainText();
    SendDatas(inputText);
}


//读取接收到的数据
void MainWindow::slot_PortReceive()
{
    if (ui.pause->isChecked()) return;  //暂停时不处理接收
    QByteArray buf;
    buf = serial.readAll();
    if (!buf.isEmpty())
    {
        QString str = QString::fromLocal8Bit(buf);  //支持中文

        if (ui.radioRxHex->isChecked())
            str = buf.toHex(' ').toUpper(); //转16进制显示，带空格大写

        if (ui.cbxWordWrap->isChecked())//换行显示
        {
            if (ui.cbxShowTime->isChecked())
            {
                QDateTime nowDataTime = QDateTime::currentDateTime();
                QString timeStr = nowDataTime.toString("[hh:mm:ss.zzz] ");
                str = timeStr + str;
            }
            
            ui.textShowRx->append(str);
        }
        else                            //不换行显示
        {
            QTextCursor tc = ui.textShowRx->textCursor();
            tc.movePosition(QTextCursor::End);
            tc.insertText(str + " ");
        }
    }
    
    RxdCount += buf.count();
    lblRxByte->setText("Rx: " + QString::number(RxdCount) + " Bytes");

    buf.clear();
}

//清理接收区
void MainWindow::on_clean_triggered()
{
    ui.textShowRx->clear();
    RxdCount = 0;
    TxdCount  = 0;
    lblTxByte->setText("Tx: 0 Bytes");
    lblRxByte->setText("Rx: 0 Bytes");
}

// 重复发送复选框
void MainWindow::on_cbxRepeat_clicked(bool state) 
{
    if (state)
    {
        timerRepeat->start(ui.spinBoxRepeat->value());
    }
    else
    {
        timerRepeat->stop();
    }
}

// 重复发送定时器槽
void MainWindow::slot_timerRepeat()
{
    if (ui.start->isChecked())
    {
        qDebug() << "发送";
        //获取输入窗口sendData的数据
        QString inputText = ui.textEditTx->toPlainText();
        SendDatas(inputText);
    }
}

void MainWindow::on_radioTxAscii_clicked(bool state)
{
    ui.cbxCRC->setEnabled(!state);
}

void MainWindow::on_radioTxHex_clicked(bool state)
{
    ui.cbxCRC->setEnabled(state);
}