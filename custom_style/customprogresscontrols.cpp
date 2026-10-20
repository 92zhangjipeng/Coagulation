#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "customprogresscontrols.h"
#include "ui_customprogresscontrols.h"
#include "cglobal.h"
#include  "QsLog/include/QsLog.h"
#include <QPainter>
#include <QPainterPath>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QTimer>
#include <globaldata.h>

namespace {
const char *kRatioLabelCss =
        "QLabel#label_ratio {"
        "    background: transparent;"
        "    border: none;"
        "    color: #3A5366;"
        "    font-family: 'Microsoft YaHei';"
        "    font-size: 16px;"
        "    font-weight: 600;"
        "}";

const int kPlateAlpha = 191;

QPainterPath hexPlatePath(const QRectF &r)
{
    const qreal cut = qBound(10.0, r.height() * 0.42, r.width() * 0.12);
    QPainterPath path;
    path.moveTo(r.left() + cut, r.top());
    path.lineTo(r.right() - cut, r.top());
    path.lineTo(r.right(), r.center().y());
    path.lineTo(r.right() - cut, r.bottom());
    path.lineTo(r.left() + cut, r.bottom());
    path.lineTo(r.left(), r.center().y());
    path.closeSubpath();
    return path;
}

void fillHexPlate(QPainter &painter, const QRectF &r, const QBrush &brush)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(hexPlatePath(r));
}

}

class HexPlateProgressBar : public QWidget
{
public:
    explicit HexPlateProgressBar(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground, true);
        setAttribute(Qt::WA_OpaquePaintEvent, false);
        setAutoFillBackground(false);
        setFixedHeight(20);
        setMinimumWidth(120);
        m_timer.setInterval(16);
        connect(&m_timer, &QTimer::timeout, this, [this]() {
            m_phase += 0.012;
            if (m_phase > 1.0) {
                m_phase -= 1.0;
            }
            update();
        });
    }

    void setBusy(bool busy)
    {
        m_busy = busy;
        if (busy) {
            if (!m_timer.isActive()) {
                m_timer.start();
            }
        } else {
            m_timer.stop();
        }
        update();
    }

    void setPercent(int percent)
    {
        m_percent = qBound(0, percent, 100);
        if (!m_busy) {
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF track = QRectF(rect()).adjusted(1.0, 3.0, -1.0, -3.0);
        fillHexPlate(painter, track, QColor(176, 192, 204, 165));
        painter.setPen(QPen(QColor(148, 168, 184, 170), 1.0));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(hexPlatePath(track));

        if (m_busy) {
            qreal ping = (m_phase < 0.5) ? (m_phase * 2.0) : (2.0 - m_phase * 2.0);
            ping = ping * ping * (3.0 - 2.0 * ping);
            const qreal sliderW = qMax(36.0, track.height() * 2.4);
            const qreal travel = qMax(0.0, track.width() - sliderW);
            const QRectF slider(track.left() + ping * travel,
                                track.top(),
                                sliderW,
                                track.height());
            QLinearGradient g(slider.topLeft(), slider.topRight());
            g.setColorAt(0.0, QColor(168, 206, 222, 40));
            g.setColorAt(0.45, QColor(156, 204, 224, 210));
            g.setColorAt(1.0, QColor(168, 206, 222, 40));
            fillHexPlate(painter, slider, g);
            return;
        }

        if (m_percent <= 0) {
            return;
        }
        painter.save();
        painter.setClipPath(hexPlatePath(track));
        const qreal fillW = track.width() * m_percent / 100.0;
        QRectF fill(track.left(), track.top(), fillW, track.height());
        QLinearGradient g(fill.topLeft(), fill.topRight());
        g.setColorAt(0.0, QColor(168, 208, 226, 200));
        g.setColorAt(1.0, QColor(126, 178, 204, 210));
        painter.fillRect(fill, g);
        painter.restore();
    }

private:
    QTimer m_timer;
    bool m_busy = false;
    int m_percent = 0;
    qreal m_phase = 0.0;
};

