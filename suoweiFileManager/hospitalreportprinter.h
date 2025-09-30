#ifndef HOSPITALREPORTPRINTER_H
#define HOSPITALREPORTPRINTER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QPixmap>

#include <QPrinter>
#include <QPainter>
#include <QFont>
#include <QMarginsF>

// 患者基本信息
struct PatientBasicInfo {
    QString patientId;      // 病历号
    QString name;           // 姓名
    QString gender;         // 性别
    int age;               // 年龄
    QString department;     // 科室
    QString bedNumber;      // 床号
    QDateTime testTime;     // 检测时间
    QString diagnosis;      // 临床诊断
};

// 检测结果数据
struct TestResult {
    QString testItem;       // 检测项目
    double resultValue;    // 结果数值
    QString unit;          // 单位
    QString referenceRange; // 参考范围
};

// 曲线数据
struct CurveData {
    QPixmap curveImage;    // 曲线图
    QString curveTitle;    // 曲线标题
    QString analysis;      // 曲线分析
};


class HospitalReportPrinter : public QObject
{
    Q_OBJECT
public:
    explicit HospitalReportPrinter(QObject *parent = nullptr);
    virtual ~HospitalReportPrinter();

    // 纸张大小设置
    enum PaperSize {
       A4,
       A5,
       B5,
       Letter,
       Legal,
       Custom
    };

    void setPaperSize(PaperSize size);
    void setCustomPaperSize(const QSizeF &sizeMM); // 毫米单位
    void setPageOrientation(QPageLayout::Orientation orientation);

    // 设置打印数据
    void setPatientInfo(const PatientBasicInfo &info);
    void setTestResults(const QVector<TestResult> &results);
    void setCurveData(const QVector<CurveData> &curves);

    // 打印报告
    bool printReport(const QString &title = "血小板聚集功能检测报告");
    bool printToPdf(const QString &fileName, const QString &title = "血小板聚集功能检测报告");

    // 获取当前页面信息
    QSizeF getPageSizeMM() const { return m_pageSizeMM; }
    PaperSize getPaperSize() const { return m_paperSize; }

protected:
    // 可重写的虚拟函数
       virtual void setupPrinter(QPrinter *printer);
       virtual void updateLayoutForPageSize();
       virtual void drawHeader(QPainter &painter, const QString &title, double pageWidth);
       virtual void drawPatientInfo(QPainter &painter, double yStart, double pageWidth);
       virtual void drawTestResults(QPainter &painter, double yStart, double pageWidth);
       virtual void drawCurves(QPainter &painter, double yStart, double pageWidth);
       virtual void drawFooter(QPainter &painter, double yStart, double pageWidth);

       // 工具函数
       virtual double drawTextSection(QPainter &painter, double x, double y,
                                    const QString &label, const QString &value,
                                    const QFont &labelFont, const QFont &valueFont);
       virtual void drawHorizontalLine(QPainter &painter, double xStart, double xEnd, double y);

       // 字体大小计算（根据页面大小自适应）
       virtual QFont getHeaderFont(double pageWidth) const;
       virtual QFont getTitleFont(double pageWidth) const;
       virtual QFont getSectionFont(double pageWidth) const;
       virtual QFont getContentFont(double pageWidth) const;
       virtual QFont getFooterFont(double pageWidth) const;

       // 页面布局参数
       struct PageLayout {
           QMarginsF margins;          // 边距（根据页面大小比例计算）
           double headerHeight;        // 头部高度
           double footerHeight;        // 页脚高度
           double lineSpacing;         // 行间距
           double sectionSpacing;      // 段落间距
           double columnSpacing;       // 列间距
       };

       PageLayout m_layout;
       PatientBasicInfo m_patientInfo;
       QVector<TestResult> m_testResults;
       QVector<CurveData> m_curveData;

private:
       void initializeLayout();
       void calculateDynamicLayout(const QSizeF &pageSizeMM);
       double mmToPixels(double mm, double dpi) const;
       double getScaleFactor() const;


       void drawFieldWithBackground(QPainter &painter,
                                    double x, double y,
                                    double width, double height,
                                    const QString &label,
                                    const QString &value,
                                    const QFont &labelFont,
                                    const QFont &valueFont);

       PaperSize m_paperSize;
       QSizeF m_pageSizeMM; // 纸张大小（毫米）
       QPageLayout::Orientation m_orientation;
       double m_currentDPI;
};

#endif // HOSPITALREPORTPRINTER_H
