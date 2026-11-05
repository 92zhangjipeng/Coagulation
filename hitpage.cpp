#ifdef _MSC_VER
#pragma warning(disable: 4819)
#pragma execution_character_set("utf-8")
#endif
#include "hitpage.h"

#include "cglobal.h"
#include "custom_style/widgetdelegate.h"
#include "hittestrunner.h"
#include "loadequipmentpos.h"

#include "custom_style/custommessagebox.h"

#include <algorithm>
#include <QAbstractItemView>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDateTimeEdit>
#include <QTime>
#include <QLineEdit>
#include <QDialog>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QList>
#include <QVector>
#include <QMap>
#include <QSet>
#include "qcustomplot.h"

#include <QPainter>
#include <QProgressBar>
#include <QPaintEvent>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

namespace {

enum HitColumn {
    ColStatus = 0,
    ColSampleId,
    ColChannel,
    ColSampleTag,
    ColStartTime,
    ColEndTime,
    ColPpp,
    ColSpontaneous,
    ColLow,
    ColHigh,
    ColNegative,
    ColJudge,
    ColCount
};

const int kRowHeight = 64;

QString buttonStyle()
{
    return QString::fromUtf8(
        "QPushButton {"
        "  font-family: '楷体';"
        "  font-size: 20px;"
        "  min-width: 120px;"
        "  min-height: 52px;"
        "  padding: 8px 12px;"
        "  border: 1px solid #b7c5d3;"
        "  border-radius: 8px;"
        "  background: #ffffff;"
        "  color: #1a4d6b;"
        "}"
        "QPushButton:hover {"
        "  border: 1px solid #4a7ab5;"
        "  background: #e7f0f8;"
        "}"
        "QPushButton:pressed {"
        "  background: #d5e4f2;"
        "}"
        "QPushButton:disabled {"
        "  background: #e6edf3;"
        "  color: #9aabba;"
        "  border: 1px solid #d3dee8;"
        "}");
}

QString stepClockText(int ms)
{
    if (ms < 0)
        ms = 0;
    const int totalSec = ms / 1000;
    return QString("%1:%2")
            .arg(totalSec / 60, 2, 10, QChar('0'))
            .arg(totalSec % 60, 2, 10, QChar('0'));
}

QString stepBarStyle()
{
    return QString::fromUtf8(
        "QProgressBar {"
        "  font-family: '楷体'; font-size: 14px; color: #1a4d6b;"
        "  background: #eef3f7; border: 1px solid #c5d5e2; border-radius: 6px;"
        "  text-align: center; margin: 14px 8px;"
        "}"
        "QProgressBar::chunk { background: #4a7ab5; border-radius: 5px; }");
}

QString tableStyle()
{
    return QString::fromUtf8(
        "QTableWidget {"
        "  background: #ffffff;"
        "  alternate-background-color: #f4f8fb;"
        "  border: 1px solid #b7c5d3;"
        "  border-radius: 8px;"
        "  font-family: '楷体';"
        "  font-size: 16px;"
        "  color: #2c3a48;"
        "  gridline-color: #d3dee8;"
        "  selection-background-color: #e7f0f8;"
        "  selection-color: #1a4d6b;"
        "}"
        "QHeaderView::section {"
        "  background: #e7f0f8;"
        "  color: #1a4d6b;"
        "  border: none;"
        "  border-bottom: 1px solid #d3dee8;"
        "  border-right: 1px solid #d3dee8;"
        "  padding: 8px 6px;"
        "  font-family: '楷体';"
        "  font-size: 16px;"
        "}"
        "QScrollBar:vertical { width: 8px; margin: 0px; background: #eef3f7; border: none; }"
        "QScrollBar::handle:vertical { background: #8aa4be; min-height: 24px; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px; width: 0px; border: none; background: transparent; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
        "QScrollBar:horizontal { height: 8px; margin: 0px; background: #eef3f7; border: none; }"
        "QScrollBar::handle:horizontal { background: #8aa4be; min-width: 24px; border-radius: 3px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "  height: 0px; width: 0px; border: none; background: transparent; }"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }"
        "QAbstractScrollArea::corner { background: #eef3f7; }");
}

void paintHitRowFrame(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!(option.state & QStyle::State_Selected) || !index.isValid())
        return;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(QPen(QColor(74, 122, 181), 2));
    const QRect r = option.rect.adjusted(1, 1, -1, -1);
    painter->drawLine(r.topLeft(), r.topRight());
    painter->drawLine(r.bottomLeft(), r.bottomRight());
    if (index.column() == 0)
        painter->drawLine(r.topLeft(), r.bottomLeft());
    if (index.model() && index.column() == index.model()->columnCount() - 1)
        painter->drawLine(r.topRight(), r.bottomRight());
    painter->restore();
}

class HitTextDelegate : public QStyledItemDelegate
{
public:
    explicit HitTextDelegate(bool editable, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_editable(editable) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        if (index.column() != ColSampleId)
            return nullptr;
        QLineEdit *edit = new QLineEdit(parent);
        edit->setFrame(false);
        edit->setAlignment(Qt::AlignCenter);
        QFont font(QString::fromUtf8("楷体"));
        font.setPixelSize(16);
        edit->setFont(font);
        edit->setAutoFillBackground(true);
        edit->setStyleSheet(QString::fromUtf8(
            "QLineEdit { background: #e7f0f8; color: #1a4d6b; border: none;"
            " selection-background-color: #d4eaf5; selection-color: #1a4d6b; }"));
        return edit;
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override
    {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(option.widget);
        const bool editing = (option.state & QStyle::State_Editing)
                || (view && view->indexWidget(index));
        if (editing) {
            painter->fillRect(option.rect, QColor(231, 240, 248));
            paintHitRowFrame(painter, option, index);
            return;
        }
        QStyleOptionViewItem opt(option);
        opt.state &= ~QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter, opt, index);
        paintHitRowFrame(painter, opt, index);
    }

private:
    bool m_editable;
};

class HitPopupItemDelegate : public QStyledItemDelegate
{
public:
    explicit HitPopupItemDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt(option);
        opt.font.setFamily(QString::fromUtf8("楷体"));
        opt.font.setPixelSize(20);
        QSize size = QStyledItemDelegate::sizeHint(opt, index);
        size.setHeight(qMax(48, size.height()));
        return size;
    }
};

void polishHitComboPopup(QComboBox *box)
{
    if (!box)
        return;
    QFont font(QString::fromUtf8("楷体"));
    font.setPixelSize(20);
    if (QAbstractItemView *view = box->view()) {
        view->setFont(font);
        view->setItemDelegate(new HitPopupItemDelegate(view));
        view->setStyleSheet(QString::fromUtf8(
            "QAbstractItemView { background: #FFFFFF; color: #16384A; border: none; outline: 0;"
            " font-family: '楷体'; font-size: 20px; }"
            "QAbstractItemView::item { min-height: 48px; padding: 10px 12px; }"
            "QAbstractItemView::item:selected { background: #D4EAF5; color: #16384A; }"
            "QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
            "QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; width: 0px; }"));
    }
    box->setStyleSheet(box->styleSheet() + QString::fromUtf8(
        "QComboBox QAbstractItemView { font-family: '楷体'; font-size: 20px; }"
        "QComboBox QAbstractItemView::item { min-height: 48px; padding: 10px 12px; font-size: 20px; }"));
}

QStringList availablePppHoles(const QTableWidget *table, int exceptRow);

class HitHoleDelegate : public WidgetDelegate
{
public:
    explicit HitHoleDelegate(QTableWidget *table)
        : WidgetDelegate(QStringList(), nullptr)
        , m_table(table) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        QWidget *editor = WidgetDelegate::createEditor(parent, option, index);
        polishHitComboPopup(qobject_cast<QComboBox *>(editor));
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QComboBox *box = qobject_cast<QComboBox *>(editor);
        if (!box)
            return;
        const QString current = index.data(Qt::DisplayRole).toString();
        QStringList holes = availablePppHoles(m_table, index.row());
        if (!current.isEmpty() && !holes.contains(current))
            holes.prepend(current);
        box->clear();
        box->addItems(holes);
        const int found = box->findText(current);
        if (found >= 0)
            box->setCurrentIndex(found);
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override
    {
        if ((index.flags() & Qt::ItemIsEditable) && event->type() == QEvent::MouseButtonRelease) {
            if (QAbstractItemView *view = qobject_cast<QAbstractItemView *>(const_cast<QWidget *>(option.widget)))
                view->edit(index);
        }
        return WidgetDelegate::editorEvent(event, model, option, index);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt(option);
        opt.state &= ~QStyle::State_HasFocus;
        WidgetDelegate::paint(painter, opt, index);
        paintHitRowFrame(painter, opt, index);
    }

private:
    QTableWidget *m_table = nullptr;
};

class HitComboDelegate : public WidgetDelegate
{
public:
    explicit HitComboDelegate(const QStringList &items)
        : WidgetDelegate(items, nullptr) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        QWidget *editor = WidgetDelegate::createEditor(parent, option, index);
        polishHitComboPopup(qobject_cast<QComboBox *>(editor));
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        WidgetDelegate::setEditorData(editor, index);
        QComboBox *box = qobject_cast<QComboBox *>(editor);
        if (!box)
            return;
        const int found = box->findText(index.data(Qt::DisplayRole).toString());
        if (found >= 0)
            box->setCurrentIndex(found);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt(option);
        opt.state &= ~QStyle::State_HasFocus;
        WidgetDelegate::paint(painter, opt, index);
        paintHitRowFrame(painter, opt, index);
    }
};

int bloodGroupCount()
{
    quint8 kind = KS1200;
    if (SingletonAxis::GetInstance())
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, kind);
    if (kind == KS600)
        return 6;
    if (kind == KS800)
        return 8;
    return 12;
}

int pppHoleAt(int index)
{
    return 1 + (index / 2) * 10 + (index % 2) * 4;
}

QStringList availablePppHoles(const QTableWidget *table, int exceptRow)
{
    const QString pending = QString::fromUtf8("待测");
    QList<int> used;
    if (table) {
        for (int row = 0; row < table->rowCount(); ++row) {
            if (row == exceptRow)
                continue;
            const QTableWidgetItem *statusItem = table->item(row, ColStatus);
            if (!statusItem || statusItem->text().trimmed() != pending)
                continue;
            const QTableWidgetItem *item = table->item(row, ColSampleTag);
            if (!item)
                continue;
            bool ok = false;
            const int hole = item->text().toInt(&ok);
            if (ok)
                used.append(hole);
        }
    }

    QStringList holes;
    const int total = bloodGroupCount() * 2;
    for (int n = 0; n < total; ++n) {
        const int hole = pppHoleAt(n);
        if (!used.contains(hole))
            holes << QString::number(hole);
    }
    return holes;
}

QTableWidgetItem *centeredItem(const QString &text, bool editable)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    if (!editable)
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

class SpinWheelGuard : public QObject
{
public:
    explicit SpinWheelGuard(QObject *parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::Wheel)
            return true;
        return QObject::eventFilter(watched, event);
    }
};

struct HitSettings {
    int pppRefUl = 150;
    int spontaneousSerumUl = 35;
    int spontaneousPrpUl = 280;
    int spontaneousBufferUl = 35;
    int spontaneousIncubateSec = 1200;
    int spontaneousRecordMin = 30;
    int lowSerumUl = 35;
    int lowPrpUl = 280;
    int lowHeparinUl = 35;
    int lowIncubateSec = 1200;
    int lowRecordMin = 30;
    int highSerumUl = 35;
    int highPrpUl = 260;
    int highHeparinUl = 55;
    int highIncubateSec = 1200;
    int highRecordMin = 30;
    int negativePrpUl = 315;
    int negativeHeparinUl = 35;
    int negativeIncubateSec = 1200;
    int negativeRecordMin = 30;
    int positiveDiffPercent = 10;
    int invalidPercent = 20;
};

