#ifndef VERIFYCOORDINATES_H
#define VERIFYCOORDINATES_H

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QPoint>

class VerifyCoordinates : public QWidget
{
    Q_OBJECT
public:
    static VerifyCoordinates* GetInstance();

    explicit VerifyCoordinates(QWidget *parent = nullptr);

    bool loadCoordinateVerification(quint8 equipmentType);
    bool loadCoordinateVerification(const QString& equipmentName);


    struct VerifyOriginAxis{
        int originX;
        int originY;

        VerifyOriginAxis(int x = 0, int y = 0) : originX(x), originY(y) {}
        void setX(int x) { originX = x; }
        void setY(int y) { originY = y; }
        int getX() const { return originX; }
        int getY() const { return originY; }
        void set(int x, int y) { originX = x; originY = y; }

        VerifyOriginAxis(const VerifyOriginAxis &other)
            : originX(other.originX), originY(other.originY) {}

        VerifyOriginAxis& operator=(const VerifyOriginAxis &other) {
            if (this != &other) {
                originX = other.originX;
                originY = other.originY;
            }
            return *this;
        }

        bool operator==(const VerifyOriginAxis &other) const {
            return (originX == other.originX && originY == other.originY);
        }

        bool operator!=(const VerifyOriginAxis &other) const {
            return !(*this == other);
        }
    };
    VerifyOriginAxis getOriginAxis() const { return m_originAxis; }
    void setOriginAxis(int x, int y) { m_originAxis.set(x, y); }
    void setOriginAxisX(int x) { m_originAxis.setX(x); }
    void setOriginAxisY(int y) { m_originAxis.setY(y); }



    struct VerifyThrowHoleAxis{
        int throwHoleX;
        int throwHoleY;

        VerifyThrowHoleAxis(int x = 0, int y = 0) : throwHoleX(x), throwHoleY(y) {}

        void setX(int x) { throwHoleX = x; }
        void setY(int y) { throwHoleY = y; }
        int getX() const { return throwHoleX; }
        int getY() const { return throwHoleY; }
        void set(int x, int y) { throwHoleX = x; throwHoleY = y; }

        VerifyThrowHoleAxis(const VerifyThrowHoleAxis &other)
            : throwHoleX(other.throwHoleX), throwHoleY(other.throwHoleY) {}

        VerifyThrowHoleAxis& operator=(const VerifyThrowHoleAxis &other) {
            if (this != &other) {
                throwHoleX = other.throwHoleX;
                throwHoleY = other.throwHoleY;
            }
            return *this;
        }

        bool operator==(const VerifyThrowHoleAxis &other) const {
            return (throwHoleX == other.throwHoleX && throwHoleY == other.throwHoleY);
        }

        bool operator!=(const VerifyThrowHoleAxis &other) const {
            return !(*this == other);
        }
    };
    VerifyThrowHoleAxis getThrowHoleAxis() const { return m_throwHoleAxis; }
    void setThrowHoleAxis(int x, int y) { m_throwHoleAxis.set(x, y); }
    void setThrowHoleAxisX(int x) { m_throwHoleAxis.setX(x); }
    void setThrowHoleAxisY(int y) { m_throwHoleAxis.setY(y); }



    struct VerifyReagentOffReagentPin{
        int reagentOffBloodPinX;
        int reagentOffBloodPinY;

        VerifyReagentOffReagentPin(int x = 0, int y = 0) : reagentOffBloodPinX(x), reagentOffBloodPinY(y) {}

        void setX(int x) { reagentOffBloodPinX = x; }
        void setY(int y) { reagentOffBloodPinY = y; }
        int getX() const { return reagentOffBloodPinX; }
        int getY() const { return reagentOffBloodPinY; }
        void set(int x, int y) { reagentOffBloodPinX = x; reagentOffBloodPinY = y; }

        VerifyReagentOffReagentPin(const VerifyReagentOffReagentPin &other)
            : reagentOffBloodPinX(other.reagentOffBloodPinX), reagentOffBloodPinY(other.reagentOffBloodPinY) {}

