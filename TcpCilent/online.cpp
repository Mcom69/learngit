#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

/**
 * 将从服务器返回的在线用户信息添加到listWidget里面
 * @brief Online::showUser
 * @param pdu
 */
void Online::showUser(PDU *pdu)
{
    ui->listWidget_online->clear();
    if(NULL == pdu)
        return;
    uint uiSize = pdu->uiMsgLen/32;
    char caTemp[32];
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caTemp,(char*)(pdu->caMsg)+(i*32),32);
        ui->listWidget_online->addItem(caTemp);
    }
}

void Online::on_pushButton_addfriend_clicked()
{
    QListWidgetItem* pItem = ui->listWidget_online->currentItem();
    if(NULL != pItem)
    {
        QString strPerUserName = pItem->text();
        QString strLoginName = TcpClient::getInstance().getLoginName();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData,strPerUserName.toStdString().c_str(),strPerUserName.size());
        memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
        return;
}

