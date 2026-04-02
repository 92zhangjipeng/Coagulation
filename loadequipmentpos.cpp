#include "loadequipmentpos.h"  //导入仪器坐标
#include "cglobal.h"
#include "QsLog/include/QsLog.h"
#include "globaldata.h"
#include "quiutils.h"
#include "warn_interface.h"
#include <unordered_map>
#include <QDesktopWidget>
#include <functional>
#include <unordered_map>
#include <QFile>
#include <QTextStream>
#include <array>

SingletonAxis *SingletonAxis::g_pSingletonAxis = new (std::nothrow) SingletonAxis;
EquipmentAXIS_ *SingletonAxis::g_pEquipAxiaspos = NULL;


//试剂耗材信息
ConsumablesOper *ConsumablesOper::g_pConsumable = nullptr /*new (std::nothrow) ConsumablesOper*/;
equipmentConsumablesVec *ConsumablesOper::g_pVecReagentInfo = nullptr;
std::mutex ConsumablesOper::m_instanceMutex;





SingletonAxis *SingletonAxis::GetInstance()
{
    return g_pSingletonAxis;
}



EquipmentAXIS_ *SingletonAxis::GetpStruct()
{
    if (g_pEquipAxiaspos == nullptr)
    {
        g_pEquipAxiaspos = new EquipmentAXIS_;
    }
    return g_pEquipAxiaspos;
}



void SingletonAxis::deleteInstance()
{
    if(g_pSingletonAxis){
        delete g_pSingletonAxis;
        g_pSingletonAxis = NULL;
    }
}


void SingletonAxis::sycnAxisState(bool bWrite, bool &sycnState)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->bsycnFinished = sycnState : sycnState = axisData->bsycnFinished;
    }
}

void SingletonAxis::equipmentKind(bool bWrite,QString &kindType)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->equipmentType = kindType : kindType = axisData->equipmentType;
    }
}


void SingletonAxis::equipmentKind(bool bWrite, quint8 &kindType)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->euqipmentIndex = kindType : kindType = axisData->euqipmentIndex;
    }
}


void SingletonAxis::paraAxisSaveTime(bool bWrite,QString &timesavestr)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->AxixsSaveTime = timesavestr : timesavestr = axisData->AxixsSaveTime;
    }
}


void SingletonAxis::originPos(const bool &bWrite, QPoint &pos)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->OriginAxis = pos : pos = axisData->OriginAxis;
    }
}


void SingletonAxis::throwTubeHolePos(bool bWrite, QPoint &pos)
{
    if (auto axisData = GetpStruct()) {
        bWrite ? axisData->ThrowHoleAxis = pos : pos = axisData->ThrowHoleAxis;
        QLOG_DEBUG() << "弃杯孔坐标:" << axisData->ThrowHoleAxis << __FUNCTION__ << __LINE__;
    }
}


void SingletonAxis::cleanZoneAxisPos(bool bWrite,int indexNedl,QPoint &pos)
{
    EquipmentAXIS_* axisData = GetpStruct(); // 改为指针

    // 使用lambda延迟获取指针，避免静态初始化问题
    static auto getMotorMap = [axisData]() {
        std::unordered_map<int, QPoint*> map;
        map[MOTOR_BLOOD_INDEX] = &axisData->cleanZoneoffsetBlodNedl;
        map[MOTOR_REAGNET_INDEX] = &axisData->cleanZoneoffsetRegNedl;
        return map;
    };

    static const auto motorMap = getMotorMap();

    auto it = motorMap.find(indexNedl);
    if (it != motorMap.end()) {
        if (bWrite) {
            *(it->second) = pos;
        } else {
            pos = *(it->second);
        }
    }
}

bool SingletonAxis::ismoveXYsuckReagent(const QPoint &locpos){
    auto &container = g_pEquipAxiaspos->reagentZoneAxispos;
    auto findIt = std::find_if(container.cbegin(), container.cend(),
            [&locpos](const REAGENTZONEAXIS_ *item) {
                Q_ASSERT_X(item != nullptr, "findReagentIndex", "Null item in reagent position container");
                return item->Axispos == locpos;
            });
    if(findIt == container.cend()){
        //QLOG_ERROR() << "试剂针移动影射试剂索引无效(非试剂区坐标)" ;
        return false;
    }
    return true;
}

quint8 SingletonAxis::outPutLossReagentIndex(const QPoint &locpos)
{
    auto &container = g_pEquipAxiaspos->reagentZoneAxispos;
    auto findIt = std::find_if(container.cbegin(), container.cend(),
            [&locpos](const REAGENTZONEAXIS_ *item) {
				Q_ASSERT_X(item != nullptr, "findReagentIndex", "Null item in reagent position container");
                return item->Axispos == locpos;
            });
    if(findIt == container.cend()){
        QLOG_ERROR() << "试剂针移动影射试剂索引无效(非试剂区坐标)" ;
        return 255;
    }
	return (*findIt)->index;
}

void SingletonAxis::reagetZoneAxisPos(bool bWrite, quint8 indexReag, QPoint &pos)
{
    auto &container = g_pEquipAxiaspos->reagentZoneAxispos;

    auto findIt = std::find_if(container.begin(), container.end(),
            [indexReag](const REAGENTZONEAXIS_ *item) {
                return item->index == indexReag;
            });

    if (bWrite) {
        if (findIt != container.end()) {
            // 存在则直接更新
            (*findIt)->Axispos = pos;
            (*findIt)->reagname.clear(); // 明确清空而非赋空字符串
        } else {
            // 不存在则创建新项（使用智能指针避免内存泄漏）
            auto newItem = std::make_unique<REAGENTZONEAXIS_>();
            newItem->index = indexReag;
            newItem->Axispos = pos;
            newItem->reagname.clear();
            container.append(newItem.release()); // 假设容器管理原始指针
        }
    } else {
        // 读取时直接赋值或保持pos不变
        if (findIt != container.end()) {
            pos = (*findIt)->Axispos;
        }
        // 未找到时返回错误状态或默认值（根据需求）
    }
	return;
}

void SingletonAxis::chnZoneAxisPos(bool bWrite,quint8 numChn,quint8 OffsetNedl ,QPoint &pos)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    auto& axisPoints = axisData->pchnAxisPoint;
    // 查找匹配项
    ChnAxis_* foundItem = nullptr;
    for (auto* item : axisPoints) {
        if (item && item->indexChn == numChn && item->offsetNeedle == OffsetNedl) {
            foundItem = item;
            break;
        }
    }

    if (bWrite) {
        if (foundItem) {
            foundItem->axisPos = pos;
        } else {
            ChnAxis_* pchnStu = new ChnAxis_;
            pchnStu->indexChn = numChn;
            pchnStu->offsetNeedle = OffsetNedl;
            pchnStu->axisPos = pos;
            axisPoints.push_back(pchnStu);
        }
    } else if (foundItem) {
        pos = foundItem->axisPos;
    }
	return;
}

void SingletonAxis::bloodSampleZonePos(bool bWrite,quint8 numhole,QPoint &pos)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    auto& bloodSampleList = axisData->bloodSampleAxisPos;
    // 查找现有记录
    auto it = std::find_if(bloodSampleList.begin(), bloodSampleList.end(),
        [numhole](SAMPLEBLOODZONEAXISPOS_* item) {
            return item != nullptr && item->index == numhole;
        });

    if (bWrite) {
        if (it != bloodSampleList.end()) {
            (*it)->axisPos = pos;
        } else {
            SAMPLEBLOODZONEAXISPOS_* newItem = new SAMPLEBLOODZONEAXISPOS_;
            newItem->index = numhole;
            newItem->axisPos = pos;
            bloodSampleList.append(newItem);
        }
    } else if (it != bloodSampleList.end()) {
        pos = (*it)->axisPos;
    }
}

//写试管区坐标
void SingletonAxis::WriteEmptyTube_Coordinate(quint8 numhole,quint8 indexNeedle,QPoint emptypos)
{
    auto& tubeList = GetpStruct()->testTubeZoneAxisPos;

    auto it = std::find_if(tubeList.begin(),tubeList.end(),
        [numhole, indexNeedle](TRYTHECUPAXIS_* item) {
            return item != nullptr &&
                   item->numTube == numhole &&
                   item->offsetNeedle == indexNeedle;
    });
    if (it != tubeList.end()) {
        // 已存在，更新位置
        (*it)->axisPos = emptypos;
    } else {
        // 不存在，创建新记录
        TRYTHECUPAXIS_* newItem = new TRYTHECUPAXIS_;
        newItem->axisPos = emptypos;
        newItem->offsetNeedle = indexNeedle;
        newItem->numTube = numhole;
        newItem->indexTray = witchoneindexTary(numhole);
        tubeList.push_back(newItem);
    }
    return;
}

//试杯孔在哪个盘
quint8 SingletonAxis::witchoneindexTary(const quint8 indextube)
{
    return indextube >= 240 ? 0 : (indextube / 60) + 1;
}

quint8 SingletonAxis::testTaryZoneAxisPos(bool bWrite,quint8 numhole,quint8 indexNeedle,QPoint &pos)
{
    if (bWrite) {
        WriteEmptyTube_Coordinate(numhole, indexNeedle, pos);
        return witchoneindexTary(numhole);
    }

    // 读取操作
    auto& tubeList = GetpStruct()->testTubeZoneAxisPos;
    auto it = std::find_if(tubeList.constBegin(), tubeList.constEnd(),
            [=](TRYTHECUPAXIS_* item) {
                return item && item->offsetNeedle == indexNeedle && item->numTube == numhole;
            });

    if (it != tubeList.constEnd()) {
        pos = (*it)->axisPos;
        return (*it)->indexTray;
    }
    return 0;
}



//修改坐标
void SingletonAxis::oper_OriginAxis(bool bNotif_x,int posValue)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    bNotif_x ? axisData->OriginAxis.setX(posValue) : axisData->OriginAxis.setY(posValue);
}

void SingletonAxis::oper_ThrowTubeHolePos(bool bNotif_x,int posValue)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    bNotif_x ? axisData->ThrowHoleAxis.setX(posValue) : axisData->ThrowHoleAxis.setY(posValue);
}

void SingletonAxis::oper_CleanZonePos(bool bNotif_x,int indexNedl,int posValue)
{
	auto axisData = GetpStruct();
	if (!axisData) return;

	QPoint* targetPoint = nullptr;

	switch (indexNedl) {
	case MOTOR_BLOOD_INDEX:
		targetPoint = &axisData->cleanZoneoffsetBlodNedl;
		break;
	case MOTOR_REAGNET_INDEX:
		targetPoint = &axisData->cleanZoneoffsetRegNedl;
		break;
	default:
		return;
	}

	if (bNotif_x) {
		targetPoint->setX(posValue);
	}
	else {
		targetPoint->setY(posValue);
	} 
}
// 修改试剂区位置
void SingletonAxis::oper_ReagentZonePos(bool bNotif_x,quint8 indexReag,quint16 posValue)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    auto& reagentList = axisData->reagentZoneAxispos;

    // 查找现有试剂
    auto it = std::find_if(reagentList.begin(), reagentList.end(),
        [indexReag](REAGENTZONEAXIS_* item) {
            return item != nullptr && item->index == indexReag;
    });

    if (it != reagentList.end()) {
        bNotif_x ? (*it)->Axispos.setX(posValue) : (*it)->Axispos.setY(posValue);
    } else {
        // 创建新试剂项
        REAGENTZONEAXIS_* newReagent = new REAGENTZONEAXIS_;
        newReagent->index = indexReag;
        newReagent->reagname = "";
        bNotif_x ? newReagent->Axispos.setX(posValue) : newReagent->Axispos.setY(posValue);
        reagentList.push_back(newReagent);
    }
}




void SingletonAxis::oper_TestChnZoneAxispos(bool isXAxis, quint8 numChn, quint8 offsetNedl, quint16 posValue)
{
    auto& points = g_pEquipAxiaspos->pchnAxisPoint;

    // 使用STL算法查找匹配项 [[2, 3]]
    auto it = std::find_if(points.begin(), points.end(),
            [numChn, offsetNedl](const ChnAxis_* entry) {
                return entry->indexChn == numChn && entry->offsetNeedle == offsetNedl;
            });

   if (it != points.end()) {
            // 更新现有项的坐标
            isXAxis ? (*it)->axisPos.setX(posValue) : (*it)->axisPos.setY(posValue);
    } 
   else 
   {
        // 创建新对象并初始化 [[5]]
        auto* newEntry = new ChnAxis_{ numChn, offsetNedl, QPoint(isXAxis ? posValue : 0, isXAxis ? 0 : posValue)
        };
        points.push_back(newEntry);
    }
    return;
}


void SingletonAxis::oper_bloodSampleZonePos(bool bNotif_x,quint8 numhole,quint16 posValue)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    auto& bloodSampleList = axisData->bloodSampleAxisPos;

    // 查找现有记录
    auto it = std::find_if(bloodSampleList.begin(), bloodSampleList.end(),
        [numhole](SAMPLEBLOODZONEAXISPOS_* item) {
            return item != nullptr && item->index == numhole;
        });

    if (it != bloodSampleList.end()) {
        // 更新现有记录
        if (bNotif_x) {
            (*it)->axisPos.setX(posValue);
        } else {
            (*it)->axisPos.setY(posValue);
        }
    } else {
        // 创建新记录
        SAMPLEBLOODZONEAXISPOS_* newItem = new SAMPLEBLOODZONEAXISPOS_;
        newItem->index = numhole;
        if (bNotif_x) {
            newItem->axisPos.setX(posValue);
        } else {
            newItem->axisPos.setY(posValue);
        }
        bloodSampleList.push_back(newItem);
    }
}

void SingletonAxis::oper_TestTrayZonaPos(bool bNotif_x,quint8 numhole,quint8 indexNeedle,quint16 posValue)
{
    auto axisData = GetpStruct();
    if (!axisData) return;

    auto& tubeList = axisData->testTubeZoneAxisPos;

    // 查找现有记录
    auto it = std::find_if(tubeList.begin(), tubeList.end(),
        [numhole, indexNeedle](TRYTHECUPAXIS_* item) {
            return item != nullptr &&
                   item->numTube == numhole &&
                   item->offsetNeedle == indexNeedle;
        });

    if (it != tubeList.end()) {
        // 更新现有记录
        if (bNotif_x) {
            (*it)->axisPos.setX(posValue);
        } else {
            (*it)->axisPos.setY(posValue);
        }
    } else {
        // 创建新记录
        TRYTHECUPAXIS_* newItem = new TRYTHECUPAXIS_;
        newItem->numTube = numhole;
        newItem->offsetNeedle = indexNeedle;
        newItem->indexTray = witchoneindexTary(numhole);
        if (bNotif_x) {
            newItem->axisPos.setX(posValue);
        } else {
            newItem->axisPos.setY(posValue);
        }
        tubeList.push_back(newItem);
    }
}



quint8 SingletonAxis::TeatTayr_findHole(int indexZ,QPoint moved)
{
    QLOG_DEBUG() << "映射找试管盘坐标" << moved << "offset针" << indexZ;

    auto axisData = GetpStruct();
    if (!axisData) return 250;

    auto& tubeList = axisData->testTubeZoneAxisPos;

    // 使用std::find_if查找匹配的元素
    auto it = std::find_if(tubeList.begin(), tubeList.end(),
        [indexZ, moved](TRYTHECUPAXIS_* item) {
            return item != nullptr &&
                   item->offsetNeedle == indexZ &&
                   item->axisPos == moved;
        });

    return (it != tubeList.end()) ? (*it)->numTube : 250;
}


SingletonAxis::SingletonAxis()
{
    if (!g_pEquipAxiaspos){
        g_pEquipAxiaspos = new EquipmentAXIS_;
        QLOG_DEBUG() << "坐标结构体分配内存";

        // 确保容器初始化为空
        g_pEquipAxiaspos->reagentZoneAxispos.clear();
        g_pEquipAxiaspos->bloodSampleAxisPos.clear();
        g_pEquipAxiaspos->testTubeZoneAxisPos.clear();
        g_pEquipAxiaspos->pchnAxisPoint.clear();
    }
}

SingletonAxis::~SingletonAxis()
{
    if (g_pEquipAxiaspos) {
        // 使用lambda函数进行安全清理
        auto safeDeleteContainer = [](auto& container) {
            for (auto item : container) {
                if (item) {
                    delete item;
                }
            }
            container.clear();
        };

        safeDeleteContainer(g_pEquipAxiaspos->reagentZoneAxispos);
        safeDeleteContainer(g_pEquipAxiaspos->bloodSampleAxisPos);
        safeDeleteContainer(g_pEquipAxiaspos->testTubeZoneAxisPos);
        safeDeleteContainer(g_pEquipAxiaspos->pchnAxisPoint);

        delete g_pEquipAxiaspos;
        g_pEquipAxiaspos = nullptr;
    }
}

bool SingletonAxis::writeCoordinate(const QString &filePath)
{
    return exportToCoordinateText(*g_pEquipAxiaspos,filePath);
}

bool SingletonAxis::exportToCoordinateText(const EquipmentAXIS_& equipment, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QLOG_ERROR() << "无法打开文件"<<filePath;
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    out << "# Equipment Configuration\n";
    out << "# Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    out << "# Equipment Type: " << equipment.equipmentType << "\n";
    out << "# Sync Status: " << (equipment.bsycnFinished ? "Completed" : "Pending") << "\n";
    out << "# Origin Axis: " << equipment.OriginAxis.x() << "," << equipment.OriginAxis.y() << "\n";
    out << "# Equipment Index: " << equipment.euqipmentIndex << "\n";
    out << "# Clean Zone (Reagent): " << equipment.cleanZoneoffsetRegNedl.x() << "," << equipment.cleanZoneoffsetRegNedl.y() << "\n";
    out << "# Clean Zone (Blood): " << equipment.cleanZoneoffsetBlodNedl.x() << "," << equipment.cleanZoneoffsetBlodNedl.y() << "\n";
    out << "# Throw Hole Axis: " << equipment.ThrowHoleAxis.x() << "," << equipment.ThrowHoleAxis.y() << "\n\n";

    // 输出试剂区坐标
    out << "# Reagent Zone Coordinates\n";
    out << "# Index,ReagentName,X,Y\n";
    for (const auto& reagent : equipment.reagentZoneAxispos) {
        out << reagent->index << ","
            << "\"" << reagent->reagname << "\","
            << reagent->Axispos.x() << ","
            << reagent->Axispos.y() << "\n";
    }
    out << "\n";

    // 输出通道区坐标
    out << "# Channel Coordinates\n";
    out << "# Index,Offset,X,Y\n";
    for (const auto& channel : equipment.pchnAxisPoint) {
        out << channel->indexChn << ","
            << channel->offsetNeedle << ","
            << channel->axisPos.x() << ","
            << channel->axisPos.y() << "\n";
    }
    out << "\n";

    // 输出试杯区坐标
    out << "# Test Tube Zone Coordinates\n";
    out << "# TubeNum,Offset,TrayIndex,X,Y\n";
    for (const auto& tube : equipment.testTubeZoneAxisPos) {
        out << tube->numTube << ","
            << tube->offsetNeedle << ","
            << tube->indexTray << ","
            << tube->axisPos.x() << ","
            << tube->axisPos.y() << "\n";
    }
    out << "\n";

    // 输出血样区坐标
    out << "# Blood Sample Zone Coordinates\n";
    out << "# Index,X,Y\n";
    for (const auto& blood : equipment.bloodSampleAxisPos) {
        out << blood->index << ","
            << blood->axisPos.x() << ","
            << blood->axisPos.y() << "\n";
    }

    file.close();
    return true;
}

