// 在文件最开头添加宏定义
#define ACCESS_MASK ACCESS_MASK

#include "testopcv.h"
#include "ui_testopcv.h"
#include "opencvfindheigh.h"
#include <QsLog/include/QsLog.h>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <globaldata.h>
#include "cglobal.h"
#include "ini_file.h"
#define BLOODLAYER_STATIC
#include "bloodlayer_api.h"
#include <QStandardPaths>
#include <cstring>
#include <QDir>
#include <QCoreApplication>
#include <QString>
#include <QPainter>
#include <QLinearGradient>
#include <QFontInfo>
#include <QFontMetrics>
#include <QImage>
#include <QLabel>
#include <QThread>
#include <QMetaObject>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSizePolicy>
#include <algorithm>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

using namespace cv;

namespace {
Mat rotateCcw90(const Mat &src)
{
    if (src.empty()) {
        return Mat();
    }
    Mat dst;
    cv::transpose(src, dst);
    cv::flip(dst, dst, 0);
    return dst;
}

struct ViewTag {
    Point pos;
    QString text;
    Scalar bgr;
    ViewTag() : pos(0, 0), bgr(0, 0, 0) {}
    ViewTag(const Point &p, const QString &t, const Scalar &c)
        : pos(p), text(t), bgr(c) {}
};

QColor tagRgb(const Scalar &bgr)
{
    return QColor(qBound(0, static_cast<int>(bgr[2]), 255),
                  qBound(0, static_cast<int>(bgr[1]), 255),
                  qBound(0, static_cast<int>(bgr[0]), 255));
}

struct HudGeom {
    bool valid;
    bool horizontal;
    Rect ref;
    Rect tube;
    Point ifacePt;
    int packedLeft;
    double packedMm;
    HudGeom()
        : valid(false)
        , horizontal(false)
        , packedLeft(0)
        , packedMm(0.0) {}
};

Rect s_tubeRoi;

QString overlayRoiHint()
{
    return QString::fromUtf8("绿框参照物，橙线血浆分层");
}

QPoint toViewPoint(int x, int y, int srcCols, bool rotate)
{
    if (!rotate) {
        return QPoint(x, y);
    }
    return QPoint(y, srcCols - 1 - x);
}

void drawGlowLine(QPainter &p, const QLine &line, const QColor &color, int coreWidth)
{
    QPen glow(color);
    glow.setCapStyle(Qt::FlatCap);
    glow.setColor(QColor(color.red(), color.green(), color.blue(), 55));
    glow.setWidth(coreWidth + 8);
    p.setPen(glow);
    p.drawLine(line);
    glow.setColor(QColor(color.red(), color.green(), color.blue(), 110));
    glow.setWidth(coreWidth + 3);
    p.setPen(glow);
    p.drawLine(line);
    QPen core(color, coreWidth);
    core.setCapStyle(Qt::FlatCap);
    p.setPen(core);
    p.drawLine(line);
}

void paintVisionHud(QPainter &p, const QSize &sz, const QString &caption, const QString &rightText)
{
    const int w = sz.width();
    const int h = sz.height();
    if (w < 40 || h < 40) {
        return;
    }

    p.setPen(Qt::NoPen);
    for (int y = 0; y < h; y += 6) {
        p.fillRect(0, y, w, 1, QColor(64, 224, 208, 14));
    }

    const QColor accent(64, 224, 208);
    QPen pen(QColor(64, 224, 208, 220), 2);
    pen.setCapStyle(Qt::SquareCap);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    const int m = 12;
    const int len = 30;
    p.drawLine(m, m, m + len, m);
    p.drawLine(m, m, m, m + len);
    p.drawLine(w - m - len, m, w - m, m);
    p.drawLine(w - m, m, w - m, m + len);
    p.drawLine(m, h - m, m + len, h - m);
    p.drawLine(m, h - m - len, m, h - m);
    p.drawLine(w - m - len, h - m, w - m, h - m);
    p.drawLine(w - m, h - m - len, w - m, h - m);
    p.setPen(QPen(QColor(64, 224, 208, 80), 1));
    p.drawRect(m + 4, m + 4, w - 2 * m - 8, h - 2 * m - 8);

    const int barH = 36;
    QLinearGradient g(0, h - barH, 0, h);
    g.setColorAt(0, QColor(6, 14, 18, 210));
    g.setColorAt(1, QColor(4, 10, 14, 230));
    p.fillRect(0, h - barH, w, barH, g);
    p.setPen(QPen(QColor(64, 224, 208, 180), 1));
    p.drawLine(0, h - barH, w, h - barH);
    p.fillRect(0, h - barH, 3, barH, accent);
    QFont barFont(QString::fromUtf8("微软雅黑"), 10, QFont::Bold);
    p.setFont(barFont);
    p.setPen(QColor(214, 240, 236));
    const QString text = caption.isEmpty()
            ? QString::fromUtf8("视觉识别  ·  血浆分层")
            : caption;
    p.drawText(QRect(16, h - barH, w / 2, barH),
               Qt::AlignVCenter | Qt::AlignLeft, text);
    if (!rightText.isEmpty()) {
        p.setPen(accent);
        p.drawText(QRect(w / 2, h - barH, w / 2 - 14, barH),
                   Qt::AlignVCenter | Qt::AlignRight, rightText);
    }
}

struct LastPreview {
    Mat bgr;
    std::vector<ViewTag> tags;
    QString caption;
    HudGeom geom;
    bool hasGeom;
};
LastPreview s_lastPreview;
bool s_replayBusy = false;

void presentLandscapeOnLabel(const Mat &landscapeBgr, QLabel *pshow,
                             const std::vector<ViewTag> &landscapeTags,
                             const QString &caption,
                             const HudGeom *geom)
{
    if (!pshow || landscapeBgr.empty()) {
        return;
    }
    if (!s_replayBusy) {
        s_lastPreview.bgr = landscapeBgr.clone();
        s_lastPreview.tags = landscapeTags;
        s_lastPreview.caption = caption;
        s_lastPreview.hasGeom = (geom != 0);
        if (geom) {
            s_lastPreview.geom = *geom;
        }
    }

    const bool rotateView = landscapeBgr.cols > landscapeBgr.rows;
    Mat view = rotateView ? rotateCcw90(landscapeBgr) : landscapeBgr.clone();
    if (view.empty()) {
        return;
    }

    Mat rgb;
    cvtColor(view, rgb, COLOR_BGR2RGB);
    QImage qImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step),
                  QImage::Format_RGB888);
    QImage canvas = qImage.copy();

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    const int srcCols = landscapeBgr.cols;
    const int W = canvas.width();
    const int H = canvas.height();

    if (geom && geom->valid && geom->horizontal && rotateView) {
        const int yRefTop = toViewPoint(geom->ref.x + geom->ref.width, 0, srcCols, true).y();
        const int yRefBot = toViewPoint(geom->ref.x, 0, srcCols, true).y();
        const int yIface = toViewPoint(geom->ifacePt.x, 0, srcCols, true).y();
        const int yPacked = toViewPoint(geom->packedLeft, 0, srcCols, true).y();

        QPen dash(QColor(80, 255, 210), 2, Qt::DashLine);
        painter.setPen(dash);
        painter.drawLine(0, yRefTop, W, yRefTop);
        drawGlowLine(painter, QLine(0, yRefTop, W, yRefTop), QColor(80, 255, 210), 2);

        painter.setPen(QPen(QColor(80, 255, 210), 1, Qt::DotLine));
        painter.drawLine(0, yRefBot, W, yRefBot);

        drawGlowLine(painter, QLine(0, yIface, W, yIface), QColor(255, 140, 50), 2);

        const int xDim = W - 28;
        const int y0 = qMin(yIface, yPacked);
        const int y1 = qMax(yIface, yPacked);
        painter.setPen(QPen(QColor(255, 210, 90), 1));
        painter.drawLine(xDim, y0, xDim, y1);
        painter.drawLine(xDim - 6, y0, xDim + 6, y0);
        painter.drawLine(xDim - 6, y1, xDim + 6, y1);
        if (geom->packedMm > 0.0) {
            QFont mmFont(QString::fromUtf8("微软雅黑"), 11, QFont::Bold);
            painter.setFont(mmFont);
            const QString mm = QString::fromUtf8("%1 mm").arg(geom->packedMm, 0, 'f', 2);
            QRect box(xDim - 78, (y0 + y1) / 2 - 12, 72, 24);
            painter.setBrush(QColor(8, 12, 16, 200));
            painter.setPen(QPen(QColor(255, 210, 90), 1));
            painter.drawRoundedRect(box, 3, 3);
            painter.drawText(box, Qt::AlignCenter, mm);
        }
    }

    QFont font(QString::fromUtf8("微软雅黑"), 11, QFont::Bold);
    if (!QFontInfo(font).exactMatch()) {
        font = QFont(QString::fromUtf8("楷体"), 12, QFont::Bold);
    }
    painter.setFont(font);
    const QFontMetrics metrics(font);
    for (size_t i = 0; i < landscapeTags.size(); ++i) {
        const ViewTag &tag = landscapeTags[i];
        QPoint pos = toViewPoint(tag.pos.x, tag.pos.y, srcCols, rotateView);
        const int textW = metrics.width(tag.text);
        const int pillW = textW + 20;
        const int pillH = 24;
        const int x = qBound(10, pos.x() + 8, W - pillW - 10);
        const int y = qBound(10, pos.y() - pillH - 6, H - 48);
        const QColor color = tagRgb(tag.bgr);
        QRect pill(x, y, pillW, pillH);
        painter.setPen(QPen(color, 1));
        painter.setBrush(QColor(8, 14, 18, 220));
        painter.drawRoundedRect(pill, 4, 4);
        painter.setPen(color);
        painter.drawText(pill, Qt::AlignCenter, tag.text);
    }

    QString right;
    if (geom && geom->valid && geom->packedMm > 0.0) {
        right = QString::fromUtf8("红细胞高 %1 mm").arg(geom->packedMm, 0, 'f', 2);
    }
    paintVisionHud(painter, canvas.size(), caption, right);
    painter.end();

    QPixmap pixmap = QPixmap::fromImage(canvas);
    QSize targetSize = pshow->size();
    if (QWidget *stage = pshow->parentWidget()) {
        const QSize host = stage->contentsRect().size();
        if (host.width() > targetSize.width()) {
            targetSize.setWidth(host.width());
        }
        if (host.height() > targetSize.height()) {
            targetSize.setHeight(host.height());
        }
    }
    // 开机后控件往往还没布局完，size 只有几十像素；按这个去缩放会缩成指甲盖。
    // 区域太小时保持转正后的分辨率，等 show/resize 再按真实区域重绘。
    const bool layoutReady = (targetSize.width() >= 200 && targetSize.height() >= 200);
    if (layoutReady) {
        pixmap = pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        QLOG_WARN() << "[测高预览] 显示区尚未布局" << pshow->width() << "x" << pshow->height()
                    << " 父控件=" << (pshow->parentWidget() ? pshow->parentWidget()->width() : 0)
                    << "x" << (pshow->parentWidget() ? pshow->parentWidget()->height() : 0)
                    << " 暂按原图" << canvas.width() << "x" << canvas.height();
    }
    pshow->clear();
    pshow->setScaledContents(false);
    pshow->setAlignment(Qt::AlignCenter);
    pshow->setPixmap(pixmap);
    pshow->update();
}