QString hitConfigPath()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/HitConfig.ini");
}

int iniInt(QSettings &ini, const char *key, int fallback)
{
    return ini.value(QString::fromLatin1(key), fallback).toInt();
}

HitSettings loadHitSettings()
{
    HitSettings data;
    QSettings ini(hitConfigPath(), QSettings::IniFormat);
    ini.setIniCodec("UTF-8");
    ini.beginGroup(QStringLiteral("Ppp"));
    data.pppRefUl = iniInt(ini, "RefUl", data.pppRefUl);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Spontaneous"));
    data.spontaneousSerumUl = iniInt(ini, "SerumUl", data.spontaneousSerumUl);
    data.spontaneousPrpUl = iniInt(ini, "PrpUl", data.spontaneousPrpUl);
    data.spontaneousBufferUl = iniInt(ini, "BufferUl", data.spontaneousBufferUl);
    data.spontaneousIncubateSec = iniInt(ini, "IncubateSec", data.spontaneousIncubateSec);
    data.spontaneousRecordMin = iniInt(ini, "RecordMin", data.spontaneousRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Low"));
    data.lowSerumUl = iniInt(ini, "SerumUl", data.lowSerumUl);
    data.lowPrpUl = iniInt(ini, "PrpUl", data.lowPrpUl);
    data.lowHeparinUl = iniInt(ini, "HeparinUl", data.lowHeparinUl);
    data.lowIncubateSec = iniInt(ini, "IncubateSec", data.lowIncubateSec);
    data.lowRecordMin = iniInt(ini, "RecordMin", data.lowRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("High"));
    data.highSerumUl = iniInt(ini, "SerumUl", data.highSerumUl);
    data.highPrpUl = iniInt(ini, "PrpUl", data.highPrpUl);
    data.highHeparinUl = iniInt(ini, "HeparinUl", data.highHeparinUl);
    data.highIncubateSec = iniInt(ini, "IncubateSec", data.highIncubateSec);
    data.highRecordMin = iniInt(ini, "RecordMin", data.highRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Negative"));
    data.negativePrpUl = iniInt(ini, "PrpUl", data.negativePrpUl);
    data.negativeHeparinUl = iniInt(ini, "HeparinUl", data.negativeHeparinUl);
    data.negativeIncubateSec = iniInt(ini, "IncubateSec", data.negativeIncubateSec);
    data.negativeRecordMin = iniInt(ini, "RecordMin", data.negativeRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Judge"));
    data.positiveDiffPercent = iniInt(ini, "PositiveDiffPercent", data.positiveDiffPercent);
    data.invalidPercent = iniInt(ini, "InvalidPercent", data.invalidPercent);
    ini.endGroup();
    return data;
}

void saveHitSettings(const HitSettings &data)
{
    QSettings ini(hitConfigPath(), QSettings::IniFormat);
    ini.setIniCodec("UTF-8");
    ini.beginGroup(QStringLiteral("Ppp"));
    ini.setValue(QStringLiteral("RefUl"), data.pppRefUl);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Spontaneous"));
    ini.setValue(QStringLiteral("SerumUl"), data.spontaneousSerumUl);
    ini.setValue(QStringLiteral("PrpUl"), data.spontaneousPrpUl);
    ini.setValue(QStringLiteral("BufferUl"), data.spontaneousBufferUl);
    ini.setValue(QStringLiteral("IncubateSec"), data.spontaneousIncubateSec);
    ini.setValue(QStringLiteral("RecordMin"), data.spontaneousRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Low"));
    ini.setValue(QStringLiteral("SerumUl"), data.lowSerumUl);
    ini.setValue(QStringLiteral("PrpUl"), data.lowPrpUl);
    ini.setValue(QStringLiteral("HeparinUl"), data.lowHeparinUl);
    ini.setValue(QStringLiteral("IncubateSec"), data.lowIncubateSec);
    ini.setValue(QStringLiteral("RecordMin"), data.lowRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("High"));
    ini.setValue(QStringLiteral("SerumUl"), data.highSerumUl);
    ini.setValue(QStringLiteral("PrpUl"), data.highPrpUl);
    ini.setValue(QStringLiteral("HeparinUl"), data.highHeparinUl);
    ini.setValue(QStringLiteral("IncubateSec"), data.highIncubateSec);
    ini.setValue(QStringLiteral("RecordMin"), data.highRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Negative"));
    ini.setValue(QStringLiteral("PrpUl"), data.negativePrpUl);
    ini.setValue(QStringLiteral("HeparinUl"), data.negativeHeparinUl);
    ini.setValue(QStringLiteral("IncubateSec"), data.negativeIncubateSec);
    ini.setValue(QStringLiteral("RecordMin"), data.negativeRecordMin);
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Judge"));
    ini.setValue(QStringLiteral("PositiveDiffPercent"), data.positiveDiffPercent);
    ini.setValue(QStringLiteral("InvalidPercent"), data.invalidPercent);
    ini.endGroup();
    ini.sync();
}

bool readPercentCell(const QTableWidget *table, int row, int col, double &out)
{
    if (!table)
        return false;
    const QTableWidgetItem *item = table->item(row, col);
    if (!item)
        return false;
    const QString text = item->text().trimmed();
    if (text.isEmpty())
        return false;
    bool ok = false;
    out = text.toDouble(&ok);
    return ok;
}

void fillHitVerdict(QTableWidget *table, int row)
{
    if (!table || row < 0 || row >= table->rowCount())
        return;
    const HitSettings settings = loadHitSettings();
    double spontaneous = 0;
    double low = 0;
    double high = 0;
    double negative = 0;
    const bool ready = readPercentCell(table, row, ColSpontaneous, spontaneous)
            && readPercentCell(table, row, ColLow, low)
            && readPercentCell(table, row, ColHigh, high)
            && readPercentCell(table, row, ColNegative, negative);
    QString text;
    QColor color(22, 56, 74);
    if (ready) {
        if (spontaneous >= settings.invalidPercent || negative >= settings.invalidPercent) {
            text = QString::fromUtf8("无效/复测");
            color = QColor(180, 110, 20);
        } else if ((low - high) >= settings.positiveDiffPercent) {
            text = QString::fromUtf8("阳性");
            color = QColor(192, 57, 43);
        } else {
            text = QString::fromUtf8("阴性");
            color = QColor(39, 140, 80);
        }
    }
    QTableWidgetItem *item = table->item(row, ColJudge);
    if (!item) {
        item = centeredItem(text, false);
        table->setItem(row, ColJudge, item);
    } else {
        item->setText(text);
    }
    item->setForeground(color);
}

QSpinBox *hitSpin(QWidget *parent, QObject *wheelGuard, int value, int maxValue,
                  const QString &suffix, int width)
{
    QSpinBox *spin = new QSpinBox(parent);
    spin->setRange(0, maxValue);
    spin->setValue(value);
    spin->setSuffix(suffix);
    spin->setAlignment(Qt::AlignCenter);
    spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spin->setFixedSize(width, 36);
    spin->installEventFilter(wheelGuard);
    return spin;
}

QWidget *reagentField(QWidget *parent, QObject *wheelGuard, const QString &name,
                      int value, const QString &suffix, QSpinBox **spinOut)
{
    QWidget *wrap = new QWidget(parent);
    wrap->setFixedWidth(118);
    QVBoxLayout *layout = new QVBoxLayout(wrap);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    QLabel *label = new QLabel(name, wrap);
    label->setObjectName(QStringLiteral("reagentName"));
    label->setAlignment(Qt::AlignCenter);
    QSpinBox *spin = hitSpin(wrap, wheelGuard, value, 2000, suffix, 108);
    layout->addWidget(label);
    layout->addWidget(spin, 0, Qt::AlignHCenter);
    *spinOut = spin;
    return wrap;
}

enum HitHoleRole {
    HitRoleNone = 0,
    HitRolePpp,
    HitRoleSerum,
    HitRolePrp
};

struct HitHoleNote {
    QString sampleText;
    int role = HitRoleNone;
};

struct HitPlaceMark {
    QString sampleId;
    int pppHole = 0;
};

QString hitFamily()
{
    return QString::fromUtf8("楷体");
}

QString shortSampleNo(const QString &sampleId)
{
    if (sampleId.size() > 4)
        return sampleId.right(4);
    return sampleId;
}

void drawRing(QPainter &painter, const QPointF &center, qreal radius,
              const QColor &fill, const QColor &rim)
{
    painter.setPen(QPen(rim, 2));
    painter.setBrush(fill);
    painter.drawEllipse(center, radius, radius);
    painter.setPen(QPen(rim.darker(115), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, radius * 0.72, radius * 0.72);
}

void drawCenterText(QPainter &painter, const QPointF &center, const QString &text,
                    const QFont &font, const QColor &color)
{
    painter.setFont(font);
    painter.setPen(color);
    const QFontMetrics fm(font);
    painter.drawText(QPointF(center.x() - fm.width(text) / 2.0,
                             center.y() + (fm.ascent() - fm.descent()) / 2.0),
                     text);
}

QFont fitPixelFont(const QString &text, int pixel, qreal limit)
{
    QFont font(hitFamily());
    font.setPixelSize(pixel);
    QFontMetrics fm(font);
    while (font.pixelSize() > 8 && fm.width(text) > limit) {
        font.setPixelSize(font.pixelSize() - 1);
        fm = QFontMetrics(font);
    }
    return font;
}

class HitPlaceMap : public QWidget
{
public:
    explicit HitPlaceMap(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAutoFillBackground(true);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::white);
        setPalette(pal);
        setMinimumSize(880, 480);
    }

    void setMarks(const QList<HitPlaceMark> &marks)
    {
        m_marks = marks;
        m_hidden.clear();
        update();
    }

    void hideHole(int hole)
    {
        if (hole <= 0 || m_hidden.contains(hole))
            return;
        m_hidden.insert(hole);
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), Qt::white);

        QMap<int, HitHoleNote> notes;
        for (int i = 0; i < m_marks.size(); ++i) {
            const HitPlaceMark &mark = m_marks.at(i);
            if (mark.pppHole <= 0)
                continue;
            const QString text = shortSampleNo(mark.sampleId);
            HitHoleNote pppNote;
            pppNote.sampleText = text;
            pppNote.role = HitRolePpp;
            HitHoleNote serumNote;
            serumNote.sampleText = text;
            serumNote.role = HitRoleSerum;
            HitHoleNote prpNote;
            prpNote.sampleText = text;
            prpNote.role = HitRolePrp;
            if (!m_hidden.contains(mark.pppHole))
                notes.insert(mark.pppHole, pppNote);
            if (!m_hidden.contains(mark.pppHole + 1))
                notes.insert(mark.pppHole + 1, serumNote);
            if (!m_hidden.contains(mark.pppHole + 2))
                notes.insert(mark.pppHole + 2, prpNote);
        }

        const int legendH = 36;
        const int reagentW = 280;
        const qreal splitGap = 16;
        const QRectF reagent(12, 10, reagentW, height() - legendH - 16);
        const QRectF blood(12 + reagentW + splitGap, 10,
                           width() - reagentW - splitGap - 24, height() - legendH - 16);
        paintReagents(painter, reagent);
        paintBlood(painter, blood, notes);
        paintLegend(painter, QRectF(12, height() - legendH - 2, width() - 24, legendH), notes.isEmpty());
    }

private:
    void paintBlood(QPainter &painter, const QRectF &area, const QMap<int, HitHoleNote> &notes) const
    {
        const int groups = bloodGroupCount();
        const int rows = 5;
        const qreal headerH = 22;
        const qreal holeNoH = 14;
        const qreal gridTop = area.top() + headerH;
        const qreal rowPitch = (area.height() - headerH) / rows;
        const qreal colPitch = area.width() / groups;
        const qreal pairGap = 10;
        const qreal sidePad = 6;
        const qreal maxByCol = (colPitch - pairGap - sidePad * 2) / 4.0;
        const qreal maxByRow = (rowPitch - holeNoH - 8) / 2.0;
        qreal radius = qMin(maxByCol, maxByRow);
        if (radius > 22)
            radius = 22;
        if (radius < 7)
            radius = qMax(4.0, maxByCol);

        static const char kLetters[] = "ABCDEFGHIJKL";
        QFont letterFont(hitFamily());
        letterFont.setPixelSize(15);
        painter.setFont(letterFont);
        for (int col = 0; col < groups && col < 12; ++col) {
            const QString letter = QString(QLatin1Char(kLetters[col]));
            painter.setPen(QColor(26, 77, 107));
            painter.drawText(QRectF(area.left() + col * colPitch, area.top(), colPitch, headerH),
                             Qt::AlignHCenter | Qt::AlignBottom, letter);
        }

        QFont holeFont(hitFamily());
        holeFont.setPixelSize(11);
        for (int col = 0; col < groups; ++col) {
            const qreal span = radius * 4 + pairGap;
            const qreal origin = area.left() + col * colPitch + (colPitch - span) / 2.0;
            const qreal leftX = origin + radius;
            const qreal rightX = origin + radius * 3 + pairGap;
            for (int row = 0; row < rows; ++row) {
                const qreal cy = gridTop + holeNoH + radius + row * rowPitch;
                paintOneHole(painter, QPointF(leftX, cy), radius, holeFont,
                             1 + row * 2 + col * 10, notes);
                paintOneHole(painter, QPointF(rightX, cy), radius, holeFont,
                             2 + row * 2 + col * 10, notes);
            }
        }
    }

    void paintOneHole(QPainter &painter, const QPointF &center, qreal radius, const QFont &holeFont,
                      int hole, const QMap<int, HitHoleNote> &notes) const
    {
        const bool used = notes.contains(hole);
        const HitHoleNote note = used ? notes.value(hole) : HitHoleNote();
        QColor fill(255, 255, 255);
        QColor rim(183, 197, 211);
        QColor textColor(90, 106, 120);
        if (note.role == HitRolePpp) {
            fill = QColor(214, 230, 245);
            rim = QColor(47, 95, 147);
            textColor = QColor(26, 77, 107);
        } else if (note.role == HitRoleSerum) {
            fill = QColor(253, 231, 208);
            rim = QColor(196, 122, 58);
            textColor = QColor(122, 72, 24);
        } else if (note.role == HitRolePrp) {
            fill = QColor(215, 240, 228);
            rim = QColor(47, 143, 107);
            textColor = QColor(24, 92, 64);
        }
        drawRing(painter, center, radius, fill, rim);

        painter.setFont(holeFont);
        painter.setPen(QColor(138, 160, 176));
        const QString holeText = QString::number(hole);
        const QFontMetrics holeFm(holeFont);
        painter.drawText(QPointF(center.x() - holeFm.width(holeText) / 2.0,
                                 center.y() - radius - 2),
                         holeText);

        if (!used)
            return;
        const QString roleText = note.role == HitRolePpp
                ? QStringLiteral("PPP")
                : (note.role == HitRoleSerum
                   ? QString::fromUtf8("血清")
                   : QStringLiteral("PRP"));
        const bool twoLines = radius >= 18;
        const QFont idFont = fitPixelFont(note.sampleText, twoLines ? 12 : 13, radius * 1.5);
        drawCenterText(painter, twoLines ? center + QPointF(0, -radius * 0.18) : center,
                       note.sampleText, idFont, textColor);
        if (twoLines) {
            const QFont roleFont = fitPixelFont(roleText, 11, radius * 1.5);
            drawCenterText(painter, center + QPointF(0, radius * 0.34), roleText, roleFont, textColor);
        }
    }

    void paintReagents(QPainter &painter, const QRectF &area) const
    {
        painter.setPen(QColor(26, 77, 107));
        QFont titleFont(hitFamily());
        titleFont.setPixelSize(16);
        painter.setFont(titleFont);
        painter.drawText(QRectF(area.left(), area.top(), area.width(), 22),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         QString::fromUtf8("试剂"));

        const QString marks[] = {
            QString::fromUtf8("缓冲液"),
            QString::fromUtf8("低剂量肝素"),
            QString::fromUtf8("高剂量肝素"),
            QString(),
            QString()
        };
        const QString physical[] = {
            QStringLiteral("AA"), QStringLiteral("ADP"), QStringLiteral("EPI"),
            QStringLiteral("COL"), QStringLiteral("RIS")
        };
        const qreal labelW = 108;
        const qreal rowPitch = (area.height() - 28) / 5.0;
        qreal radius = qMin(16.0, rowPitch * 0.32);
        if (radius < 11)
            radius = 11;
        const qreal leftX = area.left() + labelW + radius;
        const qreal rightX = leftX + radius * 2 + 12;

        QFont nameFont(hitFamily());
        nameFont.setPixelSize(13);
        QFont physicalFont(hitFamily());
        physicalFont.setPixelSize(12);
        for (int row = 0; row < 5; ++row) {
            const qreal cy = area.top() + 28 + row * rowPitch + rowPitch / 2.0;
            const bool marked = !marks[row].isEmpty();
            const QColor markedFill(214, 230, 245);
            const QColor markedRim(74, 122, 181);
            const QColor emptyFill(255, 255, 255);
            const QColor emptyRim(183, 197, 211);
            drawRing(painter, QPointF(leftX, cy), radius,
                     marked ? markedFill : emptyFill, marked ? markedRim : emptyRim);
            drawRing(painter, QPointF(rightX, cy), radius, emptyFill, emptyRim);
            if (marked) {
                painter.setFont(nameFont);
                painter.setPen(QColor(26, 77, 107));
                painter.drawText(QRectF(area.left(), cy - 12, labelW - 6, 24),
                                 Qt::AlignRight | Qt::AlignVCenter, marks[row]);
            }
            painter.setFont(physicalFont);
            painter.setPen(QColor(138, 160, 176));
            painter.drawText(QRectF(rightX + radius + 6, cy - 10, 36, 20),
                             Qt::AlignLeft | Qt::AlignVCenter, physical[row]);
        }
    }

    void paintLegendItem(QPainter &painter, qreal *x, qreal cy, const QColor &fill,
                         const QColor &rim, const QString &text) const
    {
        drawRing(painter, QPointF(*x + 8, cy), 7, fill, rim);
        QFont font(hitFamily());
        font.setPixelSize(14);
        painter.setFont(font);
        painter.setPen(QColor(26, 77, 107));
        const QFontMetrics fm(font);
        painter.drawText(QPointF(*x + 20, cy + (fm.ascent() - fm.descent()) / 2.0), text);
        *x += 28 + fm.width(text) + 16;
    }

    void paintLegend(QPainter &painter, const QRectF &area, bool empty) const
    {
        qreal x = area.left();
        const qreal cy = area.center().y();
        paintLegendItem(painter, &x, cy, QColor(214, 230, 245), QColor(47, 95, 147),
                        QString::fromUtf8("供体PPP"));
        paintLegendItem(painter, &x, cy, QColor(253, 231, 208), QColor(196, 122, 58),
                        QString::fromUtf8("血清"));
        paintLegendItem(painter, &x, cy, QColor(215, 240, 228), QColor(47, 143, 107),
                        QString::fromUtf8("供体PRP"));
        QFont font(hitFamily());
        font.setPixelSize(14);
        painter.setFont(font);
        painter.setPen(QColor(90, 106, 120));
        const QString hint = empty
                ? QString::fromUtf8("没有待测样本")
                : QString::fromUtf8("环内为样本号后四位");
        painter.drawText(QRectF(x, area.top(), area.right() - x, area.height()),
                         Qt::AlignLeft | Qt::AlignVCenter, hint);
    }

    QList<HitPlaceMark> m_marks;
    QSet<int> m_hidden;
};

QSqlDatabase hitCurveDb()
{
    const QString conn = QStringLiteral("hit-data");
    QSqlDatabase db = QSqlDatabase::database(conn, false);
    if (!db.isValid())
        db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn);
    if (db.isOpen())
        return db;
    const QString dirPath = QCoreApplication::applicationDirPath() + QStringLiteral("/SQLFile");
    QDir().mkpath(dirPath);
    db.setDatabaseName(dirPath + QStringLiteral("/HitData.db"));
    db.open();
    return db;
}

struct HitCurveSpec {
    const char *step;
    const char *nameUtf8;
    QColor color;
    int recordMin;
};

bool hitSampleIdInDb(const QString &sampleId)
{
    if (sampleId.isEmpty())
        return false;
    QSqlDatabase db = hitCurveDb();
    if (!db.isOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT 1 FROM HitRun WHERE sample_id=? LIMIT 1"));
    query.addBindValue(sampleId);
    return query.exec() && query.next();
}

void renameHitSampleData(const QString &fromId, const QString &toId)
{
    if (fromId.isEmpty() || toId.isEmpty() || fromId == toId)
        return;
    QSqlDatabase db = hitCurveDb();
    if (!db.isOpen())
        return;
    QSqlQuery runQuery(db);
    runQuery.prepare(QStringLiteral("UPDATE HitRun SET sample_id=? WHERE sample_id=?"));
    runQuery.addBindValue(toId);
    runQuery.addBindValue(fromId);
    runQuery.exec();
    QSqlQuery pointQuery(db);
    pointQuery.prepare(QStringLiteral("UPDATE HitPoint SET sample_id=? WHERE sample_id=?"));
    pointQuery.addBindValue(toId);
    pointQuery.addBindValue(fromId);
    pointQuery.exec();
}

void deleteHitSampleData(const QString &sampleId)
{
    if (sampleId.isEmpty())
        return;
    QSqlDatabase db = hitCurveDb();
    if (!db.isOpen())
        return;
    QSqlQuery pointQuery(db);
    pointQuery.prepare(QStringLiteral("DELETE FROM HitPoint WHERE sample_id=?"));
    pointQuery.addBindValue(sampleId);
    pointQuery.exec();
    QSqlQuery runQuery(db);
    runQuery.prepare(QStringLiteral("DELETE FROM HitRun WHERE sample_id=?"));
    runQuery.addBindValue(sampleId);
    runQuery.exec();
}

class HitCurveDialog : public QDialog
{
public:
    explicit HitCurveDialog(const QString &sampleId, QWidget *parent)
        : QDialog(parent)
        , m_sampleId(sampleId)
    {
        setWindowTitle(QString::fromUtf8("查看曲线  ") + sampleId);
        setModal(true);
        resize(980, 700);
        setStyleSheet(QStringLiteral("QDialog{background:#E7F0F7;}"));

        m_hint = new QLabel(this);
        m_hint->setWordWrap(true);
        m_hint->setStyleSheet(QStringLiteral(
            "color:#1A4D6B;font-family:'Microsoft YaHei';font-size:14px;"));

        m_plot = new QCustomPlot(this);
        m_plot->setBackground(QBrush(Qt::white));
        m_plot->xAxis->setLabel(QString::fromUtf8("时间(秒)"));
        m_plot->yAxis->setLabel(QString::fromUtf8("聚集率(%)"));
        QFont axisFont(QStringLiteral("Microsoft YaHei"), 10);
        m_plot->xAxis->setLabelFont(axisFont);
        m_plot->yAxis->setLabelFont(axisFont);
        m_plot->xAxis->setTickLabelFont(axisFont);
        m_plot->yAxis->setTickLabelFont(axisFont);
        m_plot->xAxis->grid()->setPen(QPen(QColor(220, 228, 236), 1, Qt::DotLine));
        m_plot->yAxis->grid()->setPen(QPen(QColor(220, 228, 236), 1, Qt::DotLine));
        m_plot->legend->setVisible(true);
        m_plot->legend->setFont(QFont(QStringLiteral("Microsoft YaHei"), 10));
        m_plot->legend->setBrush(QBrush(QColor(255, 255, 255, 220)));
        m_plot->legend->setBorderPen(QPen(QColor(183, 197, 211)));
        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

        const HitCurveSpec toggleSpecs[] = {
            {"spontaneous",
             "自发聚集对照",
             QColor(26, 111, 168), 0},
            {"low",
             "低浓度检测",
             QColor(46, 155, 90), 0},
            {"high",
             "高浓度抑制",
             QColor(224, 122, 47), 0},
            {"negative",
             "阴性对照",
             QColor(122, 78, 171), 0}
        };
        QHBoxLayout *curveBar = new QHBoxLayout;
        curveBar->setSpacing(10);
        for (int i = 0; i < 4; ++i) {
            m_showCurve[i] = true;
            QPushButton *toggle = new QPushButton(QString::fromUtf8(toggleSpecs[i].nameUtf8), this);
            toggle->setCheckable(true);
            toggle->setChecked(true);
            const QString color = toggleSpecs[i].color.name();
            toggle->setStyleSheet(QString::fromUtf8(
                "QPushButton { font-family: 'Microsoft YaHei'; font-size: 14px;"
                " min-height: 36px; padding: 0 12px; border-radius: 8px;"
                " border: 2px solid %1; background: #ffffff; color: #16384A; }"
                "QPushButton:checked { background: %1; color: #ffffff; }").arg(color));
            curveBar->addWidget(toggle, 1);
            connect(toggle, &QPushButton::toggled, this, [this, i](bool on) {
                m_showCurve[i] = on;
                reload();
            });
        }
        m_areaBox = new QCheckBox(QString::fromUtf8("显示面积"), this);
        m_areaBox->setStyleSheet(QStringLiteral(
            "QCheckBox { font-family: 'Microsoft YaHei'; font-size: 14px; color: #16384A; }"));
        connect(m_areaBox, &QCheckBox::toggled, this, [this](bool on) {
            m_showArea = on;
            reload();
        });
        curveBar->addWidget(m_areaBox);

        m_cursorLabel = new QLabel(this);
        m_cursorLabel->setWordWrap(true);
        m_cursorLabel->setStyleSheet(QStringLiteral(
            "color:#16384A;font-family:'Microsoft YaHei';font-size:14px;"));

        m_cursor = new QCPItemStraightLine(m_plot);
        m_cursor->setPen(QPen(QColor(176, 64, 64), 1.6, Qt::DashLine));
        for (int i = 0; i < 4; ++i) {
            m_tracer[i] = new QCPItemTracer(m_plot);
            m_tracer[i]->setStyle(QCPItemTracer::tsCircle);
            m_tracer[i]->setSize(7);
            m_tracer[i]->setVisible(false);
            m_tracer[i]->position->setType(QCPItemPosition::ptPlotCoords);
            m_tracer[i]->position->setAxes(m_plot->xAxis, m_plot->yAxis);
        }
        m_plot->setMouseTracking(true);
        connect(m_plot, &QCustomPlot::mouseMove, this, [this](QMouseEvent *event) {
            if (!m_plot || !m_plot->axisRect()->rect().contains(event->pos()))
                return;
            const QCPRange range = m_plot->xAxis->range();
            m_cursorSec = qBound(range.lower,
                                 m_plot->xAxis->pixelToCoord(event->pos().x()),
                                 range.upper);
            placeCursor();
        });

        QPushButton *closeButton = new QPushButton(
            QString::fromUtf8("关闭"), this);
        closeButton->setStyleSheet(buttonStyle());
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

        QHBoxLayout *buttons = new QHBoxLayout;
        buttons->addStretch(1);
        buttons->addWidget(closeButton);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(10);
        layout->addWidget(m_hint);
        layout->addWidget(m_plot, 1);
        layout->addWidget(m_cursorLabel);
        layout->addLayout(curveBar);
        layout->addLayout(buttons);

        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, [this] { reload(); });
        reload();
        m_timer->start();
    }

private:
    void reload()
    {
        const HitSettings settings = loadHitSettings();
        const HitCurveSpec specs[] = {
            {"spontaneous",
             "自发聚集对照",
             QColor(26, 111, 168), settings.spontaneousRecordMin},
            {"low",
             "低浓度检测",
             QColor(46, 155, 90), settings.lowRecordMin},
            {"high",
             "高浓度抑制",
             QColor(224, 122, 47), settings.highRecordMin},
            {"negative",
             "阴性对照",
             QColor(122, 78, 171), settings.negativeRecordMin}
        };

        int axisSec = 1;
        for (const HitCurveSpec &spec : specs) {
            int minutes = spec.recordMin;
            if (minutes < 0)
                minutes = 0;
            if (minutes > 180)
                minutes = 180;
            axisSec = qMax(axisSec, minutes * 60);
        }

        m_plot->clearGraphs();
        for (int i = 0; i < 4; ++i)
            m_curves[i] = HitDrawnCurve();
        qint64 runId = -1;
        QSqlDatabase db = hitCurveDb();
        if (db.isOpen()) {
            QSqlQuery query(db);
            query.prepare(QStringLiteral(
                "SELECT id FROM HitRun WHERE sample_id=? ORDER BY id DESC LIMIT 1"));
            query.addBindValue(m_sampleId);
            if (query.exec() && query.next())
                runId = query.value(0).toLongLong();
        }

        int drawn = 0;
        double yMin = 0;
        double yMax = 0;
        bool hasValue = false;
        int specIndex = 0;
        for (const HitCurveSpec &spec : specs) {
            const int index = specIndex;
            const bool shown = index < 4 && m_showCurve[index];
            ++specIndex;
            QVector<double> xs;
            QVector<double> ys;
            if (runId >= 0 && db.isOpen()) {
                QSqlQuery query(db);
                query.prepare(QStringLiteral(
                    "SELECT elapsed_ms, agg_percent FROM HitPoint "
                    "WHERE run_id=? AND step=? ORDER BY seq"));
                query.addBindValue(runId);
                query.addBindValue(QString::fromLatin1(spec.step));
                if (query.exec()) {
                    while (query.next()) {
                        xs.append(query.value(0).toDouble() / 1000.0);
                        ys.append(query.value(1).toDouble());
                    }
                }
            }
            if (index < 4) {
                m_curves[index].name = QString::fromUtf8(spec.nameUtf8);
                m_curves[index].color = spec.color;
                m_curves[index].xs = xs;
                m_curves[index].ys = ys;
                m_curves[index].shown = shown && !xs.isEmpty();
            }
            if (!shown || xs.isEmpty())
                continue;
            QCPGraph *graph = m_plot->addGraph();
            graph->setName(QString::fromUtf8(spec.nameUtf8));
            graph->setPen(QPen(spec.color, 2));
            if (m_showArea) {
                QColor fill = spec.color;
                fill.setAlpha(70);
                graph->setBrush(fill);
            } else {
                graph->setBrush(Qt::NoBrush);
            }
            graph->setData(xs, ys);
            ++drawn;
            for (double value : ys) {
                if (!hasValue) {
                    yMin = value;
                    yMax = value;
                    hasValue = true;
                } else {
                    yMin = qMin(yMin, value);
                    yMax = qMax(yMax, value);
                }
            }
        }

        m_plot->xAxis->setRange(0, axisSec);
        if (!hasValue) {
            m_plot->yAxis->setRange(0, 100);
        } else {
            const double pad = qMax(1.0, (yMax - yMin) * 0.08);
            m_plot->yAxis->setRange(qMin(0.0, yMin - pad), qMax(100.0, yMax + pad));
        }
        m_plot->legend->setVisible(drawn > 0);
        if (drawn > 0) {
            m_hint->setText(QString::fromUtf8(
                "样本 %1：横轴按记录时间，一秒一个点。"
                "移动鼠标，竖线停在对应时间，显示各曲线聚集率。")
                            .arg(m_sampleId));
        } else {
            m_hint->setText(QString::fromUtf8(
                "样本 %1 还没有曲线点。")
                            .arg(m_sampleId));
        }
        placeCursor();
    }

    static bool valueAt(const QVector<double> &xs, const QVector<double> &ys, double x, double &y)
    {
        if (xs.size() < 1 || xs.size() != ys.size())
            return false;
        if (x < xs.first() || x > xs.last())
            return false;
        if (xs.size() == 1) {
            y = ys.first();
            return true;
        }
        for (int i = 1; i < xs.size(); ++i) {
            if (x <= xs.at(i)) {
                const double span = xs.at(i) - xs.at(i - 1);
                if (span <= 0) {
                    y = ys.at(i);
                    return true;
                }
                const double t = (x - xs.at(i - 1)) / span;
                y = ys.at(i - 1) + t * (ys.at(i) - ys.at(i - 1));
                return true;
            }
        }
        y = ys.last();
        return true;
    }

    static double curveArea(const QVector<double> &xs, const QVector<double> &ys)
    {
        if (xs.size() < 2 || xs.size() != ys.size())
            return 0;
        double area = 0;
        for (int i = 1; i < xs.size(); ++i) {
            const double dtMin = (xs.at(i) - xs.at(i - 1)) / 60.0;
            if (dtMin <= 0)
                continue;
            area += (ys.at(i) + ys.at(i - 1)) * 0.5 * dtMin;
        }
        return area;
    }

    void placeCursor()
    {
        if (!m_plot || !m_cursor)
            return;
        const QCPRange range = m_plot->xAxis->range();
        const double x = qBound(range.lower, m_cursorSec, range.upper);
        m_cursorSec = x;
        m_cursor->point1->setCoords(x, 0);
        m_cursor->point2->setCoords(x, 1);
        m_cursor->setVisible(true);
        QStringList rates;
        QStringList areas;
        for (int i = 0; i < 4; ++i) {
            const HitDrawnCurve &curve = m_curves[i];
            double y = 0;
            const bool hit = curve.shown && valueAt(curve.xs, curve.ys, x, y);
            if (m_tracer[i]) {
                if (hit) {
                    m_tracer[i]->setPen(QPen(curve.color, 2));
                    m_tracer[i]->setBrush(QBrush(Qt::white));
                    m_tracer[i]->position->setCoords(x, y);
                    m_tracer[i]->setVisible(true);
                } else {
                    m_tracer[i]->setVisible(false);
                }
            }
            if (hit) {
                rates << curve.name + QStringLiteral(" ")
                         + QString::number(y, 'f', 1) + QStringLiteral("%");
            }
            if (m_showArea && curve.shown && curve.xs.size() >= 2) {
                areas << curve.name + QStringLiteral(" ")
                         + QString::number(curveArea(curve.xs, curve.ys), 'f', 1)
                         + QString::fromUtf8(" %·min");
            }
        }
        QStringList lines;
        lines << QString::fromUtf8("竖线 %1 秒").arg(QString::number(x, 'f', 1));
        if (!rates.isEmpty())
            lines << rates.join(QString::fromUtf8("    "));
        if (!areas.isEmpty())
            lines << QString::fromUtf8("面积  ") + areas.join(QString::fromUtf8("    "));
        if (m_cursorLabel)
            m_cursorLabel->setText(lines.join(QLatin1Char('\n')));
        m_plot->replot();
    }

    struct HitDrawnCurve {
        QString name;
        QColor color;
        QVector<double> xs;
        QVector<double> ys;
        bool shown = false;
    };

    QString m_sampleId;
    QLabel *m_hint = nullptr;
    QLabel *m_cursorLabel = nullptr;
    QCheckBox *m_areaBox = nullptr;
    QCustomPlot *m_plot = nullptr;
    QCPItemStraightLine *m_cursor = nullptr;
    QCPItemTracer *m_tracer[4] = {nullptr, nullptr, nullptr, nullptr};
    QTimer *m_timer = nullptr;
    HitDrawnCurve m_curves[4];
    double m_cursorSec = 0;
    bool m_showArea = false;
    bool m_showCurve[4] = {true, true, true, true};
};

} // namespace

HitPage::HitPage(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(217, 230, 240));
    setPalette(pal);

    buildTable();
    buildDelegates();
    m_runner = new HitTestRunner(this);
    connect(m_runner, &HitTestRunner::stepProgress, this,
            [this](int row, int mixIndex, int elapsedMs, int totalMs, int phase) {
        setStepProgress(row, mixIndex, elapsedMs, totalMs, phase);
    });
    connect(m_runner, &HitTestRunner::rowChanged, this,
            [this](int row, const QString &status, const QString &startTime, const QString &endTime,
                   const QString &ppp, const QString &spontaneous, const QString &low,
                   const QString &high, const QString &negative) {
        applyRunUpdate(row, status, startTime, endTime, ppp, spontaneous, low, high, negative);
    });
    connect(m_runner, &HitTestRunner::placeHoleConsumed, this, [this](int hole) {
        if (HitPlaceMap *map = static_cast<HitPlaceMap *>(m_placeMap))
            map->hideHole(hole);
    });
    connect(m_runner, &HitTestRunner::runFinished, this, [this](const QString &message) {
        setRunBusy(false);
        syncPlaceDialog(false);
        if (!message.isEmpty())
            CustomMessageBox::warning(this, QString::fromUtf8("提示"), message);
    });

    m_settingsButton = new QPushButton(QString::fromUtf8("HIT设置"), this);
    m_startButton = new QPushButton(QString::fromUtf8("开始"), this);
    m_pauseButton = new QPushButton(QString::fromUtf8("暂停"), this);
    m_pauseButton->setEnabled(false);
    m_addButton = new QPushButton(QString::fromUtf8("添加"), this);
    m_deleteButton = new QPushButton(QString::fromUtf8("删除"), this);
    m_viewButton = new QPushButton(QString::fromUtf8("曲线"), this);
    m_queryButton = new QPushButton(QString::fromUtf8("查询"), this);
    QPushButton *buttons[] = {
        m_settingsButton, m_startButton, m_pauseButton, m_addButton, m_deleteButton,
        m_viewButton, m_queryButton
    };
    for (QPushButton *button : buttons)
        button->setStyleSheet(buttonStyle());

    QVBoxLayout *rightBar = new QVBoxLayout;
    rightBar->setContentsMargins(0, 0, 0, 0);
    rightBar->setSpacing(14);
    rightBar->addWidget(m_startButton);
    rightBar->addWidget(m_pauseButton);
    rightBar->addWidget(m_settingsButton);
    rightBar->addWidget(m_addButton);
    rightBar->addWidget(m_deleteButton);
    rightBar->addWidget(m_viewButton);
    rightBar->addWidget(m_queryButton);
    rightBar->addStretch(1);

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(16);
    root->addWidget(m_table, 1);
    root->addLayout(rightBar);

    connect(m_settingsButton, &QPushButton::clicked, this, [this] { showHitSettings(); });
    connect(m_startButton, &QPushButton::clicked, this, [this] { showStartMap(); });
    connect(m_pauseButton, &QPushButton::clicked, this, [this] { togglePause(); });
    connect(m_addButton, &QPushButton::clicked, this, [this] { appendTask(); });
    connect(m_table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem *item) {
        commitSampleId(item);
    });
    connect(m_deleteButton, &QPushButton::clicked, this, [this] { removeSelectedRows(); });
    connect(m_viewButton, &QPushButton::clicked, this, [this] { showViewCurve(); });
    connect(m_queryButton, &QPushButton::clicked, this, [this] { showQuery(); });
    setRunBusy(false);

    const QDate today = QDate::currentDate();
    loadRuns(QString(),
             QDateTime(today, QTime(0, 0, 0)),
             QDateTime(today, QTime(23, 59, 59)),
             false);
}

