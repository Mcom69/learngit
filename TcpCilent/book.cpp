#include "book.h"
#include "tcpclient.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "opewidget.h"
#include "showfile.h"

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    strPwd = TcpClient::getInstance().getCurPath();
    qDebug()<<"构造函数pwd："<<strPwd;
    strName = TcpClient::getInstance().getLoginName();
    m_bDownload = false;
    m_pTimer = new QTimer;

    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件夹");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownloadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pSharePB = new QPushButton("分享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pSharePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);


    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushFilePB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showDirFile(QModelIndex)));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnDirFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(renameFile()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));
    connect(m_pDelFilePB,SIGNAL(clicked(bool)),this,SLOT(delRegFile()));
    connect(m_pDownloadPB,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));
    connect(m_pSharePB,SIGNAL(clicked(bool)),this,SLOT(shareFile()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),this,SLOT(moveFile()));
    connect(m_pSelectDirPB,SIGNAL(clicked(bool)),this,SLOT(selectDestDir()));
}
/**
 * 更新listWidge内容
 * @brief Book::updateFileList
 * @param pdu
 */
void Book::updateFileList(const PDU *pdu)
{
    if(NULL == pdu)
        return;

    QListWidgetItem *pItemTmp = NULL;
    int row = m_pBookListW->count();
    while(m_pBookListW->count()>0)
    {
        pItemTmp = m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row -=1;
    }
    FileInfo *pFileInfo = NULL;
    int count = pdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<count;i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0 == pFileInfo->iFileType)
            pItem->setIcon(QIcon(QPixmap(":/DIR.jpg")));
        else if(1 == pFileInfo->iFileType)
            pItem->setIcon(QIcon(QPixmap(":/FILE.jpg")));
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

/**
 * 设置下载状态
 * @brief Book::setDownloadStatus
 * @param status
 */
void Book::setDownloadStatus(bool status)
{
    m_bDownload = status;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_shareFileName;
}

QString Book::getStrPwd()
{
    return strPwd;
}

/**
 * 创建文件夹操作
 * @brief Book::createDir
 */
void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
            QMessageBox::warning(this,"新建文件夹","文件夹名超出最大长度");
        else
        {
            QString strName = TcpClient::getInstance().getLoginName();
            qDebug()<<strPwd;
            QString strCurPath = TcpClient::getInstance().getCurPath();
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy(pdu->caMsg,strPwd.toStdString().c_str(),strPwd.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","文件夹名不为空!");
    }
}

/**
 * 刷新显示文件夹
 * @brief Book::flushFile
 */