// 从文本文件导入设备坐标
QStringList parseCSVLine(const QString& line)
{
	QStringList result;
	QString currentField;
	bool inQuotes = false;
	bool wasInQuotes = false;

	for (int i = 0; i < line.length(); ++i) {
		QChar c = line[i];

		if (c == '\"') {
			if (inQuotes && i + 1 < line.length() && line[i + 1] == '\"') {
				// 转义引号
				currentField += '\"';
				i++; // 跳过下一个引号
			}
			else {
				inQuotes = !inQuotes;
				wasInQuotes = true;
			}
		}
		else if (c == ',' && !inQuotes) {
			result.append(currentField);
			currentField.clear();
			wasInQuotes = false;
		}
		else {
			currentField += c;
		}
	}

	result.append(currentField);

	// 去除字段两端的引号（如果原本有的话）
	for (int i = 0; i < result.size(); ++i) {
		if (result[i].startsWith('\"') && result[i].endsWith('\"')) {
			result[i] = result[i].mid(1, result[i].length() - 2);
		}
	}

	return result;
}

void updateCurrentSection(const QString& line, QString& currentSection)
{
    if (line.contains("Reagent Zone Coordinates", Qt::CaseInsensitive)) {
            currentSection = "Reagent";
    } else if (line.contains("Channel Coordinates", Qt::CaseInsensitive)) {
        currentSection = "Channel";
    } else if (line.contains("Test Tube Zone Coordinates", Qt::CaseInsensitive)) {
        currentSection = "TestTube";
    } else if (line.contains("Blood Sample Zone Coordinates", Qt::CaseInsensitive)) {
        currentSection = "Blood";
    } else if (line.contains("Equipment Configuration", Qt::CaseInsensitive)) {
        currentSection = "Header";
    }
}

REAGENTZONEAXIS_* parseReagentLine(const QString& line, int lineNumber)
{
    QStringList fields = parseCSVLine(line);
    if (fields.size() < 4) {
        throw std::runtime_error(QString("Reagent zone data format error at line %1").arg(lineNumber).toStdString());
    }

    REAGENTZONEAXIS_* reagent = new REAGENTZONEAXIS_();
    try {
        reagent->index = static_cast<quint8>(fields[0].toUInt());
        reagent->reagname = fields[1].replace("\"\"", "\""); // 处理转义引号
        reagent->Axispos.setX(fields[2].toInt());
        reagent->Axispos.setY(fields[3].toInt());
    } catch (...) {
        delete reagent;
        throw;
    }

    return reagent;
}

ChnAxis_* parseChannelLine(const QString& line, int lineNumber)
{
    QStringList fields = parseCSVLine(line);
    if (fields.size() < 4) {
        throw std::runtime_error(QString("Channel data format error at line %1").arg(lineNumber).toStdString());
    }

    ChnAxis_* channel = new ChnAxis_();
    try {
        channel->indexChn = static_cast<quint8>(fields[0].toUInt());
        channel->offsetNeedle = static_cast<quint8>(fields[1].toUInt());
        channel->axisPos.setX(fields[2].toInt());
        channel->axisPos.setY(fields[3].toInt());
    } catch (...) {
        delete channel;
        throw;
    }

    return channel;
}

TRYTHECUPAXIS_* parseTestTubeLine(const QString& line, int lineNumber)
{
    QStringList fields = parseCSVLine(line);
    if (fields.size() < 5) {
        throw std::runtime_error(QString("Test tube zone data format error at line %1").arg(lineNumber).toStdString());
    }

    TRYTHECUPAXIS_* tube = new TRYTHECUPAXIS_();
    try {
        tube->numTube = static_cast<quint8>(fields[0].toUInt());
        tube->offsetNeedle = static_cast<quint8>(fields[1].toUInt());
        tube->indexTray = static_cast<quint8>(fields[2].toUInt());
        tube->axisPos.setX(fields[3].toInt());
        tube->axisPos.setY(fields[4].toInt());
    } catch (...) {
        delete tube;
        throw;
    }

    return tube;
}



SAMPLEBLOODZONEAXISPOS_* parseBloodLine(const QString& line, int lineNumber)
{
    QStringList fields = parseCSVLine(line);
    if (fields.size() < 3) {
        throw std::runtime_error(QString("Blood sample zone data format error at line %1").arg(lineNumber).toStdString());
    }

    SAMPLEBLOODZONEAXISPOS_* blood = new SAMPLEBLOODZONEAXISPOS_();
    try {
        blood->index = static_cast<quint8>(fields[0].toUInt());
        blood->axisPos.setX(fields[1].toInt());
        blood->axisPos.setY(fields[2].toInt());
    } catch (...) {
        delete blood;
        throw;
    }

    return blood;
}

bool SingletonAxis::parseEquipmentInfoLine(const QString& line, EquipmentAXIS_& equipment)
{
    QString trimmedLine = line.trimmed();

    if (trimmedLine.startsWith("# Equipment Type:")) {
        equipment.equipmentType = trimmedLine.section(':', 1).trimmed();
        return true;
    }
    else if (trimmedLine.startsWith("# Origin Axis:")) {
        QString coordStr = trimmedLine.section(':', 1).trimmed();
        QStringList coords = coordStr.split(',');
        if (coords.size() == 2) {
            bool okX, okY;
            int x = coords[0].trimmed().toInt(&okX);
            int y = coords[1].trimmed().toInt(&okY);
            if (okX && okY) {
                equipment.OriginAxis.setX(x);
                equipment.OriginAxis.setY(y);
                return true;
            }
        }
    }
    else if (trimmedLine.startsWith("# Clean Zone (Reagent):")) {
        QString coordStr = trimmedLine.section(':', 1).trimmed();
        QStringList coords = coordStr.split(',');
        if (coords.size() == 2) {
            bool okX, okY;
            int x = coords[0].trimmed().toInt(&okX);
            int y = coords[1].trimmed().toInt(&okY);
            if (okX && okY) {
                equipment.cleanZoneoffsetRegNedl.setX(x);
                equipment.cleanZoneoffsetRegNedl.setY(y);
                return true;
            }
        }
    }
    else if (trimmedLine.startsWith("# Clean Zone (Blood):")) {
        QString coordStr = trimmedLine.section(':', 1).trimmed();
        QStringList coords = coordStr.split(',');
        if (coords.size() == 2) {
            bool okX, okY;
            int x = coords[0].trimmed().toInt(&okX);
            int y = coords[1].trimmed().toInt(&okY);
            if (okX && okY) {
                equipment.cleanZoneoffsetBlodNedl.setX(x);
                equipment.cleanZoneoffsetBlodNedl.setY(y);
                return true;
            }
        }
    }
    else if (trimmedLine.startsWith("# Throw Hole Axis:")) {
        QString coordStr = trimmedLine.section(':', 1).trimmed();
        QStringList coords = coordStr.split(',');
        if (coords.size() == 2) {
            bool okX, okY;
            int x = coords[0].trimmed().toInt(&okX);
            int y = coords[1].trimmed().toInt(&okY);
            if (okX && okY) {
                equipment.ThrowHoleAxis.setX(x);
                equipment.ThrowHoleAxis.setY(y);
                return true;
            }
        }
    }
    else if (trimmedLine.startsWith("# Equipment Index:")) {
        bool ok;
        int index = trimmedLine.section(':', 1).trimmed().toInt(&ok);
        if (ok) {
            equipment.euqipmentIndex = index;
            return true;
        }
    }
    else if (trimmedLine.startsWith("# Sync Status:")) {
		equipment.bsycnFinished = ("Completed" == trimmedLine.section(':', 1).trimmed()) ? true : false;
        return true;
    }
    else if (trimmedLine.startsWith("# Generated:")) {
        equipment.AxixsSaveTime = trimmedLine.section(':', 1).trimmed();
        return true;
    }

    return false; // 不是设备信息行
}

void SingletonAxis::clearEquipmentData(EquipmentAXIS_& equipment)
{
    // 清空试剂区
    qDeleteAll(equipment.reagentZoneAxispos);
    equipment.reagentZoneAxispos.clear();

    // 清空通道区
    qDeleteAll(equipment.pchnAxisPoint);
    equipment.pchnAxisPoint.clear();

    // 清空试杯区
    qDeleteAll(equipment.testTubeZoneAxisPos);
    equipment.testTubeZoneAxisPos.clear();

    // 清空血样区
    qDeleteAll(equipment.bloodSampleAxisPos);
    equipment.bloodSampleAxisPos.clear();

    // 重置其他字段（可选）
    equipment.equipmentType.clear();
    equipment.bsycnFinished = false;
    equipment.OriginAxis = QPoint(0, 0);
    equipment.euqipmentIndex = 0;
    equipment.cleanZoneoffsetRegNedl = QPoint(0, 0);
    equipment.cleanZoneoffsetBlodNedl = QPoint(0, 0);
    equipment.ThrowHoleAxis = QPoint(0, 0);
    equipment.AxixsSaveTime.clear();
}