void HitPage::buildTable()
{
    m_table = new QTableWidget(this);
    m_table->setColumnCount(ColCount);
    m_table->setHorizontalHeaderLabels(QStringList()
        << QString::fromUtf8("状态")
        << QString::fromUtf8("样本号")
        << QString::fromUtf8("通道号")
        << QString::fromUtf8("样本标号")
        << QString::fromUtf8("起始时间")
        << QString::fromUtf8("终止时间")
        << QString::fromUtf8("PPP值")
        << QString::fromUtf8("自发聚集Max%")
        << QString::fromUtf8("低浓度Max%")
        << QString::fromUtf8("高浓度Max%")
        << QString::fromUtf8("阴性对照Max%")
        << QString::fromUtf8("判定"));
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(kRowHeight);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(true);
    m_table->setStyleSheet(tableStyle());
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->horizontalHeader()->setMinimumSectionSize(88);
    const int widths[] = {88, 150, 96, 120, 108, 108, 96, 148, 132, 132, 148, 120};
    for (int col = 0; col < ColCount; ++col)
        m_table->setColumnWidth(col, widths[col]);
}

void HitPage::buildDelegates()
{
    QStringList channels;
    const int count = channelCount();
    for (int i = 1; i <= count; ++i)
        channels << QString::number(i);
    m_channelDelegate = new HitComboDelegate(channels);
    m_channelDelegate->setParent(m_table);
    m_table->setItemDelegateForColumn(ColChannel, m_channelDelegate);
    HitHoleDelegate *holeDelegate = new HitHoleDelegate(m_table);
    holeDelegate->setParent(m_table);
    m_table->setItemDelegateForColumn(ColSampleTag, holeDelegate);
    m_table->setItemDelegate(new HitTextDelegate(false, m_table));
}

