#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent)
    : QTcpServer{parent}
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
}

/**
 * 转发Socket
 * @brief MyTcpServer::resend
 * @param pername
 * @param pdu
 */
void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if(NULL == pername || NULL == pdu)
        return;
    QString strName = pername;
    for(int i = 0;i<m_tcpSocketList.size();i++)
    {
        if(strName == m_tcpSocketList.at(i)->getName())
        {
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

/**
 * 删除已经下线的Socket
 * @brief MyTcpServer::deleteSocket
 * @param mysocket 已经下线的Socket
 */
void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::iterator it = m_tcpSocketList.begin();
    for(;it!=m_tcpSocketList.end();it++){
        if(mysocket == *it)
        {
            m_tcpSocketList.erase(it);
            (*it)->deleteLater();
            *it = NULL;
            break;
        }
    }
    for(int i = 0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<<m_tcpSocketList.at(i)->getName();
    }
}

