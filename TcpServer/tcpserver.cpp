
#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);

}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray data = file.readAll();
        QString strData = data.toStdString().c_str();

        strData.replace("\r\n"," ");
        qDebug()<<strData;

        QStringList strList = strData.split(" ");  //分割字符串

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUInt();
        qDebug()<<"IP:"<<m_strIP<<"\t"<<"端口:"<<m_usPort;
    }
    else{
        QMessageBox::critical(this,"open config","open error",QMessageBox::Ok);
    }
    file.close();
}


