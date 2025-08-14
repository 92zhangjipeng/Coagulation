#ifndef PEDATA_H
#define PEDATA_H

#include <QWidget>
#include <QVector>
#include <algorithm>
#include <numeric>
#include <cmath>


struct StatsResult {
    double maxVal;
    double minVal;
    double mean;
    double cv;
};

namespace Ui {
class PEdata;
}

class PEdata : public QWidget
{
    Q_OBJECT

public:
    explicit PEdata(QWidget *parent = 0);
    ~PEdata();

    void insertdata(const QMap<int, QVector<double> > &datavec);

    StatsResult calculateStats(const int &index, const QVector<double>& data);

private slots:
    void on_pushButton_pushout_clicked();
signals:
    void makesureResult(const QString &,const QString &,const QString &);

private:
    Ui::PEdata *ui;
    StatsResult mval;
    StatsResult mvalMid;
    StatsResult mvalLow;


};

#endif // PEDATA_H
