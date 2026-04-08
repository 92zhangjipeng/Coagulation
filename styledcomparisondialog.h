#ifndef STYLEDCOMPARISONDIALOG_H
#define STYLEDCOMPARISONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMap>
#include <QPoint>
#include <QProgressBar>

class QTextEdit;
class QTreeWidget;
class QTabWidget;
class QProgressBar;

class StyledComparisonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StyledComparisonDialog(QWidget *parent = nullptr);

    // 设置对比数据
    void setComparisonData(const QMap<QString, QPair<QPoint, QPoint>>& basicPoints,
                          const QMap<int, QPair<QPoint, QPoint>>& handsChannels,
                          const QMap<int, QPair<QPoint, QPoint>>& reagentChannels,
                          const QMap<int, QPair<QPoint, QPoint>>& trayHands,
                          const QMap<int, QPair<QPoint, QPoint>>& trayBlood);

    // 设置公差
    void setTolerance(int tolerance) { m_tolerance = tolerance; }

    // 设置校准回调
    void setCalibrationCallback(std::function<bool()> callback) {
        m_calibrationCallback = callback;
    }

private slots:
    void onCalibrateClicked();
    void onExportClicked();
    void onFilterChanged(int index);

private:
    void setupUI();
    void createBasicTab();
    void createChannelsTab();
    void createTrayTab();
    void createSummaryTab();
    void populateBasicTable();
    void populateChannelsTable();
    void populateTrayTable();
    void updateSummary();
    QString getStatusIcon(bool passed);
    QString getStatusColor(bool passed);
    void exportToFile();

private:
    QTabWidget* m_tabWidget;
    QTreeWidget* m_basicTree;
    QTreeWidget* m_channelsTree;
    QTreeWidget* m_trayTree;
    QTextEdit* m_summaryText;
    QProgressBar* m_overallProgress;
    QLabel* m_statusLabel;

    int m_tolerance;
    int m_totalItems;
    int m_passedItems;

    QMap<QString, QPair<QPoint, QPoint>> m_basicPoints;
    QMap<int, QPair<QPoint, QPoint>> m_handsChannels;
    QMap<int, QPair<QPoint, QPoint>> m_reagentChannels;
    QMap<int, QPair<QPoint, QPoint>> m_trayHands;
    QMap<int, QPair<QPoint, QPoint>> m_trayBlood;

    std::function<bool()> m_calibrationCallback;
};

#endif // STYLEDCOMPARISONDIALOG_H
