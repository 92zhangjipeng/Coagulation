#ifndef HITPAGE_H
#define HITPAGE_H

#include <QDateTime>
#include <QWidget>

class QDialog;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class WidgetDelegate;
class HitTestRunner;

class HitPage : public QWidget
{
public:
    explicit HitPage(QWidget *parent = nullptr);

private:
    void buildTable();
    void buildDelegates();
    void appendTask();
    void removeSelectedRows();
    void togglePause();
    void commitSampleId(QTableWidgetItem *item);
    void showHitSettings();
    void showStartMap();
    void ensurePlaceDialog();
    void syncPlaceDialog(bool testing);
    void showViewCurve();
    void showQuery();
    void loadRuns(const QString &sampleId, const QDateTime &from, const QDateTime &to, bool tellEmpty);
    void setRunBusy(bool busy);
    void installStepBars(int row);
    void setStepProgress(int row, int mixIndex, int elapsedMs, int totalMs, int phase);
    void applyRunUpdate(int row, const QString &status, const QString &startTime,
                        const QString &endTime, const QString &ppp,
                        const QString &spontaneous, const QString &low,
                        const QString &high, const QString &negative);
    QString nextSampleId() const;
    int nextPppHole() const;
    int channelCount() const;

    QTableWidget *m_table = nullptr;
    HitTestRunner *m_runner = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_pauseButton = nullptr;
    QPushButton *m_addButton = nullptr;
    bool m_sampleEditLock = false;
    QPushButton *m_deleteButton = nullptr;
    QPushButton *m_viewButton = nullptr;
    QPushButton *m_queryButton = nullptr;
    WidgetDelegate *m_channelDelegate = nullptr;
    QDialog *m_placeDialog = nullptr;
    QWidget *m_placeMap = nullptr;
    QPushButton *m_placeConfirm = nullptr;
};

#endif