        VerifyReagentOffReagentPin& operator=(const VerifyReagentOffReagentPin &other) {
            if (this != &other) {
                reagentOffBloodPinX = other.reagentOffBloodPinX;
                reagentOffBloodPinY = other.reagentOffBloodPinY;
            }
            return *this;
        }

        bool operator==(const VerifyReagentOffReagentPin &other) const {
            return (reagentOffBloodPinX == other.reagentOffBloodPinX &&
                    reagentOffBloodPinY == other.reagentOffBloodPinY);
        }

        bool operator!=(const VerifyReagentOffReagentPin &other) const {
            return !(*this == other);
        }
    };
    VerifyReagentOffReagentPin getReagentHoleAxis() const { return m_reagentPin; }
    void setReagentHoleAxis(int x, int y) { m_reagentPin.set(x, y); }
    void setReagentHoleAxisX(int x) { m_reagentPin.setX(x); }
    void setReagentHoleAxisY(int y) { m_reagentPin.setY(y); }


    struct VerifyCleanZoneOffBloodPin{
        int cleanZoneOffBloodPinX;
        int cleanZoneOffBloodPinY;

        VerifyCleanZoneOffBloodPin(int x = 0, int y = 0) : cleanZoneOffBloodPinX(x), cleanZoneOffBloodPinY(y) {}

        void setX(int x) { cleanZoneOffBloodPinX = x; }
        void setY(int y) { cleanZoneOffBloodPinY = y; }
        int getX() const { return cleanZoneOffBloodPinX; }
        int getY() const { return cleanZoneOffBloodPinY; }
        void set(int x, int y) { cleanZoneOffBloodPinX = x; cleanZoneOffBloodPinY = y; }

        VerifyCleanZoneOffBloodPin(const VerifyCleanZoneOffBloodPin &other)
            : cleanZoneOffBloodPinX(other.cleanZoneOffBloodPinX), cleanZoneOffBloodPinY(other.cleanZoneOffBloodPinY) {}

        VerifyCleanZoneOffBloodPin& operator=(const VerifyCleanZoneOffBloodPin &other) {
            if (this != &other) {
                cleanZoneOffBloodPinX = other.cleanZoneOffBloodPinX;
                cleanZoneOffBloodPinY = other.cleanZoneOffBloodPinY;
            }
            return *this;
        }

        bool operator==(const VerifyCleanZoneOffBloodPin &other) const {
            return (cleanZoneOffBloodPinX == other.cleanZoneOffBloodPinX &&
                    cleanZoneOffBloodPinY == other.cleanZoneOffBloodPinY);
        }

        bool operator!=(const VerifyCleanZoneOffBloodPin &other) const {
            return !(*this == other);
        }
    };
    VerifyCleanZoneOffBloodPin getCleanZoneOffBloodPin() const { return m_cleanZoneBloodPin; }
    void setCleanZoneOffBloodPin(int x, int y) { m_cleanZoneBloodPin.set(x, y); }
    void setCleanZoneOffBloodPinX(int x) { m_cleanZoneBloodPin.setX(x); }
    void setCleanZoneOffBloodPinY(int y) { m_cleanZoneBloodPin.setY(y); }



    struct VerifyCleanZoneOffReagentPin{
        int cleanZoneOffReagentPinX;
        int cleanZoneOffReagentPinY;

        VerifyCleanZoneOffReagentPin(int x = 0, int y = 0) : cleanZoneOffReagentPinX(x), cleanZoneOffReagentPinY(y) {}

        void setX(int x) { cleanZoneOffReagentPinX = x; }
        void setY(int y) { cleanZoneOffReagentPinY = y; }
        int getX() const { return cleanZoneOffReagentPinX; }
        int getY() const { return cleanZoneOffReagentPinY; }
        void set(int x, int y) { cleanZoneOffReagentPinX = x; cleanZoneOffReagentPinY = y; }

