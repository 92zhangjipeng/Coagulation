#pragma execution_character_set("utf-8")
// hospitalreportprinter.cpp
#include "hospitalreportprinter.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

HospitalReportPrinter::HospitalReportPrinter(QObject *parent)
    : QObject(parent)
    , m_paperSize(A4)
    , m_orientation(QPageLayout::Portrait)
    , m_currentDPI(300.0)
{
    // 设置默认纸张大小
    setPaperSize(A4);
}

HospitalReportPrinter::~HospitalReportPrinter()
{
}

void HospitalReportPrinter::setPaperSize(PaperSize size)
{
    m_paperSize = size;

    switch (size) {
    case A4:
        m_pageSizeMM = QSizeF(210, 297);
        break;
    case A5:
        m_pageSizeMM = QSizeF(148, 210);
        break;
    case B5:
        m_pageSizeMM = QSizeF(176, 250);
        break;
    case Letter:
        m_pageSizeMM = QSizeF(215.9, 279.4);
        break;
    case Legal:
        m_pageSizeMM = QSizeF(215.9, 355.6);
        break;
    case Custom:
        // 自定义大小需要单独设置
        break;
    }

    updateLayoutForPageSize();
}

void HospitalReportPrinter::setCustomPaperSize(const QSizeF &sizeMM)
{
    m_paperSize = Custom;
    m_pageSizeMM = sizeMM;
    updateLayoutForPageSize();
}

void HospitalReportPrinter::setPageOrientation(QPageLayout::Orientation orientation)
{
    m_orientation = orientation;
    if (orientation == QPageLayout::Landscape) {
        m_pageSizeMM.transpose();
    }
    updateLayoutForPageSize();
}

void HospitalReportPrinter::updateLayoutForPageSize()
{
    calculateDynamicLayout(m_pageSizeMM);
}

void HospitalReportPrinter::calculateDynamicLayout(const QSizeF &pageSizeMM)
{
    // 根据页面大小动态计算布局参数
    double pageArea = pageSizeMM.width() * pageSizeMM.height();
    double baseArea = 210 * 297; // A4 面积作为基准

    // 缩放因子（基于A4面积的比例）- 使用std::sqrt替代qSqrt
    double scale = std::sqrt(pageArea / baseArea);
    scale = qBound(0.8, scale, 1.2); // 调整缩放范围

    // 增加边距百分比，避免内容太靠边
    double marginPercent = 0.07; // 增加到7% 边距
    double marginMM = qMin(pageSizeMM.width(), pageSizeMM.height()) * marginPercent;

    // 转换为像素（在300 DPI下）
    double marginPixels = mmToPixels(marginMM, 300);

    // 调整各个区域的高度和间距
    m_layout.margins = QMarginsF(marginPixels, marginPixels, marginPixels, marginPixels + mmToPixels(10, 300)); // 底部增加额外边距
    m_layout.headerHeight = mmToPixels(20 * scale, 300);  // 增加头部高度
    m_layout.footerHeight = mmToPixels(12 * scale, 300);   // 增加页脚高度
    m_layout.lineSpacing = mmToPixels(3 * scale, 300);    // 增加行间距
    m_layout.sectionSpacing = mmToPixels(8 * scale, 300); // 增加段落间距
    m_layout.columnSpacing = mmToPixels(15 * scale, 300); // 增加列间距
}

double HospitalReportPrinter::mmToPixels(double mm, double dpi) const
{
    // 1 inch = 25.4 mm, 所以 pixels = (mm / 25.4) * dpi
    return (mm / 25.4) * dpi;
}

double HospitalReportPrinter::getScaleFactor() const
{
    double pageArea = m_pageSizeMM.width() * m_pageSizeMM.height();
    double baseArea = 210 * 297; // A4 面积
    return std::sqrt(pageArea / baseArea);
}

void HospitalReportPrinter::setPatientInfo(const PatientBasicInfo &info)
{
    m_patientInfo = info;
}

void HospitalReportPrinter::setTestResults(const QVector<TestResult> &results)
{
    m_testResults = results;
}

void HospitalReportPrinter::setCurveData(const QVector<CurveData> &curves)
{
    m_curveData = curves;
}