bool SingletonAxis::importFromCoordinateText(const QString& filePath)
{
    EquipmentAXIS_ newEquipment;
    QFile file(filePath);

    if (!file.exists()) {
        throw std::runtime_error("文件不存在: " + filePath.toStdString());
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("无法打开文件进行读取: " + filePath.toStdString());
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");  // 设置编码为UTF-8
    QString currentSection;
    int lineNumber = 0;

    // 使用智能指针或确保异常时的内存清理
    QList<REAGENTZONEAXIS_*> tempReagents;
    QList<ChnAxis_*> tempChannels;
    QList<TRYTHECUPAXIS_*> tempTubes;
    QList<SAMPLEBLOODZONEAXISPOS_*> tempBloodSamples;

    try {
           while (!in.atEnd()) {
               QString line = in.readLine().trimmed();
               lineNumber++;

               // 跳过空行
               if (line.isEmpty()) {
                   continue;
               }

               // 处理节标识行
               if (line.startsWith("#")) {

                   if (parseEquipmentInfoLine(line, newEquipment)) {
                       continue; // 如果成功解析了设备信息，继续下一行
                   }

                   updateCurrentSection(line, currentSection);
                   continue;
               }


               if (currentSection == "Reagent") {
                   REAGENTZONEAXIS_* reagent = parseReagentLine(line, lineNumber);
                   if(reagent)
                        tempReagents.append(reagent);
               }
               else if (currentSection == "Channel") {
                   ChnAxis_* channel = parseChannelLine(line, lineNumber);
                   if(channel){
                       tempChannels.append(channel);
                   }
               }
               else if (currentSection == "TestTube") {
                   TRYTHECUPAXIS_* tube = parseTestTubeLine(line, lineNumber);
                   if(tube){
                       tempTubes.append(tube);
                   }
               }
               else if (currentSection == "Blood") {
                   SAMPLEBLOODZONEAXISPOS_* blood = parseBloodLine(line, lineNumber);
                   if(blood)
                        tempBloodSamples.append(blood);
               } 
           }

           // 所有解析成功，转移数据到equipment
           newEquipment.reagentZoneAxispos = tempReagents;
           newEquipment.pchnAxisPoint = tempChannels;
           newEquipment.testTubeZoneAxisPos = tempTubes;
           newEquipment.bloodSampleAxisPos = tempBloodSamples;

       } catch (const std::exception& e) {
           // 清理临时分配的内存
           qDeleteAll(tempReagents);
           qDeleteAll(tempChannels);
           qDeleteAll(tempTubes);
           qDeleteAll(tempBloodSamples);
           file.close();
           throw std::runtime_error(std::string("解析错误: ") + e.what());
       }
       file.close();

       try{
           EquipmentAXIS_* pequipment = GetpStruct();
           if(!pequipment){
               throw std::runtime_error("Failed to get equipment structure");
           }

           // 清空现有数据
           clearEquipmentData(*pequipment);

           // 复制数据到单例
           *pequipment = newEquipment;

          /* QLOG_INFO() << "配置文件导入成功:"
                        << "设备类型:" << newEquipment.equipmentType
                        << "原点坐标:" << newEquipment.originAxis.x() << "," << newEquipment.originAxis.y()
                        << "试剂清洁区:" << newEquipment.cleanZoneReagent.x() << "," << newEquipment.cleanZoneReagent.y()
                        << "血液清洁区:" << newEquipment.cleanZoneBlood.x() << "," << newEquipment.cleanZoneBlood.y()
                        << "废液孔:" << newEquipment.throwHoleAxis.x() << "," << newEquipment.throwHoleAxis.y();*/

           return true;
       }catch (const std::exception& e) {
           QLOG_ERROR() << "导入失败:" << e.what();
           return false;
       }
}



/**   试剂耗材
* @brief ConsumablesOper::GetpInstance
* @return
*/
ConsumablesOper *ConsumablesOper::GetpInstance()
{
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (g_pConsumable == nullptr)
    {
        g_pConsumable = new ConsumablesOper();
    }
    return g_pConsumable;
}

void ConsumablesOper::del_Instance()
{
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (g_pConsumable)
    {
        delete g_pConsumable;
        g_pConsumable = nullptr;
    }
}

ConsumablesOper::ConsumablesOper()
{
    if (g_pVecReagentInfo == nullptr){
        g_pVecReagentInfo = new equipmentConsumablesVec;
        QLOG_DEBUG() << "耗材结构体分配内存";
    }
}

ConsumablesOper::~ConsumablesOper()
{
   clearReagentInfo();
}

void ConsumablesOper::clearReagentInfo(){
    if (g_pVecReagentInfo){
        for (auto iter = g_pVecReagentInfo->begin(); iter != g_pVecReagentInfo->end(); ++iter)
        {
            if (*iter != nullptr) {
                delete (*iter);
                (*iter) = nullptr;
            }
        }
        g_pVecReagentInfo->clear();
        delete g_pVecReagentInfo;
        g_pVecReagentInfo = nullptr;
    }
}


//初始化耗材信息
void ConsumablesOper::creatReagentIndex(const quint8 index)
{
    REAGENT_CONSUMABLES_* pinitConsumables = new REAGENT_CONSUMABLES_(
                index, 0, "", 0, 0, 0
    );

    g_pVecReagentInfo->push_back(pinitConsumables);
    QLOG_DEBUG() << QString("创建耗材索引: %1").arg(index);
}


void  ConsumablesOper::iterateOverEquipmentConsumables()
{
    for (int index = INDEX_AA_CONSUMABLE; index <= INDEX_RIS_1_CONSUMABLE; ++index) {
        creatReagentIndex(index);
    }
}


void ConsumablesOper::getSuppileAllowance(QMap<quint8,quint16> &AllowanceRatio)
{
    if (!g_pVecReagentInfo) return;

    for (const auto& psingleReagent : *g_pVecReagentInfo) {
        if (psingleReagent) {
            // if (psingleReagent->bSycnMemory == true)  // 如果将来需要条件判断
            AllowanceRatio.insert(psingleReagent->indexReag, psingleReagent->remain_Ratio);
        }
    }
}

void ConsumablesOper::getSuppileInteger(QMap<quint8,quint8> &AllowanceInteger)
{
    if (!g_pVecReagentInfo) return;

    for (const auto& psingleReagent : *g_pVecReagentInfo) {
        if (psingleReagent) {
            AllowanceInteger.insert(psingleReagent->indexReag, psingleReagent->remainingNum);
        }
    }
}

void ConsumablesOper::sycnTubeTotalTray(bool bwrite,quint8 &num_Bottle) {
    if (!g_pVecReagentInfo) {
        QLOG_DEBUG() << "读写试杯盘总数[失败] 仪器耗材数据结构为NULL" << endl;
        return;
    }

    auto iter = std::find_if(g_pVecReagentInfo->begin(), g_pVecReagentInfo->end(),
            [](REAGENT_CONSUMABLES_* reagent) {
                return reagent && reagent->indexReag == INDEX_TESTTUBE_CONSUMABLE;
            });
    if (iter != g_pVecReagentInfo->end() && *iter != nullptr) {
        REAGENT_CONSUMABLES_* psingleReagent = *iter;
        if (bwrite) {
            psingleReagent->remainingNum = num_Bottle;
            QLOG_DEBUG() << "写入试杯盘总数:" << num_Bottle;
        } else {
            num_Bottle = psingleReagent->remainingNum;
            QLOG_DEBUG() << "读取试杯盘总数:" << num_Bottle;
        }
    } else {
        QLOG_ERROR() << "未找到试杯盘耗材信息(索引:" << INDEX_TESTTUBE_CONSUMABLE
                         << ")，请检查配置或更换耗材";
    }
}


void ConsumablesOper::syacnReagentTotalBottle(bool isWrite, quint8 reagentIndex, quint8& bottleCount)
{
    // 空指针检查前置
    if (g_pVecReagentInfo == nullptr) {
        QLOG_ERROR() << "[Reagent] Failed to access bottle count: reagent data is null"<<endl;
        return;
    }

    // 定义右侧试剂到左侧的索引映射表（静态常量）
    static const QHash<quint8, quint8> kRightToLeftMap = {
        {INDEX_AA_1_CONSUMABLE,  INDEX_AA_CONSUMABLE},
        {INDEX_ADP_1_CONSUMABLE, INDEX_ADP_CONSUMABLE},
        {INDEX_EPI_1_CONSUMABLE, INDEX_EPI_CONSUMABLE},
        {INDEX_COL_1_CONSUMABLE, INDEX_COL_CONSUMABLE},
        {INDEX_RIS_1_CONSUMABLE, INDEX_RIS_CONSUMABLE}
    };

    // 转换目标索引（自动处理右侧试剂）
    const quint8 targetIndex = kRightToLeftMap.value(reagentIndex, reagentIndex);

    // 使用STL算法查找目标试剂
    const auto it = std::find_if(g_pVecReagentInfo->cbegin(), g_pVecReagentInfo->cend(),
        [targetIndex](const REAGENT_CONSUMABLES_* pReagent) {
            return pReagent->indexReag == targetIndex;
        });

    if (it != g_pVecReagentInfo->cend()) {
        isWrite ? (*it)->remainingNum = bottleCount : bottleCount = (*it)->remainingNum;
    }
    else {
        QLOG_WARN() << "[Reagent] Index" << reagentIndex << "not found"<<endl;
    }
    return;
}



void ConsumablesOper::getCleanLinqueAllowance(quint16 &RatioLast)
{
    if (!g_pVecReagentInfo || g_pVecReagentInfo->isEmpty()) {
        RatioLast = 0; // 确保输出参数有默认值
        return;
    }

    auto iter = g_pVecReagentInfo->constBegin();
    while(iter != g_pVecReagentInfo->constEnd())
    {
        REAGENT_CONSUMABLES_ *psingleReagent = *iter;
        if(psingleReagent->indexReag == INDEX_CLEANLINQUE_CONSUMABLE)
        {
            RatioLast = psingleReagent->remain_Ratio;   //剩余百分比 512*100 /512
            break;
        }
        iter++;
    }
}

void ConsumablesOper::updateReplaceLocRatio(bool isWrite, quint8 indexReag,quint16 &bottleRatio)
{
    if (g_pVecReagentInfo == nullptr) {
        QLOG_DEBUG() << "更换整体体耗材比例设置为512[失败] 仪器耗材数据结构为NULL"<<endl;
        return;
    }

    if (g_pVecReagentInfo->isEmpty()) {
        return;
    }

    const auto it = std::find_if(g_pVecReagentInfo->cbegin(), g_pVecReagentInfo->cend(),
        [indexReag](const REAGENT_CONSUMABLES_* reagent) {
        return reagent->indexReag == indexReag;
    });

    if (it != g_pVecReagentInfo->cend()) {
        REAGENT_CONSUMABLES_* reagent = *it;
        if (isWrite) {
            reagent->remain_Ratio = bottleRatio;
        } else {
            bottleRatio = reagent->remain_Ratio;
        }
    }
    return;
}






void ConsumablesOper::updateReagentTotal(bool bwrite,quint8 indexReag,quint16 &ReagentTol)
{
    if (!g_pVecReagentInfo || g_pVecReagentInfo->isEmpty()) {
        QLOG_WARN() << "更新试剂容量失败: 耗材数据结构为空";
        return;
    }

    if(g_pVecReagentInfo->isEmpty()) return;
    auto iter = g_pVecReagentInfo->begin();
    while(iter != g_pVecReagentInfo->end()){
        REAGENT_CONSUMABLES_ *psingleReagent = *iter;
        if(psingleReagent->indexReag == indexReag)
        {
            if(bwrite == WRITE_OPERAT)
                psingleReagent->SingleBottleCapacity = ReagentTol;
            else
                ReagentTol = psingleReagent->SingleBottleCapacity;
            break;
        }
        iter++;
    }
    return;
}

void ConsumablesOper::updateReagentLimit(bool bwrite,quint8 indexReag,quint8 &limitArlm)
{
    if (g_pVecReagentInfo == nullptr) {
            QLOG_ERROR() << "更新试剂限位失败：仪器耗材数据结构未初始化";
            return ;
    }

    const auto it = std::find_if(g_pVecReagentInfo->cbegin(), g_pVecReagentInfo->cend(),
        [indexReag](const REAGENT_CONSUMABLES_* reagent) {
            return reagent->indexReag == indexReag;
        });

    if (it == g_pVecReagentInfo->cend()) {
        QLOG_WARN() << "未找到索引号为" << indexReag << "的试剂";
        return;
    }
    QString reagnetname = GlobalData::mapReagentNames(indexReag);
    if (bwrite) {
        (*it)->LimitAlarm = limitArlm;
        QLOG_TRACE() << "已更新试剂" <<reagnetname << "限位比:" << limitArlm;
    } else {
        limitArlm = (*it)->LimitAlarm;
        QLOG_TRACE() << "读取到试剂" << reagnetname << "限位比:" << limitArlm;
    }
    return ;
}


bool ConsumablesOper::_ObtainConsumablesBalance(const quint8 supplyIndex, quint8& remainingQuantity){

    if (!g_pVecReagentInfo) {
        QLOG_ERROR() << "[耗材查询失败] 全局耗材数据未初始化 (supplyIndex=" << supplyIndex << ")";
        return false;
    }

    static const std::unordered_map<quint8, quint8> kIndexMapping = {
            {INDEX_AA_1_CONSUMABLE,  INDEX_AA_CONSUMABLE},
            {INDEX_ADP_1_CONSUMABLE, INDEX_ADP_CONSUMABLE},
            {INDEX_EPI_1_CONSUMABLE, INDEX_EPI_CONSUMABLE},
            {INDEX_COL_1_CONSUMABLE, INDEX_COL_CONSUMABLE},
            {INDEX_RIS_1_CONSUMABLE, INDEX_RIS_CONSUMABLE}
    };

    const quint8 targetIndex = [&]() {
            auto it = kIndexMapping.find(supplyIndex);
            return (it != kIndexMapping.end()) ? it->second : supplyIndex;
    }();

    const auto it = std::find_if(g_pVecReagentInfo->cbegin(), g_pVecReagentInfo->cend(),
            [targetIndex](const REAGENT_CONSUMABLES_* p) {
                return p && (p->indexReag == targetIndex);
            });

    if (it == g_pVecReagentInfo->cend()) {
        QLOG_DEBUG() << "[耗材查询失败] 未找到索引为" << targetIndex << "的耗材记录";
        return false;
    }

    remainingQuantity = (*it)->remainingNum;
    return true;
}

void ConsumablesOper::TotalConsumablesAlarm(quint8 indexReag, bool &alarms)
{
    static const QStringList keyList = {
            "AAbottleLimit", "ADPbottleLimit", "EPIbottleLimit",
            "COLbottleLimit", "RISbottleLimit", "S1bottleLimit",
            "tubeTarybottleLimit", "S2bottleLimit"
    };

    if (indexReag >= keyList.size()) {
        alarms = false; // 或根据业务逻辑处理越界
        return;
    }

    static INI_File config;
    quint8 totalNum = 0;
    syacnReagentTotalBottle(READ_OPERRAT, indexReag, totalNum);

    const quint8 limitNum = config.rConfigPara(keyList[indexReag]).toUInt();
    alarms = (totalNum <= limitNum);
    return;
}

////////////////////////试剂耗材  end///////////////////////////////////////




loadEquipmentPos::loadEquipmentPos(QObject *parent) : QObject(parent)
{
    mwriteAxismap.clear();
    m_axiswriteequipment.clear();
    m_bReadorWrite = false;
    mcreatSetType = false;
    m_bParafileExit = false;
    m_ParaFilePath = "";
    mserialname = "";
}

loadEquipmentPos::~loadEquipmentPos()
{
	//delete this;
}

void loadEquipmentPos::StatrLoad()
{
    //QLOG_DEBUG() << "登录开始加载设备...";
    if(minitPort == nullptr) {
        try {
            minitPort = new QSerialPort();
        } catch (const std::bad_alloc& e) {
            QLOG_ERROR() << "内存分配失败:" << e.what();
            emit closetimercon(false);
            return;
        }
    }
    // 安全关闭串口
    if (minitPort->isOpen()) {
        QLOG_WARN() << "串口已打开，关闭重新打开...";
        try {
            minitPort->clear();
            minitPort->flush();
            minitPort->close();
        } catch (const std::exception& e) {
            QLOG_ERROR() << "关闭串口时发生异常:" << e.what();
            // 清理并重新创建
            delete minitPort;
            minitPort = new QSerialPort();
        }
    }

    // 查找匹配的串口
    bool found = false;
    QString targetPortName;

    try {
        auto ports = QSerialPortInfo::availablePorts();
        for (const auto& portInfo : ports) {
            // 检查VID/PID有效性
            if (!portInfo.hasVendorIdentifier() || !portInfo.hasProductIdentifier()) {
                continue;
           }

            if (portInfo.vendorIdentifier() == VID_NUM &&
                portInfo.productIdentifier() == PID_NUM) {
                found = true;
                targetPortName = portInfo.portName();
                QLOG_INFO() << "找到目标串口:" << targetPortName;
                m_hasWarned = false; // 找到设备时重置警告标志
                break;
            }
        }
    } catch (const std::exception& e) {
        QLOG_ERROR() << "枚举串口时发生异常:" << e.what();
    }

    // 处理查找结果
    if (found) {
        try {
            emit closetimercon(true);
            openLoadSerialPort(targetPortName);
        } catch (const std::exception& e) {
            QLOG_ERROR() << "打开串口失败:" << e.what();
            emit closetimercon(false);
        }
    } else {
        // 只在第一次未找到设备时提示
        if (!m_hasWarned) {
            QLOG_WARN() << "未找到摄像头设备读取参数 (VID:"
                       << QString::number(VID_NUM, 16)
                       << ", PID:" << QString::number(PID_NUM, 16) << ")";
            m_hasWarned = true; // 设置已警告标志
        }
        emit closetimercon(false);
    }
}

void loadEquipmentPos::openLoadSerialPort(const QString &portName)
{
    // 参数检查
    if (minitPort == nullptr) {
        QLOG_ERROR() << "串口对象未初始化!";
        return;
    }

    if (portName.isEmpty()) {
        QLOG_ERROR() << "串口名称为空!";
        return;
    }

    // 如果已经打开的是同一个端口，无需重新打开
    if (minitPort->isOpen() && minitPort->portName() == portName) {
        QLOG_INFO() << "串口" << portName << "已经打开";
        return;
    }
    // 设置端口名称
    minitPort->setPortName(portName);

    if(!minitPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        const QString errorString = minitPort->errorString();
        QLOG_ERROR()<< "串口" << portName << "打开失败:" << errorString;
        return;
    }

    minitPort->setBaudRate(QSerialPort::Baud1000000,QSerialPort::AllDirections);//设置波特率和读写方向
    minitPort->setDataBits(QSerialPort::Data8);              //数据位为8位
    minitPort->setFlowControl(QSerialPort::NoFlowControl);   //无流控制
    minitPort->setParity(QSerialPort::NoParity);             //无校验位
    minitPort->setStopBits(QSerialPort::OneStop);            //一位停止位

    connect(minitPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

    //读取仪器型号
    QByteArray getInstrumentType = QUIUtils::ReadcoordinateArry(EQUIPMENT_TYPED);
    handlewritedataToEquip(getInstrumentType);
    QLOG_DEBUG()<<tr("第一次问连接成功,读取仪器类型命令...")<<getInstrumentType.toHex(' ').trimmed().toUpper()<<endl;
	return;
}

void loadEquipmentPos::receiveInfo()
{
    if (!minitPort || !minitPort->isOpen()) {
        QLOG_WARN() << "串口未打开或无效，无法接收数据";
        return;
    }


    const QByteArray recvedata = minitPort->readAll();

    if(recvedata.isEmpty()){
        return;
    }

    const QString framedata  = recvedata.toHex(' ').trimmed().toUpper();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	const QStringList dataList = framedata.split(" ", Qt::SkipEmptyParts);
#else
	const QStringList dataList = framedata.split(" ", QString::SkipEmptyParts);
#endif
    const int size_ = dataList.size();

    if (size_ == 0) {
        QLOG_WARN() << "解析后数据为空";
        return;
    }

    QList<QStringList> data_;
    data_.clear();

    if((size_ % PROTOCOL_LENGTH) == 0)
    {
        int time_ = size_/PROTOCOL_LENGTH;
        QStringList singledata ;
        for(int i = 0; i < time_; i++)
        {
            singledata.clear();

            for(int k = 0; k < PROTOCOL_LENGTH; k++)
                singledata.push_back(dataList.at(i*15+k));
            data_.push_back(singledata);
        }
        for(int k = 0 ; k < data_.size() ; k++)
        {
            QStringList recv_data = data_.at(k);
            parsingReceivedMessages(recv_data); //解析

        }
    }
    else
    {
        QLOG_ERROR()<<"收到数据长度异常数据"<<recvedata.toHex(' ').trimmed().toUpper()<<
                      "长度"<<size_<<__FILE__<<__LINE__<<endl;
    }
    return;
}


void  loadEquipmentPos::parsingReceivedMessages(const QStringList ArryRecvdata)
{
    QString hexstr;
    bool ok;
    hexstr = ArryRecvdata[0];
    quint8 HexSlaveAddr = hexstr.toInt(&ok,HEX_SWITCH);
    hexstr = ArryRecvdata[1];
    quint8 HexFuncCode = hexstr.toInt(&ok,HEX_SWITCH);
    hexstr = ArryRecvdata[2];
    quint8 HexCmdNum =  hexstr.toInt(&ok,HEX_SWITCH); //命令编号

    if(MAIN_CONTROL == HexSlaveAddr && HexFuncCode == MAINBOARD_PARA_FUNCT)
    {
        _equipmentParaParsing(HexCmdNum,ArryRecvdata); //解析仪器参数
    }
    else if(MAIN_CONTROL == HexSlaveAddr && MAINBOARD_FUNCTIONCODE == HexFuncCode)
    {
        _mainbordParadata(HexCmdNum,ArryRecvdata);
        //QLOG_DEBUG() << "int解析收到主板数据"<<ArryRecvdata <<"编号:"<<HexCmdNum << endl;
    }
    return;
}


void loadEquipmentPos::RecvSuippleNextStep(const bool bRead,
                                            quint8 finished,
                                            quint8 nextSend,
                                            quint8 indexReagent,
                                            quint8 lastBottle,
                                            quint16 OneBottleRatio){

    if(!bRead){
        completeddel(finished);
        ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(WRITE_OPERAT,indexReagent,lastBottle); //设置瓶数
        ConsumablesOper::GetpInstance()->updateReplaceLocRatio(WRITE_OPERAT,  indexReagent, OneBottleRatio); //写瓶内剩余
        QLOG_DEBUG()<<GlobalData::mapReagentNames(indexReagent)<<"瓶数"<<lastBottle<<"比"<<OneBottleRatio;
       GroupReadParaCommder(nextSend);
    } else{
        _writeFinish(finished);
        _sendWriteAxisOrder(nextSend);
        //QLOG_DEBUG()<<"初始化写入耗材AA 数量=0"<<endl;
    }
    return;
}

//设置整体数 只有试管和S2
void loadEquipmentPos::RecvSuippleNextStepOnlyBottle(const bool bRead,quint8 finished,
                                                       quint8 nextSend,quint8 indexReagent,
                                                       quint8 lastBottle)
{
    if(!bRead){
        completeddel(finished);
        ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(WRITE_OPERAT,indexReagent,lastBottle);
        GroupReadParaCommder(nextSend);
    }
    else{
        _writeFinish(finished);
        _sendWriteAxisOrder(nextSend);
    }
    return;
}

//AA-RIS 右边的试剂只设置百分比
void loadEquipmentPos::RecvSuippleNextStepOnlyRatio(const bool bRead,quint8 finished,quint8 nextSend,quint8 indexReagent,
                                               quint16 OneBottleRatio)
{
    if(!bRead){
        completeddel(finished);
        ConsumablesOper::GetpInstance()->updateReplaceLocRatio(WRITE_OPERAT,
                                                               indexReagent,
                                                               OneBottleRatio);
       GroupReadParaCommder(nextSend);
    }
    else{
        _writeFinish(finished);
        _sendWriteAxisOrder(nextSend);
    }
    return;
}


void loadEquipmentPos::_mainbordParadata(quint8 indexReagent, const QStringList ArryRecvdata)
{
    // 输入数据校验
    if(ArryRecvdata.size() < 13) {
        QLOG_ERROR() << "Invalid data length:" << ArryRecvdata.size()
                    << ", required at least" << 13;
        return;
    }

    //安全解析数据 剩余瓶数
    bool convertOk = false;
    quint8 NumberofbottlesRemaining = ArryRecvdata.value(9).toInt(&convertOk, HEX_SWITCH);
    if(!convertOk) {
        QLOG_WARN() << "Invalid bottle total format:" << ArryRecvdata.value(9);
        NumberofbottlesRemaining = DEFAULT_BOTTLE_COUNT;
    }

    // 组合比例值并校验 瓶内剩余比例 max = 512
    const quint8 highByte = ArryRecvdata.value(12).toInt(&convertOk, HEX_SWITCH);
    const quint8 lowByte = ArryRecvdata.value(11).toInt(&convertOk, HEX_SWITCH);
    quint16 Remainingproportions = convertOk ? (highByte << 8) | lowByte : 0;
    Remainingproportions = qBound<quint16>(0, Remainingproportions, MAX_RATIO_SWITCT);

    //有效期
    //highByte = ArryRecvdata.value(14).toInt(&convertOk, HEX_SWITCH);
    //lowByte = ArryRecvdata.value(13).toInt(&convertOk, HEX_SWITCH);
    //quint16 periodofValidity = convertOk ? (highByte << 8) | lowByte : 0;

    switch(indexReagent)
    {
        case MAINBORD_REAGENT_AA_INFO:
        {
            RecvSuippleNextStep(m_bReadorWrite,MAINBORD_REAGENT_AA_INFO,
                                MAINBORD_REAGENT_ADP_INFO,
                                INDEX_AA_CONSUMABLE,
                                NumberofbottlesRemaining,
                                Remainingproportions);
            break;
        }
        case MAINBORD_REAGENT_ADP_INFO:
        {
            RecvSuippleNextStep(m_bReadorWrite,MAINBORD_REAGENT_ADP_INFO,
                                MAINBORD_REAGENT_EPI_INFO,
                                INDEX_ADP_CONSUMABLE,
                                NumberofbottlesRemaining,
                                Remainingproportions);
            break;
        }
        case MAINBORD_REAGENT_EPI_INFO:
                RecvSuippleNextStep(m_bReadorWrite,
                                    MAINBORD_REAGENT_EPI_INFO,
                                    MAINBORD_REAGENT_COL_INFO,
                                    INDEX_EPI_CONSUMABLE,
                                    NumberofbottlesRemaining,
                                    Remainingproportions);
        break;
        case MAINBORD_REAGENT_COL_INFO:
                RecvSuippleNextStep(m_bReadorWrite,
                                    MAINBORD_REAGENT_COL_INFO,
                                    MAINBORD_REAGENT_RIS_INFO,
                                    INDEX_COL_CONSUMABLE,
                                    NumberofbottlesRemaining,
                                    Remainingproportions);
        break;
        case MAINBORD_REAGENT_RIS_INFO:
                RecvSuippleNextStep(m_bReadorWrite,
                                    MAINBORD_REAGENT_RIS_INFO,
                                    MAINBORD_REAGENT_CLEAN_S1_INFO,
                                    INDEX_RIS_CONSUMABLE,
                                    NumberofbottlesRemaining,
                                    Remainingproportions);
        break;
        case MAINBORD_REAGENT_CLEAN_S1_INFO:
                RecvSuippleNextStep(m_bReadorWrite,
                                    MAINBORD_REAGENT_CLEAN_S1_INFO,
                                    MAINBORD_REAGENT_TESTTUBE_INFO,
                                    INDEX_CLEANLINQUE_CONSUMABLE,
                                    NumberofbottlesRemaining,
                                    Remainingproportions);
        break;
        case MAINBORD_REAGENT_TESTTUBE_INFO:
                RecvSuippleNextStepOnlyBottle(m_bReadorWrite,
                                              MAINBORD_REAGENT_TESTTUBE_INFO,
                                              MAINBORD_REAGENT_CLEAN_S2_INFO,
                                              INDEX_TESTTUBE_CONSUMABLE,
                                              NumberofbottlesRemaining);
        break;
        case MAINBORD_REAGENT_CLEAN_S2_INFO:
                RecvSuippleNextStepOnlyBottle(m_bReadorWrite,
                                              MAINBORD_REAGENT_CLEAN_S2_INFO,
                                              MAINBORD_REAGENT_AA_1_INFO,
                                              OUTSIDE_CLEANLINQUE_S2,
                                              NumberofbottlesRemaining);
        break;
        case MAINBORD_REAGENT_AA_1_INFO:
                RecvSuippleNextStepOnlyRatio(m_bReadorWrite,
                                             MAINBORD_REAGENT_AA_1_INFO,
                                             MAINBORD_REAGENT_ADP_1_INFO,
                                             INDEX_AA_1_CONSUMABLE,
                                             Remainingproportions);
        break;

        case MAINBORD_REAGENT_ADP_1_INFO:
                RecvSuippleNextStepOnlyRatio(m_bReadorWrite,
                                             MAINBORD_REAGENT_ADP_1_INFO,
                                             MAINBORD_REAGENT_EPI_1_INFO,
                                             INDEX_ADP_1_CONSUMABLE,
                                             Remainingproportions);
        break;

        case MAINBORD_REAGENT_EPI_1_INFO:
                RecvSuippleNextStepOnlyRatio(m_bReadorWrite,
                                             MAINBORD_REAGENT_EPI_1_INFO,
                                             MAINBORD_REAGENT_COL_1_INFO,
                                             INDEX_EPI_1_CONSUMABLE,
                                             Remainingproportions);
        break;

        case MAINBORD_REAGENT_COL_1_INFO:
                RecvSuippleNextStepOnlyRatio(m_bReadorWrite,
                                             MAINBORD_REAGENT_COL_1_INFO,
                                             MAINBORD_REAGENT_RIS_1_INFO,
                                             INDEX_COL_1_CONSUMABLE,
                                             Remainingproportions);
        break;

        case MAINBORD_REAGENT_RIS_1_INFO:
            if(!m_bReadorWrite){
                completeddel(MAINBORD_REAGENT_RIS_1_INFO);
                ConsumablesOper::GetpInstance()->updateReplaceLocRatio(WRITE_OPERAT,
                                                                       INDEX_RIS_1_CONSUMABLE,
                                                                       Remainingproportions);
            }else{
                _writeFinish(MAINBORD_REAGENT_RIS_1_INFO);
                QLOG_DEBUG()<<"初始化写入耗材RIS数量 0"<<endl;
            }
        break;
    default:break;
    }
    return;
}

void loadEquipmentPos::_equipmentParaParsing(quint8 index_ , const QStringList ArryRecvdata )
{
    using HandlerFunction = std::function<void(const QStringList&)>;
    std::unordered_map<quint8, HandlerFunction> handlerMap = {
        {EQUIPMENT_TYPED, [&](const QStringList& data) { recvEquipmentKind(data); }},
        {EQUIPMENTPARA_I, [&](const QStringList& data) { recvParaIData(data); }},
        {HANDSPARADATA_II, [&](const QStringList& data) { recvParaIIdData(data); }},
        {REAGENT_LIMIT, [&](const QStringList& data) { recvReagentLimit(data); }},
        {BlOODPINPARADATA, [&](const QStringList& data) { recvBloodPinq16data(data); }},
        {REAGENT_CAPACITY, [&](const QStringList& data) { recvReagentCapacity(data); }},
        {AXIS_ORIGIN_X, [&](const QStringList& data) { recvOrininAxis(data); }},
        {AXIS_ORIGIN_Y, [&](const QStringList& data) { _recvOrininAxisY(data); }},
        {AXIS_CHN1_5_X, [&](const QStringList& data) { _recvChnoffsetReagpinXI_V(data); }},
        {AXIS_CHN1_5_Y, [&](const QStringList& data) { _recvChnoffsetReagpinYI_V(data); }},
        {AXIS_CHN6_10_X, [&](const QStringList& data) { _recvChnoffsetReagpinXIV_X(data); }},
        {AXIS_CHN6_10_Y, [&](const QStringList& data) { _recvChnoffsetReagpinYIV_X(data); }},
        {AXIS_CHN11_12_X, [&](const QStringList& data) { _recvChnoffsetReagpinXXI_XII(data); }},
        {AXIS_CHN11_12_Y, [&](const QStringList& data) { _recvChnoffsetReagpinYXI_XII(data); }},
        {AXIS_CHN4_8_HANDSX, [&](const QStringList& data) { _recvHandChnVI_IIIV_X(data); }},
        {AXIS_CHN4_8_HANDSY, [&](const QStringList& data) { _recvHandChnVI_IIIV_Y(data); }},
        {AXIS_CHN9_12_HANDSX, [&](const QStringList& data) { _recvHandsChnX_XIII_X(data); }},
        {AXIS_CHN9_12_HANDSY, [&](const QStringList& data) { _recvHandsChnX_XIII_Y(data); }},
        {AXIS_TRAY_OFFSET_BLOODPINX, [&](const QStringList& data) { _recvTraytubeoffsetbloodpin_x(data); }},
        {AXIS_TRAY_OFFSET_BLOODPINY, [&](const QStringList& data) { _recvTraytubeoffsetbloodpin_y(data); }},
        {AXIS_TRAY_OFFSET_HANDSX, [&](const QStringList& data) { _recvTraytubeoffsetHands_x(data); }},
        {AXIS_TRAY_OFFSET_HANDSY, [&](const QStringList& data) { _recvTraytubeoffsetHands_y(data); }},
        {BLOODPINPARAOTHERDATA, [&](const QStringList& data) { recvBloodOtherdataAll(data); }},
        {PARAREAGENTPINDATA_I, [&](const QStringList& data) { recvReagentData(data); }},
        {PARAREAGENTPINDATA_II, [&](const QStringList& data) { recvReagentDataOther(data); }},
        {PARALIMINTBOTTLE, [&](const QStringList& data) { recveBottleLimit(data); }},
        {CONTROLGRIPPERPARA, [&](const QStringList& data) { recveNegativePressure(data); }},
		{CONTROL_MODULEDIMMINGVALUE, [&](const QStringList& data) { recveModuleDataPressure(data); }}
    };
    // 查找并调用对应的处理函数
    auto it = handlerMap.find(index_);
    if (it != handlerMap.end()) {
        it->second(ArryRecvdata);
    } else {
        // 处理默认情况
        // 可以在这里添加日志记录或其他默认处理逻辑
    }
    return;
}

//读设备参数
void loadEquipmentPos::handleReadDevicePara(const QStringList hexArray)
{
    constexpr int HEX_STATE_INDEX = 6;     // 状态字段索引
    constexpr int TIME_START_INDEX = 9;    // 时间起始索引
    constexpr int TIME_FIELD_SIZE = 6;     // 时间字段总长度（年/月/日各占2字节）
    constexpr const char* TIME_FORMAT = "保存时间:%1年%2月%3日";

    bool ok = false;
    const QString& stateByte = hexArray.value(HEX_STATE_INDEX);
    quint8 hexParaState = stateByte.toUInt(&ok, HEX_SWITCH);
    if (!ok || stateByte.isEmpty()) {
        QLOG_ERROR() << "解析设备状态失败. 原始值:" << stateByte;
        return;  // 关键参数错误，提前退出
    }


       // 时间解析（函数封装+字节序处理）
    auto parseTimeField = [&hexArray](int startIndex, bool isBigEndian) -> quint16 {
       if (startIndex + 1 >= hexArray.size()) return 0;  // 防御越界
       QString lowByte = hexArray.value(startIndex);
       QString highByte = hexArray.value(startIndex + 1);
       // 根据设备字节序决定拼接顺序
       QString combined = isBigEndian ? (highByte + lowByte) : (lowByte + highByte);
       return combined.toInt(nullptr, 16);
    };

    // 假设设备使用小端序（低字节在前），如实际为大端需调整参数
    const quint16 year  = parseTimeField(TIME_START_INDEX,     true);  // 年: 9+0,9+1
    const quint16 month = parseTimeField(TIME_START_INDEX + 2, true); // 月: 9+2,9+3
    const quint16 day   = parseTimeField(TIME_START_INDEX + 4, true);  // 日: 9+4,9+5
    const QString showText = QString(TIME_FORMAT).arg(year).arg(month).arg(day);
    QLOG_DEBUG() << "解析到时间参数:" << showText << "原始数据:" << hexArray;

    for(int k = AXIS_ORIGIN_X; k <= AXIS_TRAY_OFFSET_HANDSY ; k++)
    {
        QByteArray buffer = QUIUtils::ReadcoordinateArry(k);
        READPARAMENTER* porder = new READPARAMENTER;
        porder->_readparaorder = buffer;
        porder->readfinish = false;
        mwriteAxismap.insert(k,porder);
    }

    groupReagentinfo(true); //组织 读 耗材信息读

    Q_EMIT progresstotal(mwriteAxismap.size());

    //已读取到仪器类型--读取仪器内保存的坐标
    Q_EMIT  _whiletoReadEquipPosAixs(hexParaState,showText);

    GroupReadParaCommder(EQUIPMENTPARA_I); //开始读取参数

}


void loadEquipmentPos::recvEquipmentKind(const QStringList hexArry)
{
    const quint8 hexparastate = hexArry.at(5).toUInt(nullptr, HEX_SWITCH);
    if(NOESETQUIPMENT == hexparastate && !mcreatSetType) {
        mcreatSetType = true; //只提示一次设置仪器类型
        m_bReadorWrite = true; //未读取到仪器类型 接下来是写参数
        emit setEquipmentIndex();
        QLOG_WARN()<<"未配置仪器类型!"<<endl;

    } else if(m_bReadorWrite && mcreatSetType) {

        QLOG_DEBUG()<<"开始写初始坐标..."<<endl;
        _sendWriteAxisOrder(EQUIPMENTPARA_I);

    } else if(!m_bReadorWrite && !mcreatSetType){
         handleReadDevicePara(hexArry);
    }
    return;
}


void loadEquipmentPos::groupReagentinfo(bool bread)
{
    if(bread == true)
    {
        QByteArray buffer = QUIUtils::ReadcoordinateArry(REAGENT_LIMIT);
        READPARAMENTER* preagLimit= new READPARAMENTER;
        preagLimit->_readparaorder = buffer;
        preagLimit->readfinish = false;
        mwriteAxismap.insert(REAGENT_LIMIT,preagLimit);

        buffer = QUIUtils::ReadcoordinateArry(REAGENT_CAPACITY);
        READPARAMENTER* preagCapactiy= new READPARAMENTER;
        preagCapactiy->_readparaorder = buffer;
        preagCapactiy->readfinish = false;
        mwriteAxismap.insert(REAGENT_CAPACITY,preagCapactiy);

        buffer = QUIUtils::ReadcoordinateArry(EQUIPMENTPARA_I);
        READPARAMENTER* pParaI = new READPARAMENTER;
        pParaI->_readparaorder = buffer;
        pParaI->readfinish = false;
        mwriteAxismap.insert(EQUIPMENTPARA_I,pParaI);

        buffer = QUIUtils::ReadcoordinateArry(HANDSPARADATA_II);
        READPARAMENTER* pParaII = new READPARAMENTER;
        pParaII->_readparaorder = buffer;
        pParaII->readfinish = false;
        mwriteAxismap.insert(HANDSPARADATA_II,pParaII);

        buffer = QUIUtils::ReadcoordinateArry(BlOODPINPARADATA);
        READPARAMENTER* pParaBloodPindata = new READPARAMENTER;
        pParaBloodPindata->_readparaorder = buffer;
        pParaBloodPindata->readfinish = false;
        mwriteAxismap.insert(BlOODPINPARADATA,pParaBloodPindata);

        buffer = QUIUtils::ReadcoordinateArry(BLOODPINPARAOTHERDATA);
        READPARAMENTER* pParaBloodOtherdata = new READPARAMENTER;
        pParaBloodOtherdata->_readparaorder = buffer;
        pParaBloodOtherdata->readfinish = false;
        mwriteAxismap.insert(BLOODPINPARAOTHERDATA,pParaBloodOtherdata);

        buffer = QUIUtils::ReadcoordinateArry(PARAREAGENTPINDATA_I);
        READPARAMENTER* pParaReagentPin= new READPARAMENTER;
        pParaReagentPin->_readparaorder = buffer;
        pParaReagentPin->readfinish = false;
        mwriteAxismap.insert(PARAREAGENTPINDATA_I,pParaReagentPin);

        buffer = QUIUtils::ReadcoordinateArry(PARAREAGENTPINDATA_II);
        READPARAMENTER* pParaReagentPinOther = new READPARAMENTER;
        pParaReagentPinOther->_readparaorder = buffer;
        pParaReagentPinOther->readfinish = false;
        mwriteAxismap.insert(PARAREAGENTPINDATA_II,pParaReagentPinOther);

        //读取0x19
        buffer = QUIUtils::ReadcoordinateArry(PARALIMINTBOTTLE);
        READPARAMENTER* pBottleLimit = new READPARAMENTER;
        pBottleLimit->_readparaorder = buffer;
        pBottleLimit->readfinish = false;
        mwriteAxismap.insert(PARALIMINTBOTTLE,pBottleLimit);

        //读取0x1a
        buffer = QUIUtils::ReadcoordinateArry(CONTROLGRIPPERPARA);
        READPARAMENTER* pgripperPara = new READPARAMENTER;
        pgripperPara->_readparaorder = buffer;
        pgripperPara->readfinish = false;
        mwriteAxismap.insert(CONTROLGRIPPERPARA,pgripperPara);

        //读取0x1b
        buffer = QUIUtils::ReadcoordinateArry(CONTROL_MODULEDIMMINGVALUE);
        READPARAMENTER* pModuledimmingPara = new READPARAMENTER;
        pModuledimmingPara->_readparaorder = buffer;
        pModuledimmingPara->readfinish = false;
        mwriteAxismap.insert(CONTROL_MODULEDIMMINGVALUE,pModuledimmingPara);



        //耗材余量信息
        QByteArray buffer_info;
        QList<int> indexList;
        indexList<<INDEX_AA_CONSUMABLE <<INDEX_ADP_CONSUMABLE<<INDEX_EPI_CONSUMABLE<<INDEX_COL_CONSUMABLE
           <<INDEX_RIS_CONSUMABLE<<INDEX_CLEANLINQUE_CONSUMABLE<<INDEX_TESTTUBE_CONSUMABLE<<OUTSIDE_CLEANLINQUE_S2
           <<INDEX_AA_1_CONSUMABLE<<INDEX_ADP_1_CONSUMABLE<<INDEX_EPI_1_CONSUMABLE<<INDEX_COL_1_CONSUMABLE
           <<INDEX_RIS_1_CONSUMABLE;

        int n = 0;
        for(int k = MAINBORD_REAGENT_AA_INFO; k <= MAINBORD_REAGENT_RIS_1_INFO; k++)
        {
            buffer_info.clear();
            QUIUtils::readSuppliesControlOrder(buffer_info,k,indexList.at(n));
            READPARAMENTER* preaginfo= new READPARAMENTER;
            preaginfo->_readparaorder = buffer_info;
            preaginfo->readfinish = false;
            mwriteAxismap.insert(k,preaginfo);
			n++;
        }
        QLOG_DEBUG() << "读要发送的命令数"  << mwriteAxismap.size() << endl;
    }
    else
    {
        QByteArray _writedataLimit,_writedataCapacity;
        QUIUtils::_writeReagentLimitOrder(_writedataLimit);
        WRITEPARAMENTER* porderwrite = new WRITEPARAMENTER;
        porderwrite->_writeparaorder = _writedataLimit;
        porderwrite->_writefinish = false;  //写耗材限位
        m_axiswriteequipment.insert(REAGENT_LIMIT, porderwrite);

        QUIUtils::_writeReagentCapacity(_writedataCapacity);
        WRITEPARAMENTER* porderwritecapacity = new WRITEPARAMENTER;
        porderwritecapacity->_writeparaorder = _writedataCapacity;
        porderwritecapacity->_writefinish = false; //写耗材容量
        m_axiswriteequipment.insert(REAGENT_CAPACITY, porderwritecapacity);
    }
}

void loadEquipmentPos::_sendWriteAxisOrder(quint8 index)
{
    if(m_axiswriteequipment.contains(index))
    {
        auto iter = m_axiswriteequipment.find(index);
        if(iter.value()->_writefinish == false)
        {
           handlewritedataToEquip(iter.value()->_writeparaorder);
        }
    }
}
void loadEquipmentPos::_writeFinish(quint8 index)
{
    if(m_axiswriteequipment.contains(index))
    {
        auto iter = m_axiswriteequipment.find(index);
        iter.value()->_writefinish = true;
        emit sendUpdateProgressshow(true);
    }
    return;
}


void loadEquipmentPos::GroupReadParaCommder(quint8 index_)
{
    if(mwriteAxismap.contains(index_))
    {
        auto iter = mwriteAxismap.find(index_);
		if(iter.value()->readfinish == false)
            handlewritedataToEquip(iter.value()->_readparaorder);
    }
    return;
}

void loadEquipmentPos::completeddel(quint8 index_)
{
    if(mwriteAxismap.contains(index_))
    {
        auto iter = mwriteAxismap.find(index_);
        iter.value()->readfinish = true;
        emit sendUpdateProgressshow(false);
    }
    return;
}

void loadEquipmentPos::Translation_conversion(const QStringList origindata,QMap<quint8,quint16>& conversiondata)
{
    quint8 indexfrom = 5;
    conversiondata.clear();
    QString _data = "";
    for(int n = 0; n < 5; n++)
    {
        _data = QString("%1%2").arg(origindata.at(indexfrom + 2 * n + 1)).arg(origindata.at(indexfrom + 2*n));
        conversiondata.insert(n, _data.toUInt(nullptr,HEX_SWITCH));
    }
    return;
}

//接收解析
void loadEquipmentPos::recvParaIData(const QStringList hexArry)
{
    if(!m_bReadorWrite)  
    {
        // 错误检查：确保hexArry有足够元素
        const int minSize = 15; // 需要索引14
        if (hexArry.size() < minSize) {
            QLOG_ERROR() << "hexArry size too small, expected at least " << minSize << " elements";
            return;
        }

        auto &ini = INI_File();

        // 辅助函数：提取两个十六进制字符串组合的quint16值（参考证据1的转换思想）
        auto extractUInt16 = [&](int indexHigh, int indexLow) -> quint16 {
            return QString("%1%2").arg(hexArry.at(indexHigh)).arg(hexArry.at(indexLow)).toInt(nullptr, HEX_SWITCH);
        };

        // 优化模块温度处理：使用循环替代重复代码
        for (int i = 0; i < 3; ++i) {
            int highIndex = 6 + 2 * i; // 对应索引6,8,10
            int lowIndex = 5 + 2 * i;  // 对应索引5,7,9
            quint16 rawValue = extractUInt16(highIndex, lowIndex);
            double temperature = static_cast<double>(rawValue) * TEMP_CONVERSION_RATIO;
            ini.wConfigPara(QString("ModuleTemperature%1").arg(i + 1), temperature);
        }


        // 优化通道状态处理（索引11）
        uint8_t chnByte = hexArry.at(11).toUInt(nullptr, HEX_SWITCH); // 直接转换（参考证据2）
        for (int chn = 0; chn < 8; ++chn) {
            // 简化位操作：直接计算位状态，避免临时数组
            bool opened = (chnByte >> (7 - chn)) & 0x01; // 位顺序从高位（bit7）到低位（bit0）
            QString key = QString("TestTheChanne1Opening_%1").arg(chn + 1);
            ini.wConfigPara(key, opened);
        }

        // 优化索引12的状态处理
        uint8_t chnEndState = hexArry.at(12).toUInt(nullptr, HEX_SWITCH);
        // 通道9-12（高4位：bit7-bit4）
        for (int i = 0; i < 4; ++i) {
            int bitPos = 7 - i; // bit7, bit6, bit5, bit4
            bool opened = (chnEndState >> bitPos) & 0x01;
            ini.wConfigPara(QString("TestTheChanne1Opening_%1").arg(i + 9), opened);
        }

        // 特定开关（低4位：bit3-bit1）
        ini.wConfigPara(SCANCODEBAR, (chnEndState >> 3) & 0x01); // bit3
        ini.wConfigPara(USINGASIDEREAGENTLOC, (chnEndState >> 2) & 0x01); // bit2
        ini.wConfigPara(INITGRABCUPS, (chnEndState >> 1) & 0x01);   // bit1



        completeddel(EQUIPMENTPARA_I);
        GroupReadParaCommder(HANDSPARADATA_II);

    }
    else
    {
        _writeFinish(EQUIPMENTPARA_I);
        _sendWriteAxisOrder(HANDSPARADATA_II);
    }
}

//抓手在弃杯孔下降高度
void loadEquipmentPos::recvParaIIdData(const QStringList hexArry)
{
   const int minRequiredSize = 14;  // 最大索引13+1
   if (hexArry.size() < minRequiredSize) {
       QLOG_ERROR() << "Invalid hexArry size(II):" << hexArry.size();
       return;
   }

   if(!m_bReadorWrite){

       auto &ini = INI_File();

        quint8 throwInnerHands = QString("%1").arg(hexArry.at(5)).toUInt(nullptr,HEX_SWITCH);
        ini._sethandsdownthrowcpus(throwInnerHands);


        quint8 HandsInnerTray[4]={0};
        for(int n = 0; n < 4; n++)
        {
            HandsInnerTray[n] = QString("%1").arg(hexArry.at(6 + n)).toUInt(nullptr,HEX_SWITCH);
            ini._sethandsdownheightinTesttray(n,HandsInnerTray[n]);
        }

        quint8 HandsInnerModule[3]={0};
        for(int h = 0; h < MODULE_3; h++)
        {
            HandsInnerModule[h] = QString("%1").arg(hexArry.at(10 + h)).toUInt(nullptr,HEX_SWITCH);
            ini._sethandsdownheiht(h*4,HandsInnerModule[h]);
        }

        uint8_t HandsByte = QString("%1").arg(hexArry.at(13)).toUInt(nullptr,HEX_SWITCH);
        uint8_t HandsBit[8] = {0};
        bool  _HandsByte2Bit[8]{false};
		int i = 0;
        for(i = 0; i< 8; i++)
        {
           HandsBit[i] = (HandsByte & (0x01<<i)) == (0x01<<i) ? 1 : 0;
           _HandsByte2Bit[i] = (HandsBit[i] == 1)? true : false;
           if(i == 7)
           {
                INI_File().wConfigPara(REPEATGRABCUP,_HandsByte2Bit[i]);
                QLOG_DEBUG()<<"读取抓手从抓状态"<<_HandsByte2Bit[i];
           }
        }

        completeddel(HANDSPARADATA_II);
        GroupReadParaCommder(REAGENT_LIMIT);
    }
    else
    {
        _writeFinish(HANDSPARADATA_II);
        _sendWriteAxisOrder(REAGENT_LIMIT);
    }
}

void loadEquipmentPos::recvReagentLimit(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        // 定义耗材配置：索引、主耗材ID、备耗材ID（若无备耗材，则备ID设为-1）
        struct ReagentConfig {
            int index;
            int mainId;
            int backupId;
        };

        // 配置列表：避免硬编码索引和ID
        const QVector<ReagentConfig> configs = {
           {5, INDEX_AA_CONSUMABLE, INDEX_AA_1_CONSUMABLE},    // AA耗材
           {6, INDEX_ADP_CONSUMABLE, INDEX_ADP_1_CONSUMABLE},  // ADP耗材
           {7, INDEX_EPI_CONSUMABLE, INDEX_EPI_1_CONSUMABLE},  // EPI耗材
           {8, INDEX_COL_CONSUMABLE, INDEX_COL_1_CONSUMABLE},  // COL耗材
           {9, INDEX_RIS_CONSUMABLE, INDEX_RIS_1_CONSUMABLE},  // RIS耗材
           {10, INDEX_TESTTUBE_CONSUMABLE, -1},                // 测试杯，无备耗材
           {11, INDEX_CLEANLINQUE_CONSUMABLE, -1}              // 清洗液限位，无备耗材
        };

        auto *consumables = ConsumablesOper::GetpInstance();

        // 遍历配置列表，统一处理耗材限位更新
        for (const auto &cfg : configs) {
           if (cfg.index < 0 || cfg.index >= hexArry.size()) {
			   QLOG_WARN() << "索引越界: " << cfg.index;
               continue; // 跳过无效索引
           }

           bool ok;
           quint8 limit = hexArry.at(cfg.index).toInt(&ok, HEX_SWITCH);
           if (!ok) {
			   QLOG_WARN() << "转换失败: 索引" << cfg.index;
               continue;
           }

            consumables->updateReagentLimit(WRITE_OPERAT, cfg.mainId, limit);
            if (cfg.backupId != -1) { // 更新备耗材（如果存在）
                consumables->updateReagentLimit(WRITE_OPERAT, cfg.backupId, limit);
            }
        }

        // 单独处理清洗液总容量（拼接索引12和13）
        if (hexArry.size() >= 14) {
            bool ok1, ok2;
            quint16 cleanlinqueCapacity = QString("%1%2").arg(hexArry.at(13)).arg(hexArry.at(12))
                                          .toInt(&ok1, HEX_SWITCH);
            quint8 cleanlinqueLimit = hexArry.at(11).toInt(&ok2, HEX_SWITCH);

            if (ok1 && ok2) {
                consumables->updateReagentTotal(WRITE_OPERAT, INDEX_CLEANLINQUE_CONSUMABLE, cleanlinqueCapacity);
				QLOG_WARN() << "获取S1清洗液总容量和限位: " << cleanlinqueCapacity << "-" << cleanlinqueLimit;
            }  else {
				QLOG_WARN() << "清洗液容量转换失败";
                }
            }
            else {
                QLOG_WARN () << "索引不足，无法处理清洗液容量";
        }

        completeddel(REAGENT_LIMIT);
        GroupReadParaCommder(BlOODPINPARADATA);
    }
    else
    {
        _writeFinish(REAGENT_LIMIT);
        _sendWriteAxisOrder(BlOODPINPARADATA);
    }
}

//接收血样针参数
void loadEquipmentPos::recvBloodPinq16data(const QStringList hexArry)
{
     if(!m_bReadorWrite)
     {
          auto &ini = INI_File();
          bool ok;
          QString hexstr;
          hexstr = hexArry[6] + hexArry[5];
          quint16 q16cleanbloodauckmm = hexstr.toUInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读清洗血样针的清洗液量:"<<q16cleanbloodauckmm;
          ini.SetAbsorbWashingfluidX2(q16cleanbloodauckmm);


          hexstr = hexArry[8] + hexArry[7];
          quint16 q16suckPPPPRPmm = hexstr.toUInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读吸PPP/RPP样本单位量"<<q16suckPPPPRPmm;
          ini.SetLearnSamplevolume(q16suckPPPPRPmm);


          hexstr = hexArry[10] + hexArry[9];
          quint16 q16suckAirmm = hexstr.toUInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读吸样本前吸空气单位量"<<q16suckAirmm;
          ini._setsuckairsuckPRP(q16suckAirmm);


          hexstr = hexArry[14] + hexArry[13] + hexArry[12] + hexArry[11];
          quint32 washTime = hexstr.toInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读取到清洗时间:"<<washTime<<endl;
          ini.setWashesTime(washTime);

         completeddel(BlOODPINPARADATA);
         GroupReadParaCommder(REAGENT_CAPACITY);
     }
     else
     {
         _writeFinish(BlOODPINPARADATA);
         _sendWriteAxisOrder(REAGENT_CAPACITY);
     }
     return;
}

void loadEquipmentPos::recvReagentCapacity(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        // 边界检查
        constexpr int kRequiredSize = 15;  // 最大需要访问到索引14
        if (hexArry.size() < kRequiredSize) {
            QLOG_ERROR() << "hexArry数据不足，需要至少" << kRequiredSize
                        << "个元素，实际只有" << hexArry.size();
            return;
        }

        try {
           // 定义试剂配置结构
           struct ReagentConfig {
               int highByteIndex;
               int lowByteIndex;
               quint8 mainIndex;
               quint8 backupIndex;
               const char* name;
           };
		   // 使用C风格数组替代std::array（如果编译器不支持C++11）
		   const ReagentConfig reagents[] = {
			   { 6, 5, INDEX_AA_CONSUMABLE, INDEX_AA_1_CONSUMABLE, "AA" },
			   { 8, 7, INDEX_ADP_CONSUMABLE, INDEX_ADP_1_CONSUMABLE, "ADP" },
			   { 10, 9, INDEX_EPI_CONSUMABLE, INDEX_EPI_1_CONSUMABLE, "EPI" },
			   { 12, 11, INDEX_COL_CONSUMABLE, INDEX_COL_1_CONSUMABLE, "COL" },
			   { 14, 13, INDEX_RIS_CONSUMABLE, INDEX_RIS_1_CONSUMABLE, "RIS" }
		   };
		   const int reagentCount = sizeof(reagents) / sizeof(reagents[0]);

		   quint16 capacities[5] = { 0 };  // C风格数组替代std::array
		   ConsumablesOper* consumables = ConsumablesOper::GetpInstance();

		   // 统一处理所有试剂
		   for (int i = 0; i < reagentCount; ++i) {
			   const ReagentConfig& config = reagents[i];
			   capacities[i] = QString("%1%2")
				   .arg(hexArry.at(config.highByteIndex))
				   .arg(hexArry.at(config.lowByteIndex))
				   .toInt(nullptr, HEX_SWITCH);

			   consumables->updateReagentTotal(WRITE_OPERAT, config.mainIndex, capacities[i]);
			   consumables->updateReagentTotal(WRITE_OPERAT, config.backupIndex, capacities[i]);
		   }

		   QLOG_DEBUG() << "读取到试剂总容量:"
			   << capacities[0] << "-" << capacities[1] << "-"
			   << capacities[2] << "-" << capacities[3] << "-"
			   << capacities[4];

		   completeddel(REAGENT_CAPACITY);
		   GroupReadParaCommder(AXIS_ORIGIN_X);

		}
		catch (const std::exception& e) {
			QLOG_ERROR() << "处理试剂容量时发生异常:" << e.what();
		}
    }
    else
    {
        _writeFinish(REAGENT_CAPACITY);
        _sendWriteAxisOrder(AXIS_ORIGIN_X);
    }
}


