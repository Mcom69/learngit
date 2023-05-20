
#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>

class OperateDB : public QObject
{
    Q_OBJECT
public:
    explicit OperateDB(QObject *parent = nullptr);
    static OperateDB& getInstance();    //创建实例
    void init();
    ~OperateDB();

    bool handleRegister(const char* name,const char* pwd);              //判断是否注册成功
    bool handleLogin(const char* name,const char* pwd);                 //判断是否登陆成功
    void handleOffline(const char* name);                               //处理下线操作
    QStringList handleAllOnline();                                      //处理所有在线用户
    int handleSearchUser(const char* name);                             //处理查询用户信息
    int handleAddFriend(const char* pername,const char *name);          //处理添加好友请求
    void handleAddFriendSuccess(const char* pername,const char *name);  //成功添加好友后向数据库插入数据
    QStringList handleFlushFriend(const char* name);                    //处理好友列表刷新
    bool handleDelFriend(const char* pername,const char *name);         //处理删除好友操作

signals:

private:
    QSqlDatabase m_db;      //创建数据库对象
};

#endif // OPERATEDB_H
