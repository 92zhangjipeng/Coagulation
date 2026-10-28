// customfixtableview.h
#ifndef CUSTOMFIXTABLEVIEW_H
#define CUSTOMFIXTABLEVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QScreen>
#include <QApplication>
#include <QCloseEvent>
#include <QMutex>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <qcustomplot.h>
#include <suoweiFileManager/hospitalreportprinter.h>
#include "opencvFindRBC/customtitlebar.h"
#include "PrintReport/AggregationAnalyzer.h"

#include "creatreagentcurve.h"

namespace Ui {
    class CustomFixTableView;
}

// 常量定义
namespace TableConstants {
    static constexpr int ICON_SIZE = 16;
    static constexpr int LAYOUT_SPACING = 5;
    static constexpr int LAYOUT_MARGIN = 5;
    static constexpr int WINDOW_MIN_WIDTH = 1400;
    static constexpr int WINDOW_MIN_HEIGHT = 700;
    static constexpr int TABLE_ROW_HEIGHT = 50;
    static constexpr int LAG_TIME_WINDOW_SIZE = 15;
    static constexpr double BASELINE_THRESHOLD = 3.0;
    static constexpr int CURVE_DATA_SIZE = 300;
    static constexpr int SLOPE_WINDOW_SIZE = 15;
    static constexpr int MIN_DATA_POINTS = 10;
    static constexpr int TITLE_BAR_HEIGHT = 44;
}

// 辅助结构体
struct TimeSegmentAverages {
    double avg0to60 = 0.0;
    double avg60to180 = 0.0;
    double avg180to300 = 0.0;
    double avg0to300 = 0.0;
};