void replayVisionPreview(QLabel *pshow)
{
    if (s_replayBusy || !pshow || s_lastPreview.bgr.empty()) {
        return;
    }
    s_replayBusy = true;
    presentLandscapeOnLabel(s_lastPreview.bgr, pshow, s_lastPreview.tags,
                            s_lastPreview.caption,
                            s_lastPreview.hasGeom ? &s_lastPreview.geom : 0);
    s_replayBusy = false;
}

QString agentMetricCell(const QString &label, const QString &value, const QString &unit)
{
    return QString::fromUtf8(
                "<td width='33%' bgcolor='#E8F4FD'>"
                "<div style='color:#5A8BB0; font-size:11px; padding:6px 8px 0 8px;'>%1</div>"
                "<div style='color:#2C5F8A; font-size:18px; font-weight:bold; padding:0 8px 8px 8px;'>"
                "%2<span style='font-size:11px; color:#5A8BB0; font-weight:normal;'> %3</span></div>"
                "</td>")
            .arg(label)
            .arg(value)
            .arg(unit);
}

QString agentInfoHtml(const QString &dotColor,
                      const QString &status,
                      const QString &subtitle,
                      bool showMetrics,
                      const QString &packedMm,
                      const QString &needleMm,
                      const QString &hemoText,
                      const QString &note)
{
    QString html = QString::fromUtf8(
                "<div style='font-family:Microsoft YaHei; color:#2C5F8A;'>"
                "<div style='font-size:13px;'><span style='color:%1;'>●</span>  %2</div>"
                "<div style='color:#5A8BB0; font-size:11px; margin:2px 0 6px 0;'>%3</div>")
            .arg(dotColor)
            .arg(status)
            .arg(subtitle);
    if (showMetrics) {
        html += QString::fromUtf8("<table width='100%' cellspacing='6' cellpadding='0'><tr>");
        html += agentMetricCell(QString::fromUtf8("红细胞高度"), packedMm, QStringLiteral("mm"));
        html += agentMetricCell(QString::fromUtf8("下针高度"), needleMm, QStringLiteral("mm"));
        html += agentMetricCell(QString::fromUtf8("溶血"), hemoText, QString());
        html += QString::fromUtf8("</tr></table>");
    }
    if (!note.isEmpty()) {
        html += QString::fromUtf8(
                    "<div style='color:#C0392B; font-size:12px; margin-top:4px;'>%1</div>")
                .arg(note);
    }
    html += QString::fromUtf8("</div>");
    return html;
}

