#include "privatechat.h"
#include "ui_privatechat.h"
#include "tcpclient.h"
#include <string.h>
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
    this->setWindowTitle("私聊");
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString strName)
{
    m_strChatName = strName;
    m_strLoginName = TcpClient::getInstance().getLoginName();
}

/**
 * 更新消息
 * @brief PrivateChat::updateMsg
 * @param pdu
 */
void PrivateChat::updateMsg(const PDU *pdu)
{
    if(NULL == pdu)
        return;
    char caSendName[32] = {'\0'};
    memcpy(caSendName,pdu->caData,32);
    QString strMsg = QString("%1 says: %2").arg(caSendName).arg((char*)(pdu->caMsg));
    ui->textEdit_showMsg->append(strMsg);
}

/**
 * 发送消息
 * @brief PrivateChat::on_pushButton_sendMsg_clicked
 */
void PrivateChat::on_pushButton_sendMsg_clicked()
{
    QString strMsg = ui->lineEdit_inputMsg->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size());
        memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size());

        strcpy((char*)pdu->caMsg,strMsg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        ui->lineEdit_inputMsg->clear();
    }
    else
    {
        QMessageBox::information(this,"私聊","发送消息不为空!");
    }
}

