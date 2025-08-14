#ifndef REPLACETHETESTTUBETRAY_H
#define REPLACETHETESTTUBETRAY_H

#include "qcustomplot.h"

#include <QWidget>
#include <QPainter>
#include <QMessageBox>
#include <QTableWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include "dilag/pedata.h"
#include <Custommovements/performanceverification.h>


// RAII辅助类：管理表格更新状态
class TableUpdateGuard {
public:
    explicit TableUpdateGuard(QTableWidget* table)
        : m_table(table), m_wasEnabled(table->updatesEnabled())
    {
        m_table->setUpdatesEnabled(false);
    }

    ~TableUpdateGuard() {
        if (m_table) {
            m_table->setUpdatesEnabled(m_wasEnabled);
            if (m_wasEnabled) m_table->viewport()->update();
        }
    }

private:
    QTableWidget* m_table;
    bool m_wasEnabled;
};




// 在头文件中定义枚举
enum TableColumns { COL_ID = 0, COL_CHANNEL, COL_VALUE, COL_VALUEMID,COL_VALUELOW };


namespace Ui {
class ReplaceTheTestTubeTray;
}

class ReplaceTheTestTubeTray : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceTheTestTubeTray(quint8 indexEquip,QWidget *parent = 0);
    ~ReplaceTheTestTubeTray();

    void fillData(const QString& id, quint8 channel, const std::array<double, 3> &ratios);
    void handlecompletedPETest();





private slots:
    void on_toolButton_clicked();

    void on_pushButton_day_clicked();

    void on_pushButton_month_clicked();

    void on_pushButton_year_clicked();

    void on_pushButton_all_clicked();

    void openPEdataWindow(const QPoint &pos);

    void on_dateEdit_resultCuve_dateChanged(const QDate &date);

private:
    void createHorizontalButtons(const int &numChannel);
    QVector<QPushButton*> m_channelButtons; // 存储按钮指针

    // 可选：添加扩展功能声明
    void resetAllChannels(bool isall, const QString &indexChn);
    void disableAllButtons();  // 禁用所有按钮
    QVector<int> getActiveChannels() const;

    QMap<QString,quint8> getColumnValues(QTableWidget *table);
	bool isKeyExists(const QString &key);

    void inittestingwidget(QTableWidget *ptable);
    void sycntestingChannel(const QString &channle, bool addit);
    void removeDuplicateKeys(const QString &channle);
    void addNewRow(const QString &key, const QString &value);

    void addResultNewRow(const QString&id, const QString &channel,
                         const QString &outratio,
                         const QString &outratioM,
                         const QString &outratioL);


    void allfinishshowResult();


    void SelectedRows(QMap<int, QVector<double> > &outdata);
    void setupAxes(QCustomPlot* customPlot);


    void generateMultiDemoData(QCustomPlot* customPlot,
                               const QVector<double>& x,
                               const QVector<QVector<double>>& yValues, // 多条曲线的y值，例如{y_high, y_mid, y_low}
                               const QDate& month,
                               const QVector<QPen>& pens );

    //void generateDemoData(QCustomPlot* customPlot, const QVector<double> x, const QVector<double> y, const QDate &month);
    void viewPEResult(const bool isnow, const QString tratDate,const QDate &tragmonth);


    int  findRowForId(QTableWidget* table, const QString& id, QHash<QString, int>& cache);
    void updateTableRow(QTableWidget* table, int row, quint8 channel, const std::array<double, 3>& ratios);
    void updateDatabase(const QString& id, quint8 channel, const std::array<double, 3>& ratios);

signals:

public slots:
    void handlemakesureResult(const QString &resultdata, const QString &resultdataMid, const QString &resultdataLow);
private:
    Ui::ReplaceTheTestTubeTray *ui;

    quint8 m_equipmentKind; //仪器类型
    QVector<double> m_dayx;
    QVector<double> m_dayValy;

    QVector<double> m_dayMidValy;
    QVector<double> m_dayLowValy;


};

#endif // REPLACETHETESTTUBETRAY_H
