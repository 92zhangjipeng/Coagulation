#ifndef ADDUSERNAME_H
#define ADDUSERNAME_H

#include <QCloseEvent>
#include <QWidget>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include "opencvFindRBC/customtitlebar.h"
#include "testing.h"

namespace Ui {
class AddUserName;
}

class AddUserName : public QWidget
{
    Q_OBJECT

public:
    explicit AddUserName(char UserSet, QWidget *parent = 0);
    ~AddUserName();

    void closeEvent(QCloseEvent *event) override;

public slots:
    void ModifyUsername_Slot(QString name);

signals:
    void AddUser(const QString, const QString, bool);

private slots:
    void onAddClicked();
    void onCancelClicked();
    void onOkClicked();
    void onCancelPwdClicked();
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();

private:
    void initUI();
    void initStyle();
    void setupAddUserMode();
    void setupChangePasswordMode();

    Ui::AddUserName *ui;
    CustomTitleBar *m_titleBar;

    // 主布局
    QVBoxLayout *m_mainLayout;
    QWidget *m_centerWidget;
    QVBoxLayout *m_centerLayout;

    // 添加用户模式控件
    QWidget *m_addUserWidget;
    QGridLayout *m_addUserLayout;
    QLabel *m_enterUserLabel;
    QLineEdit *m_userLineEdit;
    QLabel *m_setPasswordLabel;
    QLineEdit *m_passwordLineEdit;
    QCheckBox *m_vipCheckBox;
    QPushButton *m_addBtn;
    QPushButton *m_addCancelBtn;

    // 修改密码模式控件
    QWidget *m_changePwdWidget;
    QGridLayout *m_changePwdLayout;
    QLabel *m_titleLabel;
    QLabel *m_oldPwdLabel;
    QLineEdit *m_oldPwdLineEdit;
    QLabel *m_newPwdLabel;
    QLineEdit *m_newPwdLineEdit;
    QLabel *m_confirmPwdLabel;
    QLineEdit *m_confirmPwdLineEdit;
    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;

    char m_show;
    QString m_NotifyUsername;
    bool m_isMaximized;
    QRect m_normalGeometry;
};

#endif // ADDUSERNAME_H
