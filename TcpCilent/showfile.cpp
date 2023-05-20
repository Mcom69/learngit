#include "showfile.h"
#include <QCheckBox>
#include <QList>
#include "tcpclient.h"
#include "opewidget.h"

ShowFile::ShowFile(QWidget *parent)
    : QWidget{parent}
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancelSelectPB = new QPushButton("取消选择");
    m_pOKPB = new QPushButton("确定");
    m_CanclePB = new QPushButton("取消");

    m_pSA = new QScrollArea();
    m_pFriendW = new QWidget();
    m_pButtonGroup = new QButtonGroup(m_pFriendW);
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup->setExclusive(false);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_CanclePB);
    pDownHBL->addStretch();

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(okShare()));
    connect(m_CanclePB,SIGNAL(clicked(bool)),this,SLOT(cancelShare()));
}

ShowFile &ShowFile::getInstance()
{
    static ShowFile instance;
    return instance;
}

void ShowFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL == pFriendList)
        return;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();
    QAbstractButton *tmp = NULL;
    for(int i=0;i<preFriendList.size();i++)
    {
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }
    QCheckBox *pCB = NULL;
    for(int i=0;i<pFriendList->count();i++)
    {
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShowFile::cancelSelect()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.count();i++)
    {
        if(cbList[i]->isChecked())
            cbList[i]->setChecked(false);
    }
}

void ShowFile::selectAll()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.count();i++)
    {
        if(!cbList[i]->isChecked())
            cbList[i]->setChecked(true);
    }
}

void ShowFile::okShare()
{
    QString strName = TcpClient::getInstance().getLoginName();
    QString strCurPath = OpeWidget::getInstance().getBook()->getStrPwd();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();

    QString strPath = strCurPath+"/"+strShareFileName;

    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    int j = 0;
    for(int i=0;i<cbList.count();i++)
    {
        if(cbList[i]->isChecked())
        {
            num++;
        }
    }

    PDU *pdu = mkPDU(32*num+strPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);
    for(int i=0;i<cbList.count();i++)
    {
        if(cbList[i]->isChecked())
        {
            memcpy((char*)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),32);
            j++;
        }
    }
    memcpy((char*)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void ShowFile::cancelShare()
{
    this->hide();
}