        VerifyCleanZoneOffReagentPin(const VerifyCleanZoneOffReagentPin &other)
            : cleanZoneOffReagentPinX(other.cleanZoneOffReagentPinX), cleanZoneOffReagentPinY(other.cleanZoneOffReagentPinY) {}

        VerifyCleanZoneOffReagentPin& operator=(const VerifyCleanZoneOffReagentPin &other) {
            if (this != &other) {
                cleanZoneOffReagentPinX = other.cleanZoneOffReagentPinX;
                cleanZoneOffReagentPinY = other.cleanZoneOffReagentPinY;
            }
            return *this;
        }

        bool operator==(const VerifyCleanZoneOffReagentPin &other) const {
            return (cleanZoneOffReagentPinX == other.cleanZoneOffReagentPinX &&
                    cleanZoneOffReagentPinY == other.cleanZoneOffReagentPinY);
        }

        bool operator!=(const VerifyCleanZoneOffReagentPin &other) const {
            return !(*this == other);
        }
    };
    VerifyCleanZoneOffReagentPin getCleanZoneOffReagentPin() const { return m_cleanZoneReagentPin; }
    void setCleanZoneOffReagentPin(int x, int y) { m_cleanZoneReagentPin.set(x, y); }
    void setCleanZoneOffReagentPinX(int x) { m_cleanZoneReagentPin.setX(x); }
    void setCleanZoneOffReagentPinY(int y) { m_cleanZoneReagentPin.setY(y); }





    struct VerifyBloodmoduleOffBloodPin{
        int bloodOffBloodPinX;
        int bloodOffBloodPinY;

        VerifyBloodmoduleOffBloodPin(int x = 0, int y = 0) : bloodOffBloodPinX(x), bloodOffBloodPinY(y) {}

        void setX(int x) { bloodOffBloodPinX = x; }
        void setY(int y) { bloodOffBloodPinY = y; }
        int getX() const { return bloodOffBloodPinX; }
        int getY() const { return bloodOffBloodPinY; }
        void set(int x, int y) { bloodOffBloodPinX = x; bloodOffBloodPinY = y; }

        VerifyBloodmoduleOffBloodPin(const VerifyBloodmoduleOffBloodPin &other)
            : bloodOffBloodPinX(other.bloodOffBloodPinX), bloodOffBloodPinY(other.bloodOffBloodPinY) {}

        VerifyBloodmoduleOffBloodPin& operator=(const VerifyBloodmoduleOffBloodPin &other) {
            if (this != &other) {
                bloodOffBloodPinX = other.bloodOffBloodPinX;
                bloodOffBloodPinY = other.bloodOffBloodPinY;
            }
            return *this;
        }

        bool operator==(const VerifyBloodmoduleOffBloodPin &other) const {
            return (bloodOffBloodPinX == other.bloodOffBloodPinX &&
                    bloodOffBloodPinY == other.bloodOffBloodPinY);
        }

        bool operator!=(const VerifyBloodmoduleOffBloodPin &other) const {
            return !(*this == other);
        }
    };
    VerifyBloodmoduleOffBloodPin getBloodmoduleOffBloodPin() const { return m_bloodPin; }
    void setBloodmoduleOffBloodPin(int x, int y) { m_bloodPin.set(x, y); }
    void setBloodmoduleOffBloodPinX(int x) { m_bloodPin.setX(x); }
    void setBloodmoduleOffBloodPinY(int y) { m_bloodPin.setY(y); }


    // 模板基类
    template<typename T>
    struct VerifyChannelBase {
        static const int CHANNEL_COUNT = 12;
        T channelX[CHANNEL_COUNT];
        T channelY[CHANNEL_COUNT];

        VerifyChannelBase() {
           for (int i = 0; i < CHANNEL_COUNT; ++i) {
               channelX[i] = 0;
               channelY[i] = 0;
           }
       }

        void setChannel(int index, T x, T y) {
            if (index >= 0 && index < CHANNEL_COUNT) {
                channelX[index] = x;
                channelY[index] = y;
            }
        }

        QPoint getChannel(int index) const {
            if (index >= 0 && index < CHANNEL_COUNT) {
                return QPoint(channelX[index], channelY[index]);
            }
            return QPoint(0, 0);
        }

