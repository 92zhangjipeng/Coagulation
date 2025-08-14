/********************************************************************************
** Form generated from reading UI file 'loginui.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINUI_H
#define UI_LOGINUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginUi
{
public:
    QVBoxLayout *verticalLayout_mian;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout;
    QLabel *TitleName;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolClose;
    QWidget *widget_pass;
    QVBoxLayout *verticalLayout;
    QWidget *widget_text;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_text;
    QWidget *widget_loginuser;
    QHBoxLayout *horizontalLayout_user;
    QLabel *label_User;
    QComboBox *comboBoxUser;
    QWidget *widget_loginpassword;
    QHBoxLayout *horizontalLayout_password;
    QLabel *label_Password;
    QWidget *widgeteditpassword;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_passwod;
    QPushButton *pushButton_show;
    QWidget *widget_loginreminder;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QLabel *Titleicon;
    QLabel *label_version;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget_btn;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButtonLogin;
    QPushButton *pushButtonCancel;
    QProgressBar *progressBar_readAxis;

    void setupUi(QWidget *LoginUi)
    {
        if (LoginUi->objectName().isEmpty())
            LoginUi->setObjectName(QStringLiteral("LoginUi"));
        LoginUi->setWindowModality(Qt::ApplicationModal);
        LoginUi->resize(408, 340);
        LoginUi->setMinimumSize(QSize(400, 340));
        LoginUi->setMaximumSize(QSize(412, 340));
        LoginUi->setFocusPolicy(Qt::ClickFocus);
        verticalLayout_mian = new QVBoxLayout(LoginUi);
        verticalLayout_mian->setSpacing(0);
        verticalLayout_mian->setObjectName(QStringLiteral("verticalLayout_mian"));
        verticalLayout_mian->setContentsMargins(0, 0, 0, 0);
        widget_title = new QWidget(LoginUi);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMaximumSize(QSize(16777215, 32));
        widget_title->setStyleSheet(QStringLiteral("background-color: rgb(135, 206, 255);"));
        horizontalLayout = new QHBoxLayout(widget_title);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        TitleName = new QLabel(widget_title);
        TitleName->setObjectName(QStringLiteral("TitleName"));
        TitleName->setMinimumSize(QSize(90, 0));
        TitleName->setMaximumSize(QSize(90, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(14);
        TitleName->setFont(font);
        TitleName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(TitleName);

        horizontalSpacer = new QSpacerItem(192, 9, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolClose = new QToolButton(widget_title);
        toolClose->setObjectName(QStringLiteral("toolClose"));
        toolClose->setStyleSheet(QLatin1String("\n"
"\n"
"QToolButton#toolClose{\n"
"   \n"
"   min-width:32px;\n"
"   min-height:32px;\n"
"   border-style:outset; \n"
"\n"
"}\n"
"QToolButton#toolClose:hover{ \n"
"\n"
" background:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #222b32, stop:0.1 #44a0f0, stop:0.98 #e3e7e9);\n"
"   border:0px;\n"
"   border-radius:4px;\n"
"   color:white;\n"
"}\n"
" \n"
"QToolButton#toolClose:pressed{\n"
" \n"
" background:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #222b32, stop:0.1 #222b32, stop:0.98 #44a0f0);\n"
"   border:0px;\n"
"   border-radius:4px;\n"
"   color:white;\n"
"} "));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/close_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolClose->setIcon(icon);
        toolClose->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolClose);


        verticalLayout_mian->addWidget(widget_title);

        widget_pass = new QWidget(LoginUi);
        widget_pass->setObjectName(QStringLiteral("widget_pass"));
        widget_pass->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(widget_pass);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(20, 5, 20, 5);
        widget_text = new QWidget(widget_pass);
        widget_text->setObjectName(QStringLiteral("widget_text"));
        widget_text->setMinimumSize(QSize(0, 50));
        widget_text->setMaximumSize(QSize(1200, 50));
        horizontalLayout_4 = new QHBoxLayout(widget_text);
        horizontalLayout_4->setSpacing(1);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(1, 1, -1, 1);
        label_text = new QLabel(widget_text);
        label_text->setObjectName(QStringLiteral("label_text"));
        label_text->setMinimumSize(QSize(358, 45));
        label_text->setMaximumSize(QSize(358, 45));
        label_text->setFont(font);
        label_text->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        horizontalLayout_4->addWidget(label_text);


        verticalLayout->addWidget(widget_text);

        widget_loginuser = new QWidget(widget_pass);
        widget_loginuser->setObjectName(QStringLiteral("widget_loginuser"));
        widget_loginuser->setMinimumSize(QSize(0, 50));
        widget_loginuser->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_user = new QHBoxLayout(widget_loginuser);
        horizontalLayout_user->setSpacing(0);
        horizontalLayout_user->setObjectName(QStringLiteral("horizontalLayout_user"));
        horizontalLayout_user->setContentsMargins(0, 0, 0, 0);
        label_User = new QLabel(widget_loginuser);
        label_User->setObjectName(QStringLiteral("label_User"));
        label_User->setMinimumSize(QSize(122, 40));
        label_User->setMaximumSize(QSize(140, 40));
        label_User->setFont(font);
        label_User->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);

        horizontalLayout_user->addWidget(label_User);

        comboBoxUser = new QComboBox(widget_loginuser);
        comboBoxUser->setObjectName(QStringLiteral("comboBoxUser"));
        comboBoxUser->setMinimumSize(QSize(190, 36));
        comboBoxUser->setMaximumSize(QSize(190, 36));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        comboBoxUser->setFont(font1);
        comboBoxUser->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font: 24px; /*\345\255\227\344\275\223*/\n"