bool HospitalReportPrinter::printReport(const QString &title)
{
    QPrinter printer(QPrinter::HighResolution);
    setupPrinter(&printer);

    m_currentDPI = printer.resolution();
    updateLayoutForPageSize();

    QPainter painter;
    if (!painter.begin(&printer)) {
        qWarning() << "Failed to initialize painter for printing";
        return false;
    }

    try {
        double pageWidth = printer.pageRect(QPrinter::DevicePixel).width();
        double currentY = m_layout.margins.top();

        // 绘制报告头部
        drawHeader(painter, title, pageWidth);
        currentY += m_layout.headerHeight + m_layout.sectionSpacing * 2;

        // 绘制患者信息 - 使用固定高度
        drawPatientInfo(painter, currentY, pageWidth);
        currentY += mmToPixels(45, m_currentDPI); // 固定患者信息区域高度

        // 绘制检测结果表格
        if (!m_testResults.isEmpty()) {
            drawTestResults(painter, currentY, pageWidth);
            currentY += m_testResults.size() * mmToPixels(8, m_currentDPI) + m_layout.sectionSpacing * 3;
        }

        // 绘制曲线图
        if (!m_curveData.isEmpty()) {
            drawCurves(painter, currentY, pageWidth);
        }

        // 绘制页脚
        double pageHeight = printer.pageRect(QPrinter::DevicePixel).height();
        drawFooter(painter, pageHeight - m_layout.footerHeight - mmToPixels(10, m_currentDPI), pageWidth);

        painter.end();
        return true;

    } catch (const std::exception &e) {
        qWarning() << "Printing error:" << e.what();
        painter.end();
        return false;
    }
}

bool HospitalReportPrinter::printToPdf(const QString &fileName, const QString &title)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    setupPrinter(&printer);

    return printReport(title);
}

void HospitalReportPrinter::setupPrinter(QPrinter *printer)
{
    switch (m_paperSize) {
    case A4:
        printer->setPageSize(QPageSize(QPageSize::A4));
        break;
    case A5:
        printer->setPageSize(QPageSize(QPageSize::A5));
        break;
    case B5:
        printer->setPageSize(QPageSize(QPageSize::B5));
        break;
    case Letter:
        printer->setPageSize(QPageSize(QPageSize::Letter));
        break;
    case Legal:
        printer->setPageSize(QPageSize(QPageSize::Legal));
        break;
    case Custom:
        printer->setPageSize(QPageSize(m_pageSizeMM, QPageSize::Millimeter));
        break;
    }

    printer->setPageOrientation(m_orientation);
    printer->setFullPage(false);
    printer->setColorMode(QPrinter::Color);
    printer->setResolution(300);
}

// 自适应字体函数
QFont HospitalReportPrinter::getHeaderFont(double pageWidth) const
{
    double baseSize = 16.0;
    double scaledSize = baseSize * getScaleFactor();
    scaledSize = qBound(12.0, scaledSize, 20.0);
    return QFont("SimHei", scaledSize, QFont::Bold);
}

QFont HospitalReportPrinter::getTitleFont(double pageWidth) const
{
    double baseSize = 14.0;
    double scaledSize = baseSize * getScaleFactor();
    scaledSize = qBound(10.0, scaledSize, 16.0);
    return QFont("SimSun", scaledSize, QFont::Bold);
}

QFont HospitalReportPrinter::getSectionFont(double pageWidth) const
{
    double baseSize = 12.0;
    double scaledSize = baseSize * getScaleFactor();
    scaledSize = qBound(9.0, scaledSize, 14.0);
    return QFont("SimHei", scaledSize, QFont::Bold);
}

QFont HospitalReportPrinter::getContentFont(double pageWidth) const
{
    double baseSize = 10.0;
    double scaledSize = baseSize * getScaleFactor();
    scaledSize = qBound(8.0, scaledSize, 12.0);
    return QFont("SimSun", scaledSize);
}

QFont HospitalReportPrinter::getFooterFont(double pageWidth) const
{
    double baseSize = 8.0;
    double scaledSize = baseSize * getScaleFactor();
    scaledSize = qBound(6.0, scaledSize, 10.0);
    return QFont("SimSun", scaledSize);
}