int HitPage::channelCount() const
{
    quint8 kind = KS1200;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, kind);
    if (kind == KS600)
        return 4;
    if (kind == KS800)
        return 8;
    return 12;
}

QString HitPage::nextSampleId() const
{
    const QString prefix = QStringLiteral("H") + QDate::currentDate().toString(QStringLiteral("yyyyMMdd"));
    int maxSeq = 0;
    for (int row = 0; row < m_table->rowCount(); ++row) {
        const QTableWidgetItem *item = m_table->item(row, ColSampleId);
        if (!item)
            continue;
        const QString text = item->text();
        if (!text.startsWith(prefix) || text.size() < prefix.size() + 4)
            continue;
        bool ok = false;
        const int seq = text.right(4).toInt(&ok);
        if (ok)
            maxSeq = qMax(maxSeq, seq);
    }
    for (int n = 0; n < 10000; ++n) {
        const QString id = prefix + QString("%1").arg(maxSeq + 1 + n, 4, 10, QChar('0'));
        bool inTable = false;
        for (int row = 0; row < m_table->rowCount(); ++row) {
            const QTableWidgetItem *item = m_table->item(row, ColSampleId);
            if (item && item->text() == id) {
                inTable = true;
                break;
            }
        }
        if (!inTable && !hitSampleIdInDb(id))
            return id;
    }
    return prefix + QString("%1").arg(maxSeq + 1, 4, 10, QChar('0'));
}

