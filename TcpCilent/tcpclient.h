
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStatusBar>
#include "protocol.h"
#include "opewidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget

{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    static TcpClient &getInstance();

    void loadConfig();
    QString getLoginName();
    QString getCurPath();
    QTcpSocket &getTcpSocket();
public slots:
    void showConnect();
    void recvMsg();         //接收TcpSocket数据

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_reg_clicked();

    void on_pushButton_logout_clicked();

private:
    Ui::TcpClient *ui;

    QStatusBar *qb;

    QString m_strIP;
    quint16 m_usPort;

    //连接服务器，与服务器进行交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
    QString m_strCurPath;

    QFile m_file;

};

#endif // TCPCLIENT_H
