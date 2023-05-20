#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QDir>

#include "protocol.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);

    void updateFileList(const PDU *pdu);
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();
    QString getShareFileName();
    QString getStrPwd();

    qint64 m_iTotal;        //总文件大小
    qint64 m_iRecved;       //已收到大小

signals:
private slots:
    void createDir();
    void flushFile();
    void showDirFile(QModelIndex item);
    void delDir();
    void returnDirFile();
    void renameFile();
    void uploadFile();
    void uploadFileData();
    void delRegFile();
    void downloadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();

private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownloadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pSharePB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;

    QString strPwd; //当前路径
    QString uploadFilePath;  //上传文件的文件路径
    QString m_strSaveFilePath; //下载文件保存地址
    QString m_shareFileName;
    QString m_moveFilePath;
    QString m_strDestDir;
    QString strName;
    QTimer *m_pTimer;

    bool m_bDownload;   //判断是否处于下载状态
    QString m_strMoveFileName;
};

#endif // BOOK_H