void applyAgentPanelStyle(QWidget *root, QLabel *info, QWidget *stage, QLabel *image)
{
    if (root) {
        root->setStyleSheet(
                    "QWidget#TestOpcv {"
                    " background-color: #F0F7FC;"
                    "}");
    }
    if (info) {
        info->setTextFormat(Qt::RichText);
        info->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        info->setWordWrap(true);
        info->setStyleSheet(
                    "QLabel#label_ratio {"
                    " background-color: #FFFFFF;"
                    " color: #2C5F8A;"
                    " padding: 10px 12px;"
                    " border: 1px solid #D4EAF5;"
                    " border-left: 3px solid #5BB8E8;"
                    "}");
    }
    if (stage) {
        stage->setAutoFillBackground(true);
        stage->setStyleSheet(
                    "QWidget#widgetShowImag {"
                    " background-color: #FFFFFF;"
                    " border: 1px solid #D4EAF5;"
                    "}");
    }
    if (image) {
        image->setAlignment(Qt::AlignCenter);
        image->setScaledContents(false);
        image->setStyleSheet(
                    "QLabel#label_showimage {"
                    " background-color: #FFFFFF;"
                    " border: none;"
                    " color: #5A8BB0;"
                    "}");
        QFont emptyFont;
        emptyFont.setFamily(QString::fromUtf8("微软雅黑"));
        emptyFont.setPixelSize(14);
        image->setFont(emptyFont);
    }
}
}


TestOpcv::TestOpcv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestOpcv),
    redBloodCellHeight(0),
    pixelToMmRatio(0.0),
    referenceObjectRect(0,0,0,0),
    redBloodCellHeightMm(0.0),
    maxNeedleDropHeight(0.0),
    referenceToBottomDistance(0.0),
    m_horizontalTube(false),
    m_hemolysisStatus(QStringLiteral("Unknown")),
    m_refTopX(-1.0),
    m_camera(nullptr),
    m_cameraThread(nullptr)
{
    ui->setupUi(this);
    initshowimg();
    startCameraPipeline();
    ui->widget_bar->hide();
}


TestOpcv::~TestOpcv()
{
    stopCameraPipeline();
#ifdef DEBUG
    cleanupDebugWindows();
#endif
    delete ui;
}

void TestOpcv::startCameraPipeline()
{
    if (m_camera) {
        return;
    }
    m_camera = new opencvfindHeigh();
    m_cameraThread = new QThread(this);
    m_camera->moveToThread(m_cameraThread);
    connect(m_cameraThread, &QThread::started, m_camera, &opencvfindHeigh::Start);
    connect(m_camera, &opencvfindHeigh::obtainPRPImage,
            this, &TestOpcv::onCapturedImage, Qt::QueuedConnection);
    connect(m_camera, &opencvfindHeigh::reOpencvImageTubePRP,
            this, &TestOpcv::onRetestImage, Qt::QueuedConnection);
    connect(m_camera, &opencvfindHeigh::FindFailed, this,
            [this](QString errorType, QString errorMessage) {
                emit captureFailed(errorType, errorMessage);
            });
    connect(m_camera, &opencvfindHeigh::Testheightfinish,
            this, &TestOpcv::captureFinished, Qt::QueuedConnection);
    m_cameraThread->start();
}

void TestOpcv::stopCameraPipeline()
{
    if (m_camera && m_cameraThread && m_cameraThread->isRunning()) {
        QMetaObject::invokeMethod(m_camera, "setCameraPower",
                                  Qt::BlockingQueuedConnection,
                                  Q_ARG(bool, false));
        m_cameraThread->quit();
        m_cameraThread->wait(2000);
    }
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }
    m_cameraThread = nullptr;
}

bool TestOpcv::hasOpenedCamera() const
{
    return m_camera && m_camera->hasOpenedCamera();
}

void TestOpcv::triggerCaptureAndRecognize()
{
    if (!m_camera) {
        emit captureFailed(tr("模块错误"), tr("测高摄像头未初始化"));
        return;
    }
    QMetaObject::invokeMethod(m_camera, "handleTriggerTestHeight", Qt::QueuedConnection);
}

void TestOpcv::setCameraPower(bool enable)
{
    if (!m_camera) {
        return;
    }
    QMetaObject::invokeMethod(m_camera, "setCameraPower", Qt::QueuedConnection,
                              Q_ARG(bool, enable));
}

void TestOpcv::recvfindCameraIndexByDevicePath(const QString &devicePath)
{
    if (!m_camera) {
        return;
    }
    QMetaObject::invokeMethod(m_camera, "recvfindCameraIndexByDevicePath",
                              Qt::QueuedConnection,
                              Q_ARG(QString, devicePath));
}

void TestOpcv::handleRepPrpheight(const QString &numid, bool replflag)
{
    if (!m_camera) {
        return;
    }
    QMetaObject::invokeMethod(m_camera, "handleRepPrpheight", Qt::QueuedConnection,
                              Q_ARG(QString, numid),
                              Q_ARG(bool, replflag));
}

void TestOpcv::onCapturedImage(const QString &imagePath)
{
    emit beginNewMeasure();
    handleSycnOpendcvImage(imagePath);
}

void TestOpcv::onRetestImage(const QString &reId, const QString &imagePath)
{
    emit replaceSampleId(reId);
    handleSycnOpendcvImage(imagePath);
}


// 调试窗口管理：用于跟踪创建的窗口，以便在退出时清理
#ifdef DEBUG
#include <set>
std::set<std::string> g_debugWindows;

void debugImshow(std::string name, Mat &image){
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    // 设置窗口大小（宽度、高度）
    cv::resizeWindow(name, 120, 640);
    // 显示图像
    cv::imshow(name, image);
    // 记录窗口名称以便后续清理
    g_debugWindows.insert(name);
}

// 清理所有调试窗口
void cleanupDebugWindows() {
    for (const auto& winName : g_debugWindows) {
        cv::destroyWindow(winName);
    }
    g_debugWindows.clear();
}
#else
// 非调试模式：空函数，不执行任何操作
#define debugImshow(name, img) ((void)0)
#define cleanupDebugWindows() ((void)0)
#endif

void TestOpcv::initshowimg()
{
    setObjectName(QStringLiteral("TestOpcv"));
    applyAgentPanelStyle(this, ui->label_ratio, ui->widgetShowImag, ui->label_showimage);
    ui->widgetShowImag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->label_showimage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->label_showimage->setMinimumSize(1, 1);
    ui->label_ratio->setText(agentInfoHtml(
                                 QStringLiteral("#2E86C1"),
                                 QString::fromUtf8("视觉识别  ·  待命"),
                                 QString::fromUtf8("等待接入测高画面"),
                                 false, QString(), QString(), QString(), QString()));
    ui->label_showimage->setText(QString::fromUtf8("等待接入画面"));
    ui->label_ratio->setMinimumHeight(72);
}