void Book::flushFile()
{
    strPwd = TcpClient::getInstance().getCurPath();
    PDU *pdu = mkPDU(strPwd.size()+1);
    qDebug()<<"flush strPwd:"<<strPwd;
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    strncpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

/**
 * 双击按钮进入下一层目录
 * @brief Book::showDirFile
 * @param item
 */
void Book::showDirFile(QModelIndex item)
{
    if(item.data().toString()== "..")
        return;
    else
    {
        qDebug()<<item.data().toString();
        strPwd += "/"+item.data().toString();
        qDebug()<<strPwd;
        PDU *pdu = mkPDU(strPwd.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
        strncpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

/**
 * 删除文件夹
 * @brief Book::delDir
 */
void Book::delDir()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
        QMessageBox::warning(this,"删除","请选择要删除的文件夹");
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strPwd.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

/**
 * 返回按钮
 * @brief Book::returnDirFile
 */
void Book::returnDirFile()
{
    QString temPwd = strPwd;
    strPwd += "/..";
    qDebug()<<strPwd;
    PDU *pdu = mkPDU(strPwd.size()+1);
    QString strPath = QString("./%1/..").arg(strName);
    if(strPwd == strPath)
    {
        free(pdu);
        pdu = NULL;
        QMessageBox::information(this,"返回","当前目录不能再返回");
        strPwd = QString("./%1").arg(strName);
        return;
    }
    else
    {
        QStringList strList = temPwd.split("/");
        strList.pop_back();
        strPwd = strList.join("/");
        qDebug()<<strPwd;

        pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
        strName = TcpClient::getInstance().getLoginName();
        memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strncpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

        free(pdu);
        pdu = NULL;
    }

}

/**
 * 重命名
 * @brief Book::renameFile
 */
void Book::renameFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
        QMessageBox::warning(this,"重命名","请选择要重命名的文件");
    else
    {
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名文件","请输入新的文件名:");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strPwd.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this,"重命名文件","新文件名不为空");
        }
    }

}

/**
 * 上传文件槽函数
 * @brief Book::uploadFile
 */
void Book::uploadFile()
{
    uploadFilePath = QFileDialog::getOpenFileName();
    qDebug()<<uploadFilePath;

    if(!uploadFilePath.isEmpty())
    {
        int index = uploadFilePath.lastIndexOf('/');
        QString strFileName = uploadFilePath.right(uploadFilePath.size()-index-1);
        qDebug()<<strFileName;

        QFile file(uploadFilePath);
        qint64 fileSize = file.size();  //获取文件大小

        PDU *pdu = mkPDU(uploadFilePath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strPwd.toStdString().c_str(),strPwd.size());
        sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

        m_pTimer->start(500);
    }
    else
    {
        QMessageBox::warning(this,"上传文件","上传文件不为空");
    }
}

/**
 * 再次发送上传请求
 * @brief Book::uploadFileData
 */
void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(uploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true)
    {
        ret = file.read(pBuffer,4096);
        if(ret>0&&ret<=4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }
        else if(ret == 0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this,"上传文件","上传文件失败:读文件失败");
            break;
        }
    }
    file.close();
    delete[] pBuffer;
    pBuffer = NULL;
}

/**
 * 删除常规文件
 * @brief Book::delRegFile
 */
void Book::delRegFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
        QMessageBox::warning(this,"删除","请选择要删除的文件");
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strPwd.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

/**
 * 下载文件
 * @brief Book::downloadFile
 */
void Book::downloadFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    QString strFileName = pItem->text();
    if(NULL == pItem)
        QMessageBox::warning(this,"下载","请选择要下载的文件");
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if(strSaveFilePath.isEmpty())
        {
            m_strSaveFilePath.clear();
            QMessageBox::warning(this,"下载文件","下载路径不为空");
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
            //            m_bDownload = true;
        }
        PDU *pdu = mkPDU(strPwd.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        strcpy(pdu->caData,strFileName.toStdString().c_str());
        memcpy((char*)(pdu->caMsg),strPwd.toStdString().c_str(),strPwd.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

/**
 * 分享文件
 * @brief Book::shareFile
 */
void Book::shareFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"分享","请选择要分享的文件");
        return;
    }
    else
    {
        m_shareFileName = pItem->text();
    }
    Friend *pFriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShowFile::getInstance().updateFriend(pFriendList);
    if(ShowFile::getInstance().isHidden())
        ShowFile::getInstance().show();
}

void Book::moveFile()
{
    QListWidgetItem * pCurItem = m_pBookListW->currentItem();
    if(NULL!=pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        m_moveFilePath = strPwd+'/'+ m_strMoveFileName;
        m_pSelectDirPB->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}

void Book::selectDestDir()
{
    QListWidgetItem * pCurItem = m_pBookListW->currentItem();
    if(NULL!=pCurItem)
    {
        QString strDestDir = pCurItem->text();
        m_strDestDir = strPwd+'/'+ strDestDir;

        int srcLen = m_moveFilePath.size();
        int destLen = m_strDestDir.size();
        PDU *pdu = mkPDU(srcLen+destLen+2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caData,"%d %d %s",srcLen,destLen,m_strMoveFileName.toStdString().c_str());

        memcpy(pdu->caMsg,m_moveFilePath.toStdString().c_str(),srcLen);
        memcpy((char*)(pdu->caMsg)+srcLen+1,m_strDestDir.toStdString().c_str(),destLen);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);
}