void loadEquipmentPos::recvOrininAxis(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        QString hexstr;
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};

        for(int n = 0; n < indexbyte; n++){
            hexstr = hexArry[2 * n + indexbyte + 1] + hexArry[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
        }

        SingletonAxis::GetInstance()->oper_OriginAxis(NOTIFY_XPOINT,recvdata[0]);
        SingletonAxis::GetInstance()->oper_CleanZonePos(NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,recvdata[1]);
        SingletonAxis::GetInstance()->oper_CleanZonePos(NOTIFY_XPOINT,MOTOR_REAGNET_INDEX,recvdata[2]);
        SingletonAxis::GetInstance()->oper_ReagentZonePos(NOTIFY_XPOINT,0,recvdata[3]);
        SingletonAxis::GetInstance()->oper_ThrowTubeHolePos(NOTIFY_XPOINT,recvdata[4]);
        QLOG_INFO()<<"[读取]<=="<<"原点x:"<<recvdata[0]
					<<"清洗血样针x:"<<recvdata[1]<<"清洗试剂针x:"<<recvdata[2]
                   <<"试剂区x:"<<recvdata[3]<<"弃杯x:"<<recvdata[4]<<endl;
        delete []recvdata;

        completeddel(AXIS_ORIGIN_X);
        GroupReadParaCommder(AXIS_ORIGIN_Y);
    } else {
        _writeFinish(AXIS_ORIGIN_X);
        _sendWriteAxisOrder(AXIS_ORIGIN_Y);
    }
    return;
}