void HospitalReportPrinter::drawHeader(QPainter &painter, const QString &title, double pageWidth)
{
    QPen originalPen = painter.pen();
    QFont originalFont = painter.font();

    // 使用自适应字体
    QFont headerFont = getHeaderFont(pageWidth);
    QFont titleFont = getTitleFont(pageWidth);

    painter.setFont(headerFont);
    painter.setPen(Qt::black);

    QRectF headerRect(m_layout.margins.left(), m_layout.margins.top(),
                     pageWidth - m_layout.margins.left() - m_layout.margins.right(),
                     m_layout.headerHeight);

    // 医院名称
    painter.drawText(headerRect, Qt::AlignTop | Qt::AlignHCenter, "XX医院检验科");

    // 报告标题
    painter.setFont(titleFont);
    painter.drawText(headerRect, Qt::AlignCenter, title);

    // 绘制横线
    drawHorizontalLine(painter,
                      m_layout.margins.left(),
                      pageWidth - m_layout.margins.right(),
                      m_layout.margins.top() + m_layout.headerHeight - 2);

    painter.setFont(originalFont);
    painter.setPen(originalPen);
}

void HospitalReportPrinter::drawPatientInfo(QPainter &painter, double yStart, double pageWidth)
{
    QFont labelFont = getSectionFont(pageWidth);
    QFont valueFont = getContentFont(pageWidth);
    double currentY = yStart;

    // 患者信息标题
    painter.setFont(QFont("SimHei", 12, QFont::Bold));
    painter.drawText(m_layout.margins.left(), currentY, "患者信息");
    currentY += painter.fontMetrics().height() + m_layout.lineSpacing * 3;

    // 创建清晰的网格布局
    double availableWidth = pageWidth - m_layout.margins.left() - m_layout.margins.right();

    // 定义4列布局
    int columns = 4;
    double columnWidth = availableWidth / columns;
    double rowHeight = painter.fontMetrics().height() + m_layout.lineSpacing * 2;

    // 第一行
    double row1Y = currentY;

    // 病历号
    drawFieldWithBackground(painter,
        m_layout.margins.left(), row1Y,
        columnWidth, rowHeight,
        "病历号", m_patientInfo.patientId,
        labelFont, valueFont);

    // 姓名
    drawFieldWithBackground(painter,
        m_layout.margins.left() + columnWidth, row1Y,
        columnWidth, rowHeight,
        "姓名", m_patientInfo.name,
        labelFont, valueFont);

    // 性别
    drawFieldWithBackground(painter,
        m_layout.margins.left() + columnWidth * 2, row1Y,
        columnWidth, rowHeight,
        "性别", m_patientInfo.gender,
        labelFont, valueFont);

    // 年龄
    drawFieldWithBackground(painter,
        m_layout.margins.left() + columnWidth * 3, row1Y,
        columnWidth, rowHeight,
        "年龄", QString::number(m_patientInfo.age),
        labelFont, valueFont);

    // 第二行
    double row2Y = row1Y + rowHeight;

    // 科室
    drawFieldWithBackground(painter,
        m_layout.margins.left(), row2Y,
        columnWidth, rowHeight,
        "科室", m_patientInfo.department,
        labelFont, valueFont);

    // 床号
    drawFieldWithBackground(painter,
        m_layout.margins.left() + columnWidth, row2Y,
        columnWidth, rowHeight,
        "床号", m_patientInfo.bedNumber,
        labelFont, valueFont);

    // 检测时间（占据两列）
    drawFieldWithBackground(painter,
        m_layout.margins.left() + columnWidth * 2, row2Y,
        columnWidth * 2, rowHeight,
        "检测时间", m_patientInfo.testTime.toString("yyyy-MM-dd hh:mm"),
        labelFont, valueFont);

    currentY = row2Y + rowHeight + m_layout.lineSpacing * 3;

    // 临床诊断（单独一行）
    painter.setFont(labelFont);
    painter.drawText(m_layout.margins.left(), currentY, "临床诊断：");

    painter.setFont(valueFont);
    QRectF diagnosisRect(m_layout.margins.left() + painter.fontMetrics().width("临床诊断：") + 10,
        currentY - painter.fontMetrics().height() + 5,
        availableWidth - painter.fontMetrics().width("临床诊断：") - 10,
        painter.fontMetrics().height() * 2);

    // 诊断背景
    painter.fillRect(diagnosisRect.adjusted(-5, -5, 5, 5), QColor(250, 250, 250));
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.drawRect(diagnosisRect.adjusted(-5, -5, 5, 5));

    painter.setPen(Qt::black);
    painter.drawText(diagnosisRect, Qt::TextWordWrap, m_patientInfo.diagnosis);

    currentY += diagnosisRect.height() + m_layout.sectionSpacing * 2;

    // 绘制分隔线
    drawHorizontalLine(painter, m_layout.margins.left(),
        pageWidth - m_layout.margins.right(), currentY);
}

