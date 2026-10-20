#include "pumppositionmanager.h"

PumpPositionManager& PumpPositionManager::instance() {
    static PumpPositionManager manager;
    return manager;
}

Positionofthepump PumpPositionManager::getPosition() const {
    QMutexLocker locker(&m_mutex);
    return m_position;
}

void PumpPositionManager::setSamplePumpLoc(int loc) {
    QMutexLocker locker(&m_mutex);
    m_position.samplePumpLoc = loc;
}

void PumpPositionManager::setReagentPumpLoc(int loc) {
    QMutexLocker locker(&m_mutex);
    m_position.reagentPumpLoc = loc;
}

void PumpPositionManager::updateBoth(int sampleLoc, int reagentLoc) {
    QMutexLocker locker(&m_mutex);
    m_position.samplePumpLoc = sampleLoc;
    m_position.reagentPumpLoc = reagentLoc;
}
