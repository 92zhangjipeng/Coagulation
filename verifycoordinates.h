#ifndef VERIFYCOORDINATES_H
#define VERIFYCOORDINATES_H

#include "styledcomparisondialog.h"

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QHash>
#include <QDebug>
#include <functional>
#include <QPointer>


class StyledComparisonDialog;

class VerifyCoordinates : public QWidget
{
    Q_OBJECT
public:
    static VerifyCoordinates* GetInstance();

    explicit VerifyCoordinates(QWidget *parent = nullptr);

    // 加载坐标校验文件
    bool loadCoordinateVerification(quint8 equipmentType);
    bool loadCoordinateVerification(const QString& equipmentName);



    // 从仪器加载当前坐标
    void loadMachineAxis();

    //退出时检验坐标
    void startComparingCoordinates(const bool initMachine);
    void compareCoordinates(const bool initMachine);
    void displayStyledComparison(const bool initMachine);


    //仪器内坐标同步写入到校准坐标文件
    bool performAxisCalibration();


    // 将配置文件坐标同步到仪器
    bool syncFileToInstrument();





    // 简化的坐标结构 - 直接使用QPoint
    using VerifyPoint = QPoint;

    // 通道基类模板
    template<typename T>
    class VerifyChannelBase {
    public:
        static constexpr int CHANNEL_COUNT = 12;

        VerifyChannelBase() {
            clear();
        }

        void setChannel(int index, T x, T y) {
            if (isValidIndex(index)) {
                m_points[index].setX(x);
                m_points[index].setY(y);
            }
        }

        QPoint getChannel(int index) const {
            return isValidIndex(index) ? m_points[index] : QPoint(0, 0);
        }

        T getX(int index) const {
            return isValidIndex(index) ? m_points[index].x() : 0;
        }

        T getY(int index) const {
            return isValidIndex(index) ? m_points[index].y() : 0;
        }

        void setX(int index, T x) {
            if (isValidIndex(index)) m_points[index].setX(x);
        }

        void setY(int index, T y) {
            if (isValidIndex(index)) m_points[index].setY(y);
        }

        int getChannelCount() const { return CHANNEL_COUNT; }

        void clear() {
            for (int i = 0; i < CHANNEL_COUNT; ++i) {
                m_points[i] = QPoint(0, 0);
            }
        }

        void print() const {
            qDebug() << "Channel count:" << CHANNEL_COUNT;
            for(int i = 0; i < CHANNEL_COUNT; ++i) {
                qDebug() << "Channel" << i << ":" << m_points[i].x() << "," << m_points[i].y();
            }
        }

    private:
        bool isValidIndex(int index) const {
            return index >= 0 && index < CHANNEL_COUNT;
        }

        QPoint m_points[CHANNEL_COUNT];
    };

    // 托盘基类模板
    template<typename T>
    class VerifyTrayBase {
    public:
        static constexpr int TRAY_COUNT = 4;

        VerifyTrayBase() {
            clear();
        }

        void setTray(int index, T x, T y) {
            if (isValidIndex(index)) {
                m_points[index].setX(x);
                m_points[index].setY(y);
            }
        }

        QPoint getTray(int index) const {
            return isValidIndex(index) ? m_points[index] : QPoint(0, 0);
        }

        T getX(int index) const {
            return isValidIndex(index) ? m_points[index].x() : 0;
        }

        T getY(int index) const {
            return isValidIndex(index) ? m_points[index].y() : 0;
        }

        void setX(int index, T x) {
            if (isValidIndex(index)) m_points[index].setX(x);
        }

        void setY(int index, T y) {
            if (isValidIndex(index)) m_points[index].setY(y);
        }

        int getTrayCount() const { return TRAY_COUNT; }

        void clear() {
            for (int i = 0; i < TRAY_COUNT; ++i) {
                m_points[i] = QPoint(0, 0);
            }
        }

        void print() const {
            qDebug() << "Tray count:" << TRAY_COUNT;
            for(int i = 0; i < TRAY_COUNT; ++i) {
                qDebug() << "Tray" << i << ":" << m_points[i].x() << "," << m_points[i].y();
            }
        }

    private:
        bool isValidIndex(int index) const {
            return index >= 0 && index < TRAY_COUNT;
        }