        void setX(int index, T x) { channelX[index] = x; }
        void setY(int index, T y) { channelY[index] = y; }
		T getX(int index) const { return channelX[index]; }  // 修正：添加参数类型，返回类型改为 T
		T getY(int index) const { return channelY[index]; }

        int getChannelCount() const { return CHANNEL_COUNT; }

        void clear() {
            for (int i = 0; i < CHANNEL_COUNT; ++i) {
               channelX[i] = 0;
               channelY[i] = 0;
           }
        }

        // 添加一个方便调试的函数
        void print() const {
            qDebug() << "Channel count:" << CHANNEL_COUNT;
            for(int i = 0; i < CHANNEL_COUNT; i++) {
                qDebug() << "Channel" << i << ":" << channelX[i] << "," << channelY[i];
            }
        }
    };

    // 使用别名定义具体的通道类型
    using VerifyChannelOffHands = VerifyChannelBase<int>;
    using VerifyChannelOffReagentPin = VerifyChannelBase<int>;

    VerifyChannelOffHands getChannelOffHands() const { return m_channels; }
    void setChannelOffHands(int  index, int x, int y) { m_channels.setChannel(index, x, y); }
    void setChannelOffHandsX(int index,int x) { m_channels.setX(index,x); }
    void setChannelOffHandsY(int index,int y) { m_channels.setY(index,y); }

    VerifyChannelOffReagentPin getChannelOffReagentPin() const { return m_channelOffReagentPin; }
    void setChannelOffReagentPin(int index, int x, int y) { m_channelOffReagentPin.setChannel(index, x, y); }
    void setChannelOffReagentPinX(int index,int x) { m_channelOffReagentPin.setX(index,x); }
    void setChannelOffReagentPinY(int index,int y) { m_channelOffReagentPin.setY(index,y); }

    // 固定4个托盘的基类
    template<typename T>
    struct VerifyTrayBase {
        static const int TRAY_COUNT = 4;
        T trayX[TRAY_COUNT];
        T trayY[TRAY_COUNT];

        VerifyTrayBase() {
            for (int i = 0; i < TRAY_COUNT; ++i) {
                trayX[i] = 0;
                trayY[i] = 0;
            }
        }

        void setTray(int index, T x, T y) {
            if (index >= 0 && index < TRAY_COUNT) {
                trayX[index] = x;
                trayY[index] = y;
            }
        }

        QPoint getTray(int index) const {
            if (index >= 0 && index < TRAY_COUNT) {
                return QPoint(trayX[index], trayY[index]);
            }
            return QPoint(0, 0);
        }

        void setX(int index, T x) { trayX[index] = x; }
        void setY(int index, T y) { trayY[index] = y; }
        T getX(int index) const { return trayX[index]; }
        T getY(int index) const { return trayY[index]; }

        int getTrayCount() const { return TRAY_COUNT; }

        void clear() {
            for (int i = 0; i < TRAY_COUNT; ++i) {
                trayX[i] = 0;
                trayY[i] = 0;
            }
        }

        void print() const {
            qDebug() << "Tray count:" << TRAY_COUNT;
            for(int i = 0; i < TRAY_COUNT; i++) {
                qDebug() << "Tray" << i << ":" << trayX[i] << "," << trayY[i];
            }
        }
    };

    using VerifyTaryTubeOffHands = VerifyTrayBase<int>;
    VerifyTaryTubeOffHands getTaryTubeOffHands() const { return m_handsTaryTube; }
    void setTaryTubeOffHands(int index, int x, int y) { m_handsTaryTube.setTray(index, x, y); }
    void setTaryTubeOffHandsX(int index,int x) { m_handsTaryTube.setX(index,x); }
    void setTaryTubeOffHandsY(int index,int y) { m_handsTaryTube.setY(index,y); }