void TestOpcv::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    replayVisionPreview(ui->label_showimage);
}

void TestOpcv::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    replayVisionPreview(ui->label_showimage);
}

/** 显示图片：横图逆时针转 90°，试管竖着看。识别仍用未旋转的原图。 **/
void displayImage(cv::Mat cvImage,QLabel *pshow)
{
    presentLandscapeOnLabel(cvImage, pshow, std::vector<ViewTag>(),
                            QString::fromUtf8("视觉识别  ·  实时画面"), 0);
}

void TestOpcv::showImage(Mat destImage){
   displayImage(destImage, ui->label_showimage);
}

void TestOpcv::calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio)
{
    if (pixelToMmRatio <= 0) {
        QLOG_ERROR() << "错误: 像素到毫米比例无效";
        return;
    }

    // 获取INI配置
    auto &ini = INI_File();

    //redBloodCellHeightMm = rbcHeightPixels / pixelToMmRatio; //红细胞高度
    referenceToBottomDistance = ini.GetFixedHigh(); //参照物top到针高度
    double safetyMargin = ini.GetTestDifference();  //偏移高度
    double rotb  =   ini.getRefBottomDistance();   //参照物到底部距离

    //  红细胞top与参照物top像素差（实际偏差像素）  = 红细胞TOP像素 - 参照物TOP像素
    double interfaceToReferenceTopPixels = interfaceY - referenceObjectRect.y;
    //像素差实际高度mm(像素/比列) （实际偏差高度）
    double interfaceToReferenceTopMm = interfaceToReferenceTopPixels / pixelToMmRatio;
    QLOG_DEBUG()<<"（实际偏差高度）"<<interfaceToReferenceTopMm;

    //针到参照物top的高度mm + （实际偏差高度）
    double totalDropDistance = rotb - interfaceToReferenceTopMm - RAISETHERULER; //输出血样高度
    redBloodCellHeightMm = max(0.0, totalDropDistance);
    redBloodCellHeightMm = round(redBloodCellHeightMm * 100) / 100; //保留2位小数

    maxNeedleDropHeight = referenceToBottomDistance + rotb - redBloodCellHeightMm - safetyMargin - RAISETHERULER;

    QLOG_DEBUG() << "计算参数:";
    QLOG_DEBUG() << "红细胞高度像素: " << rbcHeightPixels << "px";
    QLOG_DEBUG() << "像素到毫米比例: " << pixelToMmRatio << "px/mm";
    QLOG_DEBUG() << "红细胞高度: " << redBloodCellHeightMm << "mm";
    QLOG_DEBUG() << "分界面Y坐标: " << interfaceY << "px";
    QLOG_DEBUG() << "参照物顶部Y坐标: " << referenceObjectRect.y << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopPixels << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopMm << "mm";
    QLOG_DEBUG() << "参照物到针底部距离: " << referenceToBottomDistance << "mm";
    QLOG_DEBUG() << "下针最大下降高度: " << maxNeedleDropHeight << "mm"<<endl;
}

void TestOpcv::showFailureWithReference(const QString& title, const QString& reason)
{
    QLOG_WARN() << "[血浆分层] 失败" << title << reason
                << " 横放=" << m_horizontalTube
                << " 图" << (imageOrinin.empty() ? 0 : imageOrinin.cols)
                << "x" << (imageOrinin.empty() ? 0 : imageOrinin.rows)
                << " 参照物" << referenceObjectRect.x << referenceObjectRect.y
                << referenceObjectRect.width << "x" << referenceObjectRect.height
                << " 分界" << detectedInterface.x << "," << detectedInterface.y
                << " 细节" << m_layerFailDetail;
    QString message = reason;
    QString detail = m_layerFailDetail;
    const QString ctx = QString::fromUtf8(
                "横放=%1  图=%2x%3\n参照物=(%4,%5) %6x%7\n分界=(%8,%9)")
            .arg(m_horizontalTube ? 1 : 0)
            .arg(imageOrinin.empty() ? 0 : imageOrinin.cols)
            .arg(imageOrinin.empty() ? 0 : imageOrinin.rows)
            .arg(referenceObjectRect.x)
            .arg(referenceObjectRect.y)
            .arg(referenceObjectRect.width)
            .arg(referenceObjectRect.height)
            .arg(detectedInterface.x)
            .arg(detectedInterface.y);
    if (detail.isEmpty()) {
        detail = ctx;
    } else {
        detail = ctx + QLatin1Char('\n') + detail;
    }
    ui->label_ratio->setText(agentInfoHtml(
                                 QStringLiteral("#ff8b6a"),
                                 QString::fromUtf8("视觉识别  ·  未完成"),
                                 reason,
                                 false, QString(), QString(), QString(),
                                 QString()));
    if (!imageOrinin.empty()
            && referenceObjectRect.width > 0
            && referenceObjectRect.height > 0) {
        Mat preview = imageOrinin.clone();
        markResultsOnOriginalImage(preview, Point(-1, -1), 0, referenceObjectRect);
        std::vector<ViewTag> tags;
        tags.push_back(ViewTag(Point(referenceObjectRect.x, referenceObjectRect.y),
                               QString::fromUtf8("参照物"), Scalar(80, 220, 60)));
        HudGeom failGeom;
        failGeom.valid = true;
        failGeom.horizontal = m_horizontalTube;
        failGeom.ref = referenceObjectRect;
        failGeom.tube = s_tubeRoi;
        presentLandscapeOnLabel(preview, ui->label_showimage, tags,
                                QString::fromUtf8("视觉识别  ·  需复核"), &failGeom);
        ui->label_ratio->setText(agentInfoHtml(
                                     QStringLiteral("#ff8b6a"),
                                     QString::fromUtf8("视觉识别  ·  未完成"),
                                     reason,
                                     false, QString(), QString(), QString(),
                                     overlayRoiHint()));
        message += QString::fromUtf8("\n\n图中绿色框为已识别的参照物，请核对位置是否正确。");
    }
    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(title);
    box.setText(message);
    box.setInformativeText(QString::fromUtf8("可展开「显示详细信息」查看识别过程。"));
    box.setDetailedText(detail);
    box.setStandardButtons(QMessageBox::Ok);
    box.setDefaultButton(QMessageBox::Ok);
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowModality(Qt::WindowModal);
    box.show();
    box.raise();
    box.activateWindow();
    box.exec();
}