void loadEquipmentPos::_recvOrininAxisY(const QStringList hexArry)
{
    constexpr int MIN_HEX_SIZE = 15; // 确保至少15个元素
    if (hexArry.size() < MIN_HEX_SIZE) {
        QLOG_ERROR() << "Invalid hex data size:" << hexArry.size();
        return;
    }
    if(!m_bReadorWrite){
        QString hexstr;
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};

        for(int n = 0; n < indexbyte; n++){
            hexstr = hexArry[2 * n + indexbyte + 1] + hexArry[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
        }

        auto* axisInstance = SingletonAxis::GetInstance();
        axisInstance->oper_OriginAxis(NOTIFY_YPOINT,recvdata[0]);
        axisInstance->oper_CleanZonePos(NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,recvdata[1]);
        axisInstance->oper_CleanZonePos(NOTIFY_YPOINT,MOTOR_REAGNET_INDEX,recvdata[2]);
        axisInstance->oper_ReagentZonePos(NOTIFY_YPOINT,0,recvdata[3]);
        axisInstance->oper_ThrowTubeHolePos(NOTIFY_YPOINT,recvdata[4]);
        QLOG_INFO()<<"[读取]<=="<<"原点y:"<<recvdata[0]<<"清洗血样针y:"<<recvdata[1]<<"清洗试剂针y:"<<recvdata[2]
                   <<"试剂区y:"<<recvdata[3]<<"弃杯y:"<<recvdata[4]<<endl;
        delete []recvdata;

        QPoint firstReagentaxis(0,0);
        axisInstance->reagetZoneAxisPos(READ_OPERRAT, 0, firstReagentaxis);

        //生成其它试剂位坐标
        QMap<quint8,QPoint> reagentPoints;
        QUIUtils::CreatReagArsOtherAxis(firstReagentaxis,reagentPoints);


        auto iter = reagentPoints.begin();
        while(iter != reagentPoints.end()){
            quint8 holekey = iter.key();
            QPoint holeval = iter.value();
			axisInstance->reagetZoneAxisPos(WRITE_OPERAT,holekey,holeval);
            iter++;
        }

        completeddel(AXIS_ORIGIN_Y);
        GroupReadParaCommder(AXIS_CHN1_5_X);
    } else {
        _writeFinish(AXIS_ORIGIN_Y);
        _sendWriteAxisOrder(AXIS_CHN1_5_X);
    }
    return;
}


