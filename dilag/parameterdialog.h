#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QMap>
#include <QLabel>

class ParameterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParameterDialog(QWidget *parent = nullptr);

    // 获取当前选中的试剂
    QString currentReagent() const;
    void setCurrentReagent(const QString& reagent);

    // 获取当前试剂的参数（偏移值）
    double offsetValue() const;
    void setOffsetValue(double val);

    // 延时参数（仅COL有效）
    int colDelayTime() const;
    void setColDelayTime(int val);

    // 判定参数
    quint8 getTriggerInvalid() const;
    void setTriggerInvalid(quint8 percentage);

    quint8 getTriggerOffsetVal() const;
    void setTriggerOffsetVal(quint8 val);

    quint8 getInvalidMin() const;
    void setInvalidMin(quint8 minPerce);

    quint8 getInvalidMax() const;
    void setInvalidMax(quint8 maxPerce);

    // 批量设置/获取所有试剂数据（用于外部保存）
    void setReagentData(const QString& reagent, double offset, quint8 triggerInvalid,
                        quint8 offsetRatio, quint8 invalidMin, quint8 invalidMax);
    void getReagentData(const QString& reagent, double& offset, quint8& triggerInvalid,
                        quint8& offsetRatio, quint8& invalidMin, quint8& invalidMax) const;

private slots:
    void onAccepted();
    void resetToDefault();
    void onReagentChanged(const QString& reagent);

private:
    void setupUI();
    void setupStyle();
    void loadReagentData(const QString& reagent);
    void updateDelayGroupVisibility(const QString& reagent);
    void saveCurrentReagentData();

    // 主控件
    QComboBox *m_selectReagent;

    // 通用的参数控件
    QDoubleSpinBox *m_offsetSpin;        // 偏移值 (%)
    QSpinBox       *m_triggerInvalidSpin; // 触发无效 (%)
    QSpinBox       *m_offsetRatioSpin;   // 偏移百分比 (%)
    QSpinBox       *m_invalidMinSpin;    // 起始时间 (s)
    QSpinBox       *m_invalidMaxSpin;    // 终止时间 (s)

    // COL 特有的延时控件
    QLabel         *m_delayLabel;        // 延时标签
    QSpinBox       *m_colDelaySpin;      // COL延时 (秒)
    QGroupBox      *m_delayGroup;        // 延时所在的组

    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;

    // 存储每种试剂的数据
    struct ReagentData {
        double offset;              // 偏移值
        int colDelay;              // COL延时
        quint8 triggerInvalid;     // 触发无效
        quint8 offsetRatio;        // 偏移百分比
        quint8 invalidMin;         // 起始时间
        quint8 invalidMax;         // 终止时间
        bool hasDelay;             // 是否有延时
    };

    QMap<QString, ReagentData> m_reagentData;
    QString m_currentReagent;
};

#endif // PARAMETERDIALOG_H