// Max%列自定义编辑Delegate：美化编辑框、限制在单元格内
class MaxPercentDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MaxPercentDelegate(int editableCol, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_editableCol(editableCol) {}

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
        const QModelIndex &index) const override
    {
        if (index.column() != m_editableCol)
            return nullptr;

        QString currentText = index.data(Qt::DisplayRole).toString();
        if (currentText.isEmpty() || currentText.trimmed().isEmpty())
            return nullptr;

        QLineEdit *editor = new QLineEdit(parent);
        editor->setAlignment(Qt::AlignCenter);
        editor->setFrame(false);
        editor->setTextMargins(4, 2, 4, 2);
        editor->setStyleSheet(QString(
            "QLineEdit {"
            "    background-color: #ffffff;"
            "    border: 2px solid #1e90ff;"
            "    border-radius: 4px;"
            "    padding: 2px 8px;"
            "    color: #005c99;"
            "    font-family: '微软雅黑';"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "    selection-background-color: #4da6ff;"
            "    selection-color: #ffffff;"
            "}"
        ));
        editor->setValidator(new QDoubleValidator(0.0, 999.99, 2, editor));
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        if (!lineEdit) return;
        lineEdit->setText(index.data(Qt::DisplayRole).toString());
        lineEdit->selectAll();
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const override
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        if (!lineEdit) return;
		QString newText = lineEdit->text().trimmed();
		const QString oldText = index.data(Qt::DisplayRole).toString().trimmed();
		if (newText.isEmpty() || newText == oldText) {
			return;
		}
		model->setData(index, newText, Qt::EditRole);
		emit editorDataCommitted(index.row(), index.column());
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override
    {
        Q_UNUSED(index);
        QRect rect = option.rect;
        rect.adjust(2, 4, -2, -4);
        editor->setGeometry(rect);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
signals:
	void editorDataCommitted(int row, int col) const;
private:
    int m_editableCol;
};

class CustomFixTableView : public QWidget
{
    Q_OBJECT

public:
    explicit CustomFixTableView(QWidget *parent = nullptr);
    ~CustomFixTableView();

    void showWithAnimation();
    void hideWithAnimation();
    void setClickViewId(QString idstr, const int& idindex);
    void showResult(const bool &smooth);
    void setCheckBoxState(const bool& isstate, const QString &sampleSex,
        const QList<QString> &TableSampleidList);
    void saveOriginalCurveData(int row);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onHideFinished();
    void on_checkBoxSmooth_clicked();
    void on_checkBoxAUC_clicked();
    void updateAUCDisplay();
    void on_pushButtonBack_clicked();
    void on_pushButtonNext_clicked();
    void on_printButton_clicked();
    void onCurveMouseMove(QMouseEvent* event);
    void onMaximizeRequested();
    void onMinimizeRequested();
    void onCloseRequested();
	void onTableCellDataChanged(int row, int col);
    void onTableContextMenu(const QPoint &pos);

public slots:
    void viewOneReagentCurve(int rows, int cols);

private:
    Ui::CustomFixTableView *ui;

    // 自定义标题栏
    CustomTitleBar *m_titleBar;

    // 主内容区域容器
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;

    // UI控件成员变量
    QGridLayout *m_gridLayout;
    QWidget *m_widgetShowInfo;
    QWidget *m_widgetbottom;
    QTableWidget *m_tableWidget;
    QCustomPlot *m_widgetCurveShow;
    QLabel *m_analysisReport;
    QPushButton *m_pushButtonBack;
    QPushButton *m_pushButtonNext;
    QPushButton *m_printButton;
    QLabel *m_labelname;
    QLabel *m_labelAAColor, *m_labelAAcurve;
    QLabel *m_labelADPColor, *m_labelADpcurve;
    QLabel *m_labelEPIColor, *m_labelEPIcurve;
    QLabel *m_labelcolColor, *m_labelcolcurve;
    QLabel *m_labelrisColor, *m_labelriscurve;

    // 动画相关 - 使用原始指针
    QPropertyAnimation *m_showAnim;
    QPropertyAnimation *m_hideAnim;
    QScreen *m_currentScreen;

    // 交互式曲线元素 - 使用原始指针
    QCPItemStraightLine *m_verticalLine;
    QCPItemText *m_dataPointLabel;
    QCPItemTracer *m_dataPointTracerAA;
    QCPItemTracer *m_dataPointTracerADP;
    QCPItemTracer *m_dataPointTracerEPI;
    QCPItemTracer *m_dataPointTracerCOL;
    QCPItemTracer *m_dataPointTracerRIS;
    QVector<double> m_currentCurveData;

    // 业务变量
    QString m_viewIDstr;
    int m_viewIDNum;
    QString m_sampleSex;
    QList<QString> m_SampleidList;

    // 可复用的容器
    QVector<double> m_tempXData;
    QVector<double> m_tempYData;

    // 线程安全
    mutable QMutex m_curveDataMutex;

    enum class TableItemnum {
        ReagentName,
        ReagentState,
        Aggregation60s,
        Aggregation180s,
        Aggregation300s,
        AggregationMax,
        Slope,
        TMAtime,
        LagTime,
        AUC,
        OutResult,
        ReferenceValue
    };

    enum class Status {
        Completed,
        NotTested,
        Pending
    };

    // 主要功能函数
    void showCustomAnalyzerResult(double maxAggregation, double slope, double auc,
        double timeToMax, double lagTime,
        double avg0to60, double avg60to180,
        double avg180to300, double avg0to300);

    void initCreateCPGraph(QCustomPlot* pshowcurvedata);
    void initCreateCurveWidget(QCustomPlot *customPlot);
    void setupSmoothInteractions(QCustomPlot* customPlot);
    void setupBeautifulAxes(QCustomPlot* customPlot);
    void setupMedicalAxis(QCPAxis* axis, const QString& label,
        double lower, double upper,
        double tickStep, int tickCount);
    void setupElegantGrid(QCustomPlot* customPlot);
    void addShadowEffect(QCustomPlot* customPlot);
    void showCurveTestEnd(const quint8& testEndReagent, const bool &smooth);

    // 数据验证和计算辅助函数
    bool validateCurveData(const QVector<double>& data, quint8 reagent) const;
    TimeSegmentAverages calculateTimeAverages(const QVector<double>& data) const;

    // 曲线数据清除
    QList<QCPGraph*> calibrationGraphs() const;
    void clearGraphData(QCPGraph* graph);
    void CreateResultCurve();
    void updatetabletestedResult(const quint8& indexReag);
    void clearResultTable(QTableWidget *pTable);

    // 曲线图形指针
    QCPGraph *m_showAACpgraph;
    QCPGraph *m_showADPCpgraph;
    QCPGraph *m_showEPICpgraph;
    QCPGraph *m_showCOLCpgraph;
    QCPGraph *m_showRISCpgraph;

    // UI控件指针
    QCheckBox *m_checkBoxAUC;
    QCheckBox *m_checkBoxSmooth;

    // 表格初始化
    void initShowResultWidget(QTableWidget *Table);
    void insertColumnText(QTableWidget *tablewiget,
        const int row, const int col,
        const QColor &bgmC,
        const QString& text);
    void insertColumnText(QTableWidget *tablewiget,
        const int row, const int col,
        const QString& text);
    void updateParaState(QTableWidget *tablewiget,
        const int row, const int col,
        const QColor &bgmC,
        const QString& iconPath,
        const QString& text);
    void setReagentStatus(int row, int col, Status status);
    void spanTableWidget(const int &fromrows, const int& indexCols, QString pathicon, QString outResult);

    // 参考值相关
    bool updateSetSexReferValue();
    void Analyzeresultingvalues(const bool &alreadysetSex);
    void viewOneSelf(const QString &idstr, const int &idinter);

    // 辅助标记成员变量 - 使用原始指针
    QCPItemTracer *m_maxAggregationTracer;
    QCPItemText *m_maxAggregationLabel;
    QCPItemCurve *m_aucCurve;
    QVector<QCPGraph*> m_aucFillGraphs;
    QCPItemStraightLine *m_slopeLine;
    QCPItemText *m_slopeLabel;
    QCPItemTracer *m_slopeStartTracer;
    QCPItemTracer *m_slopeEndTracer;
    QCPItemTracer *m_lagTimeTracer;
    QCPItemStraightLine *m_lagTimeLine;
    QCPItemText *m_lagTimeLabel;

    // 辅助方法
    QPair<double, double> markMaxAggregation(const QVector<double>& data, QCPGraph* graph);
    double calculateAndDrawAUC(const QVector<double>& data, QCPGraph* graph);
    void refreshAucFill();
    double calculateAndDrawSlope(const QVector<double>& data, QCPGraph* graph);
    void clearAuxiliaryItems();
    QPair<int, int> findSteepestSegment(const QVector<double>& data, int windowSize = 10);
    double calculateSlope(const QVector<double>& data, int start, int end);
    double calculateLagTime(const QVector<double>& data);
    void drawLagTime(double lagTimeMinutes, const QVector<double>& data);

    void updateTableWithCalculatedParams(const quint8& reagent, double slope, double timeToMax, double lagTime, double auc);

    // UI创建辅助函数
    void createDynamicUI();
    void createColorLabelPair(QHBoxLayout *layout, const QString &text, const QString &color,
        QLabel *&colorLabel, QLabel *&textLabel);

    // 创建UI组件的辅助函数
    QLabel* createImageLabel(const QString& iconPath);
    QLabel* createTextLabel(const QString& text);
    void configureLayout(QHBoxLayout* layout, QLabel* imageLabel, QLabel* textLabel);
    void configureContainer(QWidget* container, QHBoxLayout* layout, const QColor& bgmC);


    double m_lastMouseX;   // 上一次鼠标移动时的 X 坐标（用于节流）
};


// 安全删除 QCustomPlot 中的 item（带空指针检查）
#define SAFE_DELETE_CURVE_ITEM(widget, item) \
    do { \
        if (widget && item) { \
            try { \
                widget->removeItem(item); \
                 /* 不要 delete item，因为 Qt 的对象树会管理其生命周期 */ \
                item = nullptr; \
            } catch (...) { \
                item = nullptr; \
            } \
        } else if (item) { \
            item = nullptr; \
        } \
    } while(0)

// 安全删除动画
#define SAFE_DELETE_ANIM(item) \
    do { \
        if (item) { \
            try { \
                item->stop(); \
                delete item; \
                item = nullptr; \
            } catch (...) { \
                item = nullptr; \
            } \
        } \
    } while(0)

#endif // CUSTOMFIXTABLEVIEW_H
