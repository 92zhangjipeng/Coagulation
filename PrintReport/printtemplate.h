#ifndef PRINTTEMPLATE_H
#define PRINTTEMPLATE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QPageSetupDialog>
#include <QGridLayout>
#include <QSpacerItem>
#include <QVector>
#include <QMap>
#include <QCheckBox>
#include <algorithm>
#include <cmath>
#include "customcreatsql.h"
#include "AggregationAnalyzer.h"

// 自定义曲线绘制控件
class CurveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurveWidget(QWidget *parent = nullptr);
    void setCurveData(const QMap<quint8, QVector<double>> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<quint8, QVector<double>> m_curveData;
};

// 试剂信息结构
struct ReagentInfo {
    quint8 id;
    QString displayName;        // 显示名称，如 "AA(氨基酸)"
    QString refDbKey;           // 数据库中参考值对应的键名，如 "AA聚集率-Max"
    double defaultLow;          // 默认参考范围下限
    double defaultHigh;         // 默认参考范围上限
};

class PrintTemplate : public QWidget
{
    Q_OBJECT

public:
    explicit PrintTemplate(QWidget *parent = nullptr, CustomCreatSql *sql = nullptr);
    ~PrintTemplate();

    void setSampleId(const QString &sampleId);
    QString getSampleId() const;
    void printReport();
    void exportToPDF();
    void setShowAdhesionRate(bool show);
    void setShowCurveChart(bool show);

private:
    void initUI();
    QString getHospitalNameFromDB();
    QString getReferenceRangeFromDB(const QString &reagentKey);
    QPair<double, double> parseReferenceRange(const QString &refRange);
    bool loadDataFromDatabase();
    void updateReportDisplay();
    void clearData();
    QVector<double> splitPercentages(const QString& input);

    // 动态表格管理
    void clearResultRows();                         // 清除所有动态行
    void addResultRow(const ReagentInfo &info);     // 添加一行试剂结果
    void updateResultRow(quint8 reagentId);         // 更新某一行的数据

    // 更新单个结果单元格并添加异常标记
    void updateResultWithMark(QLabel *label, double value, double low, double high);

private:
    CustomCreatSql *m_sqlDatabase;
    QString m_sampleId;
    PatientInformationStu m_patientInfo;
    QMap<quint8, QVector<double>> m_curveData;

    // UI 控件
    QVBoxLayout *m_mainLayout;
    QLabel *m_hospitalLabel;
    QLabel *m_reportTitleLabel;
    QLabel *m_reportDateLabel;

    // 患者信息标签
    QLabel *m_nameLabel;
    QLabel *m_sexLabel;
    QLabel *m_ageLabel;
    QLabel *m_departmentLabel;
    QLabel *m_sampleIdLabel;
    QLabel *m_barcodeLabel;
    QLabel *m_submissionTimeLabel;
    QLabel *m_inspectionTimeLabel;
    QLabel *m_referDoctorLabel;
    QLabel *m_reviewDoctorLabel;
    QLabel *m_remarkLabel;
    QFrame *m_remarkSection;

    // 结果表格区域
    QFrame *m_resultFrame;
    QGridLayout *m_resultGrid;
    QLabel *m_adhesionHeaderLabel;      // 粘附率表头，用于动态显示/隐藏

    // 动态行的控件集合
    struct RowWidgets {
        QLabel *testItemLabel;
        QLabel *result60SLabel;
        QLabel *result180SLabel;
        QLabel *result300SLabel;
        QLabel *resultMaxLabel;
        QLabel *aucLabel;
        QLabel *maxSlopeLabel;
        QLabel *tmaTimeLabel;
        QLabel *lagTimeLabel;
        QLabel *adhesionLabel;
        QLabel *refRangeLabel;
    };
    QList<RowWidgets> m_resultRows;                 // 每行的控件
    QMap<quint8, int> m_reagentRowMap;              // 试剂ID -> 行索引

    // 支持的试剂列表（顺序即为表格显示顺序）
    QList<ReagentInfo> m_reagents;

    // 曲线图
    QFrame *m_curveSection;
    CurveWidget *m_curveWidget;
    QCheckBox *m_showCurveCheck;

    // 底部按钮
    QPushButton *m_printButton;
    QPushButton *m_exportPDFButton;
    QPushButton *m_closeButton;

    // 粘附率显示控制
    bool m_showAdhesionRate;
};

#endif // PRINTTEMPLATE_H