int HitPage::nextPppHole() const
{
    const QStringList holes = availablePppHoles(m_table, -1);
    if (holes.isEmpty())
        return -1;
    return holes.first().toInt();
}

void HitPage::appendTask()
{
    const int hole = nextPppHole();
    if (hole < 0)
        return;
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setRowHeight(row, kRowHeight);
    m_table->setItem(row, ColStatus, centeredItem(QString::fromUtf8("待测"), false));
    QTableWidgetItem *sampleItem = centeredItem(nextSampleId(), true);
    sampleItem->setData(Qt::UserRole, sampleItem->text());
    m_table->setItem(row, ColSampleId, sampleItem);
    m_table->setItem(row, ColChannel, centeredItem(QStringLiteral("1"), true));
    m_table->setItem(row, ColSampleTag, centeredItem(QString::number(hole), true));
    for (int col = ColStartTime; col < ColSpontaneous; ++col)
        m_table->setItem(row, col, centeredItem(QString(), false));
    installStepBars(row);
    m_table->selectRow(row);
    m_table->scrollTo(m_table->model()->index(row, 0));
}

void HitPage::removeSelectedRows()
{
    QList<int> rows;
    const QModelIndexList selected = m_table->selectionModel()->selectedRows();
    for (const QModelIndex &index : selected)
        rows.append(index.row());
    if (rows.isEmpty()) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("请先选中要删除的样本"));
        return;
    }
    const QString ask = QString::fromUtf8(
        "确定删除选中的 %1 条样本吗？\n"
        "数据库里这些样本的结果和曲线也会一起清除。")
            .arg(rows.size());
    if (CustomMessageBox::question(this, QString::fromUtf8("提示"), ask,
                                   QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;
    std::sort(rows.begin(), rows.end(), [](int a, int b) { return a > b; });
    for (int row : rows) {
        const QTableWidgetItem *item = m_table->item(row, ColSampleId);
        if (item)
            deleteHitSampleData(item->text().trimmed());
        m_table->removeRow(row);
    }
}

void HitPage::togglePause()
{
    if (!m_runner || !m_runner->running())
        return;
    const bool pause = !m_runner->paused();
    m_runner->setPaused(pause);
    if (m_pauseButton) {
        m_pauseButton->setText(pause
            ? QString::fromUtf8("继续")
            : QString::fromUtf8("暂停"));
    }
}

void HitPage::commitSampleId(QTableWidgetItem *item)
{
    if (!item || item->column() != ColSampleId || m_sampleEditLock)
        return;
    const QString now = item->text().trimmed();
    const QString oldId = item->data(Qt::UserRole).toString();
    if (now == oldId)
        return;
    bool used = now.isEmpty();
    for (int row = 0; row < m_table->rowCount() && !used; ++row) {
        if (row == item->row())
            continue;
        const QTableWidgetItem *other = m_table->item(row, ColSampleId);
        if (other && other->text().trimmed() == now)
            used = true;
    }
    if (!used)
        used = hitSampleIdInDb(now);
    if (used) {
        m_sampleEditLock = true;
        item->setText(oldId);
        m_sampleEditLock = false;
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            now.isEmpty()
                ? QString::fromUtf8("样本号不能为空")
                : QString::fromUtf8("样本号已存在"));
        return;
    }
    m_sampleEditLock = true;
    item->setText(now);
    item->setData(Qt::UserRole, now);
    m_sampleEditLock = false;
    renameHitSampleData(oldId, now);
}