        QPoint m_points[TRAY_COUNT];
    };

    // 类型别名
    using VerifyChannelOffHands = VerifyChannelBase<int>;
    using VerifyChannelOffReagentPin = VerifyChannelBase<int>;
    using VerifyTaryTubeOffHands = VerifyTrayBase<int>;
    using VerifyTaryTubeOffBloodPin = VerifyTrayBase<int>;

    // Getter 方法
    QPoint getOriginAxis() const { return m_originAxis; }
    QPoint getThrowHoleAxis() const { return m_throwHoleAxis; }
    QPoint getReagentHoleAxis() const { return m_reagentPin; }
    QPoint getBloodModuleOffBloodPin() const { return m_bloodPin; }
    QPoint getCleanZoneOffBloodPin() const { return m_cleanZoneBloodPin; }
    QPoint getCleanZoneOffReagentPin() const { return m_cleanZoneReagentPin; }

    VerifyChannelOffHands getChannelOffHands() const { return m_channels; }
    VerifyChannelOffReagentPin getChannelOffReagentPin() const { return m_channelOffReagentPin; }
    VerifyTaryTubeOffHands getTaryTubeOffHands() const { return m_handsTaryTube; }
    VerifyTaryTubeOffBloodPin getTaryTubeOffBloodPin() const { return m_bloodPinTaryTube; }

    // Setter 方法
    void setOriginAxis(const QPoint& point) { m_originAxis = point; emit originAxisChanged(point); }
    void setThrowHoleAxis(const QPoint& point) { m_throwHoleAxis = point; emit throwHoleAxisChanged(point); }
    void setReagentHoleAxis(const QPoint& point) { m_reagentPin = point; emit reagentOffBloodPinChanged(point); }
    void setBloodModuleOffBloodPin(const QPoint& point) { m_bloodPin = point; emit bloodmoduleOffBloodPinChanged(point); }
    void setCleanZoneOffBloodPin(const QPoint& point) { m_cleanZoneBloodPin = point; emit cleanZoneOffBloodPinChanged(point); }
    void setCleanZoneOffReagentPin(const QPoint& point) { m_cleanZoneReagentPin = point; emit cleanZoneOffReagentPinChanged(point); }

    void setChannelOffHands(int index, int x, int y) {
        m_channels.setChannel(index, x, y);
        emit channelOffHandsChanged(index, x, y);
    }

    void setChannelOffReagentPin(int index, int x, int y) {
        m_channelOffReagentPin.setChannel(index, x, y);
        emit channelOffReagentPinChanged(index, x, y);
    }

    void setTaryTubeOffHands(int index, int x, int y) {
        m_handsTaryTube.setTray(index, x, y);
        emit taryTubeOffHandsChanged(index, x, y);
    }

    void setTaryTubeOffBloodPin(int index, int x, int y) {
        m_bloodPinTaryTube.setTray(index, x, y);
        emit taryTubeOffBloodPinChanged(index, x, y);
    }

    // 加载的坐标 Getter (用于对比)
    QPoint getLoadedOriginAxis() const { return m_loadedOriginAxis; }
    QPoint getLoadedThrowHoleAxis() const { return m_loadedThrowHoleAxis; }
    QPoint getLoadedReagentHoleAxis() const { return m_loadedReagentPin; }
    QPoint getLoadedBloodModuleOffBloodPin() const { return m_loadedBloodPin; }
    QPoint getLoadedCleanZoneOffBloodPin() const { return m_loadedCleanZoneBloodPin; }
    QPoint getLoadedCleanZoneOffReagentPin() const { return m_loadedCleanZoneReagentPin; }

    VerifyChannelOffHands getLoadedChannelOffHands() const { return m_loadedChannels; }
    VerifyChannelOffReagentPin getLoadedChannelOffReagentPin() const { return m_loadedChannelOffReagentPin; }
    VerifyTaryTubeOffHands getLoadedTaryTubeOffHands() const { return m_loadedHandsTaryTube; }
    VerifyTaryTubeOffBloodPin getLoadedTaryTubeOffBloodPin() const { return m_loadedBloodPinTaryTube; }