void TestOpcv::markResultsOnOriginalImage(Mat& originalImage, const Point& interfacePoint,
                                          int rbcHeight, const Rect& referenceRect)
{
     Q_UNUSED(rbcHeight);
     if (originalImage.empty()) {
         return;
     }

     // 横放试管：血浆分层是竖直线（红细胞在左、血浆在右）
     if (m_horizontalTube && interfacePoint.x >= 0 && interfacePoint.x < originalImage.cols) {
         line(originalImage, Point(interfacePoint.x, 0),
              Point(interfacePoint.x, originalImage.rows - 1),
              Scalar(255, 255, 255), 4, LINE_AA);
         line(originalImage, Point(interfacePoint.x, 0),
              Point(interfacePoint.x, originalImage.rows - 1),
              Scalar(40, 90, 255), 2, LINE_AA);
     } else if (!m_horizontalTube && interfacePoint.y >= 0 && interfacePoint.y < originalImage.rows) {
         line(originalImage, Point(0, interfacePoint.y),
              Point(originalImage.cols, interfacePoint.y),
              Scalar(255, 255, 255), 4, LINE_AA);
         line(originalImage, Point(0, interfacePoint.y),
              Point(originalImage.cols, interfacePoint.y),
              Scalar(40, 90, 255), 2, LINE_AA);
     }

     if (referenceRect.width > 0 && referenceRect.height > 0) {
         Rect safeRef = referenceRect & Rect(0, 0, originalImage.cols, originalImage.rows);
         if (safeRef.width > 0 && safeRef.height > 0) {
             Mat overlay = originalImage.clone();
             rectangle(overlay, safeRef, Scalar(60, 220, 80), FILLED);
             addWeighted(overlay, 0.18, originalImage, 0.82, 0, originalImage);
             rectangle(originalImage, safeRef, Scalar(60, 220, 80), 2, LINE_AA);
         }

         if (m_horizontalTube) {
             // 竖看时的「参照顶」：横图里是参照物右端，转 90° 后变成横线。
             const int refTopX = qBound(0, referenceRect.x + referenceRect.width - 1,
                                        originalImage.cols - 1);
             line(originalImage, Point(refTopX, 0),
                  Point(refTopX, originalImage.rows - 1),
                  Scalar(255, 255, 255), 4, LINE_AA);
             line(originalImage, Point(refTopX, 0),
                  Point(refTopX, originalImage.rows - 1),
                  Scalar(80, 255, 210), 2, LINE_AA);
         } else {
             const int datumY = referenceRect.y;
             if (datumY >= 0 && datumY < originalImage.rows) {
                 line(originalImage, Point(0, datumY),
                      Point(originalImage.cols - 1, datumY),
                      Scalar(255, 255, 255), 3, LINE_AA);
                 line(originalImage, Point(0, datumY),
                      Point(originalImage.cols - 1, datumY),
                      Scalar(80, 255, 210), 2, LINE_AA);
             }
         }
     }
}

void TestOpcv::displayResults(const double& khemolysisIndex)
{
    const bool hasInterface = m_horizontalTube
            ? (detectedInterface.x >= 0)
            : (detectedInterface.y >= 0);
    if (processedImage.empty() || !hasInterface || pixelToMmRatio <= 0.0) {
       QMessageBox::warning(this, "错误", "未找到有效结果");
       return;
   }

   Mat resultImage = imageOrinin.clone();
   markResultsOnOriginalImage(resultImage, detectedInterface, redBloodCellHeight, referenceObjectRect);
   std::vector<ViewTag> tags;
   if (referenceObjectRect.width > 0) {
       const int refTopX = referenceObjectRect.x + referenceObjectRect.width;
       tags.push_back(ViewTag(Point(refTopX, qMax(0, referenceObjectRect.y)),
                              QString::fromUtf8("参照顶"), Scalar(80, 255, 210)));
   }
   if (m_horizontalTube && detectedInterface.x >= 0) {
       tags.push_back(ViewTag(Point(detectedInterface.x, detectedInterface.y),
                              QString::fromUtf8("血浆分层"), Scalar(40, 90, 255)));
   } else if (!m_horizontalTube && detectedInterface.y >= 0) {
       tags.push_back(ViewTag(Point(10, detectedInterface.y),
                              QString::fromUtf8("血浆分层"), Scalar(40, 90, 255)));
   }
   HudGeom geom;
   geom.valid = true;
   geom.horizontal = m_horizontalTube;
   geom.ref = referenceObjectRect;
   geom.tube = s_tubeRoi;
   geom.ifacePt = detectedInterface;
   geom.packedLeft = m_horizontalTube
           ? qMax(0, detectedInterface.x - redBloodCellHeight)
           : 0;
   geom.packedMm = redBloodCellHeightMm;
   presentLandscapeOnLabel(resultImage, ui->label_showimage, tags,
                           QString::fromUtf8("视觉识别  ·  识别完成"), &geom);

   const QString hemolysisPct = QString::number(khemolysisIndex * 100.0, 'f', 1);
   const bool hemolyzed = (m_hemolysisStatus == QStringLiteral("Hemolyzed"));
   const QString hemolysisText = hemolyzed
           ? QString::fromUtf8("疑似")
           : QString::fromUtf8("未见");
   QString note;
   if (hemolyzed) {
       if (!note.isEmpty()) {
           note += QString::fromUtf8("；");
       }
       note += QString::fromUtf8("请检查血浆是否溶血");
   }
   if (!m_horizontalTube && maxInfoColor == "Black") {
       if (!note.isEmpty()) {
           note += QString::fromUtf8("；");
       }
       note += QString::fromUtf8("请检查血细胞有压积层");
   }
   ui->label_ratio->setText(agentInfoHtml(
                                hemolyzed ? QStringLiteral("#ff8b6a") : QStringLiteral("#5ee0a0"),
                                QString::fromUtf8("视觉识别  ·  完成"),
                                QString::fromUtf8("血浆分层已锁定，下针高度已下发"),
                                true,
                                QString::number(redBloodCellHeightMm, 'f', 2),
                                QString::number(maxNeedleDropHeight, 'f', 2),
                                hemolysisText + QStringLiteral(" ") + hemolysisPct + QStringLiteral("%"),
                                note));

   QString redBloodCellHeightMmstr = QString("%1").arg(redBloodCellHeightMm, 0, 'f', 2);
   emit imageoutResult(redBloodCellHeightMmstr);
   QLOG_INFO() << "[血浆分层] 已下发高度" << redBloodCellHeightMmstr << "mm"
               << " 横放=" << m_horizontalTube;
}

