#ifndef SHOWFILE_H
#define SHOWFILE_H


#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>

class ShowFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShowFile(QWidget *parent = nullptr);

    static ShowFile &getInstance();

    void updateFriend(QListWidget *pFriendList);

signals:
private slots:
    void cancelSelect();
    void selectAll();
    void okShare();
    void cancelShare();

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_CanclePB;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QButtonGroup *m_pButtonGroup;

    QVBoxLayout *m_pFriendWVBL;

};

#endif // SHOWFILE_H
