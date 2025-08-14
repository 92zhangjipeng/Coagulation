#ifndef CUSTOMCURVEADP_H
#define CUSTOMCURVEADP_H

#include <QMap>
#include <QObject>
#include <QSet>
//#include <operclass/fullyautomatedplatelets.h>

struct curveADP
{
    QString samplenum;
    int index_;
    double data;
};

typedef QSet<curveADP* > adpdata_;

class CustomCurveADP
{
public:
    CustomCurveADP();
    ~CustomCurveADP();
public:
     void generatecurve(QString samplnum );  //生成ADP数据

	 double  outadpcurvedata(QString samplnum,int num); //输出adp数据

     void generatecurve_aa(QString samplenum); //生成AA

     double outcurvedataAA(QString samplnum, int num);

     void generatecurve_col(QString samplnum);

     double outcolcurvedata(QString samplnum, int num);

     void generatecurve_epi(QString samplnum);

     double outcurvedataEPI(QString samplnum, int num);

     void deldata(quint8 index_reag,QString samplnum);

private:

    QMap<QString,adpdata_> curve_data;

    QMap<QString,adpdata_> aa_curve_data;

    QMap<QString,adpdata_> col_curve_data;

    QMap<QString,adpdata_> epi_curve_data;

};

#endif // CUSTOMCURVEADP_H