void TestOpcv::handleSycnOpendcvImage(const QString &imagePath){
    ui->label_ratio->setText(agentInfoHtml(
                                 QStringLiteral("#2E86C1"),
                                 QString::fromUtf8("视觉识别  ·  画面已接入"),
                                 QString::fromUtf8("正在准备血浆分层"),
                                 false, QString(), QString(), QString(), QString()));

    // 路径安全校验：防止路径遍历攻击
    QFileInfo fileInfo(imagePath);
    QString canonicalPath = fileInfo.canonicalFilePath();

    // 验证路径不为空
    if (canonicalPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "无效的文件路径");
        return;
    }

    QStringList allowedDirs;
    allowedDirs << QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    allowedDirs << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    allowedDirs << QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    allowedDirs << QDir::currentPath();
    const QString appDir = QCoreApplication::applicationDirPath();
    allowedDirs << appDir;
    allowedDirs << QDir(appDir).filePath(QStringLiteral("FindHighImage"));

    const QString canonicalUnix = QDir::fromNativeSeparators(canonicalPath);
    bool pathAllowed = false;
    foreach (const QString& allowedDir, allowedDirs) {
        if (allowedDir.isEmpty()) {
            continue;
        }
        const QString allowedCanon = QDir::fromNativeSeparators(QDir(allowedDir).canonicalPath());
        if (allowedCanon.isEmpty()) {
            continue;
        }
        if (canonicalUnix.compare(allowedCanon, Qt::CaseInsensitive) == 0
            || canonicalUnix.startsWith(allowedCanon + QLatin1Char('/'), Qt::CaseInsensitive)) {
            pathAllowed = true;
            break;
        }
    }

    if (!pathAllowed) {
        QMessageBox::warning(this, "错误", "不允许访问此目录");
        QLOG_WARN() << "非法路径访问尝试: " << imagePath;
        return;
    }

    // 检查文件扩展名
    QString ext = fileInfo.suffix().toLower();
    QStringList allowedExts = {"jpg", "jpeg", "png", "bmp", "pgm", "pbm"};
    if (!allowedExts.contains(ext)) {
        QMessageBox::warning(this, "错误", "不支持的文件格式");
        return;
    }

    QFile file(canonicalPath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "错误", "无法加载图像文件");
        return;
    }

    // 检查文件大小，防止DoS攻击（限制最大10MB）
    const qint64 MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB
    qint64 fileSize = file.size();
    if (fileSize > MAX_FILE_SIZE) {
        QMessageBox::warning(this, "错误", "图像文件过大，最大支持10MB");
        QLOG_WARN() << "文件过大: " << fileSize << " bytes";
        return;
    }

    QByteArray imageData = file.readAll();
    Mat img = imdecode(Mat(1, imageData.size(), CV_8UC1, imageData.data()), IMREAD_COLOR);
    if (img.empty()) {
        QMessageBox::warning(this, "错误", "无法解码图像文件");
        QLOG_WARN() << "[血浆分层] 无法解码" << canonicalPath;
        return;
    }

    QLOG_INFO() << "[血浆分层] 读图" << canonicalPath
                << " 原始" << img.cols << "x" << img.rows
                << " 通道" << img.channels();

    // 旧流程会把 640x480 横图转成 480x640，蓝条到左侧、光斑到管底。
    // 统一转成横图：参照物在上、血球在左、血浆在右。
    bool orientationChanged = false;
    if (img.rows > img.cols) {
        Mat landscape;
        cv::transpose(img, landscape);
        cv::flip(landscape, landscape, 1);
        QLOG_INFO() << "[血浆分层] 竖图转横图" << img.cols << "x" << img.rows
                    << " -> " << landscape.cols << "x" << landscape.rows;
        img = landscape;
        orientationChanged = true;
    }

    // 识别用全分辨率；预览可缩放。bloodlayer 验收基准是原图像素，勿先压到 640x480。
    imageOrinin = img.clone();
    {
        Mat preview = img;
        const int maxW = 640;
        const int maxH = 480;
        if (img.cols > maxW || img.rows > maxH) {
            const double sx = double(maxW) / img.cols;
            const double sy = double(maxH) / img.rows;
            const double s = qMin(sx, sy);
            cv::resize(img, preview, Size(), s, s, INTER_AREA);
        }
        displayImage(preview, ui->label_showimage);
        QLOG_INFO() << "[血浆分层] 识别分辨率" << imageOrinin.cols << "x" << imageOrinin.rows
                    << " 预览" << preview.cols << "x" << preview.rows;
    }

    // 路径接口内部用 QImage 解码；竖转横后写临时 JPEG，保证与库约定一致
    if (orientationChanged) {
        const QString dir = QDir(appDir).filePath(QStringLiteral("FindHighImage"));
        QDir().mkpath(dir);
        const QString tmpPath = QDir(dir).filePath(QStringLiteral("_bloodlayer_detect.jpg"));
        Mat rgb;
        cvtColor(img, rgb, COLOR_BGR2RGB);
        QImage qimg(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step),
                    QImage::Format_RGB888);
        if (!qimg.copy().save(tmpPath, "JPG", 98)) {
            QMessageBox::warning(this, "错误", "无法写入识别临时图");
            QLOG_WARN() << "[血浆分层] 写临时图失败" << tmpPath;
            return;
        }
        m_detectImagePath = tmpPath;
        QLOG_INFO() << "[血浆分层] 识别路径(横放临时)" << m_detectImagePath;
    } else {
        m_detectImagePath = canonicalPath;
        QLOG_INFO() << "[血浆分层] 识别路径(原图)" << m_detectImagePath;
    }

    // 重置检测结果
    redBloodCellHeight = 0;
    detectedInterface = Point(-1, -1);
    referenceObjectRect = Rect(0,0,0,0);
    redBloodCellHeightMm = 0.0;
    maxNeedleDropHeight = 0.0;
    referenceToBottomDistance = 0.0;
    m_horizontalTube = false;

    trayfindImg();
}

