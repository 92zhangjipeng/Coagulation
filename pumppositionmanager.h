#pragma once
#include <QMutex>

struct Positionofthepump
{
   int samplePumpLoc;
   int reagentPumpLoc;
};

class PumpPositionManager {
public:
    static PumpPositionManager& instance();

    // 获取当前泵位置（拷贝，线程安全）
    Positionofthepump getPosition() const;

    // 更新样本泵位置（线程安全）
    void setSamplePumpLoc(int loc);

    // 更新试剂泵位置（线程安全）
    void setReagentPumpLoc(int loc);

    // 同时更新两个（如吸样完成后）
    void updateBoth(int sampleLoc, int reagentLoc);

private:
    PumpPositionManager() = default;
    ~PumpPositionManager() = default;
    PumpPositionManager(const PumpPositionManager&) = delete;
    PumpPositionManager& operator=(const PumpPositionManager&) = delete;

    mutable QMutex m_mutex;
    // 与灌注/洗针后一致，避免加样前软件泵位为 0 导致 PPP 吐量偏小
    Positionofthepump m_position {360, 0};
};