// 辅助函数：绘制带背景的字段
void HospitalReportPrinter::drawFieldWithBackground(QPainter &painter,
                                                   double x, double y,
                                                   double width, double height,
                                                   const QString &label,
                                                   const QString &value,
                                                   const QFont &labelFont,
                                                   const QFont &valueFont)
{
    // 绘制背景
    painter.fillRect(QRectF(x, y, width, height), QColor(250, 250, 250));
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.drawRect(QRectF(x, y, width, height));

    // 绘制标签
    painter.setFont(labelFont);
    painter.setPen(Qt::black);
    painter.drawText(QRectF(x + 5, y + 5, width - 10, height/2 - 5),
                    Qt::AlignLeft | Qt::AlignBottom, label + "：");

    // 绘制值
    painter.setFont(valueFont);
    painter.drawText(QRectF(x + 5, y + height/2, width - 10, height/2 - 5),
                    Qt::AlignLeft | Qt::AlignTop, value);
}




void HospitalReportPrinter::drawTestResults(QPainter &painter, double yStart, double pageWidth)
{
    if (m_testResults.isEmpty()) return;

    QFont headerFont = getSectionFont(pageWidth);
    painter.setFont(headerFont);

    // 增加检测结果标题与上面内容的间距
    double titleY = yStart + m_layout.sectionSpacing;
    painter.drawText(m_layout.margins.left(), titleY, "检测结果");

    double tableTop = titleY + painter.fontMetrics().height() + m_layout.lineSpacing * 2; // 增加表格上方的间距
    double rowHeight = mmToPixels(7 * getScaleFactor(), m_currentDPI); // 增加行高
    double availableWidth = pageWidth - m_layout.margins.left() - m_layout.margins.right();

    // 调整列宽比例，给检测项目列更多空间
    double col1 = availableWidth * 0.35; // 增加检测项目列宽度
    double col2 = availableWidth * 0.15;
    double col3 = availableWidth * 0.15;
    double col4 = availableWidth * 0.35; // 增加参考范围列宽度

    // 表头字体
    QFont tableFont = getContentFont(pageWidth);
    painter.setFont(tableFont);

    // 绘制表头背景
    painter.fillRect(QRectF(m_layout.margins.left(), tableTop, availableWidth, rowHeight), QColor(240, 240, 240));

    // 绘制表头文字 - 确保文字在单元格内居中
    painter.drawText(QRectF(m_layout.margins.left(), tableTop, col1, rowHeight), Qt::AlignCenter, "检测项目");
    painter.drawText(QRectF(m_layout.margins.left() + col1, tableTop, col2, rowHeight), Qt::AlignCenter, "检测结果");
    painter.drawText(QRectF(m_layout.margins.left() + col1 + col2, tableTop, col3, rowHeight), Qt::AlignCenter, "单位");
    painter.drawText(QRectF(m_layout.margins.left() + col1 + col2 + col3, tableTop, col4, rowHeight), Qt::AlignCenter, "参考范围");

    // 绘制表格线
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(QRectF(m_layout.margins.left(), tableTop, availableWidth, rowHeight * (m_testResults.size() + 1)));

    // 绘制内部竖线
    painter.drawLine(m_layout.margins.left() + col1, tableTop,
                    m_layout.margins.left() + col1, tableTop + rowHeight * (m_testResults.size() + 1));
    painter.drawLine(m_layout.margins.left() + col1 + col2, tableTop,
                    m_layout.margins.left() + col1 + col2, tableTop + rowHeight * (m_testResults.size() + 1));
    painter.drawLine(m_layout.margins.left() + col1 + col2 + col3, tableTop,
                    m_layout.margins.left() + col1 + col2 + col3, tableTop + rowHeight * (m_testResults.size() + 1));

    // 绘制数据行
    for (int i = 0; i < m_testResults.size(); ++i) {
        const TestResult &result = m_testResults[i];
        double rowTop = tableTop + rowHeight * (i + 1);

        // 绘制内部横线
        painter.drawLine(m_layout.margins.left(), rowTop,
                        m_layout.margins.left() + availableWidth, rowTop);

        // 绘制单元格内容，确保文字不超出边界
        painter.drawText(QRectF(m_layout.margins.left() + 2, rowTop, col1 - 4, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, result.testItem);
        painter.drawText(QRectF(m_layout.margins.left() + col1 + 2, rowTop, col2 - 4, rowHeight),
                        Qt::AlignCenter, QString::number(result.resultValue, 'f', 2));
        painter.drawText(QRectF(m_layout.margins.left() + col1 + col2 + 2, rowTop, col3 - 4, rowHeight),
                        Qt::AlignCenter, result.unit);
        painter.drawText(QRectF(m_layout.margins.left() + col1 + col2 + col3 + 2, rowTop, col4 - 4, rowHeight),
                        Qt::AlignCenter, result.referenceRange);
    }
}

void HospitalReportPrinter::drawCurves(QPainter &painter, double yStart, double pageWidth)
{
   if (m_curveData.isEmpty()) return;

   QFont headerFont = getSectionFont(pageWidth);
   painter.setFont(headerFont);

   // 增加曲线图标题与上面内容的间距
   double titleY = yStart + m_layout.sectionSpacing * 2;
   painter.drawText(m_layout.margins.left(), titleY, "检测曲线图");

   double currentY = titleY + painter.fontMetrics().height() + m_layout.lineSpacing * 2;
   double availableWidth = pageWidth - m_layout.margins.left() - m_layout.margins.right();
   double imageHeight = mmToPixels(60 * getScaleFactor(), m_currentDPI); // 调整图片高度

   for (const CurveData &curve : m_curveData) {
       if (!curve.curveImage.isNull()) {
           // 绘制曲线标题
           QFont titleFont = getContentFont(pageWidth);
           titleFont.setBold(true);
           painter.setFont(titleFont);
           painter.drawText(m_layout.margins.left(), currentY, curve.curveTitle);
           currentY += painter.fontMetrics().height() + m_layout.lineSpacing;

           // 绘制曲线图（缩放适应）
           QPixmap scaledImage = curve.curveImage.scaled(availableWidth, imageHeight,
                                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);
           painter.drawPixmap(m_layout.margins.left(), currentY, scaledImage);
           currentY += scaledImage.height() + m_layout.lineSpacing * 2;

           // 绘制曲线分析 - 增加分析区域的间距
           if (!curve.analysis.isEmpty()) {
               QFont analysisFont = getContentFont(pageWidth);
               painter.setFont(analysisFont);
               QRectF analysisRect(m_layout.margins.left(), currentY, availableWidth,
                                 painter.fontMetrics().height() * 4); // 增加分析区域高度
               painter.drawText(analysisRect, Qt::TextWordWrap, "分析：" + curve.analysis);
               currentY += analysisRect.height() + m_layout.sectionSpacing * 2;
           }

           currentY += m_layout.sectionSpacing;
       }
   }

   // 在曲线图区域底部画一条分隔线
   drawHorizontalLine(painter, m_layout.margins.left(),
                     pageWidth - m_layout.margins.right(), currentY);
}

void HospitalReportPrinter::drawFooter(QPainter &painter, double yStart, double pageWidth)
{
    QFont footerFont = getFooterFont(pageWidth);
    painter.setFont(footerFont);
    painter.setPen(Qt::gray);

    QString footerText = QString("打印时间：%1    审核医生：__________    检验医生：__________")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));

    // 确保页脚有足够的边距
    double footerRectY = yStart - mmToPixels(2, m_currentDPI);
    double footerRectHeight = m_layout.footerHeight + mmToPixels(2, m_currentDPI);

    painter.drawText(QRectF(m_layout.margins.left(), footerRectY,
                           pageWidth - m_layout.margins.left() - m_layout.margins.right(),
                           footerRectHeight),
                    Qt::AlignRight | Qt::AlignVCenter, footerText);
}

double HospitalReportPrinter::drawTextSection(QPainter &painter, double x, double y,
                                            const QString &label, const QString &value,
                                            const QFont &labelFont, const QFont &valueFont)
{
    painter.setFont(labelFont);

    // 在Qt 5.10中使用width()替代horizontalAdvance()
    QFontMetrics labelMetrics(labelFont);
    double labelWidth = labelMetrics.width(label);

    painter.drawText(x, y, label);

    painter.setFont(valueFont);
    painter.drawText(x + labelWidth, y, value);

    return y + painter.fontMetrics().height() + m_layout.lineSpacing;
}

void HospitalReportPrinter::drawHorizontalLine(QPainter &painter, double xStart, double xEnd, double y)
{
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(xStart, y, xEnd, y);
}