void TestOpcv::trayfindImg()
{
    // 测高识别入口：bloodlayer C API（与太原 testOpencv 同一套逻辑）
    // 1) bloodlayer_detect_interface 找参照物 + 红细胞/血浆分界
    // 2) 比例仍用 REFERENCE_HEIGHT(20mm) 和 max(宽,高)/20（下针公式不变）
    // 3) 下针仍走 calculateNeedleDropFromPackedLength
    m_horizontalTube = false;
    m_hemolysisStatus = QStringLiteral("Unknown");
    m_layerFailDetail.clear();
    s_tubeRoi = Rect();
    if (imageOrinin.empty()) {
        QMessageBox::warning(this, "错误", "没有可处理的图像");
        return;
    }
    if (m_detectImagePath.isEmpty()) {
        QMessageBox::warning(this, "错误", "没有可识别的图像路径");
        return;
    }

    Mat image = imageOrinin.clone();

    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    Scalar meanBrightness = mean(gray);
    QLOG_INFO() << "[血浆分层] 开始识别 bloodlayer" << bloodlayer_version()
                << " 路径" << m_detectImagePath
                << " 图" << image.cols << "x" << image.rows
                << " 亮度=" << qRound(meanBrightness[0]);

    BloodDetectResultC det;
    std::memset(&det, 0, sizeof(det));
    const QByteArray utf8Path = m_detectImagePath.toUtf8();
    const int apiOk = bloodlayer_detect_interface(utf8Path.constData(), &det, nullptr);
    const QString detMsg = QString::fromUtf8(det.message);

    QLOG_INFO() << "[血浆分层] bloodlayer" << detMsg
                << " apiOk=" << apiOk
                << " ok=" << det.ok
                << " 横管=" << det.horizontal
                << " 参照=" << det.refFound
                << " 管ROI=" << det.tubeX << "," << det.tubeY
                << det.tubeW << "x" << det.tubeH
                << " rbcPlasmaPos=" << det.rbcPlasmaPos
                << " 层=" << det.nLayers
                << " 分界数=" << det.nInterfaces
                << " refHeightPx=" << det.refHeightPx
                << " pxPerMm=" << det.pxPerMm;

    referenceObjectRect = Rect(det.refX, det.refY, det.refW, det.refH);
    s_tubeRoi = Rect(det.tubeX, det.tubeY, det.tubeW, det.tubeH);
    m_horizontalTube = det.horizontal || (image.cols > image.rows);
    m_refTopX = -1.0;
    pixelToMmRatio = 0.0;
    if (det.refFound && referenceObjectRect.width > 0 && referenceObjectRect.height > 0) {
        m_refTopX = static_cast<double>(referenceObjectRect.x + referenceObjectRect.width);
        pixelToMmRatio = static_cast<double>(
                    qMax(referenceObjectRect.width, referenceObjectRect.height))
                / static_cast<double>(REFERENCE_HEIGHT);
    }

    QLOG_INFO() << "[测高] 参照物物理高 REFERENCE_HEIGHT=" << REFERENCE_HEIGHT << "mm"
                << " 矩形" << referenceObjectRect.width << "x" << referenceObjectRect.height
                << " 比例=" << pixelToMmRatio << "px/mm (max边/" << REFERENCE_HEIGHT << ")"
                << " 参照顶X=" << m_refTopX
                << " 库pxPerMm(仅日志)=" << det.pxPerMm;

    if (!det.refFound || pixelToMmRatio <= 0.0) {
        QLOG_WARN() << "[测高] 参照物失败" << detMsg
                    << " refFound=" << det.refFound
                    << " rect=" << referenceObjectRect.width << "x" << referenceObjectRect.height;
        m_layerFailDetail = detMsg.isEmpty()
                ? QString::fromUtf8("未找到蓝色参照块。请检查参照物是否入画、是否被遮挡。")
                : detMsg;
        showFailureWithReference(tr("提示"), tr("未找到参照物"));
        return;
    }

    int ifacePos = det.rbcPlasmaPos;
    int rbcLeft = -1;
    for (int i = 0; i < det.nInterfaces; ++i) {
        const BloodIfaceC &it = det.interfaces[i];
        const QString name = QString::fromUtf8(it.name);
        if (name.contains(QString::fromUtf8("红细胞"))) {
            ifacePos = it.pos;
        }
    }
    for (int i = 0; i < det.nLayers; ++i) {
        const BloodLayerC &ly = det.layers[i];
        const QString name = QString::fromUtf8(ly.name);
        if (name.contains(QString::fromUtf8("红细胞"))) {
            rbcLeft = ly.a0;
            break;
        }
    }

    if (!det.ok || ifacePos < 0) {
        QLOG_WARN() << "[测高] 分层失败 ok=" << det.ok
                    << " 红细胞分界X=" << ifacePos
                    << " 压积左缘=" << rbcLeft
                    << " 层数=" << det.nLayers
                    << " 分界数=" << det.nInterfaces
                    << " " << detMsg;
        for (int i = 0; i < det.nLayers; ++i) {
            const BloodLayerC &ly = det.layers[i];
            QLOG_WARN() << "[测高] 层" << i << QString::fromUtf8(ly.name)
                        << ly.a0 << "-" << ly.a1
                        << QString::number(ly.percent, 'f', 1) << "%";
        }
        for (int i = 0; i < det.nInterfaces; ++i) {
            const BloodIfaceC &it = det.interfaces[i];
            QLOG_WARN() << "[测高] 分界" << i << QString::fromUtf8(it.name)
                        << " pos=" << it.pos;
        }
        m_layerFailDetail = detMsg;
        showFailureWithReference(tr("错误"),
                                 detMsg.isEmpty()
                                 ? tr("未找到血浆分层（左红细胞、右血浆）")
                                 : detMsg);
        return;
    }

    const int packedLeft = (rbcLeft >= 0) ? rbcLeft : qMax(0, s_tubeRoi.x);
    const int packedPx = qMax(1, ifacePos - packedLeft);
    detectedInterface = m_horizontalTube
            ? Point(ifacePos, s_tubeRoi.y + qMax(1, s_tubeRoi.height) / 2)
            : Point(s_tubeRoi.x + qMax(1, s_tubeRoi.width) / 2, ifacePos);
    processedImage = (s_tubeRoi.width > 4 && s_tubeRoi.height > 4)
            ? image(s_tubeRoi).clone()
            : image.clone();
    redBloodCellHeight = packedPx;
    maxInfoColor = "DarkRed";

    if (m_horizontalTube) {
        calculateNeedleDropFromPackedLength(packedPx, pixelToMmRatio);
    } else {
        calculateNeedleDropParameters(detectedInterface.y, packedPx, pixelToMmRatio);
    }

    QLOG_INFO() << "[血浆分层] 识别完成 分界=" << detectedInterface.x << "," << detectedInterface.y
                << " 压积左缘=" << packedLeft
                << " 图测压积=" << packedPx << "px /" << redBloodCellHeightMm << "mm"
                << " 下针=" << maxNeedleDropHeight << "mm";

    InspectionResult resultPrp;
    if (m_horizontalTube) {
        const int localX = ifacePos - s_tubeRoi.x;
        resultPrp = inspectPRPAlongX(processedImage, localX);
    } else {
        resultPrp = inspectPRP(processedImage, static_cast<double>(qMax(0, ifacePos)));
    }
    QLOG_INFO() << "[血浆分层] 血浆溶血" << resultPrp.result
                << " index=" << resultPrp.hemolysisIndex;
    displayResults(resultPrp.hemolysisIndex);
}

