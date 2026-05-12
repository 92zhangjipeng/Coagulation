#ifndef CONFIGUREPROJECTITEM_H
#define CONFIGUREPROJECTITEM_H

#include <QRadioButton>
#include <QWidget>
#include <QMessageBox>
#include "ini_file.h"
#include "opencvFindRBC/customtitlebar.h"  // 添加自定义标题栏头文件


#define  PROJECT_1  "AA"
#define  PROJECT_2  "ADP"
#define  PROJECT_3  "EPI"
#define  PROJECT_4  "COL"
#define  PROJECT_5  "RIS"

class QMouseEvent;
class QCloseEvent;
class QKeyEvent;
class QVBoxLayout;  // 添加布局头文件

namespace Ui {
class ConfigureProjectItem;
}

class ConfigureProjectItem : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigureProjectItem(QWidget *parent = 0);
    ~ConfigureProjectItem();

    void _initstyle();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void _setallsampletestproject(QString);
    void setsinglesampleproject(int,int,QString); //设置单个样本测试项目

public:
    void Slot_ChangeSingleItem(int rows, int clos,bool signal); //修改全部、修改单个

private slots:
    // 标题栏槽函数
    void onCloseRequested();
    void onMinimizeRequested();
    void onMaximizeRequested();

private:
    bool SelectRadittom();
    void CreatTableNumTab();
    void AddCheckBoxInntoTable(int Rows);

    void RadioButtonAllNoChecked();
    void ShowTestGroupText(QStringList GroupReagent, QString & ShowText);

    void ClickCheckBox(bool pchecked);
    void CancelReagentGroup();

    void AddGroupReagentTest(); //++组
    void DeleteReagentGroup();  //--组

    void ConfigTestingReagnet(); //保存

    void initTitleBar();  // 添加初始化标题栏函数

private:
    Ui::ConfigureProjectItem *ui;
    QColor bgmcColor;

    QList<QRadioButton*> m_ptestingradioList;
    QList<QCheckBox*> m_checkBoxList;

    int m_rows = 0;
    int m_clos = 0;
    bool m_singleitem = false;

    CustomTitleBar *m_titleBar;  // 添加自定义标题栏指针
    QVBoxLayout *m_mainLayout;   // 主布局

    enum TableItemSels{Table_TestReagentroup,Table_CheckState};
};

#endif // CONFIGUREPROJECTITEM_H