void loadEquipmentPos::_recvChnoffsetReagpinXI_V(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        QString hexstr;
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};
        auto* axisInstance = SingletonAxis::GetInstance();
        for(int n = 0; n < indexbyte; n++){
            hexstr = hexArry[2 * n + indexbyte + 1] + hexArry[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
            axisInstance->oper_TestChnZoneAxispos(NOTIFY_XPOINT,n,MOTOR_REAGNET_INDEX,recvdata[n]);
            QLOG_INFO()<<"通道"<<n+1<<"offset试剂针X坐标:["<<recvdata[n]<<"]";
        }
        delete []recvdata;

        completeddel(AXIS_CHN1_5_X);
        GroupReadParaCommder(AXIS_CHN1_5_Y);
    }
    else
    {
        _writeFinish(AXIS_CHN1_5_X);
        _sendWriteAxisOrder(AXIS_CHN1_5_Y);
    }

}
void loadEquipmentPos::_recvChnoffsetReagpinYI_V(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        QString hexstr;
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};
        auto* axisInstance = SingletonAxis::GetInstance();
        for(int n = 0; n < indexbyte; n++){
            hexstr = hexArry[2 * n + indexbyte + 1] + hexArry[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
            axisInstance->oper_TestChnZoneAxispos(NOTIFY_YPOINT,n,MOTOR_REAGNET_INDEX,recvdata[n]);
            QLOG_INFO()<<"通道"<<n+1<<"offset试剂针Y坐标:["<<recvdata[n]<<"]";
        }
        delete []recvdata;

        completeddel(AXIS_CHN1_5_Y);
        GroupReadParaCommder(AXIS_CHN6_10_X);
    }else {
        _writeFinish(AXIS_CHN1_5_Y);
        _sendWriteAxisOrder(AXIS_CHN6_10_X);
    }
    return;
}

void loadEquipmentPos::_recvChnoffsetReagpinXIV_X(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};
        auto* axisInstance = SingletonAxis::GetInstance();
        for(int n = 0; n < indexbyte; n++){
			QString hexstr = ArryRecvdata[2 * n + indexbyte + 1] + ArryRecvdata[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
            axisInstance->oper_TestChnZoneAxispos(NOTIFY_XPOINT,n + 5,MOTOR_REAGNET_INDEX,recvdata[n]);
            QLOG_INFO()<<"通道offset试剂针"<<n+1<<"x:"<<recvdata[n];
        }
        delete []recvdata;


        completeddel(AXIS_CHN6_10_X);
        GroupReadParaCommder(AXIS_CHN6_10_Y);
    }
    else
    {
        _writeFinish(AXIS_CHN6_10_X);
        _sendWriteAxisOrder(AXIS_CHN6_10_Y);
    }
}
void loadEquipmentPos::_recvChnoffsetReagpinYIV_X(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QString hexstr;
        bool ok;
        const quint8 indexbyte = 5;
        int *recvdata = new int[5]{};
        auto* axisInstance = SingletonAxis::GetInstance();
        for(int n = 0; n < indexbyte; n++){
            hexstr = ArryRecvdata[2 * n + indexbyte + 1] + ArryRecvdata[2 * n + indexbyte];
            recvdata[n] = hexstr.toInt(&ok,HEX_SWITCH);
            axisInstance->oper_TestChnZoneAxispos(NOTIFY_YPOINT,n + 5,MOTOR_REAGNET_INDEX,recvdata[n]);
            QLOG_INFO()<<"通道offset试剂针"<<n+1<<"y:"<<recvdata[n];
        }
        delete []recvdata;

        completeddel(AXIS_CHN6_10_Y);
        GroupReadParaCommder(AXIS_CHN11_12_X);
    }
    else
    {
        _writeFinish(AXIS_CHN6_10_Y);
        _sendWriteAxisOrder(AXIS_CHN11_12_X);
    }
}

void loadEquipmentPos::_recvChnoffsetReagpinXXI_XII(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_XPOINT,10,MOTOR_REAGNET_INDEX,data_[0]);
        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_XPOINT,11,MOTOR_REAGNET_INDEX,data_[1]);
        for(int i = 0 ;i< 3 ;i++)
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_XPOINT,i,MOTOR_HANDS_INDEX,data_[i+2]);

        completeddel(AXIS_CHN11_12_X);
        GroupReadParaCommder(AXIS_CHN11_12_Y);
    }
    else
    {
        _writeFinish(AXIS_CHN11_12_X);
        _sendWriteAxisOrder(AXIS_CHN11_12_Y);
    }
}
void loadEquipmentPos::_recvChnoffsetReagpinYXI_XII(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);

        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_YPOINT,10,MOTOR_REAGNET_INDEX,data_[0]);
        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_YPOINT,11,MOTOR_REAGNET_INDEX,data_[1]);
        for(int i = 0 ;i< 3 ;i++)
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_YPOINT,i,MOTOR_HANDS_INDEX,data_[i+2]);
        completeddel(AXIS_CHN11_12_Y);
        GroupReadParaCommder(AXIS_CHN4_8_HANDSX);
    }
    else
    {
        _writeFinish(AXIS_CHN11_12_Y);
        _sendWriteAxisOrder(AXIS_CHN4_8_HANDSX);
    }
}

void loadEquipmentPos::_recvHandChnVI_IIIV_X(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 5 ; i++)
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_XPOINT,i+3,MOTOR_HANDS_INDEX,data_[i]);
        completeddel(AXIS_CHN4_8_HANDSX);
        GroupReadParaCommder(AXIS_CHN4_8_HANDSY);
    }
    else
    {
        _writeFinish(AXIS_CHN4_8_HANDSX);
        _sendWriteAxisOrder(AXIS_CHN4_8_HANDSY);
    }

}
void loadEquipmentPos::_recvHandChnVI_IIIV_Y(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 5 ; i++)
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_YPOINT,i+3,MOTOR_HANDS_INDEX,data_[i]);
        completeddel(AXIS_CHN4_8_HANDSY);
        GroupReadParaCommder(AXIS_CHN9_12_HANDSX);
    }
    else
    {
        _writeFinish(AXIS_CHN4_8_HANDSY);
        _sendWriteAxisOrder(AXIS_CHN9_12_HANDSX);
    }
}

void loadEquipmentPos::_recvHandsChnX_XIII_X(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 4 ; i++) //8 -12通道
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_XPOINT,i+8,MOTOR_HANDS_INDEX,data_[i]);
        SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_XPOINT,0,data_[4]);
        //QLOG_DEBUG()<<"读取到血样区坐标x:"<<data_[4];
        completeddel(AXIS_CHN9_12_HANDSX);
        GroupReadParaCommder(AXIS_CHN9_12_HANDSY);
    }
    else
    {
        _writeFinish(AXIS_CHN9_12_HANDSX);
        _sendWriteAxisOrder(AXIS_CHN9_12_HANDSY);
    }
}
void loadEquipmentPos::_recvHandsChnX_XIII_Y(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 4 ; i++)
            SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(NOTIFY_YPOINT,i+8,MOTOR_HANDS_INDEX,data_[i]);
        SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_YPOINT,0,data_[4]);
        //QLOG_DEBUG()<<"读取到血样区坐标y:"<<data_[4];


        //生成血样区其它坐标
        QMap<quint8,QPoint> alltheBloodHoleAxis;
        QPoint bloodHeadAxis(0,0);
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,0,bloodHeadAxis);
        quint8 equipmenttype = 0;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmenttype);
        QUIUtils::creatBloodSampleAxis(equipmenttype, bloodHeadAxis, alltheBloodHoleAxis);
        auto iter = alltheBloodHoleAxis.constBegin();
        while(iter != alltheBloodHoleAxis.constEnd())
        {
           quint8 holeIndex = iter.key();
           QPoint holeAxis = iter.value();
           SingletonAxis::GetInstance()->bloodSampleZonePos(WRITE_OPERAT,holeIndex,holeAxis);
            //QLOG_DEBUG()<<"生成所有血样孔坐标n=:"<<holeIndex<<"pos="<<holeAxis;
           iter++;
        }
        //QLOG_DEBUG()<<"生成所有血样孔坐标完成类型:"<<equipmenttype<<endl;

        completeddel(AXIS_CHN9_12_HANDSY);
        GroupReadParaCommder(AXIS_TRAY_OFFSET_BLOODPINX);
    }
    else
    {
        _writeFinish(AXIS_CHN9_12_HANDSY);
        _sendWriteAxisOrder(AXIS_TRAY_OFFSET_BLOODPINX);
    }
}

void loadEquipmentPos::_recvTraytubeoffsetbloodpin_x(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 4 ; i++)
            SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT,i+(i*59),MOTOR_BLOOD_INDEX,data_[i]);
        SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT,0,MOTOR_HANDS_INDEX,data_[4]);

        completeddel(AXIS_TRAY_OFFSET_BLOODPINX);
        GroupReadParaCommder(AXIS_TRAY_OFFSET_BLOODPINY);
    }
    else
    {
        _writeFinish(AXIS_TRAY_OFFSET_BLOODPINX);
        _sendWriteAxisOrder(AXIS_TRAY_OFFSET_BLOODPINY);
    }
}
void loadEquipmentPos::_recvTraytubeoffsetbloodpin_y(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 0 ;i< 4 ; i++)
            SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT,i+(i*59),MOTOR_BLOOD_INDEX,data_[i]);
        SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT,0,MOTOR_HANDS_INDEX,data_[4]);

        //读取生成血样针offset试管盘坐标
        uint spaceTube = 150;
        int numtray = 0;
        quint8 equipmenttype = 0;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmenttype);
        switch(equipmenttype)
        {
            case KS600: numtray = 2;  break;
            case KS800: numtray = 3;  break;
            case KS1200: numtray = 4; break;
            default: numtray = 4; break;
        }
        quint8 keyhole = 0;
		QPoint tsetTubehole(0,0);
        for (int t = 0; t < numtray; t++)
        {
             QPoint trayholeAxis(0,0);
             SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,t+(t*59),MOTOR_BLOOD_INDEX,trayholeAxis);
             for (int R = 0; R < 10; R++){
                for (int L = 0; L < 6; L++){
                    tsetTubehole.setX(trayholeAxis.x() + L*spaceTube);
                    tsetTubehole.setY(trayholeAxis.y() + R*spaceTube);
                    SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT, keyhole, MOTOR_BLOOD_INDEX, tsetTubehole);
                    //QLOG_DEBUG() << "血样针offset试管盘" << keyhole << "坐标=" << tsetTubehole;
                    keyhole++;
                }
            }
        }
        QLOG_DEBUG()<<"读取生成试管区血样针完成";

        completeddel(AXIS_TRAY_OFFSET_BLOODPINY);
        GroupReadParaCommder(AXIS_TRAY_OFFSET_HANDSX);
    }
    else
    {
        _writeFinish(AXIS_TRAY_OFFSET_BLOODPINY);
        _sendWriteAxisOrder(AXIS_TRAY_OFFSET_HANDSX);
    }
}

void  loadEquipmentPos::_recvTraytubeoffsetHands_x(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 1 ;i< 4 ; i++)
            SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT,i+(i*59),MOTOR_HANDS_INDEX,data_[i -1]);

        completeddel(AXIS_TRAY_OFFSET_HANDSX);
        GroupReadParaCommder(AXIS_TRAY_OFFSET_HANDSY);
    }
    else
    {
        _writeFinish(AXIS_TRAY_OFFSET_HANDSX);
        _sendWriteAxisOrder(AXIS_TRAY_OFFSET_HANDSY);
    }
}

void  loadEquipmentPos::_recvTraytubeoffsetHands_y(const QStringList ArryRecvdata)
{
    if(!m_bReadorWrite)
    {
        QMap<quint8,quint16> data_;
        Translation_conversion(ArryRecvdata,data_);
        for(int i = 1 ;i< 4 ; i++)
            SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT,i+(i*59),MOTOR_HANDS_INDEX,data_[i - 1]);

        //读取生成抓手offset试管盘坐标
        uint spaceTube = 150;
        int numtray = 0;
        quint8 equipmenttype = 0;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmenttype);
        switch(equipmenttype)
        {
            case KS600: numtray = 2;  break;
            case KS800: numtray = 3;  break;
            case KS1200: numtray = 4; break;
            default: numtray = 4; break;
        }
        quint8 keyhole = 0;
		QPoint tsetTubehole(0, 0);
        for (int t = 0; t < numtray; t++)
        {
             QPoint trayholeAxis(0,0);
             SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,t+(t*59),MOTOR_HANDS_INDEX,trayholeAxis);
             for (int R = 0; R < 10; R++){
                for (int L = 0; L < 6; L++){
                    tsetTubehole.setX(trayholeAxis.x() + L*spaceTube);
                    tsetTubehole.setY(trayholeAxis.y() + R*spaceTube);
                    SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT, keyhole, MOTOR_HANDS_INDEX, tsetTubehole);
                    //QLOG_DEBUG() << "抓手offset试管盘" << keyhole << "坐标=" << tsetTubehole;
                    keyhole++;
                }
            }
        }

        QLOG_DEBUG()<<"读取生成试管区血样针完成 nexstep 读AA";

        completeddel(AXIS_TRAY_OFFSET_HANDSY);
        GroupReadParaCommder(BLOODPINPARAOTHERDATA);
    }
    else
    {
        _writeFinish(AXIS_TRAY_OFFSET_HANDSY);
        _sendWriteAxisOrder(BLOODPINPARAOTHERDATA);
    }
}
void loadEquipmentPos::recvBloodOtherdataAll(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        double max_value = 400;
        auto &ini = INI_File();

        bool ok;
        QString hexstr;
        hexstr = hexArry[5]; //血样针下降高度
        quint8 bloodNeedleDownHeigh = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetEmptyTubeDownHigh(bloodNeedleDownHeigh);


        hexstr = hexArry[6];//ppp样本系数
        quint8 PPPratio = hexstr.toUInt(&ok,HEX_SWITCH);
        double conversionScale = static_cast<double>((PPPratio/255.0) * max_value)/100.0; // 显式转换
        ini.SetPPPConversionScale(conversionScale);


        hexstr = hexArry[7];//空回
        quint8 NullBackValueValue = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetSecurityValue(NullBackValueValue);


        hexstr = hexArry[8];//针到基准物高度
        quint8 PhysicalHeightValue = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetFixedHigh(PhysicalHeightValue);

        hexstr = hexArry[9];//样本针探测清洗液失败下降高度
        quint8 CleanReagentDetectionDownHighValue = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetFailedCleanLinqueHigh(CleanReagentDetectionDownHighValue);

        hexstr = hexArry[10];//血浆模式下降高度
        quint8 NeedleInPlasmaModeVal = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetAbsorbTubeBottom(NeedleInPlasmaModeVal);

        hexstr = hexArry[11];//贫血探测失败下降高度
        quint8 PPPDetectionDownHigh = hexstr.toUInt(&ok,HEX_SWITCH);
        ini.SetFailedLinqueHigh(PPPDetectionDownHigh);

        hexstr = hexArry[12];//prp样本系数
        quint8 Prpratio = hexstr.toUInt(&ok,HEX_SWITCH);
        double valPrpratio = static_cast<double>((Prpratio/255.0) * max_value) / 100.0;
        ini.setPRPConvertTheratioColumn(valPrpratio);

        hexstr = hexArry[13];
        quint8 testHeightoffset = hexstr.toInt(&ok,HEX_SWITCH);
        double valoffsetheigh = static_cast<double>(testHeightoffset) / 10.0;// 显式转换
        ini.SetTestDifference(valoffsetheigh);
        //测高偏移读上来/10
        QLOG_DEBUG()<<"读取到测高偏移:"<<valoffsetheigh<<endl;

        completeddel(BLOODPINPARAOTHERDATA);
        GroupReadParaCommder(PARAREAGENTPINDATA_I);
    }
    else
    {
        _writeFinish(BLOODPINPARAOTHERDATA);
        _sendWriteAxisOrder(PARAREAGENTPINDATA_I);
    }
}


void loadEquipmentPos::recvReagentData(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {

        // 检查数组长度是否足够
        if (hexArry.size() < PROTOCOL_LENGTH) {
            QLOG_ERROR() << "hexArry 0x17数据长度不足，无法读取试剂参数";
            completeddel(PARAREAGENTPINDATA_I);
            GroupReadParaCommder(PARAREAGENTPINDATA_II);
            return;
        }

        auto &ini = INI_File();
        constexpr int kReagentCount = 5;  // 5 种试剂

        for (int i = 0; i < kReagentCount; ++i) {
            const int reagent = AA_REAGENT + i;

                // 优化 避免重复计算，直接转换 hexArry
            const quint8 suckVolume = hexArry.at(5 + i).toInt(nullptr, HEX_SWITCH);
            ini.setTypesReagentSuckVolume(reagent, suckVolume);

            const quint8 needleDownHigh = hexArry.at(10 + i).toInt(nullptr, HEX_SWITCH);
            ini.setTypesReagentNeedleDownHigh(reagent, needleDownHigh);
        }


        completeddel(PARAREAGENTPINDATA_I);
        GroupReadParaCommder(PARAREAGENTPINDATA_II);
    }
    else
	{
        _writeFinish(PARAREAGENTPINDATA_I);
        _sendWriteAxisOrder(PARAREAGENTPINDATA_II);
    }
}

