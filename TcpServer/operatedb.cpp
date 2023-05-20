
#include "operatedb.h"
#include <QDebug>
#include <QMessageBox>

OperateDB::OperateDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
//    init();
}

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}
/**
 * 初始化数据库
 * @brief OperateDB::init
 */
void OperateDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\WorkSpace\\Study\\qt\\practice\\Pan\\TcpServer\\cloud.db");
    if(m_db.open()){
        QMessageBox::information(NULL,"打开数据库","数据库打开成功！",QMessageBox::Ok);
        qDebug()<<"打开成功";
        QSqlQuery qs;
        qs.exec("select * from userInfo");
        while(qs.next())
        {
            QString data = QString("%1,%2,%3,%4").arg(qs.value(0).toString())
                                              .arg(qs.value(1).toString())
                                              .arg(qs.value(2).toString())
                                              .arg(qs.value(3).toString());
            qDebug()<<data;
        }
    }
    else
    {
        QMessageBox::warning(NULL,"打开数据库","打开数据库失败！");
    }

}

OperateDB::~OperateDB()
{
    m_db.close();
}

/**
 * 判断是否注册成功
 * @brief OperateDB::handleRegister
 * @param name
 * @param pwd
 * @return
 */
bool OperateDB::handleRegister(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd)
    {
        qDebug()<<"name | pwd is NULL";
        return false;
    }
    QString dataStr = QString("insert into userInfo(name,pwd) values('%1','%2')").arg(name).arg(pwd);
    qDebug()<<dataStr;
    QSqlQuery qs;
    return qs.exec(dataStr);
}

/**
 * 判断是否登录成功
 * @brief OperateDB::handleLogin
 * @param name
 * @param pwd
 * @return
 */
bool OperateDB::handleLogin(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd)
    {
        qDebug()<<"name | pwd is NULL";
        return false;
    }
    QString dataStr = QString("select * from userInfo where name = '%1' and pwd = '%2' and online = 0")
                              .arg(name).arg(pwd);
    qDebug()<<dataStr;
    QSqlQuery qs;
    qs.exec(dataStr);
    if(qs.next())
    {
        dataStr = QString("update userInfo set online = 1 where name = '%1' and pwd = '%2'")
                      .arg(name).arg(pwd);
        qDebug()<<dataStr;
        QSqlQuery qs;
        qs.exec(dataStr);

        return true;
    }
    else
        return false;
}

/**
 * 判断是否下线
 * @brief OperateDB::handleOffline
 * @param name
 */
void OperateDB::handleOffline(const char *name)
{
    if(NULL == name)
    {
        qDebug()<<"name | pwd is NULL";
    }
    QString dataStr = QString("update userInfo set online = 0 where name = '%1'").arg(name);
    QSqlQuery qs;
    qs.exec(dataStr);
}

/**
 * 处理所有在线用户列表
 * @brief OperateDB::handleAllOnline
 * @return
 */
QStringList OperateDB::handleAllOnline()
{
    QString dataStr = QString("select name from userInfo where online = 1");
    QSqlQuery qs;
    qs.exec(dataStr);
    QStringList result;
    result.clear();

    while(qs.next())
    {
        result.append(qs.value("name").toString());
    }
    return result;

}

/**
 * 搜索用户并判断是否存在该用户及用户是否在线
 * @brief OperateDB::handleSearchUser
 * @param name
 * @return
 */
int OperateDB::handleSearchUser(const char *name)
{
    if(NULL == name)
    {
        return -1;
    }
    QString dataStr = QString("select online from userInfo where name = '%1'").arg(name);
    QSqlQuery qs;
    qs.exec(dataStr);
    if(qs.next())
    {
        // 判断用户是否在线
        int ret = qs.value("online").toInt();
        if(1 == ret)
            return 1;   //在线
        else if(0 == ret)
            return 2;   //不在线
    }
    else
        return 3;      //不存在
//    return 0;
}
/**
 * 处理添加好友请求
 * @brief OperateDB::handleAddFriend
 * @param pername
 * @param name
 * @return
 */
int OperateDB::handleAddFriend(const char *pername, const char *name)
{
    if(NULL == pername || NULL == name)
        return -1;
    QString dataStr = QString("select * from friend where (id=(select id from userInfo where name = '%1')"
                              "and friendId =(select id from userInfo where name = '%2')) "
                              "or (id=(select id from userInfo where name = '%3')"
                              "and friendId =(select id from userInfo where name = '%4'))")
                              .arg(pername).arg(name).arg(name).arg(pername);
    qDebug()<<dataStr;
    QSqlQuery qs;
    qs.exec(dataStr);
    if(qs.next())
    {
        return 0;   //双方是好友
    }
    else
    {
        return handleSearchUser(pername);      //判断是否在线以及用户是否存在
    }
}

/**
 * 添加好友后，向数据库中添加好友数据
 * @brief OperateDB::handleAddFriendSuccess
 * @param pername
 * @param name
 */
void OperateDB::handleAddFriendSuccess(const char *pername, const char *name)
{
    QString dataStr1 = QString("select id from userInfo where name = '%1'").arg(pername);
    QString dataStr2 = QString("select id from userInfo where name = '%1'").arg(name);

    int perid;
    int id;
    QSqlQuery qs;
    qs.exec(dataStr1);
    while(qs.next())
    {
        perid = qs.value("id").toInt();
        qDebug()<<perid;
    }

    qs.clear();
    qs.exec(dataStr2);
    while(qs.next())
    {
        id = qs.value("id").toInt();
        qDebug()<<id;
    }

    QString dataStr3 = QString("insert into friend(id,friendId) values(%1,%2)").arg(perid).arg(id);
    qDebug()<<dataStr3;
    qs.exec(dataStr3);
}

/**
 * 处理好友列表刷新
 * @brief OperateDB::handleFlushFriend
 * @param name
 * @return 在线好友列表
 */
QStringList OperateDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(NULL == name)
    {
        return strFriendList;
    }
    QString dataStr = QString("select name from userInfo where online = 1 and id in ("
                              "select id from friend where friendId = ("
                              "select id from userInfo where name = '%1'))").arg(name);
    qDebug()<<dataStr;
    QSqlQuery qs;
    qs.exec(dataStr);
    while(qs.next())
    {
        strFriendList.append(qs.value("name").toString());
        qDebug()<<qs.value("name").toString();
    }

    qs.clear();
    dataStr = QString("select name from userInfo where online = 1 and id in ("
                      "select friendId from friend where id = ("
                      "select id from userInfo where name = '%1'))").arg(name);
    qs.exec(dataStr);
    qDebug()<<dataStr;
    while(qs.next())
    {
        strFriendList.append(qs.value("name").toString());
        qDebug()<<qs.value("name").toString();
    }
    return strFriendList;
}

/**
 * 处理删除好友数据库操作
 * @brief OperateDB::handleDelFriend
 * @param pername
 * @param name
 */
bool OperateDB::handleDelFriend(const char *name, const char *friendName)
{
    if(NULL == name || NULL ==friendName)
        return false;
    QString dataStr = QString("delete from friend where id = ("
                              "select id from userInfo where name = '%1')and friendId = ("
                              "select id from userInfo where name = '%2')").arg(name).arg(friendName);
    QSqlQuery qs;
    qs.exec(dataStr);

    dataStr = QString("delete from friend where id = ("
                              "select id from userInfo where name = '%1')and friendId = ("
                              "select id from userInfo where name = '%2')").arg(friendName).arg(name);
    qs.exec(dataStr);

    return true;
}

