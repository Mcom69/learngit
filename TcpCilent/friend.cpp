#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include "privatechat.h"

#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    // 页面布局
    m_pOnline = new Online();
    m_pShowMsgTE = new QTextEdit();
    m_pFriendListWidget = new QListWidget();
    m_pInputMsgLE = new QLineEdit();

    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout();
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout();
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout();
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    QVBoxLayout *pMain = new QVBoxLayout();
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();
    setLayout(pMain);

    qb = new QStatusBar(this);
    qb->showMessage(QString("Welcom %1！").arg(TcpClient::getInstance().getLoginName()));
    this->layout()->addWidget(qb);

    //当点击显示用户信息按钮时，响应显示在线用户函数
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));

    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUser()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(refreshFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));
}

/**
 * 其他函数简介调用Online.cpp里的showUser来显示当前在线的所有用户
 * @brief Friend::showAllOnlineUser
 * @param pdu
 */
void Friend::showAllOnlineUser(PDU *pdu)
{
    if(NULL == pdu)
        return;
    m_pOnline->showUser(pdu);
}

/**
 * 更新好友列表
 * @brief Friend::updateFriendList
 * @param pdu
 */
void Friend::updateFriendList(PDU *pdu)
{
    m_pFriendListWidget->clear();
    if(NULL == pdu)
        return;
    uint uiSize = pdu->uiMsgLen/32;
    char caName[32] = {'\0'};
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

/**
 * 更新群聊消息
 * @brief Friend::updateGroupMsg
 * @param pdu
 */
void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg = QString("%1 says: %2").arg(pdu->caData).arg((char*)(pdu->caMsg));
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

/**
 * 显示所有在线用户
 * @brief Friend::showOnline
 */
void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
        m_pOnline->hide();
}

/**
 * 搜索用户
 * @brief Friend::searchUser
 */
void Friend::searchUser()
{
    m_strSearchName = QInputDialog::getText(this,"搜索好友","用户名：");
    if(!m_strSearchName.isEmpty())
    {
        qDebug()<<m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

/**
 * 刷新好友列表
 * @brief Friend::refreshFriend
 */
void Friend::refreshFriend()
{
    QString strName = TcpClient::getInstance().getLoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

/**
 * 删除好友
 * @brief Friend::delFriend
 */
void Friend::delFriend()
{
    // 获取删除好友名字
    if(NULL != m_pFriendListWidget->currentItem())
    {
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        qDebug()<<strFriendName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString selfName = TcpClient::getInstance().getLoginName();
        memcpy(pdu->caData,selfName.toStdString().c_str(),selfName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

/**
 * 私聊好友槽函数
 * @brief Friend::privateChat
 */
void Friend::privateChat()
{
    if(NULL != m_pFriendListWidget->currentItem())
    {
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if(PrivateChat::getInstance().isHidden())
            PrivateChat::getInstance().show();
    }
    else
    {
        QMessageBox::warning(this,"私聊","请选择私聊对象");
    }
}

/**
 * 群聊消息槽函数
 * @brief Friend::groupChat
 */
void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strName = TcpClient::getInstance().getLoginName();
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strncpy((char*)(pdu->caMsg),strMsg.toStdString().c_str(),strMsg.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        m_pInputMsgLE->clear();
    }
    else
        QMessageBox::warning(this,"群聊","发送消息不为空");
}