"  font-family: \346\226\260\345\256\213\344\275\223;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font: 24px; /*\345\255\227\344\275\223*/\n"
"    font-family: \346\226\260\345\256\213\344\275\223;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265"
                        "\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 8px;\n"
"}\n"
"QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 8px;\n"
"}"));
        comboBoxUser->setMaxVisibleItems(30);
        comboBoxUser->setModelColumn(0);

        horizontalLayout_user->addWidget(comboBoxUser);


        verticalLayout->addWidget(widget_loginuser);

        widget_loginpassword = new QWidget(widget_pass);
        widget_loginpassword->setObjectName(QStringLiteral("widget_loginpassword"));
        widget_loginpassword->setMinimumSize(QSize(0, 50));
        widget_loginpassword->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_password = new QHBoxLayout(widget_loginpassword);
        horizontalLayout_password->setSpacing(0);
        horizontalLayout_password->setObjectName(QStringLiteral("horizontalLayout_password"));
        horizontalLayout_password->setContentsMargins(0, 0, 0, 0);
        label_Password = new QLabel(widget_loginpassword);
        label_Password->setObjectName(QStringLiteral("label_Password"));
        label_Password->setMinimumSize(QSize(140, 32));
        label_Password->setMaximumSize(QSize(140, 32));
        label_Password->setFont(font);
        label_Password->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_password->addWidget(label_Password);

        widgeteditpassword = new QWidget(widget_loginpassword);
        widgeteditpassword->setObjectName(QStringLiteral("widgeteditpassword"));
        widgeteditpassword->setMinimumSize(QSize(0, 36));
        widgeteditpassword->setMaximumSize(QSize(190, 36));
        widgeteditpassword->setStyleSheet(QString::fromUtf8("QWidget#widgeteditpassword{  \n"
"  color:#666666;\n"
"  font: 24px; /*\345\255\227\344\275\223*/\n"
"  font-family: \346\226\260\345\256\213\344\275\223;\n"
"  padding: 1px 1px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 1px 1px;	\n"
"} "));
        horizontalLayout_2 = new QHBoxLayout(widgeteditpassword);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(1, 0, 1, 0);
        lineEdit_passwod = new QLineEdit(widgeteditpassword);
        lineEdit_passwod->setObjectName(QStringLiteral("lineEdit_passwod"));
        lineEdit_passwod->setMinimumSize(QSize(150, 32));
        lineEdit_passwod->setMaximumSize(QSize(160, 32));
        lineEdit_passwod->setFont(font1);
        lineEdit_passwod->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
