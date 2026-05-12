#ifndef PRINTTEMPLATE_H
#define PRINTTEMPLATE_H

#include <QObject>
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
#include <QMessageBox>
#include <QGridLayout>
#include <QSpacerItem>
#include <QVector>
#include <QMap>
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

    // 设置曲线数据
    void setCurveData(const QMap<quint8, QVector<double>> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<quint8, QVector<double>> m_curveData;
};

class PrintTemplate : public QWidget
{
    Q_OBJECT

public:
    explicit PrintTemplate(QWidget *parent = nullptr, CustomCreatSql *sql = nullptr);
    ~PrintTemplate();

    // 设置样本号并加载数据
    void setSampleId(const QString &sampleId);

    // 获取当前样本号
    QString getSampleId() const;

    // 打印报告
    void printReport();

    // 导出PDF报告
    void exportToPDF();

    // 设置粘附率显示状态
    void setShowAdhesionRate(bool show);

private:
    // 初始化UI
    void initUI();

    // 从数据库获取医院名
    QString getHospitalNameFromDB();

    // 从数据库获取参考值
    QString getReferenceRangeFromDB(const QString &reagentName);

    // 解析参考值范围
    QPair<double, double> parseReferenceRange(const QString &refRange);

    // 从数据库加载数据
    bool loadDataFromDatabase();

    // 更新报告显示
    void updateReportDisplay();

    // 清空数据
    void clearData();

    //拆分测试结果
    QVector<double> splitPercentages(const QString& input);


private:
    CustomCreatSql *m_sqlDatabase;          // 数据库操作对象
    QString m_sampleId;                     // 当前样本号
    PatientInformationStu m_patientInfo;    // 患者信息和结果数据
    QMap<quint8, QVector<double>> m_curveData; // 曲线数据 AA,ADP,EPI,COL,RIS

    // UI控件
    QVBoxLayout *m_mainLayout;
    QLabel *m_hospitalLabel;                // 医院标题
    QLabel *m_reportTitleLabel;             // 报告标题
    QLabel *m_reportDateLabel;              // 报告日期

    // 患者信息标签
    QLabel *m_nameLabel;
    QLabel *m_sexLabel;
    QLabel *m_ageLabel;
    QLabel *m_departmentLabel;
    QLabel *m_sampleIdLabel;
    QLabel *m_barcodeLabel;
    QLabel *m_submissionTimeLabel;
    QLabel *m_inspectionTimeLabel;

    // 测试结果标签
    QLabel *m_aaResult60SLabel;
    QLabel *m_aaResult180SLabel;
    QLabel *m_aaResult300SLabel;
    QLabel *m_aaResultMaxLabel;

    QLabel *m_adpResult60SLabel;
    QLabel *m_adpResult180SLabel;
    QLabel *m_adpResult300SLabel;
    QLabel *m_adpResultMaxLabel;

    QLabel *m_epiResult60SLabel;
    QLabel *m_epiResult180SLabel;
    QLabel *m_epiResult300SLabel;
    QLabel *m_epiResultMaxLabel;

    QLabel *m_colResult60SLabel;
    QLabel *m_colResult180SLabel;
    QLabel *m_colResult300SLabel;
    QLabel *m_colResultMaxLabel;

    QLabel *m_risResult60SLabel;
    QLabel *m_risResult180SLabel;
    QLabel *m_risResult300SLabel;
    QLabel *m_risResultMaxLabel;

    // 新增参数标签
    QLabel *m_aaAUCLabel;
    QLabel *m_aaMaxSlopeLabel;
    QLabel *m_aaTMAtimeLabel;
    QLabel *m_aaLagTimeLabel;
    QLabel *m_aaAdhesionLabel;
    QLabel *m_aaRefData;

    QLabel *m_adpAUCLabel;
    QLabel *m_adpMaxSlopeLabel;
    QLabel *m_adpTMAtimeLabel;
    QLabel *m_adpLagTimeLabel;
    QLabel *m_adpAdhesionLabel;
    QLabel *m_adpRefData;

    QLabel *m_epiAUCLabel;
    QLabel *m_epiMaxSlopeLabel;
    QLabel *m_epiTMAtimeLabel;
    QLabel *m_epiLagTimeLabel;
    QLabel *m_epiAdhesionLabel;
    QLabel *m_epiRefData;

    QLabel *m_colAUCLabel;
    QLabel *m_colMaxSlopeLabel;
    QLabel *m_colTMAtimeLabel;
    QLabel *m_colLagTimeLabel;
    QLabel *m_colAdhesionLabel;
    QLabel *m_colRefData;

    QLabel *m_risAUCLabel;
    QLabel *m_risMaxSlopeLabel;
    QLabel *m_risTMAtimeLabel;
    QLabel *m_risLagTimeLabel;
    QLabel *m_risAdhesionLabel;
    QLabel *m_risRefData;

    // 数据库访问对象
    CustomCreatSql *m_dbAccess;

    // 粘附率显示控制
    bool m_showAdhesionRate;

    // 曲线图区域
    CurveWidget *m_curveWidget;

    // 底部按钮区域
    QPushButton *m_printButton;
    QPushButton *m_exportPDFButton;
    QPushButton *m_closeButton;
};

#endif // PRINTTEMPLATE_H