void TestOpcv::calculateNeedleDropFromPackedLength(int packedRbcPixels, double ratio)
{
    if (ratio <= 0.0) {
        QLOG_ERROR() << "错误: 像素到毫米比例无效";
        return;
    }

    auto &ini = INI_File();
    referenceToBottomDistance = ini.GetFixedHigh();
    const double safetyMargin = ini.GetTestDifference();
    const double rotb = ini.getRefBottomDistance();

    // 与原来竖管 calculateNeedleDropParameters 同一套：
    // 横图参照物右端 = 旧流程转 90° 后的参照物顶部。
    // 红细胞高度 = 参照物到底部 - 参照顶到分界 - 刻度尺垫高 2mm。
    const double refTopX = (m_refTopX > 0.0)
            ? m_refTopX
            : static_cast<double>(referenceObjectRect.x + referenceObjectRect.width);
    const double interfaceToReferenceTopPixels = refTopX - static_cast<double>(detectedInterface.x);
    const double interfaceToReferenceTopMm = interfaceToReferenceTopPixels / ratio;

    redBloodCellHeightMm = qMax(0.0, rotb - interfaceToReferenceTopMm - RAISETHERULER);
    redBloodCellHeightMm = round(redBloodCellHeightMm * 100.0) / 100.0;
    redBloodCellHeight = qMax(1, qRound(redBloodCellHeightMm * ratio));

    maxNeedleDropHeight = referenceToBottomDistance + rotb - redBloodCellHeightMm
            - safetyMargin - RAISETHERULER;

    QLOG_INFO() << "[测高] 下针计算"
                << " REFERENCE_HEIGHT=" << REFERENCE_HEIGHT << "mm"
                << " RAISETHERULER=" << RAISETHERULER << "mm"
                << " 参照顶X=" << refTopX
                << " 分界X=" << detectedInterface.x
                << " 顶到分界=" << qRound(interfaceToReferenceTopPixels) << "px /"
                << interfaceToReferenceTopMm << "mm"
                << " 底距rotb=" << rotb
                << " 图测压积=" << packedRbcPixels << "px"
                << " 红细胞高度=" << redBloodCellHeightMm << "mm"
                << " 固定高=" << referenceToBottomDistance
                << " 偏移=" << safetyMargin
                << " 下针=" << maxNeedleDropHeight << "mm";
    if (redBloodCellHeightMm < 3.0 || redBloodCellHeightMm > 25.0) {
        QLOG_WARN() << "[测高] 红细胞高度异常" << redBloodCellHeightMm
                    << "mm（常见约8-18）。检查参照顶X、分界X、rotb=" << rotb;
    }
    if (pixelToMmRatio < 8.0 || pixelToMmRatio > 20.0) {
        QLOG_WARN() << "[测高] 比例异常" << pixelToMmRatio
                    << "px/mm（横图参照物通常约12-13）。参照物矩形"
                    << referenceObjectRect.width << "x" << referenceObjectRect.height;
    }
}

InspectionResult TestOpcv::inspectPRPAlongX(const Mat &testTubeImage, int interfaceX)
{
    InspectionResult result;
    if (testTubeImage.empty()) {
        result.result = "InvalidRegion";
        result.hemolysisIndex = 0.0;
        return result;
    }

    const int height = testTubeImage.rows;
    const int width = testTubeImage.cols;
    const int plasmaWidth = qMax(8, width - interfaceX);
    const int gap = qBound(12, plasmaWidth / 5, plasmaWidth / 3);
    const int startX = qBound(0, interfaceX + gap, width - 8);
    const int regionW = qBound(8, plasmaWidth / 2, width - startX);
    const int startY = height / 5;
    const int regionH = qMax(8, height * 3 / 5);
    if (startX + regionW > width || startY + regionH > height) {
        result.result = "InvalidRegion";
        result.hemolysisIndex = 0.0;
        return result;
    }

    Rect plasmaRegion(startX, startY, regionW, regionH);
    Mat plasmaMat = testTubeImage(plasmaRegion);

    Mat hsvImg;
    cvtColor(plasmaMat, hsvImg, COLOR_BGR2HSV);
    Mat blurredImg;
    GaussianBlur(hsvImg, blurredImg, Size(5, 5), 0);

    Mat maskRed1, maskRed2, redMask;
    inRange(blurredImg,
            Scalar(m_lowerRed1.lowH, m_lowerRed1.lowS, m_lowerRed1.lowV),
            Scalar(m_lowerRed1.highH, m_lowerRed1.highS, m_lowerRed1.highV),
            maskRed1);
    inRange(blurredImg,
            Scalar(m_lowerRed2.lowH, m_lowerRed2.lowS, m_lowerRed2.lowV),
            Scalar(m_lowerRed2.highH, m_lowerRed2.highS, m_lowerRed2.highV),
            maskRed2);
    bitwise_or(maskRed1, maskRed2, redMask);

    const int totalPixels = static_cast<int>(redMask.total());
    const int redPixels = countNonZero(redMask);
    const double redRatio = totalPixels > 0
            ? static_cast<double>(redPixels) / totalPixels
            : 0.0;

    result.result = (redRatio > m_hemolysisThreshold) ? "Hemolyzed" : "Normal";
    result.hemolysisIndex = redRatio;
    m_hemolysisStatus = result.result;
    return result;
}

InspectionResult TestOpcv::inspectPRP(const Mat &testTubeImage, double topRegionRatio)
{
    InspectionResult result;

    // 提取顶部区域（从topRegionRatio位置向上50PX）
    int height = testTubeImage.rows;
    int width = testTubeImage.cols;

    // 计算顶分层处再往试管口方向离开交界区，避免把压积层边缘算成溶血
    int startY = max(0, static_cast<int>(topRegionRatio) - 90);
    int regionHeight = 50;
    if (startY + regionHeight >= height) {
        result.result = "InvalidRegion";
        result.hemolysisIndex = 0.0;
        m_hemolysisStatus = result.result;
        return result;
    }

    Rect topRegion(0, startY , width, regionHeight);
    Mat topRegionMat = testTubeImage(topRegion);

    Mat hsvImg;
    cvtColor(topRegionMat, hsvImg, COLOR_BGR2HSV);

    Mat blurredImg;
    GaussianBlur(hsvImg, blurredImg, Size(5, 5), 0);

    Mat maskRed1, maskRed2, redMask;
    inRange(blurredImg,
            Scalar(m_lowerRed1.lowH, m_lowerRed1.lowS, m_lowerRed1.lowV),
            Scalar(m_lowerRed1.highH, m_lowerRed1.highS, m_lowerRed1.highV),
            maskRed1);

    inRange(blurredImg,
            Scalar(m_lowerRed2.lowH, m_lowerRed2.lowS, m_lowerRed2.lowV),
            Scalar(m_lowerRed2.highH, m_lowerRed2.highS, m_lowerRed2.highV),
            maskRed2);

    bitwise_or(maskRed1, maskRed2, redMask);

    int totalPixels = static_cast<int>( redMask.total());
    int redPixels = countNonZero(redMask);
    double redRatio = static_cast<double>(redPixels) / totalPixels;

    result.result = (redRatio > m_hemolysisThreshold) ? "Hemolyzed" : "Normal";
    result.hemolysisIndex = redRatio;
    m_hemolysisStatus = result.result;

    return result;
}
