#ifndef CONFIGUREPROJECTITEM_H
#define CONFIGUREPROJECTITEM_H

#include <QRadioButton>
#include <QWidget>
#include <QMessageBox>
#include "ini_file.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define  PROJECT_1  "AA"
#define  PROJECT_2  "ADP"
#define  PROJECT_3  "EPI"
#define  PROJECT_4  "COL"
#define  PROJECT_5  "RIS"

class QMouseEvent;
class QCloseEvent;
class QKeyEvent;

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
	//拖拽窗口
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
signals:
    void _setallsampletestproject(QString);
    void _setsinglesampleproject(int,int,QString); //设置单个样本测试项目
public:
    void Slot_ChangeSingleItem(int rows, int clos,bool signal); //修改全部、修改单个
private slots:

private:
    bool SelectRadittom();
    void CreatTableNumTab();
    void AddCheckBoxInntoTable(int Rows);

    void RadioButtonAllNoChecked();
    void ShowTestGroupText(QStringList GroupReagent, QString & ShowText);

   void  ClickCheckBox(bool pchecked);
   void  CancelReagentGroup();

    void AddGroupReagentTest(); //++组
    void DeleteReagentGroup();  //--组

    void ConfigTestingReagnet(); //保存
private:
    Ui::ConfigureProjectItem *ui;
	QColor bgmcColor;
	QPoint mousePos;
	QPoint windowsPos;
	QPoint dPos;


    QList<QRadioButton*> m_ptestingradioList;

    QList<QCheckBox*> m_checkBoxList;

    int m_rows = 0;
    int m_clos = 0;
    bool m_singleitem = false;
    enum TableItemSels{Table_TestReagentroup,Table_CheckState};
};

#endif // CONFIGUREPROJECTITEM_H
