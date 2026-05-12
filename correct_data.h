#ifndef CORRECT_DATA_H
#define CORRECT_DATA_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "opencvFindRBC/customtitlebar.h"

#define  SAMLPE_NAME 1
#define  HEIGHT_DATA 2

namespace Ui {
class Correct_Data;
}

class Correct_Data : public QWidget
{
    Q_OBJECT

public:
    explicit Correct_Data( QWidget *parent = nullptr);
    ~Correct_Data();

    void setrows(const int &row);
    void setcols(const int &col);
    void setorigindata(const QString &Originaldata);
    void ClickSamplename(const QString);

protected:
    bool    eventFilter(QObject *watched, QEvent *event);
    void    closeEvent(QCloseEvent *);

signals:
    void    ConfigureData(unsigned int, int, QString);
    void    repTestHeight(const QString);
    void    NotifyTestHeight(int, int, QString);

private slots:
    void onClickokBtn();
    void onClickCancelBtn();
    void onClickResetHeighBtn();

    void    onCloseRequested();
    void    onMinimizeRequested();
    void    onMaximizeRequested();

public:
    static bool RetestHight; //重测高

private:
    void repLoading();

    bool processSampleChange(const QString &inputText);
    bool processBloodHeightChange(const QString &inputText);
    void showErrorMessage(int index);

    void initCustomTitleBar();
    void setupUI();           // 动态创建UI控件
    void applyStyles();       // 应用样式表

private:
    //Ui::Correct_Data *ui;

    // 自定义标题栏成员
    CustomTitleBar *m_titleBar = nullptr;


    QString m_Samplename;
    int m_Index;
    unsigned int m_rows;
    QString m_originclickData;
    QString m_sampleDate;

    // 动态创建的控件指针
    QLabel      *m_labelText = nullptr;
    QLineEdit   *m_lineEditChangeText = nullptr;
    QToolButton *m_toolButtonRetest = nullptr;
    QToolButton *m_toolButtonOk = nullptr;
    QToolButton *m_toolButtonCancel = nullptr;
    QWidget     *m_widgetText = nullptr;
    QWidget     *m_widgetButtons = nullptr;

    enum IndexCols { Cols_Sample = 1, Cols_BloodHeight };

};

#endif // CORRECT_DATA_H