void loadEquipmentPos::recvReagentDataOther(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        auto &ini = INI_File();
        if (hexArry.size() != PROTOCOL_LENGTH) {
            QLOG_ERROR() << "hexArry 0x18数据长度不足";
            completeddel(PARAREAGENTPINDATA_II);
            GroupReadParaCommder(PARALIMINTBOTTLE);
            return;
        }
        try {
			// 基础参数设置
			ini.SetAbsorbWashingfluidX1(hexArry.at(5).toInt(nullptr, HEX_SWITCH));
			ini.setFailedCleanLinqueReagNeedle(hexArry.at(6).toInt(nullptr, HEX_SWITCH));
			ini.SetFailedCleanLinqueHigh(hexArry.at(7).toInt(nullptr, HEX_SWITCH));

			// 试剂比例设置 - 使用数组和循环优化
			const quint8 kReagents[] = {
				AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
			};
			const int reagentCount = sizeof(kReagents) / sizeof(kReagents[0]);

			for (int i = 0; i < reagentCount; ++i) {
				bool ok;
				double ratio = hexArry.at(8 + i).toInt(&ok, HEX_SWITCH) / 100.0;
				if (!ok) {
					QLOG_ERROR() << "Failed to convert hex value to double for reagent at index " << i;
					ratio = 0.0; // Default value if conversion fails
				}
				ini.setTypesReagentSuckRatio(kReagents[i], ratio);
			}

		}
		catch (const std::exception &e) {
			QLOG_ERROR() << "参数设置异常：" << e.what();
			// 异常处理
		}

      completeddel(PARAREAGENTPINDATA_II);
      GroupReadParaCommder(PARALIMINTBOTTLE);//发送写主板参数0x19
    }
    else
    {
        _writeFinish(PARAREAGENTPINDATA_II);
        _sendWriteAxisOrder(PARALIMINTBOTTLE);
    }

}
void loadEquipmentPos::recveBottleLimit(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        bool ok;
        quint8 *psaveData = new quint8[8];
        for(int i = 0; i < 8; ++i){
             QString hexstr = hexArry[i+5];
             psaveData[i] = hexstr.toUInt(&ok,HEX_SWITCH);
             if(!ok){
                 psaveData[i] = 0;
                 QLOG_ERROR()<<"转换失败";
             }
        }
        QVariantMap configs = {
                {"AAbottleLimit",  psaveData[0]},
                {"ADPbottleLimit", psaveData[1]},
                {"EPIbottleLimit", psaveData[2]},
                {"COLbottleLimit", psaveData[3]},
                {"RISbottleLimit", psaveData[4]},
                {"S1bottleLimit", psaveData[5]},
                {"tubeTarybottleLimit", psaveData[6]},
                {"S2bottleLimit", psaveData[7]}
        };
        INI_File().wBatchConfigPara(configs);
        delete []psaveData;

        //接收解析0x19完成
        completeddel(PARALIMINTBOTTLE);

        //next0x1a== CONTROLGRIPPERPARA 抓手控制参数
        GroupReadParaCommder(CONTROLGRIPPERPARA);
    }
    else
    {
        //收到写完配置
        _writeFinish(PARALIMINTBOTTLE);
        //写耗材
        _sendWriteAxisOrder(CONTROLGRIPPERPARA);
    }
}

void loadEquipmentPos::recveNegativePressure(const QStringList hexArry){
    auto& ini = INI_File(); // 获取INI实例引用，避免重复调用
    const auto hexToUInt16 = [&](int highIndex, int lowIndex) -> quint16 {
        // 添加边界安全检查
        if (highIndex >= hexArry.size() || lowIndex >= hexArry.size() ||
            highIndex < 0 || lowIndex < 0) {
            QLOG_WARN () << "Hex array index out of range: " << highIndex << "," << lowIndex;
            return 0;
        }

        bool ok;
        QString hexStr = hexArry[highIndex] + hexArry[lowIndex];
        return hexStr.toUShort(&ok, HEX_SWITCH);
    };

    const auto safeGetHexByte = [&](int index, quint8 defaultValue = 0) -> quint8 {
        if (index >= hexArry.size() || index < 0) {
            QLOG_WARN() << "Hex array index out of range: " << index;
            return defaultValue;
        }

        bool ok;
        return hexArry[index].toUShort(&ok, HEX_SWITCH) & 0xFF; // 确保返回单字节
    };

    if(!m_bReadorWrite)
    {
        // 使用统一转换函数处理16位HEX数据
        quint16 lessthanMax = hexToUInt16(6, 5);
        quint16 bigthanMin = hexToUInt16(8, 7);
        quint16 grippersuckTime = hexToUInt16(10, 9);

        // 结构化配置参数（提高可读性）
        QVariantMap configs = {
           {GRIPPERLESSTHANMAX, lessthanMax},
           {GRIPPERBIGTHANMIM, bigthanMin},
           {GRIPPERSUCKTIME, grippersuckTime}
        };

        ini.wBatchConfigPara(configs);

        ini.setFilteringMode(safeGetHexByte(11)); // quint8
        ini.setexperimentalMode(safeGetHexByte(12) != 0); // bool (非零为true)
        ini.wConfigPara("AbsorbanceAlgorithm", safeGetHexByte(13) != 0); // bool

        completeddel(CONTROLGRIPPERPARA);//接收解析完控制抓手参数
        GroupReadParaCommder(CONTROL_MODULEDIMMINGVALUE); //next开始读0x1b

    }else{

        //0x1a写完配置
        _writeFinish(CONTROLGRIPPERPARA);

        //写ox1b
        _sendWriteAxisOrder(CONTROL_MODULEDIMMINGVALUE);
    }
}


void loadEquipmentPos::recveModuleDataPressure(const QStringList& hexArry){

    auto& ini = INI_File(); // 获取INI实例引用，避免重复调用
    const auto hexToUInt16 = [&](int highIndex, int lowIndex) -> quint16 {
        // 添加边界安全检查
        if (highIndex >= hexArry.size() || lowIndex >= hexArry.size() ||
            highIndex < 0 || lowIndex < 0) {
            QLOG_WARN () << "Hex array index out of range: " << highIndex << "," << lowIndex;
            return 0;
        }

        bool ok;
        QString hexStr = hexArry[highIndex] + hexArry[lowIndex];
        return hexStr.toUShort(&ok, HEX_SWITCH);
    };

    if(!m_bReadorWrite){
        // 使用统一转换函数处理16位HEX数据
        quint16 moduleIDimming = hexToUInt16(6, 5);
        quint16 moduleIIDimming = hexToUInt16(8, 7);
        quint16 moduleIIIDimming = hexToUInt16(10, 9);

        ini.setModuledimmingVal(MODULE_1, moduleIDimming);
        ini.setModuledimmingVal(MODULE_2, moduleIIDimming);
        ini.setModuledimmingVal(MODULE_3, moduleIIIDimming);

        completeddel(CONTROL_MODULEDIMMINGVALUE);//接收解析完模组参数
        GroupReadParaCommder(MAINBORD_REAGENT_AA_INFO); //next开始读主板试剂信息
    }else{
        //0x1b写完配置
        _writeFinish(CONTROL_MODULEDIMMINGVALUE);
        //写耗材
        _sendWriteAxisOrder(MAINBORD_REAGENT_AA_INFO);
    }
}

/////////////////////////////写坐标参数--start////////////////////////////////////
void loadEquipmentPos::writeEquipmenttyped(const quint8 &index_,bool _ExitParaFile,QString _ParaPath)
{
	quint8 indexEquip = index_;
    SingletonAxis::GetInstance()->equipmentKind(WRITE_OPERAT, indexEquip);

    initOriginAxis(index_);
    initCleanLinqueoffsetBloodPin(index_);
    initCleanLinqueoffsetReagentPin(index_);
    initReagentHoleAxis(index_);
    initThrowCupsAxis(index_);
    initReagentPinOffsetChn(index_);
    initHandsoffsetChn(index_);
    initBloodZoneAxisPos(index_);
    initTrayTubeOffsetHands(index_);
    initTrayTubeOffsetBloodPin(index_);

    QLOG_DEBUG()<<"默认初始坐标写入内存完成"<<__FILE__<<__LINE__<<endl;

    _obatinwriteOrder();

    //写设置参数 0x01 通道开关 调光值 模组温度
    initOrderNumI();

    //写抓手参数信息 0x02
	initWriteHandsParaII(_ExitParaFile,_ParaPath);

    //0x04 写血样针参数
    _initwriteBloodPinParaX(_ExitParaFile,_ParaPath);

    //0x16 写血样针其它参数
    _initwriteBloodpinotherPara(_ExitParaFile,_ParaPath);

    //写试剂针参数0x17 - 0x18 -0x19
    initwritereagentParaDataI();

    //初始化写主板试剂信息到仪器
    initwriteMainReagNum();

    emit this->progresstotal(m_axiswriteequipment.size());//写入总条数
    QLOG_DEBUG() <<"写配置条数"<< m_axiswriteequipment.size()<<endl;

	QByteArray writedata_;
	QUIUtils::WriteEquipmentType(index_, EQUIPMENT_TYPED, writedata_);
    handlewritedataToEquip(writedata_);
    return;
}

void loadEquipmentPos::_obatinwriteOrder()
{
    QMap<quint8,QByteArray> _writedata;
    QUIUtils::mapEquipmentAxisCommmd(_writedata);
    auto it = _writedata.begin();
    while(it != _writedata.end())
    {
        WRITEPARAMENTER* porder = new WRITEPARAMENTER;
        porder->_writeparaorder = it.value();
        porder->_writefinish = false;
        m_axiswriteequipment.insert(it.key(), porder);
		it++;
    }

    groupReagentinfo(false); //写试剂信息

    return;
}


void loadEquipmentPos::configAxisPoint(QPoint &input,uint x_,uint y_)
{
    input.setX(x_);
    input.setY(y_);
}
void loadEquipmentPos::LoadCoordinateData(QString filePath, QString keydata_x, QString keydata_y, QPoint& outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    quint16 AxisPos_x = configIniWrite->value(keydata_x).toInt();
    quint16 AxisPos_y = configIniWrite->value(keydata_y).toInt();
    outdata.setX( AxisPos_x);
    outdata.setY(AxisPos_y );
    delete configIniWrite;
    return;
}

void loadEquipmentPos::loadParaData(bool _bexit, QString filePath,  QString _key ,quint8& outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    if(_bexit)
        outdata = configIniWrite->value(_key).toUInt(); //配置文件存在==取值
    else
        configIniWrite->setValue(_key,outdata);
    delete configIniWrite;
    return;
}
void loadEquipmentPos::loadParaData(bool _bexit, QString filePath,  QString _key ,quint16& outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    if(_bexit)
        outdata = configIniWrite->value(_key).toInt(); //配置文件存在==取值
    else
        configIniWrite->setValue(_key,outdata);
    delete configIniWrite;
    return;
}
void loadEquipmentPos::loadParaData(bool _bexit, QString filePath,  QString _key ,bool& outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    if(_bexit)
        outdata = configIniWrite->value(_key).toBool(); //配置文件存在==取值
    else
        configIniWrite->setValue(_key,outdata);
    delete configIniWrite;
    return;
}
void loadEquipmentPos::loadParaData(bool _bexit, QString filePath,  QString _key ,double& outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    if(_bexit)
        outdata = configIniWrite->value(_key).toDouble(); //配置文件存在==取值
    else
        configIniWrite->setValue(_key,outdata);
    delete configIniWrite;
    return;
}
void loadEquipmentPos::loadParaData(bool _bexit, QString filePath,  QString _key, quint32 &outdata)
{
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF8");
    if(_bexit)
        outdata = configIniWrite->value(_key).toInt(); //配置文件存在==取值
    else
        configIniWrite->setValue(_key,outdata);
    delete configIniWrite;
    return;
}
void loadEquipmentPos::loadParaData(bool isexit, QString filePath, QString key, QVariant& data) {
	QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
	configIniWrite->setIniCodec("UTF8");
	if (isexit)
		data = configIniWrite->value(key).toInt(); 
	else
		configIniWrite->setValue(key, data);
	delete configIniWrite;
	return;
}


QPoint loadEquipmentPos::initOriginAxis(const quint8 &indexEquipment)
{
     QPoint originAxis(0,0); //原点坐标
     int originx = 0,originy = 0;
     switch (indexEquipment) {
         case KS600:
             originx = 122;
             originy = 101;
         break;
         case KS800:
             originx = 122;
             originy = 101;
         break;
         case KS1200:
            originx = 101;
            originy = 106;
         break;
         default:
             break;
     }
     configAxisPoint(originAxis, originx, originy);
     SingletonAxis::GetInstance()->originPos(WRITE_OPERAT, originAxis);
     return originAxis;
}

QPoint loadEquipmentPos::initCleanLinqueoffsetBloodPin(const quint8 &indexEquipment)
{
    QPoint cleanZoneoffsetBloodPin(0,0);//清洗液offset血样针
    switch(indexEquipment) {
        case KS600:  configAxisPoint(cleanZoneoffsetBloodPin,250,634);  break;
        case KS800:  configAxisPoint(cleanZoneoffsetBloodPin,250,634);  break;
        case KS1200: configAxisPoint(cleanZoneoffsetBloodPin,230,634);  break;
        default:break;
    }
    SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT,MOTOR_BLOOD_INDEX,cleanZoneoffsetBloodPin);
    return cleanZoneoffsetBloodPin;
}

QPoint loadEquipmentPos::initCleanLinqueoffsetReagentPin(const quint8 &indexEquipment)
{
    QPoint cleanZoneoffsetKttsPin(0,0);//清洗液offset试剂针
    switch(indexEquipment)
    {
        case KS600:  configAxisPoint(cleanZoneoffsetKttsPin,250,850);  break;
        case KS800:  configAxisPoint(cleanZoneoffsetKttsPin,250,850);  break;
        case KS1200: configAxisPoint(cleanZoneoffsetKttsPin,230,850);  break;
        default:break;
    }
    SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT,MOTOR_REAGNET_INDEX,cleanZoneoffsetKttsPin);
    return cleanZoneoffsetKttsPin;
}

QPoint loadEquipmentPos::initThrowCupsAxis(const quint8 &indexEquipment)
{
    QPoint throwsTubePos(0,0);
    switch(indexEquipment)
    {
        case KS600:  configAxisPoint(throwsTubePos,5270,330);  break;
        case KS800:  configAxisPoint(throwsTubePos,5270,330);  break;
        case KS1200: configAxisPoint(throwsTubePos,5245,335);  break;
        default:break;
    }
    SingletonAxis::GetInstance()->throwTubeHolePos(WRITE_OPERAT,throwsTubePos);
    return throwsTubePos;
}

QPoint loadEquipmentPos::initReagentHoleAxis(const quint8 &indexEquipment)
{
    QMap<quint8,QPoint> ReagentZoneOffsetKitsPin;//试剂区到试剂针坐标
    ReagentZoneOffsetKitsPin.clear();
    QPoint firstReagentZone(0,0);
    switch(indexEquipment)
    {
        case KS600:  configAxisPoint(firstReagentZone,125,1355);  break;
        case KS800:  configAxisPoint(firstReagentZone,125,1355);  break;
        case KS1200: configAxisPoint(firstReagentZone,105,1365);  break;
        default:break;
    }
    QUIUtils::CreatReagArsOtherAxis(firstReagentZone,ReagentZoneOffsetKitsPin);
    auto itmap = ReagentZoneOffsetKitsPin.begin();
    while(itmap != ReagentZoneOffsetKitsPin.end())
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    return firstReagentZone;
}

void loadEquipmentPos::initHandsoffsetChn(const quint8 &indexEquipment)
{
    QMap<quint8,QPoint> chnoffsetHands;
    chnoffsetHands.clear();
    QPoint firstHoleAxis(0,0);
    QPoint otherChnAxispos(0,0);
    const int chnnum = (indexEquipment + 1) * 4;
    switch(indexEquipment)
    {
        case KS600:  configAxisPoint(firstHoleAxis,2313,332);  break;
        case KS800:  configAxisPoint(firstHoleAxis,885,338);  break;
        case KS1200: configAxisPoint(firstHoleAxis,885,338);  break;
        default:break;
    }

    for(int n = 0 ; n < chnnum; n++)
    {
        otherChnAxispos.setX(firstHoleAxis.x() + n*350);
        otherChnAxispos.setY(firstHoleAxis.y());
        chnoffsetHands.insert(n,otherChnAxispos);
    }


    auto itmap = chnoffsetHands.begin();
    while(itmap != chnoffsetHands.end())
    {
        SingletonAxis::GetInstance()->chnZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_HANDS_INDEX,itmap.value());
        itmap++;
    }
    return;
}

void loadEquipmentPos::initReagentPinOffsetChn(const quint8 &indexEquipment)
{
    QMap<quint8,QPoint> chnoffsetReagentPin;
    chnoffsetReagentPin.clear();
    QPoint firstHoleAxis(0,0);
    QPoint otherChnAxispos(0,0);
    const int chnnum = (indexEquipment + 1) * 4;

    switch(indexEquipment)
    {
        case KS600:  configAxisPoint(firstHoleAxis,2309,100);  break;
        case KS800:  configAxisPoint(firstHoleAxis,2309,100);  break;
        case KS1200: configAxisPoint(firstHoleAxis,885,100);  break;
        default:break;
    }

    for(int n = 0 ; n < chnnum; n++)
    {
        otherChnAxispos.setX(firstHoleAxis.x() + n*350);
        otherChnAxispos.setY(firstHoleAxis.y());
        chnoffsetReagentPin.insert(n,otherChnAxispos);
    }

    auto itmap = chnoffsetReagentPin.begin();
    while(itmap != chnoffsetReagentPin.end())
    {
        SingletonAxis::GetInstance()->chnZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_REAGNET_INDEX,itmap.value());
        itmap++;
    }
    return;
}

void loadEquipmentPos::initBloodZoneAxisPos(const quint8 &indexEquipment)
{
    QMap<quint8,QPoint> BloodZoneAxis;
    BloodZoneAxis.clear();
    QPoint firstBloodZone(0,0);

    switch(indexEquipment)
    {
        case KS600:
            configAxisPoint(firstBloodZone,1600,2500);
            QUIUtils::creatBloodSampleAxis(KS600, firstBloodZone,BloodZoneAxis);
        break;
        case KS800:
            configAxisPoint(firstBloodZone,893,2483);
            QUIUtils::creatBloodSampleAxis(KS800, firstBloodZone,BloodZoneAxis);
        break;
        case KS1200:
            configAxisPoint(firstBloodZone,880,2500);
            QUIUtils::creatBloodSampleAxis(KS1200, firstBloodZone,BloodZoneAxis);
        break;
        default:break;
    }
    auto itmap = BloodZoneAxis.begin();
    while(itmap != BloodZoneAxis.end())
    {
        SingletonAxis::GetInstance()->bloodSampleZonePos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    return;
}

void loadEquipmentPos::initTrayTubeOffsetHands(const quint8 &indexEquipment)
{
    QPoint firstAxispos[4]{};
    int traynum = 0;
    switch(indexEquipment)
    {
        case KS600:  traynum = 2; break;
        case KS800:  traynum = 3; break;
        case KS1200: traynum = 4; break;
        default: break;
    }

    switch(indexEquipment)
    {
        case KS600:

            configAxisPoint(firstAxispos[0],1545,952);
            configAxisPoint(firstAxispos[1],3152,952);
        break;
        case KS800:
            configAxisPoint(firstAxispos[0],895,1059);
            configAxisPoint(firstAxispos[1],2097,1057);
            configAxisPoint(firstAxispos[2],3293,1055);
        break;
        case KS1200:
            configAxisPoint(firstAxispos[0],880,958);
            configAxisPoint(firstAxispos[1],2083,956);
            configAxisPoint(firstAxispos[2],3284,956);
            configAxisPoint(firstAxispos[3],4489,954);
        break;
        default:break;
    }

	//QLOG_DEBUG() << "发散设置抓手试管 头坐标" << firstAxispos[0] << firstAxispos[1] << firstAxispos[2] << firstAxispos[3];
	quint8 keyhole = 0;
    const uint spaceTube = 150; //内部试管横/纵向间距相同
	QPoint tsetTubehole(0,0);
	for (int t = 0; t < traynum; t++)
	{
		for (int R = 0; R < 10; R++)
		{
			for (int L = 0; L < 6; L++)
			{
				tsetTubehole.setX(firstAxispos[t].x() + L*spaceTube);
				tsetTubehole.setY(firstAxispos[t].y() + R*spaceTube);
				SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT, keyhole, MOTOR_HANDS_INDEX, tsetTubehole);
                //QLOG_DEBUG() << "抓手offset试管盘" << keyhole <<"坐标="<< tsetTubehole;
				keyhole++;
			}
		}
	}
    return;
}

