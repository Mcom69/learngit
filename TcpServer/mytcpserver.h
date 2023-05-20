
#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);

    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescriptor);

    void resend(const char* pername,PDU* pdu);       //转发

public slots:
    void deleteSocket(MyTcpSocket* mysocket);        //删除Socket

private:
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
