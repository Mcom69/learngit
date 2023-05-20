/********************************************************************************
** Form generated from reading UI file 'privatechat.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATECHAT_H
#define UI_PRIVATECHAT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateChat
{
public:
    QGridLayout *gridLayout;
    QTextEdit *textEdit_showMsg;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_inputMsg;
    QPushButton *pushButton_sendMsg;

    void setupUi(QWidget *PrivateChat)
    {
        if (PrivateChat->objectName().isEmpty())
            PrivateChat->setObjectName(QString::fromUtf8("PrivateChat"));
        PrivateChat->setEnabled(true);
        PrivateChat->resize(527, 398);
        gridLayout = new QGridLayout(PrivateChat);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textEdit_showMsg = new QTextEdit(PrivateChat);
        textEdit_showMsg->setObjectName(QString::fromUtf8("textEdit_showMsg"));
        textEdit_showMsg->setEnabled(false);
        QFont font;
        font.setPointSize(24);
        textEdit_showMsg->setFont(font);
        textEdit_showMsg->setFrameShape(QFrame::Box);
        textEdit_showMsg->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(textEdit_showMsg, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit_inputMsg = new QLineEdit(PrivateChat);
        lineEdit_inputMsg->setObjectName(QString::fromUtf8("lineEdit_inputMsg"));
        lineEdit_inputMsg->setFont(font);

        horizontalLayout->addWidget(lineEdit_inputMsg);

        pushButton_sendMsg = new QPushButton(PrivateChat);
        pushButton_sendMsg->setObjectName(QString::fromUtf8("pushButton_sendMsg"));
        pushButton_sendMsg->setFont(font);

        horizontalLayout->addWidget(pushButton_sendMsg);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);


        retranslateUi(PrivateChat);

        QMetaObject::connectSlotsByName(PrivateChat);
    } // setupUi

    void retranslateUi(QWidget *PrivateChat)
    {
        PrivateChat->setWindowTitle(QCoreApplication::translate("PrivateChat", "Form", nullptr));
        pushButton_sendMsg->setText(QCoreApplication::translate("PrivateChat", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PrivateChat: public Ui_PrivateChat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATECHAT_H