void loadEquipmentPos::initTrayTubeOffsetBloodPin(const quint8 &indexEquipment)
{
	int traytTestNum = 0;
    QPoint firstAxispos[4]{};
    switch(indexEquipment)
    {
        case KS600: traytTestNum = 2; break;
        case KS800: traytTestNum = 3; break;
        case KS1200: traytTestNum = 4; break;
        default: break;
    }

    switch(indexEquipment)
    {
        case KS600:
            configAxisPoint(firstAxispos[0],1543,500);
            configAxisPoint(firstAxispos[1],3150,500);
        break;
        case KS800:
            configAxisPoint(firstAxispos[0],895,580);
            configAxisPoint(firstAxispos[1],2097,580);
            configAxisPoint(firstAxispos[2],3295,570);
        break;
        case KS1200:
            configAxisPoint(firstAxispos[0],875,494);
            configAxisPoint(firstAxispos[1],2080,494);
            configAxisPoint(firstAxispos[2],3285,495);
            configAxisPoint(firstAxispos[3],4485,488);
        break;
        default:break;
    }

	quint8 keyhole = 0;
	const uint spaceTube = 150; //内部试管横 纵向间距相同
	QPoint tsetTubehole(0, 0);
	for (int t = 0; t < traytTestNum; t++)
	{
		for (int R = 0; R < 10; R++)
		{
			for (int L = 0; L < 6; L++)
			{
				tsetTubehole.setX(firstAxispos[t].x() + L*spaceTube);
				tsetTubehole.setY(firstAxispos[t].y() + R*spaceTube);
				SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT, keyhole, MOTOR_BLOOD_INDEX, tsetTubehole);
                //QLOG_DEBUG() << "血样针offset试管盘" << keyhole << "坐标=" << tsetTubehole;
				keyhole++;
			}
		}
	}
	return;
}

void loadEquipmentPos::initOrderNumI()
{
    auto &ini = INI_File();
    quint16 moduledata[3]= {0,0,0};
    moduledata[0] = ini.rConfigPara(MODULETEMPA).toDouble()/TEMP_CONVERSION_RATIO;
    moduledata[1] = ini.rConfigPara(MODULETEMPB).toDouble()/TEMP_CONVERSION_RATIO;
    moduledata[2] = ini.rConfigPara(MODULETEMPC).toDouble()/TEMP_CONVERSION_RATIO;
    const QString keyFormat = "TestTheChanne1Opening_%1";
    bool channelstate[TEST_CHANNEL_12]{true};
    for(int n = TEST_CHANNEL_1; n <= TEST_CHANNEL_12; n++)
    {
        QString keyStr = keyFormat.arg(n);
        channelstate[n -1] = ini.rConfigPara(keyStr).toBool();
    }

    const quint16 disUsed = 0;
    bool bScanBarcode = ini.rConfigPara(SCANCODEBAR).toBool();
    bool buseRightReagent = ini.rConfigPara(USINGASIDEREAGENTLOC).toBool();
    bool bbootCathCups = ini.rConfigPara(INITGRABCUPS).toBool();

    QByteArray sendOrder;
    QUIUtils::_writeParaNumIOrder(sendOrder,moduledata,channelstate,bScanBarcode,
                                  buseRightReagent,bbootCathCups,disUsed);

    WRITEPARAMENTER* pParaI = new WRITEPARAMENTER;
    pParaI->_writeparaorder = sendOrder;
    pParaI->_writefinish = false;
    m_axiswriteequipment.insert(EQUIPMENTPARA_I, pParaI);
    return;
}

void loadEquipmentPos::initWriteHandsParaII(bool _bexit, QString _path)
{
     QString handsdown_inerThrowHole = "InstrumentParameters/GripperDownThrowcups";
     quint8 Throwholedownmm = 0;
     loadParaData(_bexit,_path,handsdown_inerThrowHole,Throwholedownmm);

     quint8 HandsInnerTray[4]={0};
     for(int n = 0; n < 4; n++)
     {
         QString handsdown_inerTrayTube = QString("InstrumentParameters/HandsdownHeightinTray%1").arg(n);
         quint8 data_downsmm = 0;
         loadParaData(_bexit,_path,handsdown_inerTrayTube,data_downsmm);
         HandsInnerTray[n] = data_downsmm;
     }

     quint8 HandsInnerModule[3]={0};
     for(int h = 0; h < MODULE_3; h++)
     {
         QString keystr = QString("InstrumentParameters/HandsdownHeightinModule%2").arg(h + 1); //从1开始
         quint8 handsdownmminnerChn = 0;
         loadParaData(_bexit,_path,keystr,handsdownmminnerChn);
         HandsInnerModule[h] = handsdownmminnerChn;
     }

     QString replackHnadsCatch = "InstrumentParameters/RepeattheGrabCup";
     bool breplackHnadsCatch = false;
     loadParaData(_bexit,_path,replackHnadsCatch,breplackHnadsCatch);

     QByteArray sendOrder;
     QUIUtils::_writeParaNumIIHandsOrder(sendOrder,Throwholedownmm,HandsInnerTray,HandsInnerModule,breplackHnadsCatch);

     WRITEPARAMENTER* pParaII = new WRITEPARAMENTER;
     pParaII->_writeparaorder = sendOrder;
     pParaII->_writefinish = false;
     m_axiswriteequipment.insert(HANDSPARADATA_II, pParaII);
     return;
}

 //写血样针参数
void loadEquipmentPos::_initwriteBloodPinParaX(bool _bexit,QString _path)
{
    static const QString basickey = "InstrumentParameters";

    const QString cleanbloodauckmm = QString("%1/%2").arg(basickey).arg("WashBloodSampleNeedleAbsorbCleanning");
    const QString suckPPPPRPmm = QString("%1/%2").arg(basickey).arg("TaketheSampleSize");
    const QString suckAirmm =    QString("%1/%2").arg(basickey).arg("suckPRPsuckair"); //吸PRP前吸空气量
    const QString cleanTimekey = QString("%1/%2").arg(basickey).arg("WashesTime");    //清洗时间
    quint16 *data = new quint16[3];
    memset(data, 0, 3 * sizeof(quint16));  // 所有字节置0

    loadParaData(_bexit,_path,cleanbloodauckmm,data[0]);
    loadParaData(_bexit,_path,suckPPPPRPmm,data[1]);
    loadParaData(_bexit,_path,suckAirmm,data[2]);

    quint32 dataend = 1000;
    loadParaData(_bexit,_path,cleanTimekey,dataend);

    QByteArray sendOrder;
    QUIUtils::_writeParaNumBloodOrder(sendOrder,data[0],data[1],data[2],dataend,false);

    delete []data;

    WRITEPARAMENTER* pPara = new WRITEPARAMENTER;
    pPara->_writeparaorder = sendOrder;
    pPara->_writefinish = false;
    m_axiswriteequipment.insert(BlOODPINPARADATA, pPara);
    return;
}
void loadEquipmentPos::_initwriteBloodpinotherPara(bool _bexit,QString _path)
{
    static const QString basickey = "InstrumentParameters";
    const QString bloodpindownmmKey = QString("%1/%2").arg(basickey).arg("DownHighGrabTestCups");
    const QString ConversionScaleKey = QString("%1/%2").arg(basickey).arg("PPPConversionScale");
    const QString NullBackValueKey =    QString("%1/%2").arg(basickey).arg("NullBackValue"); //吸PRP前吸空气量
    const QString PhysicalHeightKey = QString("%1/%2").arg(basickey).arg("PhysicalHeight");

    const QString CleanReagentDetectionDownHighKey = QString("%1/%2").arg(basickey).arg("CleanReagentDetectionDownHigh");
    const QString NeedleInPlasmaModeKey = QString("%1/%2").arg(basickey).arg("NeedleInPlasmaMode");
    const QString AnaemiaDetectionDownHighKey = QString("%1/%2").arg(basickey).arg("AnaemiaDetectionDownHigh");
    const QString ConversionScalePRPKey = QString("%1/%2").arg(basickey).arg("PRPConvertTheratioColumn");
    const QString testHeightOffset = QString("%1/%2").arg(basickey).arg("TestHeightDifference");

    QStringList datakey;
    datakey<<bloodpindownmmKey<<ConversionScaleKey<<NullBackValueKey<<PhysicalHeightKey<<CleanReagentDetectionDownHighKey
          <<NeedleInPlasmaModeKey<<AnaemiaDetectionDownHighKey<<ConversionScalePRPKey<<testHeightOffset;
    quint8 *data = new quint8[10];
    memset(data, 0, 10 * sizeof(quint16));  // 所有字节置0

    for(int n = 0; n < datakey.size() ; n++){
        loadParaData(_bexit,_path,datakey[n],data[n]);
    }

    QByteArray sendOrder;
	quint8* ptr = data;
	auto& arrRef = reinterpret_cast<quint8(&)[10]>(*ptr);  // 强制转换
    QUIUtils::_writeParaBloodOtherOrder(sendOrder, arrRef);
	
	delete []data;

    WRITEPARAMENTER* pPara = new WRITEPARAMENTER;
    pPara->_writeparaorder = sendOrder;
    pPara->_writefinish = false;
    m_axiswriteequipment.insert(BLOODPINPARAOTHERDATA, pPara);
    return;
}





// 新增辅助函数（建议添加到类中）
void loadEquipmentPos::addParameterToEquipment(quint8 type, const QByteArray& data)
{
    auto para = std::make_unique<WRITEPARAMENTER>();
    para->_writeparaorder = data;
    para->_writefinish = false;
    m_axiswriteequipment.insert(type, para.release());
}

void loadEquipmentPos::initwritereagentParaDataI()
{
    auto &ini = INI_File();

    // 使用原生数组替代std::array
    const int REAGENT_COUNT = 5;
    quint8 reagentTypes[REAGENT_COUNT] = {
        AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
    };

    // 初始化数组
    quint8 ReagentPinSuckum[REAGENT_COUNT] = {0};
    quint8 ReagentDownPinmm[REAGENT_COUNT] = {0};
    quint8 otherReagentData[8] = {0};

    for (int i = 0; i < REAGENT_COUNT; ++i) {
        ReagentPinSuckum[i] = ini.getTypesReagentSuckVolume(reagentTypes[i]);
        ReagentDownPinmm[i] = ini.getTypesReagentNeedleDownHigh(reagentTypes[i]);
    }

    otherReagentData[0] = ini.GetAbsorbWashingfluidX1();
    otherReagentData[1] = ini.getFailedCleanLinqueReagNeedle();
    otherReagentData[2] = ini.GetFailedCleanLinqueHigh();

    for (int i = 0; i < REAGENT_COUNT; ++i) {
        otherReagentData[3 + i] = static_cast<quint8>(ini.getTypesReagentSuckRatio(reagentTypes[i]) * 100.0);
    }

    // 生成命令数据
    QByteArray sendOrder, sendOtherOrder;
    QUIUtils::_writeReagPinParaDataOrder(sendOrder, sendOtherOrder,
                                         ReagentPinSuckum,
                                         ReagentDownPinmm,
                                        otherReagentData);

    // 使用辅助函数创建参数对象
    auto createWriteParameter = [](const QByteArray& data) -> WRITEPARAMENTER* {
        WRITEPARAMENTER* para = new WRITEPARAMENTER;
        para->_writeparaorder = data;
        para->_writefinish = false;
        return para;
    };

    // 批量添加参数到容器
    m_axiswriteequipment.insert(PARAREAGENTPINDATA_I, createWriteParameter(sendOrder));
    m_axiswriteequipment.insert(PARAREAGENTPINDATA_II, createWriteParameter(sendOtherOrder));

    // 添加其他参数配置
    //写入试剂整体限位
    addParameterToEquipment(PARALIMINTBOTTLE, initWriteParaintoEquipment());

    //写入抓手参数
    addParameterToEquipment(CONTROLGRIPPERPARA, initWriteGripperPara());

    //写入模组调光的值
    addParameterToEquipment(CONTROL_MODULEDIMMINGVALUE, initModuleDimmingValPara());

    return;
}

QByteArray loadEquipmentPos::initWriteParaintoEquipment()
{
    QByteArray sendOrder = GlobalData::writeBottleLimit2Equipment();
    QLOG_DEBUG() << "写入编号0x19初始整体瓶限位:" << sendOrder.toHex(' ').toUpper();
    return sendOrder;
}

QByteArray loadEquipmentPos::initWriteGripperPara(){
    auto &ini = INI_File();
    const quint16 Lessthanmax = ini.rConfigPara(GRIPPERLESSTHANMAX).toInt();
    const quint16 bigthanmin = ini.rConfigPara(GRIPPERBIGTHANMIM).toInt();
    const quint16 suckTime = ini.rConfigPara(GRIPPERSUCKTIME).toInt();

    const quint8  filteringStyle = ini.getFilteringMode();
    const bool    experimentalMode = ini.getexperimentalMode();
    const bool    Logarithmicformula = ini.rConfigPara("AbsorbanceAlgorithm").toBool();

    return QUIUtils::writeGripperParaDataArry(Lessthanmax,bigthanmin,suckTime,filteringStyle,
                                              experimentalMode,Logarithmicformula,true);
}


QByteArray loadEquipmentPos::initModuleDimmingValPara(){
    auto& ini = INI_File();
    return QUIUtils::writeModuleDimmingVal0x1b(
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_1)),
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_2)),
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_3)),
           true
    );
}


//初始耗材主板全部置0
void loadEquipmentPos::initwriteMainReagNum()
{
    typedef std::pair<quint8, quint8> ReagentConfig;
    const ReagentConfig reagentConfigs[] = {
        ReagentConfig(MAINBORD_REAGENT_AA_INFO,   INDEX_AA_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_AA_1_INFO, INDEX_AA_1_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_ADP_INFO,  INDEX_ADP_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_ADP_1_INFO,INDEX_ADP_1_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_EPI_INFO,  INDEX_EPI_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_EPI_1_INFO,INDEX_EPI_1_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_COL_INFO,  INDEX_COL_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_COL_1_INFO,INDEX_COL_1_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_RIS_INFO,INDEX_RIS_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_RIS_1_INFO,INDEX_RIS_1_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_CLEAN_S1_INFO,INDEX_CLEANLINQUE_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_TESTTUBE_INFO,INDEX_TESTTUBE_CONSUMABLE),
        ReagentConfig(MAINBORD_REAGENT_CLEAN_S2_INFO,OUTSIDE_CLEANLINQUE_S2)
    };

    // 循环处理配置
    for (size_t i = 0; i < sizeof(reagentConfigs)/sizeof(reagentConfigs[0]); ++i) {
        const quint8 mainKey = reagentConfigs[i].first;
        const quint8 indexKey = reagentConfigs[i].second;

        QByteArray suppileInfo;
        QUIUtils::writeSuppliesBottleControlOrder(suppileInfo, mainKey,indexKey,
            0,    // param1
            0,    // param2
            true // flag
        );

        // 创建参数对象（兼容C++11内存管理）
        WRITEPARAMENTER* pSetting = new WRITEPARAMENTER;
        pSetting->_writeparaorder = suppileInfo;
        pSetting->_writefinish = false;

        // 插入到容器
        m_axiswriteequipment.insert(mainKey, pSetting);
    }
   return;
}


void loadEquipmentPos::handlewritedataToEquip(const QByteArray &arry)
{

    if (!minitPort || !minitPort->isOpen()) {
        QLOG_DEBUG() << "串口未打开，无法发送数据";
        return;
    }

    if (arry.isEmpty()) {
        QLOG_DEBUG() << "发送数据为空";
        return ;
    }

    const qint64 bytesWritten = minitPort->write(arry);

    if (bytesWritten == -1) {
        QLOG_WARN() << "写入数据失败:" << minitPort->errorString();
        return ;
    }

    if (bytesWritten != arry.size()) {
        QLOG_WARN() << "数据未完全写入，期望:" << arry.size()
                              << "实际:" << bytesWritten<<"失败命令:"<<arry.toHex(' ').trimmed().toUpper();
        return;
    }

    if (!minitPort->waitForBytesWritten(1000)) {
        QLOG_WARN() << "等待数据写入超时";
        return ;
    }

    QLOG_DEBUG() << "成功写入" << bytesWritten << "字节数据";
    return ;
}

void loadEquipmentPos::_sycnobtainEquipmenttyped(bool Parafilestate,QString ParaFilePath)
{
    m_bParafileExit = Parafilestate;
    m_ParaFilePath = ParaFilePath;
    QLOG_DEBUG()<<"读取参数配置文件状态"<<m_bParafileExit<<"路径"<<m_ParaFilePath<<endl;
    return;
}


//关闭串口
void loadEquipmentPos::CloseSerial()
{
    if (!minitPort) {
        return;
    }

    disconnect(minitPort, &QSerialPort::readyRead,
               this, &loadEquipmentPos::receiveInfo);

    if (minitPort->isOpen()) {
        try {
            minitPort->flush();
            minitPort->clear();
            minitPort->close();
            QLOG_DEBUG() << "坐标串口已安全关闭";
        } catch (const std::exception &e) {
            QLOG_WARN() << "关闭串口时发生异常:" << e.what();
        }
    }

    delete minitPort;
    minitPort = nullptr;

    qDeleteAll(m_axiswriteequipment);
    m_axiswriteequipment.clear();


    qDeleteAll(mwriteAxismap);
    mwriteAxismap.clear();

}
