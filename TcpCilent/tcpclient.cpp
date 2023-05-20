
#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "privatechat.h"
#include <QByteArray>
#include <QMessageBox>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    qb = new QStatusBar(this);
    qb->showMessage("您似乎没有与服务器连接成功哟~");
    this->layout()->addWidget(qb);

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&TcpClient::recvMsg);

    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);

}

TcpClient::~TcpClient()
{
    delete ui;
}

/**
 * 加载配置文件(IP,Port)
 * @brief TcpClient::loadConfig
 */
void TcpClient::loadConfig()
{
    QFile file(":/client.config");
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

QString TcpClient::getLoginName()
{
    return m_strLoginName;
}

QString TcpClient::getCurPath()
{
    return m_strCurPath;
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

/**
 * 连接服务器成功后提示
 * @brief TcpClient::showConnect
 */
void TcpClient::showConnect()
{
    qb->clearMessage();
    qb->showMessage("连接服务器成功，欢迎使用");
}

/**
 * 处理接收消息
 * @brief TcpClient::recvMsg
 */
void TcpClient::recvMsg()
{
    bool flag = OpeWidget::getInstance().getBook()->getDownloadStatus();
    if(!flag)
    {
        qDebug()<<this->m_tcpSocket.bytesAvailable();
        uint uiPDULen = 0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));  //获取消息总大小
        uint uiMsgLen = uiPDULen-sizeof(PDU);             //实际消息长度
        PDU *pdu = mkPDU(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));  //接收剩余PDU长度
        qDebug()<<pdu->uiMsgType<<(char*)pdu->caMsg;

        // 判断Msg类型
        switch (pdu->uiMsgType) {
            case ENUM_MSG_TYPE_REGIST_RESPOND:
            {
                if(0 == strcmp(pdu->caData,REGIST_OK))
                    QMessageBox::information(this,"注册",REGIST_OK);
                else if(0 == strcmp(pdu->caData,LOGIN_FAILED))
                    QMessageBox::warning(this,"注册",LOGIN_FAILED);
                break;
            }
            case ENUM_MSG_TYPE_LOGIN_RESPOND:
            {
                if(0 == strcmp(pdu->caData,LOGIN_OK))
                {
                    m_strCurPath = QString("./%1").arg(m_strLoginName);
                    qDebug()<<"m_strCurPath:"<<m_strCurPath;
                    QMessageBox::information(this,"登录",LOGIN_OK);
                    OpeWidget::getInstance().show();                //登录后跳转到主界面
                    this->hide();
                }
                else if(0 == strcmp(pdu->caData,LOGIN_FAILED))
                    QMessageBox::warning(this,"登录",LOGIN_FAILED);
                break;
            }
            case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
            {
                OpeWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
                break;
            }
            case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
            {

                if(0 == strcmp(SEARCH_USER_NO,pdu->caData))
                    QMessageBox::information(this,"搜索",QString("%1 not exist")
                                                               .arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                else if(0 == strcmp(SEARCH_USER_ONLINE,pdu->caData))
                    QMessageBox::information(this,"搜索",QString("%1 online")
                                                               .arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                else if(0 == strcmp(SEARCH_USER_OFFLINE,pdu->caData))
                    QMessageBox::information(this,"搜索",QString("%1 offline")
                                                               .arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
            {
                char caName[32] = {'\0'};
                strncpy(caName,pdu->caData+32,32);
                int ret = QMessageBox::information(this,"添加好友",QString("%1 wants to add you as friend?").arg(pdu->caData+32)
                                        ,QMessageBox::Yes,QMessageBox::No);
                PDU *respdu = mkPDU(0);
                memcpy(respdu->caData,pdu->caData,32);

                if(QMessageBox::Yes == ret)
                {
                    memcpy(respdu->caData+32,pdu->caData+32,32);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_AGREE;
                }
                else
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_REFUSE;
                }
                m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
            {
                QMessageBox::information(this,"添加好友",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
            {
                OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
            {
                char caName[32] = {'\0'};
                memcpy(caName,pdu->caData,32);
                QMessageBox::information(this,"删除好友",QString("%1与您解除了好友关系").arg(caName));
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
            {
                QMessageBox::information(this,"删除好友","删除好友成功");
                break;
            }
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
            {
                if(PrivateChat::getInstance().isHidden())
                {
                    PrivateChat::getInstance().show();
                }
                char caSendName[32] = {'\0'};
                memcpy(caSendName,pdu->caData,32);
                QString strSendName = caSendName;
                PrivateChat::getInstance().setChatName(strSendName);
                PrivateChat::getInstance().updateMsg(pdu);
                break;
            }
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
            {
                OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
                break;
            }
            case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
            {
                QMessageBox::information(this,"创建文件夹",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
            {
                OpeWidget::getInstance().getBook()->updateFileList(pdu);
                break;
            }
            case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
            {
                QMessageBox::information(this,"删除文件夹",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
            {
                QMessageBox::information(this,"重命名文件",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
            {
                QMessageBox::information(this,"上传文件",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
            {
                QMessageBox::information(this,"删除文件",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
            {
                qDebug()<<pdu->caData;
                char caFileName[32] = {'\0'};
                sscanf(pdu->caData,"%s %lld",&caFileName,&OpeWidget::getInstance().getBook()->m_iTotal);
                if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal>0)
                {
                    OpeWidget::getInstance().getBook()->setDownloadStatus(true);
                    m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
                    if(!m_file.open(QIODevice::WriteOnly))
                    {
                        QMessageBox::warning(this,"下载","获取保存路径失败");
                    }
                }
                break;
            }
            case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
            {
                QMessageBox::information(this,"分享",pdu->caData);
                break;

            }
            case ENUM_MSG_TYPE_SHARE_FILE_NOTE:
            {
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,(char*)pdu->caMsg,pdu->uiMsgLen);
                qDebug()<<"tcpclient分享路径:"<<pPath;
                char *pos = strrchr(pPath,'/');
                qDebug()<<"pos:"<<pos;
                qDebug()<<"pdu->caMsg:"<<*pdu->caMsg;
                if(NULL != pos)
                {
                    pos++;
                    QString strNote = QString("%1 share file ->%2 \n Do you want receive").arg(pdu->caData).arg(pos);
                    int ret = QMessageBox::question(this,"分享",strNote);
                    if(QMessageBox::Yes == ret)
                    {
                        PDU *respdu = mkPDU(pdu->uiMsgLen);
                        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                        memcpy(respdu->caMsg,(char*)(pdu->caMsg),pdu->uiMsgLen);
                        QString strName = TcpClient::getInstance().getLoginName();
                        strcpy(respdu->caData,strName.toStdString().c_str());
                        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                    }
                }
                break;
            }
            case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
            {
                QMessageBox::information(this,"移动文件",pdu->caData);
                break;
            }
            default:
                break;
            }
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QByteArray buffer = m_tcpSocket.readAll();
        m_file.write(buffer);

        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if(pBook->m_iTotal == pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            QMessageBox::information(this,"下载","下载成功");
        }
        else if(pBook->m_iTotal < pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            QMessageBox::critical(this,"下载","保存失败");
        }
    }
}

void TcpClient::on_pushButton_login_clicked()
{
    QString strName = ui->lineEdit_username->text();
    QString strPwd = ui->lineEdit_pwd->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        m_strLoginName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败，用户名或密码为空",QMessageBox::Ok);
    }
}

void TcpClient::on_pushButton_reg_clicked()
{
    QString strName = ui->lineEdit_username->text();
    QString strPwd = ui->lineEdit_pwd->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败",QMessageBox::Ok);
    }
}

void TcpClient::on_pushButton_logout_clicked()
{

}
