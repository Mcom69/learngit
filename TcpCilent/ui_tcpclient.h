/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_username;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit_pwd;
    QPushButton *pushButton_login;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_reg;
    QLabel *label_3;
    QPushButton *pushButton_logout;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(341, 216);
        gridLayout = new QGridLayout(TcpClient);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(TcpClient);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(22);
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        lineEdit_username = new QLineEdit(TcpClient);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));
        QFont font1;
        font1.setPointSize(15);
        lineEdit_username->setFont(font1);

        horizontalLayout_2->addWidget(lineEdit_username);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(TcpClient);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        horizontalLayout->addWidget(label_2);

        lineEdit_pwd = new QLineEdit(TcpClient);
        lineEdit_pwd->setObjectName(QString::fromUtf8("lineEdit_pwd"));
        lineEdit_pwd->setFont(font1);
        lineEdit_pwd->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(lineEdit_pwd);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        pushButton_login = new QPushButton(TcpClient);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));
        QFont font2;
        font2.setPointSize(24);
        pushButton_login->setFont(font2);

        gridLayout->addWidget(pushButton_login, 2, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButton_reg = new QPushButton(TcpClient);
        pushButton_reg->setObjectName(QString::fromUtf8("pushButton_reg"));
        QFont font3;
        font3.setPointSize(20);
        pushButton_reg->setFont(font3);

        horizontalLayout_3->addWidget(pushButton_reg);

        label_3 = new QLabel(TcpClient);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        pushButton_logout = new QPushButton(TcpClient);
        pushButton_logout->setObjectName(QString::fromUtf8("pushButton_logout"));
        pushButton_logout->setFont(font3);

        horizontalLayout_3->addWidget(pushButton_logout);


        gridLayout->addLayout(horizontalLayout_3, 3, 0, 1, 1);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        label->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        lineEdit_username->setText(QString());
        label_2->setText(QCoreApplication::translate("TcpClient", "\345\257\206  \347\240\201\357\274\232", nullptr));
        lineEdit_pwd->setText(QString());
        pushButton_login->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        pushButton_reg->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        label_3->setText(QString());
        pushButton_logout->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