void HitPage::showHitSettings()
{
    if (m_runner && m_runner->running()) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("测试进行中，不能修改设置"));
        return;
    }
    const HitSettings loaded = loadHitSettings();
    const QString ul = QStringLiteral(" ul");
    const QString sec = QString::fromUtf8(" 秒");
    const QString minute = QStringLiteral(" min");

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("HIT设置"));
    dialog.setAttribute(Qt::WA_StyledBackground, true);
    dialog.setStyleSheet(QString::fromUtf8(
        "QDialog { background: #E7F0F7; }"
        "QFrame#card { background: #FFFFFF; border: 1px solid #D3DEE8; border-radius: 10px; }"
        "QLabel { font-family: '楷体'; font-size: 16px; color: #1A4D6B; background: transparent; }"
        "QLabel#columnHead { font-size: 15px; color: #5C7384; }"
        "QLabel#reagentName { font-size: 13px; color: #5C7384; }"
        "QSpinBox { font-family: '楷体'; font-size: 16px; color: #16384A;"
        "  background: #F7FBFD; border: 1px solid #C5D5E2; border-radius: 6px; padding: 0 8px; }"
        "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }"
        "QSpinBox::up-arrow, QSpinBox::down-arrow { width: 0px; height: 0px; }"));

    SpinWheelGuard *wheelGuard = new SpinWheelGuard(&dialog);
    QVBoxLayout *root = new QVBoxLayout(&dialog);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(14);
    root->setSizeConstraint(QLayout::SetFixedSize);

    QFrame *pppCard = new QFrame(&dialog);
    pppCard->setObjectName(QStringLiteral("card"));
    pppCard->setAttribute(Qt::WA_StyledBackground, true);
    QHBoxLayout *pppRow = new QHBoxLayout(pppCard);
    pppRow->setContentsMargins(18, 14, 18, 14);
    pppRow->setSpacing(12);
    QLabel *pppLabel = new QLabel(QString::fromUtf8("吸PPP参照"), pppCard);
    pppLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QSpinBox *pppRef = hitSpin(pppCard, wheelGuard, loaded.pppRefUl, 2000, ul, 120);
    pppRow->addWidget(pppLabel);
    pppRow->addWidget(pppRef);
    pppRow->addStretch(1);
    root->addWidget(pppCard);

    QFrame *tableCard = new QFrame(&dialog);
    tableCard->setObjectName(QStringLiteral("card"));
    tableCard->setAttribute(Qt::WA_StyledBackground, true);
    QGridLayout *grid = new QGridLayout(tableCard);
    grid->setContentsMargins(16, 14, 16, 16);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(12);
    const QString headers[] = {
        QString::fromUtf8("血清量"),
        QString::fromUtf8("供体PRP"),
        QString::fromUtf8("试剂"),
        QString::fromUtf8("预孵时间"),
        QString::fromUtf8("记录时间")
    };
    for (int col = 0; col < 5; ++col) {
        QLabel *head = new QLabel(headers[col], tableCard);
        head->setObjectName(QStringLiteral("columnHead"));
        head->setAlignment(Qt::AlignCenter);
        grid->addWidget(head, 0, col + 1);
    }

    auto nameAt = [&](int row, const QString &text) {
        QLabel *name = new QLabel(text, tableCard);
        name->setAlignment(Qt::AlignRight | Qt::AlignBottom);
        name->setMinimumWidth(120);
        grid->addWidget(name, row, 0);
    };

    nameAt(1, QString::fromUtf8("自发聚集对照"));
    QSpinBox *spontaneousSerum = hitSpin(tableCard, wheelGuard, loaded.spontaneousSerumUl, 2000, ul, 108);
    QSpinBox *spontaneousPrp = hitSpin(tableCard, wheelGuard, loaded.spontaneousPrpUl, 2000, ul, 108);
    QSpinBox *spontaneousBuffer = nullptr;
    QSpinBox *spontaneousIncubate = hitSpin(tableCard, wheelGuard, loaded.spontaneousIncubateSec, 7200, sec, 108);
    QSpinBox *spontaneousRecord = hitSpin(tableCard, wheelGuard, loaded.spontaneousRecordMin, 180, minute, 108);
    grid->addWidget(spontaneousSerum, 1, 1, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(spontaneousPrp, 1, 2, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(reagentField(tableCard, wheelGuard,
        QString::fromUtf8("缓冲液"), loaded.spontaneousBufferUl, ul, &spontaneousBuffer), 1, 3, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(spontaneousIncubate, 1, 4, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(spontaneousRecord, 1, 5, Qt::AlignBottom | Qt::AlignHCenter);

    nameAt(2, QString::fromUtf8("低浓度检测"));
    QSpinBox *lowSerum = hitSpin(tableCard, wheelGuard, loaded.lowSerumUl, 2000, ul, 108);
    QSpinBox *lowPrp = hitSpin(tableCard, wheelGuard, loaded.lowPrpUl, 2000, ul, 108);
    QSpinBox *lowHeparin = nullptr;
    QSpinBox *lowIncubate = hitSpin(tableCard, wheelGuard, loaded.lowIncubateSec, 7200, sec, 108);
    QSpinBox *lowRecord = hitSpin(tableCard, wheelGuard, loaded.lowRecordMin, 180, minute, 108);
    grid->addWidget(lowSerum, 2, 1, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(lowPrp, 2, 2, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(reagentField(tableCard, wheelGuard,
        QString::fromUtf8("低剂量肝素"), loaded.lowHeparinUl, ul, &lowHeparin), 2, 3, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(lowIncubate, 2, 4, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(lowRecord, 2, 5, Qt::AlignBottom | Qt::AlignHCenter);

    nameAt(3, QString::fromUtf8("高浓度抑制"));
    QSpinBox *highSerum = hitSpin(tableCard, wheelGuard, loaded.highSerumUl, 2000, ul, 108);
    QSpinBox *highPrp = hitSpin(tableCard, wheelGuard, loaded.highPrpUl, 2000, ul, 108);
    QSpinBox *highHeparin = nullptr;
    QSpinBox *highIncubate = hitSpin(tableCard, wheelGuard, loaded.highIncubateSec, 7200, sec, 108);
    QSpinBox *highRecord = hitSpin(tableCard, wheelGuard, loaded.highRecordMin, 180, minute, 108);
    grid->addWidget(highSerum, 3, 1, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(highPrp, 3, 2, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(reagentField(tableCard, wheelGuard,
        QString::fromUtf8("高剂量肝素"), loaded.highHeparinUl, ul, &highHeparin), 3, 3, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(highIncubate, 3, 4, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(highRecord, 3, 5, Qt::AlignBottom | Qt::AlignHCenter);

    nameAt(4, QString::fromUtf8("阴性对照"));
    QSpinBox *negativePrp = hitSpin(tableCard, wheelGuard, loaded.negativePrpUl, 2000, ul, 108);
    QSpinBox *negativeHeparin = nullptr;
    QSpinBox *negativeIncubate = hitSpin(tableCard, wheelGuard, loaded.negativeIncubateSec, 7200, sec, 108);
    QSpinBox *negativeRecord = hitSpin(tableCard, wheelGuard, loaded.negativeRecordMin, 180, minute, 108);
    grid->addWidget(negativePrp, 4, 2, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(reagentField(tableCard, wheelGuard,
        QString::fromUtf8("低剂量肝素"), loaded.negativeHeparinUl, ul, &negativeHeparin), 4, 3, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(negativeIncubate, 4, 4, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(negativeRecord, 4, 5, Qt::AlignBottom | Qt::AlignHCenter);

    root->addWidget(tableCard);

    QFrame *judgeCard = new QFrame(&dialog);
    judgeCard->setObjectName(QStringLiteral("card"));
    judgeCard->setAttribute(Qt::WA_StyledBackground, true);
    QGridLayout *judgeGrid = new QGridLayout(judgeCard);
    judgeGrid->setContentsMargins(18, 14, 18, 14);
    judgeGrid->setHorizontalSpacing(12);
    judgeGrid->setVerticalSpacing(10);
    QLabel *judgeTitle = new QLabel(QString::fromUtf8("结果判读"), judgeCard);
    judgeTitle->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold;"));
    judgeGrid->addWidget(judgeTitle, 0, 0, 1, 2);
    QLabel *diffLabel = new QLabel(QString::fromUtf8("阳性差值阈值"), judgeCard);
    diffLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QSpinBox *positiveDiff = hitSpin(judgeCard, wheelGuard, loaded.positiveDiffPercent, 100,
                                      QStringLiteral(" %"), 120);
    QLabel *diffHint = new QLabel(
        QString::fromUtf8("低浓度最大聚集率 − 高浓度最大聚集率 ≥ 该值，且对照未超无效阈值，判阳性"),
        judgeCard);
    diffHint->setWordWrap(true);
    diffHint->setObjectName(QStringLiteral("columnHead"));
    judgeGrid->addWidget(diffLabel, 1, 0);
    judgeGrid->addWidget(positiveDiff, 1, 1);
    judgeGrid->addWidget(diffHint, 1, 2);
    QLabel *invalidLabel = new QLabel(QString::fromUtf8("无效/复测阈值"), judgeCard);
    invalidLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QSpinBox *invalidPercent = hitSpin(judgeCard, wheelGuard, loaded.invalidPercent, 100,
                                       QStringLiteral(" %"), 120);
    QLabel *invalidHint = new QLabel(
        QString::fromUtf8("自发聚集对照或阴性对照的最大聚集率 ≥ 该值，判无效/复测"),
        judgeCard);
    invalidHint->setWordWrap(true);
    invalidHint->setObjectName(QStringLiteral("columnHead"));
    judgeGrid->addWidget(invalidLabel, 2, 0);
    judgeGrid->addWidget(invalidPercent, 2, 1);
    judgeGrid->addWidget(invalidHint, 2, 2);
    root->addWidget(judgeCard);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addStretch(1);
    QPushButton *saveButton = new QPushButton(QString::fromUtf8("保存"), &dialog);
    QPushButton *cancelButton = new QPushButton(QString::fromUtf8("取消"), &dialog);
    saveButton->setCursor(Qt::PointingHandCursor);
    cancelButton->setCursor(Qt::PointingHandCursor);
    saveButton->setStyleSheet(QString::fromUtf8(
        "QPushButton { font-family: '楷体'; font-size: 18px;"
        "  min-width: 112px; min-height: 40px; border-radius: 6px;"
        "  background: #1A6FA8; color: #FFFFFF; border: none; }"
        "QPushButton:hover { background: #155E90; }"
        "QPushButton:pressed { background: #114E78; }"));
    cancelButton->setStyleSheet(QString::fromUtf8(
        "QPushButton { font-family: '楷体'; font-size: 18px;"
        "  min-width: 112px; min-height: 40px; border-radius: 6px;"
        "  background: #FFFFFF; color: #1A4D6B; border: 1px solid #C5D5E2; }"
        "QPushButton:hover { background: #F4F8FB; }"
        "QPushButton:pressed { background: #E7F0F7; }"));
    buttons->addWidget(saveButton);
    buttons->addWidget(cancelButton);
    root->addLayout(buttons);

    connect(saveButton, &QPushButton::clicked, &dialog, [&](bool) {
        HitSettings data;
        data.pppRefUl = pppRef->value();
        data.spontaneousSerumUl = spontaneousSerum->value();
        data.spontaneousPrpUl = spontaneousPrp->value();
        data.spontaneousBufferUl = spontaneousBuffer->value();
        data.spontaneousIncubateSec = spontaneousIncubate->value();
        data.spontaneousRecordMin = spontaneousRecord->value();
        data.lowSerumUl = lowSerum->value();
        data.lowPrpUl = lowPrp->value();
        data.lowHeparinUl = lowHeparin->value();
        data.lowIncubateSec = lowIncubate->value();
        data.lowRecordMin = lowRecord->value();
        data.highSerumUl = highSerum->value();
        data.highPrpUl = highPrp->value();
        data.highHeparinUl = highHeparin->value();
        data.highIncubateSec = highIncubate->value();
        data.highRecordMin = highRecord->value();
        data.negativePrpUl = negativePrp->value();
        data.negativeHeparinUl = negativeHeparin->value();
        data.negativeIncubateSec = negativeIncubate->value();
        data.negativeRecordMin = negativeRecord->value();
        data.positiveDiffPercent = positiveDiff->value();
        data.invalidPercent = invalidPercent->value();
        saveHitSettings(data);
        dialog.accept();
    });
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void HitPage::setRunBusy(bool busy)
{
    if (m_startButton)
        m_startButton->setEnabled(true);
    if (m_addButton)
        m_addButton->setEnabled(!busy);
    if (m_deleteButton)
        m_deleteButton->setEnabled(!busy);
    if (m_settingsButton)
        m_settingsButton->setEnabled(!busy);
    if (m_queryButton)
        m_queryButton->setEnabled(!busy);
    if (m_pauseButton) {
        m_pauseButton->setEnabled(busy);
        if (!busy) {
            m_pauseButton->setText(QString::fromUtf8("暂停"));
            if (m_runner)
                m_runner->setPaused(false);
        }
    }
    if (!m_table)
        return;
    const QString pending = QString::fromUtf8("待测");
    for (int row = 0; row < m_table->rowCount(); ++row) {
        const QTableWidgetItem *statusItem = m_table->item(row, ColStatus);
        const bool pendingRow = statusItem && statusItem->text() == pending;
        const int columns[] = {ColSampleId, ColSampleTag};
        for (int col : columns) {
            QTableWidgetItem *idItem = m_table->item(row, col);
            if (!idItem)
                continue;
            Qt::ItemFlags flags = idItem->flags();
            const bool editable = !busy && (col == ColSampleId || pendingRow);
            if (editable)
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEditable;
            idItem->setFlags(flags);
        }
    }
}

void HitPage::installStepBars(int row)
{
    if (!m_table || row < 0 || row >= m_table->rowCount())
        return;
    const HitSettings settings = loadHitSettings();
    const int incubateMs[] = {
        settings.spontaneousIncubateSec * 1000,
        settings.lowIncubateSec * 1000,
        settings.highIncubateSec * 1000,
        settings.negativeIncubateSec * 1000
    };
    const int columns[] = {ColSpontaneous, ColLow, ColHigh, ColNegative};
    for (int i = 0; i < 4; ++i) {
        int totalMs = incubateMs[i];
        if (totalMs < 1000)
            totalMs = 1000;
        QProgressBar *bar = new QProgressBar(m_table);
        bar->setRange(0, totalMs);
        bar->setValue(0);
        bar->setTextVisible(true);
        bar->setAlignment(Qt::AlignCenter);
        bar->setFormat(QString::fromUtf8("预孵 ") + stepClockText(0)
                       + QStringLiteral(" / ") + stepClockText(totalMs));
        bar->setStyleSheet(stepBarStyle());
        m_table->setCellWidget(row, columns[i], bar);
    }
}

void HitPage::setStepProgress(int row, int mixIndex, int elapsedMs, int totalMs, int phase)
{
    if (!m_table || row < 0 || row >= m_table->rowCount() || mixIndex < 0 || mixIndex > 3 || totalMs <= 0)
        return;
    const int columns[] = {ColSpontaneous, ColLow, ColHigh, ColNegative};
    const int column = columns[mixIndex];
    QProgressBar *bar = qobject_cast<QProgressBar *>(m_table->cellWidget(row, column));
    if (!bar) {
        bar = new QProgressBar(m_table);
        bar->setTextVisible(true);
        bar->setAlignment(Qt::AlignCenter);
        bar->setStyleSheet(stepBarStyle());
        m_table->setCellWidget(row, column, bar);
    }
    if (elapsedMs < 0)
        elapsedMs = 0;
    if (elapsedMs > totalMs)
        elapsedMs = totalMs;
    const QString title = phase == 1
            ? QString::fromUtf8("记录 ")
            : QString::fromUtf8("预孵 ");
    bar->setRange(0, totalMs);
    bar->setValue(elapsedMs);
    bar->setFormat(title + stepClockText(elapsedMs) + QStringLiteral(" / ") + stepClockText(totalMs));
}

void HitPage::applyRunUpdate(int row, const QString &status, const QString &startTime,
                             const QString &endTime, const QString &ppp,
                             const QString &spontaneous, const QString &low,
                             const QString &high, const QString &negative)
{
    if (!m_table || row < 0 || row >= m_table->rowCount())
        return;
    const auto clockText = [](const QString &text) {
        const int space = text.lastIndexOf(QLatin1Char(' '));
        if (space > 0 && text.left(space).contains(QLatin1Char('-')))
            return text.mid(space + 1);
        return text;
    };
    const QString texts[] = {
        status, QString(), QString(), QString(), clockText(startTime), clockText(endTime), ppp,
        spontaneous, low, high, negative
    };
    const int columns[] = {
        ColStatus, ColSampleId, ColChannel, ColSampleTag, ColStartTime, ColEndTime, ColPpp,
        ColSpontaneous, ColLow, ColHigh, ColNegative
    };
    const int updateCount = static_cast<int>(sizeof(columns) / sizeof(columns[0]));
    for (int i = 0; i < updateCount; ++i) {
        if (texts[i].isEmpty())
            continue;
        if (columns[i] >= ColSpontaneous)
            m_table->removeCellWidget(row, columns[i]);
        QTableWidgetItem *item = m_table->item(row, columns[i]);
        if (!item) {
            m_table->setItem(row, columns[i], centeredItem(texts[i], false));
            continue;
        }
        item->setText(texts[i]);
    }
    fillHitVerdict(m_table, row);
}

void collectPendingHit(const QTableWidget *table, int channelMax,
                       QList<HitPlaceMark> &marks, QVector<HitSampleJob> &jobs)
{
    marks.clear();
    jobs.clear();
    if (!table)
        return;
    const QString pending = QString::fromUtf8("待测");
    for (int row = 0; row < table->rowCount(); ++row) {
        const QTableWidgetItem *statusItem = table->item(row, ColStatus);
        const QTableWidgetItem *idItem = table->item(row, ColSampleId);
        const QTableWidgetItem *channelItem = table->item(row, ColChannel);
        const QTableWidgetItem *tagItem = table->item(row, ColSampleTag);
        if (!statusItem || statusItem->text().trimmed() != pending || !tagItem)
            continue;
        bool ok = false;
        const int hole = tagItem->text().toInt(&ok);
        bool channelOk = false;
        const int channel = channelItem ? channelItem->text().toInt(&channelOk) : 0;
        if (!ok || hole <= 0 || !channelOk || channel < 1 || channel > channelMax)
            continue;
        HitPlaceMark mark;
        mark.sampleId = idItem ? idItem->text() : QString();
        mark.pppHole = hole;
        marks.append(mark);
        HitSampleJob job;
        job.row = row;
        job.sampleId = mark.sampleId;
        job.channel = channel;
        job.pppHole = hole;
        jobs.append(job);
    }
}

void HitPage::ensurePlaceDialog()
{
    if (m_placeDialog)
        return;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QString::fromUtf8("HIT加样位置"));
    dialog->setAttribute(Qt::WA_StyledBackground, true);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setStyleSheet(QString::fromUtf8("QDialog { background: #d9e6f0; }"));
    const QSize host = window() ? window()->size() : QSize(1200, 760);
    dialog->resize(qMax(960, host.width() - 80), qMax(560, host.height() - 120));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);
    HitPlaceMap *map = new HitPlaceMap(dialog);
    layout->addWidget(map, 1);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addStretch(1);
    QPushButton *closeButton = new QPushButton(QString::fromUtf8("关闭"), dialog);
    QPushButton *confirmButton = new QPushButton(QString::fromUtf8("确认测试"), dialog);
    closeButton->setStyleSheet(buttonStyle());
    confirmButton->setStyleSheet(buttonStyle());
    buttons->addWidget(closeButton);
    buttons->addWidget(confirmButton);
    layout->addLayout(buttons);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::hide);
    connect(confirmButton, &QPushButton::clicked, this, [this] {
        if (!m_runner || (m_runner->running()))
            return;
        QList<HitPlaceMark> marks;
        QVector<HitSampleJob> jobs;
        collectPendingHit(m_table, channelCount(), marks, jobs);
        QString reason;
        if (!m_runner->start(jobs, &reason)) {
            if (!reason.isEmpty())
                CustomMessageBox::warning(this, QString::fromUtf8("提示"), reason);
            return;
        }
        if (m_placeConfirm)
            m_placeConfirm->setEnabled(false);
        setRunBusy(true);
    });

    m_placeDialog = dialog;
    m_placeMap = map;
    m_placeConfirm = confirmButton;
}

void HitPage::syncPlaceDialog(bool testing)
{
    if (!m_placeDialog)
        return;
    if (!testing) {
        QList<HitPlaceMark> marks;
        QVector<HitSampleJob> jobs;
        collectPendingHit(m_table, channelCount(), marks, jobs);
        if (HitPlaceMap *map = static_cast<HitPlaceMap *>(m_placeMap))
            map->setMarks(marks);
    }
    if (m_placeConfirm)
        m_placeConfirm->setEnabled(!testing);
}

void HitPage::showStartMap()
{
    ensurePlaceDialog();
    const bool testing = m_runner && m_runner->running();
    syncPlaceDialog(testing);
    m_placeDialog->show();
    m_placeDialog->raise();
    m_placeDialog->activateWindow();
}

void HitPage::showViewCurve()
{
    const int row = m_table ? m_table->currentRow() : -1;
    if (row < 0) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("请先选中一行样本"));
        return;
    }
    const QTableWidgetItem *item = m_table->item(row, ColSampleId);
    const QString sampleId = item ? item->text().trimmed() : QString();
    if (sampleId.isEmpty()) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("请先选中一行样本"));
        return;
    }
    HitCurveDialog dialog(sampleId, this);
    dialog.exec();
}

void HitPage::showQuery()
{
    if (m_runner && m_runner->running()) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("测试进行中，不能查询"));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("查询"));
    dialog.setAttribute(Qt::WA_StyledBackground, true);
    dialog.setStyleSheet(QString::fromUtf8(
        "QDialog { background: #E7F0F7; }"
        "QLabel { font-family: '楷体'; font-size: 16px; color: #1A4D6B; background: transparent; }"
        "QLineEdit { font-family: '楷体'; font-size: 16px; color: #16384A;"
        "  background: #FFFFFF; border: 1px solid #C5D5E2; border-radius: 6px; padding: 0 8px;"
        "  selection-background-color: #D4EAF5; selection-color: #16384A; }"
        "QDateTimeEdit { font-family: '楷体'; font-size: 16px; color: #16384A;"
        "  background-color: #FFFFFF; border: 1px solid #C5D5E2; border-radius: 6px; padding: 0 8px;"
        "  selection-background-color: #D4EAF5; selection-color: #16384A; }"
        "QDateTimeEdit QLineEdit { background-color: #FFFFFF; color: #16384A; border: none;"
        "  selection-background-color: #D4EAF5; selection-color: #16384A; }"
        "QDateTimeEdit::drop-down { width: 22px; border: none; background: #FFFFFF; }"
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background: #E7F0F7; }"
        "QCalendarWidget QToolButton { background: #E7F0F7; color: #1A4D6B; font-size: 14px; }"
        "QCalendarWidget QToolButton:hover { background: #D4EAF5; }"
        "QCalendarWidget QMenu { background: #FFFFFF; color: #16384A; }"
        "QCalendarWidget QSpinBox { background: #FFFFFF; color: #16384A;"
        "  selection-background-color: #D4EAF5; selection-color: #16384A; }"
        "QCalendarWidget QAbstractItemView:enabled { background: #FFFFFF; color: #16384A;"
        "  selection-background-color: #4A7AB5; selection-color: #FFFFFF; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #A0B0BC; }"));

    SpinWheelGuard *wheelGuard = new SpinWheelGuard(&dialog);
    QVBoxLayout *root = new QVBoxLayout(&dialog);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);
    root->setSizeConstraint(QLayout::SetFixedSize);

    QLabel *hint = new QLabel(
        QString::fromUtf8("样本号和时间各自生效。都填写时同时按两个条件查。"),
        &dialog);
    hint->setWordWrap(true);
    hint->setFixedWidth(360);
    root->addWidget(hint);

    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(10);
    QLineEdit *sampleEdit = new QLineEdit(&dialog);
    sampleEdit->setFixedHeight(36);
    sampleEdit->setMinimumWidth(230);
    const QDate today = QDate::currentDate();
    QDateTimeEdit *fromEdit = new QDateTimeEdit(QDateTime(today, QTime(0, 0, 0)), &dialog);
    QDateTimeEdit *toEdit = new QDateTimeEdit(QDateTime(today, QTime(23, 59, 59)), &dialog);
    QDateTimeEdit *edits[] = {fromEdit, toEdit};
    for (QDateTimeEdit *edit : edits) {
        edit->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        edit->setCalendarPopup(true);
        edit->setFixedHeight(36);
        edit->installEventFilter(wheelGuard);
        QPalette pal = edit->palette();
        pal.setColor(QPalette::Base, QColor(255, 255, 255));
        pal.setColor(QPalette::Text, QColor(22, 56, 74));
        pal.setColor(QPalette::Window, QColor(255, 255, 255));
        pal.setColor(QPalette::Button, QColor(255, 255, 255));
        pal.setColor(QPalette::ButtonText, QColor(22, 56, 74));
        pal.setColor(QPalette::Highlight, QColor(212, 234, 245));
        pal.setColor(QPalette::HighlightedText, QColor(22, 56, 74));
        edit->setPalette(pal);
        edit->setAutoFillBackground(true);
        if (QLineEdit *inner = edit->findChild<QLineEdit *>()) {
            inner->setPalette(pal);
            inner->setAutoFillBackground(true);
            inner->setStyleSheet(QString::fromUtf8(
                "QLineEdit { background-color: #FFFFFF; color: #16384A; border: none;"
                " selection-background-color: #D4EAF5; selection-color: #16384A; }"));
        }
        if (QCalendarWidget *calendar = edit->calendarWidget()) {
            QPalette calendarPal = calendar->palette();
            calendarPal.setColor(QPalette::Base, QColor(255, 255, 255));
            calendarPal.setColor(QPalette::Window, QColor(231, 240, 247));
            calendarPal.setColor(QPalette::Text, QColor(22, 56, 74));
            calendarPal.setColor(QPalette::WindowText, QColor(26, 77, 107));
            calendarPal.setColor(QPalette::Button, QColor(231, 240, 247));
            calendarPal.setColor(QPalette::ButtonText, QColor(26, 77, 107));
            calendarPal.setColor(QPalette::Highlight, QColor(74, 122, 181));
            calendarPal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
            calendar->setPalette(calendarPal);
            calendar->setAutoFillBackground(true);
            calendar->setStyleSheet(QString::fromUtf8(
                "QCalendarWidget QWidget#qt_calendar_navigationbar { background: #E7F0F7; }"
                "QCalendarWidget QToolButton { background: #E7F0F7; color: #1A4D6B; }"
                "QCalendarWidget QMenu { background: #FFFFFF; color: #16384A; }"
                "QCalendarWidget QSpinBox { background: #FFFFFF; color: #16384A; }"
                "QCalendarWidget QAbstractItemView:enabled { background: #FFFFFF; color: #16384A;"
                " selection-background-color: #4A7AB5; selection-color: #FFFFFF; }"));
        }
    }
    grid->addWidget(new QLabel(QString::fromUtf8("样本号"), &dialog), 0, 0);
    grid->addWidget(sampleEdit, 0, 1);
    grid->addWidget(new QLabel(QString::fromUtf8("开始时间"), &dialog), 1, 0);
    grid->addWidget(fromEdit, 1, 1);
    grid->addWidget(new QLabel(QString::fromUtf8("结束时间"), &dialog), 2, 0);
    grid->addWidget(toEdit, 2, 1);
    root->addLayout(grid);

    QHBoxLayout *quick = new QHBoxLayout;
    quick->addStretch(1);
    QPushButton *todayButton = new QPushButton(QString::fromUtf8("本日"), &dialog);
    QPushButton *monthButton = new QPushButton(QString::fromUtf8("本月"), &dialog);
    todayButton->setStyleSheet(buttonStyle());
    monthButton->setStyleSheet(buttonStyle());
    quick->addWidget(todayButton);
    quick->addWidget(monthButton);
    root->addLayout(quick);
    connect(todayButton, &QPushButton::clicked, &dialog, [fromEdit, toEdit, today] {
        fromEdit->setDateTime(QDateTime(today, QTime(0, 0, 0)));
        toEdit->setDateTime(QDateTime(today, QTime(23, 59, 59)));
    });
    connect(monthButton, &QPushButton::clicked, &dialog, [fromEdit, toEdit, today] {
        const QDate first(today.year(), today.month(), 1);
        const QDate last(today.year(), today.month(), today.daysInMonth());
        fromEdit->setDateTime(QDateTime(first, QTime(0, 0, 0)));
        toEdit->setDateTime(QDateTime(last, QTime(23, 59, 59)));
    });

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addStretch(1);
    QPushButton *cancelButton = new QPushButton(QString::fromUtf8("取消"), &dialog);
    QPushButton *queryButton = new QPushButton(QString::fromUtf8("查询"), &dialog);
    cancelButton->setStyleSheet(buttonStyle());
    queryButton->setStyleSheet(buttonStyle());
    buttons->addWidget(cancelButton);
    buttons->addWidget(queryButton);
    root->addLayout(buttons);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(queryButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString sampleId = sampleEdit->text().trimmed();
    if (fromEdit->dateTime() > toEdit->dateTime()) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("开始时间不能晚于结束时间"));
        return;
    }
    loadRuns(sampleId, fromEdit->dateTime(), toEdit->dateTime(), true);
}

void HitPage::loadRuns(const QString &sampleId, const QDateTime &from, const QDateTime &to, bool tellEmpty)
{
    if (m_runner && m_runner->running()) {
        if (tellEmpty) {
            CustomMessageBox::warning(this, QString::fromUtf8("提示"),
                QString::fromUtf8("测试进行中，不能查询"));
        }
        return;
    }
    if (!m_table)
        return;

    QSqlDatabase db = hitCurveDb();
    if (!db.isOpen()) {
        if (tellEmpty) {
            CustomMessageBox::warning(this, QString::fromUtf8("提示"),
                QString::fromUtf8("还没有HIT记录"));
        }
        return;
    }

    const QString id = sampleId.trimmed();
    QString sql = QStringLiteral(
        "SELECT sample_id, channel, sample_tag, start_time, end_time, ppp, "
        "spontaneous_percent, low_percent, high_percent, negative_percent, status "
        "FROM HitRun WHERE 1=1");
    if (!id.isEmpty())
        sql += QStringLiteral(" AND sample_id LIKE ? ESCAPE '\\'");
    const bool useTime = from.isValid() && to.isValid();
    const QDateTime todayStart(QDate::currentDate(), QTime(0, 0, 0));
    const QDateTime todayEnd(QDate::currentDate(), QTime(23, 59, 59));
    const bool coversToday = useTime && from <= todayEnd && to >= todayStart;
    if (useTime) {
        if (coversToday) {
            sql += QStringLiteral(
                " AND ((start_time >= ? AND start_time <= ?) "
                "OR start_time NOT LIKE '____-__-__ %')");
        } else {
            sql += QStringLiteral(" AND start_time >= ? AND start_time <= ?");
        }
    }
    sql += QStringLiteral(" ORDER BY start_time DESC, id DESC");

    QSqlQuery query(db);
    query.prepare(sql);
    if (!id.isEmpty()) {
        QString escaped = id;
        escaped.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
        escaped.replace(QLatin1Char('%'), QStringLiteral("\\%"));
        escaped.replace(QLatin1Char('_'), QStringLiteral("\\_"));
        query.addBindValue(QStringLiteral("%") + escaped + QStringLiteral("%"));
    }
    if (useTime) {
        query.addBindValue(from.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
        query.addBindValue(to.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    }
    if (!query.exec()) {
        if (tellEmpty) {
            CustomMessageBox::warning(this, QString::fromUtf8("提示"),
                QString::fromUtf8("还没有HIT记录"));
        }
        return;
    }

    const auto clockText = [](const QString &text) {
        const int space = text.lastIndexOf(QLatin1Char(' '));
        if (space > 0 && text.left(space).contains(QLatin1Char('-')))
            return text.mid(space + 1);
        return text;
    };
    const auto percentText = [&query](int column) {
        if (query.value(column).isNull())
            return QString();
        return QString::number(query.value(column).toDouble(), 'f', 1);
    };
    const QString pending = QString::fromUtf8("待测");
    m_sampleEditLock = true;
    for (int row = m_table->rowCount() - 1; row >= 0; --row) {
        const QTableWidgetItem *statusItem = m_table->item(row, ColStatus);
        if (!statusItem || statusItem->text() != pending)
            m_table->removeRow(row);
    }

    int found = 0;
    while (query.next()) {
        const int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setRowHeight(row, kRowHeight);
        m_table->setItem(row, ColStatus, centeredItem(query.value(10).toString(), false));
        const QString loadedId = query.value(0).toString();
        QTableWidgetItem *sampleItem = centeredItem(loadedId, true);
        sampleItem->setData(Qt::UserRole, loadedId);
        m_table->setItem(row, ColSampleId, sampleItem);
        m_table->setItem(row, ColChannel, centeredItem(
            query.value(1).isNull() ? QString() : QString::number(query.value(1).toInt()), false));
        m_table->setItem(row, ColSampleTag, centeredItem(
            query.value(2).isNull() ? QString() : QString::number(query.value(2).toInt()), false));
        m_table->setItem(row, ColStartTime, centeredItem(clockText(query.value(3).toString()), false));
        m_table->setItem(row, ColEndTime, centeredItem(clockText(query.value(4).toString()), false));
        m_table->setItem(row, ColPpp, centeredItem(
            query.value(5).isNull() ? QString() : QString::number(query.value(5).toDouble()), false));
        m_table->setItem(row, ColSpontaneous, centeredItem(percentText(6), false));
        m_table->setItem(row, ColLow, centeredItem(percentText(7), false));
        m_table->setItem(row, ColHigh, centeredItem(percentText(8), false));
        m_table->setItem(row, ColNegative, centeredItem(percentText(9), false));
        fillHitVerdict(m_table, row);
        ++found;
    }
    m_sampleEditLock = false;
    if (tellEmpty && found == 0) {
        CustomMessageBox::warning(this, QString::fromUtf8("提示"),
            QString::fromUtf8("没有符合条件的记录"));
    }
}