"  color:#666666;\n"
"  font: 24px; /*\345\255\227\344\275\223*/\n"
"  font-family: \346\226\260\345\256\213\344\275\223;\n"
"  padding: 1px 1px 1px 0px;\n"
"  background-repeat: no-repeat;\n"
"  border:0px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
" \n"
"}\n"
"\n"
""));
        lineEdit_passwod->setFrame(true);

        horizontalLayout_2->addWidget(lineEdit_passwod);

        pushButton_show = new QPushButton(widgeteditpassword);
        pushButton_show->setObjectName(QStringLiteral("pushButton_show"));
        pushButton_show->setMinimumSize(QSize(33, 32));
        pushButton_show->setMaximumSize(QSize(33, 32));
        pushButton_show->setStyleSheet(QLatin1String("border:0px solid rgba(150,150,150,1);\n"
"background-color: \n"
"#FFFFFF;"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/SetPng/passwodNotSee.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_show->setIcon(icon1);
        pushButton_show->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButton_show);


        horizontalLayout_password->addWidget(widgeteditpassword);


        verticalLayout->addWidget(widget_loginpassword);

        widget_loginreminder = new QWidget(widget_pass);
        widget_loginreminder->setObjectName(QStringLiteral("widget_loginreminder"));
        widget_loginreminder->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_3 = new QHBoxLayout(widget_loginreminder);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        Titleicon = new QLabel(widget_loginreminder);
        Titleicon->setObjectName(QStringLiteral("Titleicon"));
        Titleicon->setMinimumSize(QSize(128, 48));
        Titleicon->setMaximumSize(QSize(128, 48));

        horizontalLayout_3->addWidget(Titleicon);

        label_version = new QLabel(widget_loginreminder);
        label_version->setObjectName(QStringLiteral("label_version"));
        label_version->setMinimumSize(QSize(160, 0));
        label_version->setMaximumSize(QSize(160, 30));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font2.setPointSize(12);
        font2.setUnderline(true);
        label_version->setFont(font2);
        label_version->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_version);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_loginreminder);

        widget_btn = new QWidget(widget_pass);
        widget_btn->setObjectName(QStringLiteral("widget_btn"));
        widget_btn->setMinimumSize(QSize(0, 70));
        widget_btn->setMaximumSize(QSize(16777215, 70));
        horizontalLayout_5 = new QHBoxLayout(widget_btn);
        horizontalLayout_5->setSpacing(5);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(5, 0, -1, 5);
        pushButtonLogin = new QPushButton(widget_btn);
        pushButtonLogin->setObjectName(QStringLiteral("pushButtonLogin"));
        pushButtonLogin->setMinimumSize(QSize(0, 0));
        pushButtonLogin->setMaximumSize(QSize(120, 50));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        pushButtonLogin->setFont(font3);
        pushButtonLogin->setStyleSheet(QString::fromUtf8("QPushButton#pushButtonLogin{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButtonLogin:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButtonLogin:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:18px;\n"
"	colo"
                        "r:white;\n"
"}\n"
"\n"
""));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Picture/login_in.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonLogin->setIcon(icon2);
        pushButtonLogin->setIconSize(QSize(28, 28));

        horizontalLayout_5->addWidget(pushButtonLogin);

        pushButtonCancel = new QPushButton(widget_btn);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setMinimumSize(QSize(0, 0));
        pushButtonCancel->setMaximumSize(QSize(120, 50));
        pushButtonCancel->setFont(font3);
        pushButtonCancel->setStyleSheet(QString::fromUtf8("QPushButton#pushButtonCancel{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButtonCancel:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButtonCancel:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:18px;\n"
"	c"
                        "olor:white;\n"
"}\n"
"\n"
""));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Picture/exit_out.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonCancel->setIcon(icon3);
        pushButtonCancel->setIconSize(QSize(32, 32));

        horizontalLayout_5->addWidget(pushButtonCancel);


        verticalLayout->addWidget(widget_btn);

        progressBar_readAxis = new QProgressBar(widget_pass);
        progressBar_readAxis->setObjectName(QStringLiteral("progressBar_readAxis"));
        progressBar_readAxis->setValue(0);

        verticalLayout->addWidget(progressBar_readAxis);


        verticalLayout_mian->addWidget(widget_pass);


        retranslateUi(LoginUi);

        QMetaObject::connectSlotsByName(LoginUi);
    } // setupUi

    void retranslateUi(QWidget *LoginUi)
    {
        LoginUi->setWindowTitle(QApplication::translate("LoginUi", "Form", nullptr));
        TitleName->setText(QApplication::translate("LoginUi", "TextLabel", nullptr));
        toolClose->setText(QApplication::translate("LoginUi", "...", nullptr));
        label_text->setText(QApplication::translate("LoginUi", "\345\205\250\350\207\252\345\212\250\350\241\200\345\260\217\346\235\277\350\201\232\351\233\206\345\210\206\346\236\220\347\263\273\347\273\237\n"
"KS-1200", nullptr));
        label_User->setText(QApplication::translate("LoginUi", "TextLabel", nullptr));
        label_Password->setText(QApplication::translate("LoginUi", "TextLabel", nullptr));
        pushButton_show->setText(QString());
        Titleicon->setText(QApplication::translate("LoginUi", "TextLabel", nullptr));
        label_version->setText(QApplication::translate("LoginUi", "\350\275\257\344\273\266\347\211\210\346\234\254\357\274\232V1", nullptr));
        pushButtonLogin->setText(QApplication::translate("LoginUi", "\347\231\273\345\205\245", nullptr));
        pushButtonCancel->setText(QApplication::translate("LoginUi", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginUi: public Ui_LoginUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINUI_H