    using VerifyTaryTubeOffBloodPin = VerifyTrayBase<int>;
    VerifyTaryTubeOffBloodPin getTaryTubeOffBloodPin() const { return m_bloodPinTaryTube; }
    void setTaryTubeOffBloodPin(int index, int x, int y) { m_bloodPinTaryTube.setTray(index, x, y); }
    void setTaryTubeOffBloodPinX(int index,int x) { m_bloodPinTaryTube.setX(index,x); }
    void setTaryTubeOffBloodPinY(int index,int y) { m_bloodPinTaryTube.setY(index,y); }


signals:
    void originAxisChanged(const VerifyOriginAxis &origin);
    void throwHoleAxisChanged(const VerifyThrowHoleAxis &hole);
    void reagentOffBloodPinChanged(const VerifyReagentOffReagentPin &pin);
    void bloodmoduleOffBloodPinChanged(const VerifyBloodmoduleOffBloodPin &pin);
    void cleanZoneOffBloodPinChanged(const VerifyCleanZoneOffBloodPin &pin);
    void cleanZoneOffReagentPinChanged(const VerifyCleanZoneOffReagentPin &pin);

    void channelOffHandsChanged(int channelIndex, int x, int y);
    void channelOffReagentPinChanged(int channelIndex, int x, int y);
    void taryTubeOffHandsChanged(int trayIndex, int x, int y);
    void taryTubeOffBloodPinChanged(int trayIndex, int x, int y);


public slots:
    void updateOriginAxis(int x, int y) {
        m_originAxis.set(x, y);
        emit originAxisChanged(m_originAxis);
    }

    void updateThrowHoleAxis(int x, int y) {
        m_throwHoleAxis.set(x, y);
        emit throwHoleAxisChanged(m_throwHoleAxis);
    }

    void updateReagentOffBloodPin(int x, int y) {
        m_reagentPin.set(x, y);
        emit reagentOffBloodPinChanged(m_reagentPin);
    }

    void updateBloodmoduleOffBloodPin(int x, int y) {
        m_bloodPin.set(x, y);
        emit bloodmoduleOffBloodPinChanged(m_bloodPin);
    }

    void updateCleanZoneOffBloodPin(int x, int y) {
        m_cleanZoneBloodPin.set(x, y);
        emit cleanZoneOffBloodPinChanged(m_cleanZoneBloodPin);
    }

    void updateCleanZoneOffReagentPin(int x, int y) {
        m_cleanZoneReagentPin.set(x, y);
        emit cleanZoneOffReagentPinChanged(m_cleanZoneReagentPin);
    }

    void updateChannelOffHands(int index, int x, int y) {
        m_channels.setChannel(index, x, y);
        emit channelOffHandsChanged(index, x, y);
    }

    void updateChannelOffReagentPin(int index, int x, int y) {
        m_channelOffReagentPin.setChannel(index, x, y);
        emit channelOffReagentPinChanged(index, x, y);
    }

    void updateTaryTubeOffHands(int index, int x, int y) {
        m_handsTaryTube.setTray(index, x, y);
        emit taryTubeOffHandsChanged(index, x, y);
    }

    void updateTaryTubeOffBloodPin(int index, int x, int y) {
        m_bloodPinTaryTube.setTray(index, x, y);
        emit taryTubeOffBloodPinChanged(index, x, y);
    }


private:
    // 所有成员变量都会在构造函数中初始化
    VerifyOriginAxis m_originAxis;
    VerifyThrowHoleAxis m_throwHoleAxis;
    VerifyReagentOffReagentPin m_reagentPin;

    VerifyBloodmoduleOffBloodPin m_bloodPin;
    VerifyCleanZoneOffBloodPin m_cleanZoneBloodPin;
    VerifyCleanZoneOffReagentPin m_cleanZoneReagentPin;

    VerifyChannelOffHands m_channels;  //通道offset抓手
    VerifyChannelOffReagentPin m_channelOffReagentPin; //通道Offset试剂针

    VerifyTaryTubeOffHands m_handsTaryTube;
    VerifyTaryTubeOffBloodPin m_bloodPinTaryTube;

};

#endif // VERIFYCOORDINATES_H