customProgresscontrols::customProgresscontrols(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customProgresscontrols),
    m_capsuleBar(nullptr)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                             | Qt::WindowDoesNotAcceptFocus | Qt::NoDropShadowWindowHint);
    setWindowOpacity(1.0);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);

    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setFixedSize(540, 118);
    setStyleSheet(QString());

    if (ui->verticalLayout) {
        ui->verticalLayout->setContentsMargins(52, 22, 52, 22);
        ui->verticalLayout->setSpacing(12);
    }

    m_totalnum = 1;
    ui->label_ratio->setStyleSheet(QLatin1String(kRatioLabelCss));
    ui->label_ratio->setMinimumHeight(28);
    ui->label_ratio->setMaximumHeight(36);
    QFont labelFont(QString::fromUtf8("Microsoft YaHei"), 16);
    labelFont.setBold(true);
    ui->label_ratio->setFont(labelFont);

    ui->progressBar_ratio->hide();
    m_capsuleBar = new HexPlateProgressBar(this);
    if (ui->verticalLayout) {
        const int barIndex = ui->verticalLayout->indexOf(ui->progressBar_ratio);
        ui->verticalLayout->insertWidget(barIndex >= 0 ? barIndex : 2, m_capsuleBar);
    }

    if(m_showAnimation.isNull()){
        m_showAnimation.reset(new QPropertyAnimation(this, "geometry"));
    }

    m_showAnimation->setDuration(500);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);

    if(m_closeAnimation.isNull()){
        m_closeAnimation.reset(new QPropertyAnimation(this, "geometry"));
    }
    m_closeAnimation->setDuration(500);
    m_closeAnimation->setEasingCurve(QEasingCurve::InCubic);

    connect(m_closeAnimation.data(), &QPropertyAnimation::finished, this, [=](){
        QWidget::close();
    });

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    setGeometry(-width(),
                (screenGeometry.height() - height()) / 2,
                width(),
                height());

}

customProgresscontrols::~customProgresscontrols()
{
    m_showAnimation->stop();
    m_closeAnimation->stop();
    QLOG_DEBUG()<<"析构进度框类["<<__FILE__<<"]";
    delete ui;
}

void customProgresscontrols::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF plate = QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0);
    const QPainterPath path = hexPlatePath(plate);

    QLinearGradient body(plate.topLeft(), plate.bottomLeft());
    body.setColorAt(0.0, QColor(246, 250, 252, kPlateAlpha));
    body.setColorAt(1.0, QColor(226, 234, 240, kPlateAlpha));
    painter.fillPath(path, body);

    painter.setPen(QPen(QColor(156, 176, 192, 170), 1.4));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    QRectF inner = plate.adjusted(5.0, 5.0, -5.0, -5.0);
    painter.setPen(QPen(QColor(176, 192, 206, 110), 1.0));
    painter.drawPath(hexPlatePath(inner));
}

void customProgresscontrols::showEvent(QShowEvent *event) {
    if (!m_isAnimating) {
        m_isAnimating = true;

        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();

        QRect startRect = geometry();

        QRect endRect = QRect(
            (screenGeometry.width() - width()) / 2,
            (screenGeometry.height() - height()) / 2,
            width(),
            height()
        );

        m_showAnimation->setStartValue(startRect);
        m_showAnimation->setEndValue(endRect);
        m_showAnimation->start();
    }

    QWidget::showEvent(event);
}

void customProgresscontrols::closeEvent(QCloseEvent *event) {
    if (!m_isAnimating) {
        event->ignore();
        m_isAnimating = true;

        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();

        QRect startRect = geometry();

        QRect endRect = QRect(
            screenGeometry.width(),
            startRect.y(),
            width(),
            height()
        );

        m_closeAnimation->setStartValue(startRect);
        m_closeAnimation->setEndValue(endRect);
        m_closeAnimation->start();
    }
}

void customProgresscontrols::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void customProgresscontrols::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}


void customProgresscontrols::_settitle(const QString title)
{
    this->setWindowTitle(title);
}

void customProgresscontrols::_setprogressvalue(const QString info,int numstep)
{
    const int percent = (m_totalnum == 0) ? 0 : (numstep * 100 / m_totalnum);
    ui->label_ratio->setText(QString("%1%2%").arg(info).arg(percent));
    if (m_capsuleBar) {
        m_capsuleBar->setBusy(false);
        m_capsuleBar->setPercent(percent);
    }
}

void customProgresscontrols::_setprogressRand(int min,int max)
{
    if (m_capsuleBar) {
        m_capsuleBar->setBusy(min == max);
    }
}


void customProgresscontrols::_setprogresstotalnum(int numtotal)
{
    if(numtotal != 0)
        m_totalnum = numtotal;
}

void customProgresscontrols::_setlabletext(const QString reminder)
{
    ui->label_ratio->setText(reminder);
}

void customProgresscontrols::setBusyWait(const QString &hint)
{
    ui->label_ratio->setText(hint);
    if (m_capsuleBar) {
        m_capsuleBar->setBusy(true);
    }
}

void customProgresscontrols::_setMinvalue(int) { }

void  customProgresscontrols::_setMaxvalue(int) { }

int  customProgresscontrols::_getprogressdata()
{
    return 0;
}
