#ifndef CORRECT_DATA_H
#define CORRECT_DATA_H

#include <QWidget>
//#include "virtualkeyboard.h"
#define  SAMLPE_NAME 1
#define  HEIGHT_DATA 2

namespace Ui {
class Correct_Data;
}

class Correct_Data : public QWidget
{
    Q_OBJECT

public:
    explicit Correct_Data(unsigned int rows, int Column, QString changeData, QWidget *parent = 0);
    ~Correct_Data();

    void setrows(const int &row);
    void setcols(const int &col);
    void setorigindata(const QString &Originaldata);
protected:
    bool    eventFilter(QObject *watched, QEvent *event);
    void    closeEvent(QCloseEvent *);
signals:
    void    ConfigureData(unsigned int,int, QString );
    void    repTestHeight(const QString);
    void    ShowPointBtn(bool);

    void    NotifyTestHeight(int,int,QString);


private slots:
    void    on_toolButton_Ok_clicked();
    void    on_toolButton_Cancel_clicked();
    void    on_toolButton_Retest_clicked();
public:
     static bool RetestHight; //重测高
     void  ClickSamplename(const QString);
private:
    Ui::Correct_Data *ui;
    QString m_dateSample;
    QString m_Samplename;
    int m_Index;
    unsigned int m_rows;
    QString m_Data_str;
    //Virtualkeyboard  *myFrmnum;
    //QValidator *accountValidator ;
	enum IndexCols { Cols_Sample = 1, Cols_BloodHeight};
};

#endif // CORRECT_DATA_H