    // 坐标验证方法
    bool verifyCoordinates() const;
    QString getCoordinateDifferences() const;

signals:
    void originAxisChanged(const QPoint& origin);
    void throwHoleAxisChanged(const QPoint& hole);
    void reagentOffBloodPinChanged(const QPoint& pin);
    void bloodmoduleOffBloodPinChanged(const QPoint& pin);
    void cleanZoneOffBloodPinChanged(const QPoint& pin);
    void cleanZoneOffReagentPinChanged(const QPoint& pin);

    void channelOffHandsChanged(int channelIndex, int x, int y);
    void channelOffReagentPinChanged(int channelIndex, int x, int y);
    void taryTubeOffHandsChanged(int trayIndex, int x, int y);
    void taryTubeOffBloodPinChanged(int trayIndex, int x, int y);

    // 新增信号
    void coordinateVerificationFailed(const QString& error);

public slots:
    void updateOriginAxis(int x, int y) { setOriginAxis(QPoint(x, y)); }
    void updateThrowHoleAxis(int x, int y) { setThrowHoleAxis(QPoint(x, y)); }
    void updateReagentOffBloodPin(int x, int y) { setReagentHoleAxis(QPoint(x, y)); }
    void updateBloodmoduleOffBloodPin(int x, int y) { setBloodModuleOffBloodPin(QPoint(x, y)); }
    void updateCleanZoneOffBloodPin(int x, int y) { setCleanZoneOffBloodPin(QPoint(x, y)); }
    void updateCleanZoneOffReagentPin(int x, int y) { setCleanZoneOffReagentPin(QPoint(x, y)); }

    void updateChannelOffHands(int index, int x, int y) { setChannelOffHands(index, x, y); }
    void updateChannelOffReagentPin(int index, int x, int y) { setChannelOffReagentPin(index, x, y); }
    void updateTaryTubeOffHands(int index, int x, int y) { setTaryTubeOffHands(index, x, y); }
    void updateTaryTubeOffBloodPin(int index, int x, int y) { setTaryTubeOffBloodPin(index, x, y); }

private:
    // 辅助方法
    bool parseKeyValue(const QString& line, QString& key, QString& value) const;
    bool parsePoint(const QString& value, int& x, int& y) const;
    bool validateCoordinateRange(int value, int min, int max, const QString& name) const;
    void initParsers();
    void printLoadedCoordinates() const;

    // 坐标校验文件解析
    bool parseFixedCoordinate(const QString& key, int x, int y);
    bool parseDynamicCoordinate(const QString& key, int x, int y);

    // 成员变量 - 校验坐标
    QPoint m_originAxis;
    QPoint m_throwHoleAxis;
    QPoint m_reagentPin;
    QPoint m_bloodPin;
    QPoint m_cleanZoneBloodPin;
    QPoint m_cleanZoneReagentPin;

    VerifyChannelOffHands m_channels;           // 通道offset抓手
    VerifyChannelOffReagentPin m_channelOffReagentPin;  // 通道Offset试剂针
    VerifyTaryTubeOffHands m_handsTaryTube;     // 托盘抓手
    VerifyTaryTubeOffBloodPin m_bloodPinTaryTube;       // 托盘血针

    // 成员变量 - 仪器内加载的坐标（用于对比）
    QPoint m_loadedOriginAxis;
    QPoint m_loadedThrowHoleAxis;
    QPoint m_loadedReagentPin;
    QPoint m_loadedBloodPin;
    QPoint m_loadedCleanZoneBloodPin;
    QPoint m_loadedCleanZoneReagentPin;

    VerifyChannelOffHands m_loadedChannels;
    VerifyChannelOffReagentPin m_loadedChannelOffReagentPin;
    VerifyTaryTubeOffHands m_loadedHandsTaryTube;
    VerifyTaryTubeOffBloodPin m_loadedBloodPinTaryTube;

    // 解析器映射
    QHash<QString, std::function<void(int, int)>> m_parsers;

    // 常量定义
    static constexpr int DEFAULT_MIN_COORDINATE = 0;
    static constexpr int DEFAULT_MAX_COORDINATE = 10000;
    static constexpr int COORDINATE_TOLERANCE = 5;  // 坐标允许的误差范围

	QPointer<StyledComparisonDialog> m_comparisonDialog;
};

#endif // VERIFYCOORDINATES_H
