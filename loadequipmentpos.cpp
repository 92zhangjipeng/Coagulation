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

SingletonAxis *SingletonAxis::g_pSingletonAxis = new (std::nothrow) SingletonAxis;
EquipmentAXIS_ *SingletonAxis::g_pEquipAxiaspos = NULL;

//试剂耗材信息
ConsumablesOper *ConsumablesOper::g_pConsumable = new (std::nothrow) ConsumablesOper;
equipmentConsumablesVec *ConsumablesOper::g_pVecReagentInfo = nullptr;





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
    if(g_pSingletonAxis)
    {
        delete g_pSingletonAxis;
        g_pSingletonAxis = NULL;
    }
}


void SingletonAxis::sycnAxisState(bool bWrite, bool &sycnState)
{
    if(bWrite == true)
        g_pEquipAxiaspos->bsycnFinished = sycnState;
    else
        sycnState = g_pEquipAxiaspos->bsycnFinished;
    return;
}

void SingletonAxis::equipmentKind(bool bWrite,QString &kindType)
{
    if(bWrite == true)
        g_pEquipAxiaspos->equipmentType = kindType;
    else
        kindType = g_pEquipAxiaspos->equipmentType;
    return;
}


void SingletonAxis::equipmentKind(bool bWrite, quint8 &kindType)
{
	if (g_pEquipAxiaspos)
	{
		if (bWrite == true)
			g_pEquipAxiaspos->euqipmentIndex = kindType;
		else
			kindType = g_pEquipAxiaspos->euqipmentIndex;
	}
    return ;
}


void SingletonAxis::paraAxisSaveTime(bool bWrite,QString &timesavestr)
{
    if(bWrite == true)
        g_pEquipAxiaspos->AxixsSaveTime = timesavestr;
    else
        timesavestr = g_pEquipAxiaspos->AxixsSaveTime;
    return;
}


void SingletonAxis::originPos(const bool &bWrite, QPoint &pos)
{
    if(bWrite == true)
        g_pEquipAxiaspos->OriginAxis = pos;
    else
        pos = g_pEquipAxiaspos->OriginAxis;
}


void SingletonAxis::throwTubeHolePos(bool bWrite, QPoint &pos)
{
    if(bWrite == true)
        g_pEquipAxiaspos->ThrowHoleAxis = pos;
    else
        pos = g_pEquipAxiaspos->ThrowHoleAxis;
    QLOG_DEBUG()<<"弃杯孔坐标:"<< g_pEquipAxiaspos->ThrowHoleAxis<<__FUNCTION__<<__LINE__<<endl;
}


void SingletonAxis::cleanZoneAxisPos(bool bWrite,int indexNedl,QPoint &pos)
{
    if(bWrite == true)
    {
        switch(indexNedl)
        {
            case MOTOR_BLOOD_INDEX:
                g_pEquipAxiaspos->cleanZoneoffsetBlodNedl = pos;
            break;
            case MOTOR_REAGNET_INDEX:
                g_pEquipAxiaspos->cleanZoneoffsetRegNedl  = pos;
            break;
        default:break;
        }
    }
    else
    {
        switch(indexNedl)
        {
            case MOTOR_BLOOD_INDEX:
                pos = g_pEquipAxiaspos->cleanZoneoffsetBlodNedl;
            break;
            case MOTOR_REAGNET_INDEX:
                pos = g_pEquipAxiaspos->cleanZoneoffsetRegNedl;
            break;
        default:break;
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


//	auto it = g_pEquipAxiaspos->reagentZoneAxispos.begin();
//	if (bWrite)
//	{
//		bool hadkey = false; //写的时候如果存在就是修改 不存在就是++ 存在就是改
//		while (it != g_pEquipAxiaspos->reagentZoneAxispos.end())
//		{
//			if ((*it)->index == indexReag) {
//				(*it)->reagname = "";
//				(*it)->Axispos = pos;
//				hadkey = true;
//				break;
//			}
//			it++;
//		}
//		if (!hadkey) {
//			REAGENTZONEAXIS_ *psingReaget = new REAGENTZONEAXIS_;
//			psingReaget->index = indexReag;
//			psingReaget->Axispos = pos;
//			psingReaget->reagname = "";
//			g_pEquipAxiaspos->reagentZoneAxispos.append(psingReaget);
//		}
//	}
//	else
//	{
//		while (it != g_pEquipAxiaspos->reagentZoneAxispos.end()){
//			if ((*it)->index == indexReag){
//				pos = (*it)->Axispos;
//				break;
//			}
//			it++;
//		}
//	}
	return;
}

void SingletonAxis::chnZoneAxisPos(bool bWrite,quint8 numChn,quint8 OffsetNedl ,QPoint &pos)
{
   
	auto iter = g_pEquipAxiaspos->pchnAxisPoint.begin();
    if(bWrite ==  true)
    {
		bool bHanditChn = false;
        while(iter != g_pEquipAxiaspos->pchnAxisPoint.end())
        {
             if((*iter)->indexChn == numChn && (*iter)->offsetNeedle == OffsetNedl)
             {
				 (*iter)->axisPos = pos;
                 bHanditChn = true;
                 break;
             }
             iter++;
        }
        if(!bHanditChn)
        {
            ChnAxis_ * pchnStu = new ChnAxis_;
            pchnStu->indexChn = numChn;
            pchnStu->offsetNeedle = OffsetNedl;
            pchnStu->axisPos = pos;
            g_pEquipAxiaspos->pchnAxisPoint.push_back(pchnStu);
        }
    }
    else
    {
        while(iter != g_pEquipAxiaspos->pchnAxisPoint.end())
        {
            if((*iter)->offsetNeedle == OffsetNedl && numChn == (*iter)->indexChn)
            {
                pos = (*iter)->axisPos;
                break;
            }
            iter++;
        }
    }
	return;
}

void SingletonAxis::bloodSampleZonePos(bool bWrite,quint8 numhole,QPoint &pos)
{
    if(bWrite == true)
    {
        SAMPLEBLOODZONEAXISPOS_ *pbloodholestu = new SAMPLEBLOODZONEAXISPOS_;
        pbloodholestu->index = numhole;
        pbloodholestu->axisPos = pos;
        g_pEquipAxiaspos->bloodSampleAxisPos.append(pbloodholestu);
    }
    else
    {
        auto it = g_pEquipAxiaspos->bloodSampleAxisPos.begin();
        while(it != g_pEquipAxiaspos->bloodSampleAxisPos.end())
        {
            if((*it)->index == numhole){
                pos = (*it)->axisPos;
                break;
            }
            it++;
        }
    }
}

//写试管区坐标
void SingletonAxis::WriteEmptyTube_Coordinate(quint8 numhole,quint8 indexNeedle,QPoint emptypos)
{
    TRYTHECUPAXIS_ *ptestTaryAxisStu = nullptr;
    bool baddtube = false;
    if(g_pEquipAxiaspos->testTubeZoneAxisPos.isEmpty())
    {
        ptestTaryAxisStu = new TRYTHECUPAXIS_;
        ptestTaryAxisStu->axisPos = emptypos;
        ptestTaryAxisStu->offsetNeedle = indexNeedle;
        ptestTaryAxisStu->numTube = numhole;
        ptestTaryAxisStu->indexTray = witchoneindexTary(numhole); //在哪个盘
        g_pEquipAxiaspos->testTubeZoneAxisPos.push_back(ptestTaryAxisStu);
    }
    else
    {
         auto iter = g_pEquipAxiaspos->testTubeZoneAxisPos.begin();
         while(iter != g_pEquipAxiaspos->testTubeZoneAxisPos.end())
         {
             ptestTaryAxisStu = *iter;
             if(ptestTaryAxisStu->numTube == numhole && ptestTaryAxisStu->offsetNeedle == indexNeedle)
             {
                 ptestTaryAxisStu->axisPos = emptypos;
                 baddtube = true;
                 break;
             }
             iter++;
         }
         if(baddtube == false)
         {
             ptestTaryAxisStu = new TRYTHECUPAXIS_;
             ptestTaryAxisStu->axisPos = emptypos;
             ptestTaryAxisStu->offsetNeedle = indexNeedle;
             ptestTaryAxisStu->numTube = numhole;
             ptestTaryAxisStu->indexTray = witchoneindexTary(numhole); //在哪个盘
             g_pEquipAxiaspos->testTubeZoneAxisPos.push_back(ptestTaryAxisStu);
         }
    }
    return;
}
//试杯孔在哪个盘
quint8 SingletonAxis::witchoneindexTary(const quint8 indextube)
{
    if (indextube >= 240) return 0;
    return (indextube / 60) + 1;
}

quint8 SingletonAxis::testTaryZoneAxisPos(bool bWrite,quint8 numhole,quint8 indexNeedle,QPoint &pos)
{
    quint8 indexTray = 0;
    if(bWrite)
    {
        QPoint tubecoord = pos;
        WriteEmptyTube_Coordinate(numhole,indexNeedle,tubecoord);
    }
    else
    {
        auto iter = g_pEquipAxiaspos->testTubeZoneAxisPos.constBegin();
        while(iter != g_pEquipAxiaspos->testTubeZoneAxisPos.constEnd())
        {
            TRYTHECUPAXIS_ *ptestTaryAxisStu = (*iter);
            if(ptestTaryAxisStu->offsetNeedle == indexNeedle && numhole == ptestTaryAxisStu->numTube)
            {
                pos = ptestTaryAxisStu->axisPos;
                indexTray = ptestTaryAxisStu->indexTray;
                break;
            }
            iter++;
        }
    }
    return indexTray;
}



//修改坐标
void SingletonAxis::oper_OriginAxis(bool bNotif_x,int posValue)
{
    if(bNotif_x == true)
        g_pEquipAxiaspos->OriginAxis.setX(posValue);
    else
        g_pEquipAxiaspos->OriginAxis.setY(posValue);
}

void SingletonAxis::oper_ThrowTubeHolePos(bool bNotif_x,int posValue)
{
    if(bNotif_x == true)
        g_pEquipAxiaspos->ThrowHoleAxis.setX(posValue);
    else
        g_pEquipAxiaspos->ThrowHoleAxis.setY(posValue);
}

void SingletonAxis::oper_CleanZonePos(bool bNotif_x,int indexNedl,int posValue)
{
    if(bNotif_x == true)
    {
        switch(indexNedl)
        {
            case MOTOR_BLOOD_INDEX:   g_pEquipAxiaspos->cleanZoneoffsetBlodNedl.setX(posValue);   break;
            case MOTOR_REAGNET_INDEX: g_pEquipAxiaspos->cleanZoneoffsetRegNedl.setX(posValue);    break;
        default:break;
        }
    }
    else
    {
        switch(indexNedl)
        {
            case MOTOR_BLOOD_INDEX:     g_pEquipAxiaspos->cleanZoneoffsetBlodNedl.setY(posValue);   break;
            case MOTOR_REAGNET_INDEX:   g_pEquipAxiaspos->cleanZoneoffsetRegNedl.setY(posValue);    break;
        default:break;
        }
    }
}

void SingletonAxis::oper_ReagentZonePos(bool bNotif_x,quint8 indexReag,quint16 posValue)
{
    if(g_pEquipAxiaspos->reagentZoneAxispos.isEmpty())
    {
         REAGENTZONEAXIS_ *psingReaget = new REAGENTZONEAXIS_;
         psingReaget->index = indexReag;
         psingReaget->reagname = "";
         if(bNotif_x == true)
            psingReaget->Axispos.setX(posValue);
         else
            psingReaget->Axispos.setY(posValue);
         g_pEquipAxiaspos->reagentZoneAxispos.push_back(psingReaget);
    }
    else
    {
        auto it = g_pEquipAxiaspos->reagentZoneAxispos.begin();
        while(it != g_pEquipAxiaspos->reagentZoneAxispos.end())
        {
            REAGENTZONEAXIS_ *psingReaget = (*it);
            if(psingReaget->index == indexReag)
            {
                if(bNotif_x == NOTIFY_XPOINT)
                    psingReaget->Axispos.setX(posValue);
                else
                    psingReaget->Axispos.setY(posValue);
                break;
            }
            it++;
        }
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
    if(g_pEquipAxiaspos->bloodSampleAxisPos.isEmpty())
    {
        SAMPLEBLOODZONEAXISPOS_ *pbloodholestu = new SAMPLEBLOODZONEAXISPOS_;
        pbloodholestu->index = numhole;
        if(bNotif_x == true)
            pbloodholestu->axisPos.setX(posValue);
        else
            pbloodholestu->axisPos.setY(posValue);
        g_pEquipAxiaspos->bloodSampleAxisPos.push_back(pbloodholestu);
    }
    else
    {
        auto it = g_pEquipAxiaspos->bloodSampleAxisPos.begin();
        while(it != g_pEquipAxiaspos->bloodSampleAxisPos.end())
        {
            SAMPLEBLOODZONEAXISPOS_ *pbloodholestu = (*it);
            if(pbloodholestu->index == numhole)
            {
                if(bNotif_x == true)
                    pbloodholestu->axisPos.setX(posValue);
                else
                    pbloodholestu->axisPos.setY(posValue);
                break;
            }
            it++;
        }
    }

}

void SingletonAxis::oper_TestTrayZonaPos(bool bNotif_x,quint8 numhole,quint8 indexNeedle,quint16 posValue)
{
    bool hand = false;
    TRYTHECUPAXIS_ *ptestTaryAxisStu = nullptr;
    auto iter = g_pEquipAxiaspos->testTubeZoneAxisPos.begin();
    while(iter != g_pEquipAxiaspos->testTubeZoneAxisPos.end())
    {
        ptestTaryAxisStu = *iter;
        if(numhole == ptestTaryAxisStu->numTube && ptestTaryAxisStu->offsetNeedle == indexNeedle)
        {
            if(bNotif_x == true)
                ptestTaryAxisStu->axisPos.setX(posValue);
            else
                ptestTaryAxisStu->axisPos.setY(posValue);
            hand = true;
            break;
        }
        iter++;
    }
    if(hand == false)
    {
        TRYTHECUPAXIS_ *ptestTaryAxisStu = new TRYTHECUPAXIS_;
        ptestTaryAxisStu->offsetNeedle = indexNeedle;
        ptestTaryAxisStu->numTube = numhole;
        ptestTaryAxisStu->indexTray = witchoneindexTary(numhole); //在哪个盘
        if(bNotif_x == true)
            ptestTaryAxisStu->axisPos.setX(posValue);
        else
            ptestTaryAxisStu->axisPos.setY(posValue);
        g_pEquipAxiaspos->testTubeZoneAxisPos.push_back(ptestTaryAxisStu);
    }
    return;
}



quint8 SingletonAxis::TeatTayr_findHole(int indexZ,QPoint moved)
{
    quint8 backhole = 250;
    QLOG_DEBUG()<<"映射找试管盘坐标"<<moved<<"offset针"<<indexZ;
    auto iter = g_pEquipAxiaspos->testTubeZoneAxisPos.begin();
    while(iter != g_pEquipAxiaspos->testTubeZoneAxisPos.end())
    {
        TRYTHECUPAXIS_ *ptestTaryAxisStu = (*iter);
        if(ptestTaryAxisStu->offsetNeedle == indexZ)
        {
            if(ptestTaryAxisStu->axisPos == moved)
                backhole = ptestTaryAxisStu->numTube;
            break;
        }
        iter++;
    }
    return backhole;
}


SingletonAxis::SingletonAxis()
{
    if (g_pEquipAxiaspos == nullptr)
    {
        g_pEquipAxiaspos = new EquipmentAXIS_;
        QLOG_DEBUG() << "坐标结构体分配内存";
    }
}

SingletonAxis::~SingletonAxis()
{
    for(auto it = g_pEquipAxiaspos->reagentZoneAxispos.begin(); it != g_pEquipAxiaspos->reagentZoneAxispos.end();++it){
        if((*it) != nullptr){
            delete (*it);
            (*it) = nullptr;
        }
    }
    g_pEquipAxiaspos->reagentZoneAxispos.clear();

    qDeleteAll(g_pEquipAxiaspos->bloodSampleAxisPos.begin(),g_pEquipAxiaspos->bloodSampleAxisPos.end());
    g_pEquipAxiaspos->bloodSampleAxisPos.clear();

    qDeleteAll(g_pEquipAxiaspos->testTubeZoneAxisPos.begin(),g_pEquipAxiaspos->testTubeZoneAxisPos.end());
    g_pEquipAxiaspos->testTubeZoneAxisPos.clear();

    qDeleteAll(g_pEquipAxiaspos->pchnAxisPoint.begin(),g_pEquipAxiaspos->pchnAxisPoint.end());
    g_pEquipAxiaspos->pchnAxisPoint.clear();

    if(g_pEquipAxiaspos)
    {
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
EquipmentAXIS_ SingletonAxis::importFromCoordinateText(const QString& filePath)
{
    EquipmentAXIS_ equipment;
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

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // 跳过空行和注释行
        if (line.isEmpty() || line.startsWith("#")) {
            // 检查是否有节标题
            if (line.contains("Reagent Zone")) currentSection = "Reagent";
            else if (line.contains("Channel Coordinates")) currentSection = "Channel";
            else if (line.contains("Test Tube Zone")) currentSection = "TestTube";
            else if (line.contains("Blood Sample Zone")) currentSection = "Blood";
            continue;
        }

        try {
            if (currentSection == "Reagent") {
                QStringList fields = line.split(",");
                if (fields.size() < 4) {
                    throw std::runtime_error(QString("试剂区数据格式错误，行 %1").arg(lineNumber).toStdString());
                }

                REAGENTZONEAXIS_* reagent = new REAGENTZONEAXIS_;
                reagent->index = static_cast<quint8>(fields[0].toUInt());

                // 处理可能带引号的试剂名
                QString name = fields[1];
                if (name.startsWith('"') && name.endsWith('"')) {
                    name = name.mid(1, name.length() - 2);
                }
                reagent->reagname = name;

                reagent->Axispos.setX(fields[2].toInt());
                reagent->Axispos.setY(fields[3].toInt());
                equipment.reagentZoneAxispos.append(reagent);
            }
            else if (currentSection == "Channel") {
                QStringList fields = line.split(",");
                if (fields.size() < 4) {
                    throw std::runtime_error(QString("通道区数据格式错误，行 %1").arg(lineNumber).toStdString());
                }

                ChnAxis_* channel = new ChnAxis_;
                channel->indexChn = static_cast<quint8>(fields[0].toUInt());
                channel->offsetNeedle = static_cast<quint8>(fields[1].toUInt());
                channel->axisPos.setX(fields[2].toInt());
                channel->axisPos.setY(fields[3].toInt());
                equipment.pchnAxisPoint.append(channel);
            }
            else if (currentSection == "TestTube") {
                QStringList fields = line.split(",");
                if (fields.size() < 5) {
                    throw std::runtime_error(QString("试杯区数据格式错误，行 %1").arg(lineNumber).toStdString());
                }

                TRYTHECUPAXIS_* tube = new TRYTHECUPAXIS_;
                tube->numTube = static_cast<quint8>(fields[0].toUInt());
                tube->offsetNeedle = static_cast<quint8>(fields[1].toUInt());
                tube->indexTray = static_cast<quint8>(fields[2].toUInt());
                tube->axisPos.setX(fields[3].toInt());
                tube->axisPos.setY(fields[4].toInt());
                equipment.testTubeZoneAxisPos.append(tube);
            }
            else if (currentSection == "Blood") {
                QStringList fields = line.split(",");
                if (fields.size() < 3) {
                    throw std::runtime_error(QString("血样区数据格式错误，行 %1").arg(lineNumber).toStdString());
                }

                SAMPLEBLOODZONEAXISPOS_* blood = new SAMPLEBLOODZONEAXISPOS_;
                blood->index = static_cast<quint8>(fields[0].toUInt());
                blood->axisPos.setX(fields[1].toInt());
                blood->axisPos.setY(fields[2].toInt());
                equipment.bloodSampleAxisPos.append(blood);
            }
            else {
                if (line.startsWith("Equipment Type: ")) {
                    equipment.equipmentType = line.mid(16).trimmed();
                }
                else if (line.startsWith("Sync Status: ")) {
                    equipment.bsycnFinished = (line.mid(13).trimmed() == "Completed");
                }
                else if (line.startsWith("Origin Axis: ")) {
                    QString coords = line.mid(13).trimmed();
                    if (coords.startsWith("(") && coords.endsWith(")")) {
                        coords = coords.mid(1, coords.length()-2);
                        QStringList xy = coords.split(",");
                        if (xy.size() == 2) {
                            equipment.OriginAxis.setX(xy[0].toInt());
                            equipment.OriginAxis.setY(xy[1].toInt());
                        }
                    }
                }
                else if (line.startsWith("Equipment Index: ")) {
                    equipment.euqipmentIndex = static_cast<quint8>(line.mid(17).trimmed().toUInt());
                }
                else if (line.startsWith("Clean Zone (Reagent): ")) {
                    QString coords = line.mid(22).trimmed();
                    QStringList xy = coords.split(",");
                    if (xy.size() == 2) {
                        equipment.cleanZoneoffsetRegNedl.setX(xy[0].toInt());
                        equipment.cleanZoneoffsetRegNedl.setY(xy[1].toInt());
                    }
                }
                else if (line.startsWith("Clean Zone (Blood): ")) {
                    QString coords = line.mid(20).trimmed();
                    QStringList xy = coords.split(",");
                    if (xy.size() == 2) {
                        equipment.cleanZoneoffsetBlodNedl.setX(xy[0].toInt());
                        equipment.cleanZoneoffsetBlodNedl.setY(xy[1].toInt());
                    }
                }
                else if (line.startsWith("Throw Hole Axis: ")) {
                    QString coords = line.mid(17).trimmed();
                    QStringList xy = coords.split(",");
                    if (xy.size() == 2) {
                        equipment.ThrowHoleAxis.setX(xy[0].toInt());
                        equipment.ThrowHoleAxis.setY(xy[1].toInt());
                    }
                }
                else if (line.startsWith("Generated: ")) {
                    equipment.AxixsSaveTime = line.mid(11).trimmed();
                }
            }
        }
        catch (const std::exception& e) {
            file.close();
            throw std::runtime_error(QString("解析错误，行 %1: %2").arg(lineNumber).arg(e.what()).toStdString());
        }
    }

    file.close();
    return equipment;
}






//导入文件把坐标写入到机器
bool SingletonAxis::importtCoordinate(const QString filePath)
{
	QSettings *configIniRead = new QSettings(filePath, QSettings::IniFormat);
	quint8  equipmenttype = configIniRead->value("/equipmenttypede/kind").toInt();
	equipmentKind(WRITE_OPERAT, equipmenttype); //写入仪器类型

	//原点坐标
	QPoint originpos;
	originpos.setX(configIniRead->value("/orinigaxis/x").toInt());
	originpos.setY(configIniRead->value("/orinigaxis/y").toInt());
	originPos(WRITE_OPERAT, originpos);
    QLOG_DEBUG() << "配置文件写入仪器原点坐标" << originpos<<"=仪器类型"<< equipmenttype;

    //弃杯坐标
    QPoint throwpos;
    throwpos.setX(configIniRead->value("/throwholeaxis/x").toInt());
    throwpos.setY(configIniRead->value("/throwholeaxis/y").toInt());
    throwTubeHolePos(WRITE_OPERAT,throwpos);
    QLOG_DEBUG() << "配置文件写入仪器弃杯孔坐标" << throwpos;

    //血样针offset清洗区
    QPoint bloodpinoffsetcleanpos;
    bloodpinoffsetcleanpos.setX(configIniRead->value("/bloodpinoffsetclean/x").toInt());
    bloodpinoffsetcleanpos.setY(configIniRead->value("/bloodpinoffsetclean/y").toInt());
    cleanZoneAxisPos(WRITE_OPERAT,MOTOR_BLOOD_INDEX,bloodpinoffsetcleanpos);

    //试剂针offset清洗区
    QPoint reagpinoffsetcleanpos;
    reagpinoffsetcleanpos.setX(configIniRead->value("/reagpinoffsetclean/x").toInt());
    reagpinoffsetcleanpos.setY(configIniRead->value("/reagpinoffsetclean/y").toInt());
    cleanZoneAxisPos(WRITE_OPERAT,MOTOR_REAGNET_INDEX,reagpinoffsetcleanpos);

    //试剂针offset试剂
    QPoint firstholepos;
    firstholepos.setX(configIniRead->value("/reagpinoffsetreaglinque/x").toInt());
    firstholepos.setY(configIniRead->value("/reagpinoffsetreaglinque/y").toInt());
    QMap<quint8,QPoint> reagentallPos;
    reagentallPos.clear();
    QUIUtils::CreatReagArsOtherAxis(firstholepos,reagentallPos);
    auto it = reagentallPos.begin();
    while(it != reagentallPos.end())
    {
        reagetZoneAxisPos(WRITE_OPERAT,it.key(),it.value());
        it++;
    }

    //血样针offset 血样区
    QPoint firstbloodpos;
    firstbloodpos.setX(configIniRead->value("/bloodpinoffsetBloodpos/x").toInt());
    firstbloodpos.setY(configIniRead->value("/bloodpinoffsetBloodpos/y").toInt());
    QMap<quint8,QPoint> bloodSampleZone;
    bloodSampleZone.clear();
    QUIUtils::creatBloodSampleAxis(equipmenttype, firstbloodpos, bloodSampleZone);
    auto itmap = bloodSampleZone.begin();
    while(itmap != bloodSampleZone.end())
    {
        bloodSampleZonePos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }

    quint8 _totalchn,_toyaltray;
    switch(equipmenttype){
        case KS600:
            _totalchn = 4;
            _toyaltray = 2;
        break;
        case KS800:
            _totalchn = 8;
            _toyaltray = 3;
        break;
        case KS1200:
            _totalchn = 12;
            _toyaltray = 4;
        break;
        default:
            _totalchn = 12;
            _toyaltray = 4;
        break;
    }


    //血样针offset试管盘
    QMap<quint8,QPoint> bloodpinTrayhole;
    QPoint EmptyTubeFirstPos[4]={};
    for(int n = 0; n < _toyaltray; n++)
    {
         int _nhole = 60*n;
         QPoint bloodpinoffsetTraypos;
         QString out_key_x = QString("/bloodpinoffsettubehole%1/x").arg(_nhole);
         QString out_key_y = QString("/bloodpinoffsettubehole%1/y").arg(_nhole);
         bloodpinoffsetTraypos.setX(configIniRead->value(out_key_x).toInt());
         bloodpinoffsetTraypos.setY(configIniRead->value(out_key_y).toInt());
         EmptyTubeFirstPos[n] = bloodpinoffsetTraypos;
    }
    QUIUtils::creatTeatTubeAxiis(MOTOR_BLOOD_INDEX,equipmenttype,bloodpinTrayhole,EmptyTubeFirstPos,4);
    auto it_ = bloodpinTrayhole.begin();
    while(it_ != bloodpinTrayhole.end())
    {
        testTaryZoneAxisPos(WRITE_OPERAT,it_.key(),MOTOR_BLOOD_INDEX,it_.value());
        it_++;
    }

    //抓手offset试管盘
    QMap<quint8,QPoint> handsTrayhole;
    QPoint handstrayPos[4]={};
    for(int n = 0; n < _toyaltray; n++)
    {
         int _nhole = 60*n;
         QPoint HandsoffsetTraypos;
         QString out_key_x = QString("/handsoffsettubehole%1/x").arg(_nhole);
         QString out_key_y = QString("/handsoffsettubehole%1/y").arg(_nhole);
         HandsoffsetTraypos.setX(configIniRead->value(out_key_x).toInt());
         HandsoffsetTraypos.setY(configIniRead->value(out_key_y).toInt());
         handstrayPos[n] = HandsoffsetTraypos;
    }
    QUIUtils::creatTeatTubeAxiis(MOTOR_HANDS_INDEX,equipmenttype,handsTrayhole,handstrayPos,4);
    auto _iterhands = handsTrayhole.begin();
    while(_iterhands != handsTrayhole.end())
    {
        testTaryZoneAxisPos(WRITE_OPERAT,_iterhands.key(),MOTOR_HANDS_INDEX,_iterhands.value());
        _iterhands++;
    }

    //抓手offset通道
    for(int chn = 0; chn < _totalchn ;chn++){
        QPoint chnoffsethands;
        QString key_x = QString("/handsoffsetChn%1/x").arg(chn + 1);
        QString key_y = QString("/handsoffsetChn%1/y").arg(chn + 1);
        chnoffsethands.setX(configIniRead->value(key_x).toInt());
        chnoffsethands.setY(configIniRead->value(key_y).toInt());
        chnZoneAxisPos(WRITE_OPERAT,chn,MOTOR_HANDS_INDEX,chnoffsethands);
    }

    //试剂针offset通道
    for(int chn = 0; chn < _totalchn ;chn++){
        QPoint chnoffsetreagpin;
        QString key_x = QString("/reagpinoffsetChn%1/x").arg(chn + 1);
        QString key_y = QString("/reagpinoffsetChn%1/y").arg(chn + 1);
        chnoffsetreagpin.setX(configIniRead->value(key_x).toInt());
        chnoffsetreagpin.setY(configIniRead->value(key_y).toInt());
        chnZoneAxisPos(WRITE_OPERAT,chn,MOTOR_REAGNET_INDEX,chnoffsetreagpin);
    }

    delete configIniRead;
    QLOG_DEBUG()<<"未连接读取仪器坐标打开软件应用配置坐标";
    return true;
}


////////////////////////试剂耗材///////////////////////////////////////
ConsumablesOper *ConsumablesOper::GetpInstance()
{
    return g_pConsumable;
}

void ConsumablesOper::del_Instance()
{
    if(g_pConsumable)
    {
        delete g_pConsumable;
        g_pConsumable = nullptr;
    }
}

ConsumablesOper::ConsumablesOper()
{
    if (g_pVecReagentInfo == nullptr)
    {
        g_pVecReagentInfo = new equipmentConsumablesVec;
        QLOG_DEBUG() << "耗材结构体分配内存";
    }
}

ConsumablesOper::~ConsumablesOper()
{
    for(auto iter = g_pVecReagentInfo->begin();iter != g_pVecReagentInfo->end(); ++iter)
    {
        if(*iter != nullptr){
            delete (*iter);
            (*iter) = nullptr;
        }
    }
    g_pVecReagentInfo->clear();
    g_pVecReagentInfo->squeeze();
    if(g_pVecReagentInfo)
    {
        delete g_pVecReagentInfo;
        g_pVecReagentInfo = nullptr;
    }
}

//初始化耗材信息
void ConsumablesOper::creatReagentIndex(const quint8 index)
{
    quint8  Bottlenum = 0;
    quint16 BottleRatio = 0;
    quint8  BottleLimit = 0;
    quint16 BottleCapacity = 0;

    REAGENT_CONSUMABLES_ *pinitConsumables = new REAGENT_CONSUMABLES_;
    pinitConsumables->indexReag = index;
    pinitConsumables->remain_Ratio = BottleRatio;
    pinitConsumables->Expirationdate = "";
    pinitConsumables->remainingNum = Bottlenum; //剩余的总量 单位是瓶
    pinitConsumables->LimitAlarm = BottleLimit;
    pinitConsumables->SingleBottleCapacity = BottleCapacity;
    g_pVecReagentInfo->push_back(pinitConsumables);
    return;
}


void  ConsumablesOper::iterateOverEquipmentConsumables()
{
    for(int index_ = INDEX_AA_CONSUMABLE; index_ <= INDEX_RIS_1_CONSUMABLE; index_++){
        creatReagentIndex(index_);
    }
    return;
}


void ConsumablesOper::getSuppileAllowance(QMap<quint8,quint16> &AllowanceRatio)
{
    auto iter = g_pVecReagentInfo->begin();
    while(iter != g_pVecReagentInfo->end())
    {
        REAGENT_CONSUMABLES_ *psingleReagent = *iter;
        //if(psingleReagent->bSycnMemory == true)
        {
            AllowanceRatio.insert(psingleReagent->indexReag,psingleReagent->remain_Ratio);
        }
        iter++;
    }
}

void ConsumablesOper::getSuppileInteger(QMap<quint8,quint8> &AllowanceInteger)
{
    auto iter = g_pVecReagentInfo->constBegin();
    while(iter != g_pVecReagentInfo->constEnd())
    {
        REAGENT_CONSUMABLES_ *psingleReagent = *iter;
        AllowanceInteger.insert(psingleReagent->indexReag,psingleReagent->remainingNum);
        iter++;
    }
}

void ConsumablesOper::sycnTubeTotalTray(bool bwrite,quint8 &num_Bottle){
    if (g_pVecReagentInfo == nullptr) {
        QLOG_DEBUG() << "读写试杯盘总数[失败] 仪器耗材数据结构为NULL" << endl;
        return;
    }

    auto iter = std::find_if(g_pVecReagentInfo->begin(), g_pVecReagentInfo->end(),
                            [](REAGENT_CONSUMABLES_* reagent) {
                                return reagent->indexReag == INDEX_TESTTUBE_CONSUMABLE;
                            });
    if (iter != g_pVecReagentInfo->end()) {
            REAGENT_CONSUMABLES_* psingleReagent = *iter;
            if (bwrite) {
                psingleReagent->remainingNum = num_Bottle;  // 写入模式
            } else {
                num_Bottle = psingleReagent->remainingNum;  // 读取模式
            }
    } else {
        QLOG_DEBUG() << "未找到试杯盘耗材信息，请检查配置或更换耗材" << endl;  // 增强错误提示[[6]]
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
    if(g_pVecReagentInfo->isEmpty()) return;
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
    if(g_pVecReagentInfo == nullptr){
        QLOG_DEBUG()<<"更新试剂容量[失败] 仪器耗材数据结构为NULL"<<endl;
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
    if(minitPort == nullptr)
        minitPort = new QSerialPort(); //实例化串口类一个对象
	if (minitPort->isOpen()) //如果串口已经打开了 先给他关闭了
	{
		QLOG_WARN() << tr("串口已打开，关闭重新打开...");
		minitPort->clearError();
		minitPort->clear();
		minitPort->flush();
		minitPort->close();
	}
    bool _findedcom = false;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        int intHex_VID = info.vendorIdentifier();
        int intHex_PID = info.productIdentifier();
        if(intHex_VID == VID_NUM && intHex_PID == PID_NUM)
        {
            _findedcom = true;
			emit this->closetimercon(true);
            mserialname = info.portName();
            QLOG_INFO()<<QString("仪器串口名称:%1").arg(info.portName());//机器串口名
            openLoadSerialPort(info.portName());
            break;
        }
    }
   
    if(!_findedcom)
        emit closetimercon(false);
    return;
}

void loadEquipmentPos::openLoadSerialPort(const QString Portname)
{
    minitPort->setPortName(Portname);
    if(!minitPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        QLOG_ERROR()<<QString("仪器串口%1打开失败!").arg(Portname)<<endl;
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
	slotwritedataToEquip(getInstrumentType);
    QLOG_DEBUG()<<tr("第一次问连接成功,读取仪器类型命令...")<<getInstrumentType.toHex(' ').trimmed().toUpper()<<endl;
	return;
}

void loadEquipmentPos::receiveInfo()
{
    QByteArray recvedata;
    recvedata = minitPort->readAll();
    QString framedata  = recvedata.toHex(' ').trimmed().toUpper();
    QStringList dataList = framedata.split(" ");
    int size_ = dataList.size();
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

            _Parsing_received_messages(recv_data); //解析

        }
    }
    else
    {
        QLOG_ERROR()<<"收到数据长度异常数据"<<recvedata.toHex(' ').trimmed().toUpper()<<"长度"<<size_<<__FILE__<<__LINE__<<endl;
    }
    recvedata.clear();
    return;
}


void  loadEquipmentPos::_Parsing_received_messages(const QStringList ArryRecvdata)
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
        _completeddel(finished);
        ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(WRITE_OPERAT,indexReagent,lastBottle); //设置瓶数
        ConsumablesOper::GetpInstance()->updateReplaceLocRatio(WRITE_OPERAT,  indexReagent, OneBottleRatio); //写瓶内剩余
        QLOG_DEBUG()<<GlobalData::mapReagentNames(indexReagent)<<"瓶数"<<lastBottle<<"比"<<OneBottleRatio;
       _GroupReadParaCommder(nextSend);
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
        _completeddel(finished);
        ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(WRITE_OPERAT,indexReagent,lastBottle);
       _GroupReadParaCommder(nextSend);
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
        _completeddel(finished);
        ConsumablesOper::GetpInstance()->updateReplaceLocRatio(WRITE_OPERAT,
                                                               indexReagent,
                                                               OneBottleRatio);
       _GroupReadParaCommder(nextSend);
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
                _completeddel(MAINBORD_REAGENT_RIS_1_INFO);
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
        {REAGENT_CAPACITY, [&](const QStringList& data) { _recvReagentCapacity(data); }},
        {AXIS_ORIGIN_X, [&](const QStringList& data) { _recvOrininAxis(data); }},
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
        {BLOODPINPARAOTHERDATA, [&](const QStringList& data) { _recvBloodOtherdataAll(data); }},
        {PARAREAGENTPINDATA_I, [&](const QStringList& data) { _recvReagentData(data); }},
        {PARAREAGENTPINDATA_II, [&](const QStringList& data) { _recvReagentDataOther(data); }},
        {PARALIMINTBOTTLE, [&](const QStringList& data) { recveBottleLimit(data); }},
        {CONTROLGRIPPERPARA, [&](const QStringList& data) { recveNegativePressure(data); }}
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

    _GroupReadParaCommder(EQUIPMENTPARA_I); //开始读取参数

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
           slotwritedataToEquip(iter.value()->_writeparaorder); 
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


void loadEquipmentPos::_GroupReadParaCommder(quint8 index_)
{
    if(mwriteAxismap.contains(index_))
    {
        auto iter = mwriteAxismap.find(index_);
		if(iter.value()->readfinish == false)
			slotwritedataToEquip(iter.value()->_readparaorder);
    }
    return;
}

void loadEquipmentPos::_completeddel(quint8 index_)
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

        // 优化dimmLed处理
        quint16 dimmLed = extractUInt16(14, 13); // 使用辅助函数
        ini.wConfigPara(MODULETARGETBASICVAL, dimmLed);

        _completeddel(EQUIPMENTPARA_I);
        _GroupReadParaCommder(HANDSPARADATA_II);

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

        _completeddel(HANDSPARADATA_II);
        _GroupReadParaCommder(REAGENT_LIMIT);
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

        _completeddel(REAGENT_LIMIT);
        _GroupReadParaCommder(BlOODPINPARADATA);
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
          QLOG_DEBUG()<<"读吸样本单位量"<<q16suckPPPPRPmm;
          ini.SetLearnSamplevolume(q16suckPPPPRPmm);


          hexstr = hexArry[10] + hexArry[9];
          quint16 q16suckAirmm = hexstr.toUInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读吸样本前吸空气单位量"<<q16suckAirmm;
          ini._setsuckairsuckPRP(q16suckAirmm);


          hexstr = hexArry[14] + hexArry[13] + hexArry[12] + hexArry[11];
          quint32 washTime = hexstr.toInt(&ok,HEX_SWITCH);
          QLOG_DEBUG()<<"读取到清洗时间:"<<washTime<<endl;
          ini.setWashesTime(washTime);

         _completeddel(BlOODPINPARADATA);
         _GroupReadParaCommder(REAGENT_CAPACITY);
     }
     else
     {
         _writeFinish(BlOODPINPARADATA);
         _sendWriteAxisOrder(REAGENT_CAPACITY);
     }
     return;
}

void loadEquipmentPos::_recvReagentCapacity(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        quint16 AACapacity = QString("%1%2").arg(hexArry.at(6)).arg(hexArry.at(5)).toInt(nullptr,HEX_SWITCH);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_AA_CONSUMABLE,AACapacity);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_AA_1_CONSUMABLE,AACapacity);

        quint16 ADPCapacity = QString("%1%2").arg(hexArry.at(8)).arg(hexArry.at(7)).toInt(nullptr,HEX_SWITCH);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_ADP_CONSUMABLE,ADPCapacity);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_ADP_1_CONSUMABLE,ADPCapacity);

        quint16 EPICapacity = QString("%1%2").arg(hexArry.at(10)).arg(hexArry.at(9)).toInt(nullptr,HEX_SWITCH);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_EPI_CONSUMABLE,EPICapacity);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_EPI_1_CONSUMABLE,EPICapacity);

        quint16 COLCapacity = QString("%1%2").arg(hexArry.at(12)).arg(hexArry.at(11)).toInt(nullptr,HEX_SWITCH);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_COL_CONSUMABLE,COLCapacity);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_COL_1_CONSUMABLE,COLCapacity);

        quint16 RISCapacity = QString("%1%2").arg(hexArry.at(14)).arg(hexArry.at(13)).toInt(nullptr,HEX_SWITCH);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_RIS_CONSUMABLE,RISCapacity);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_RIS_1_CONSUMABLE,RISCapacity);
        QLOG_DEBUG()<<"读取到试剂总容量:"<<AACapacity<<"-"<<ADPCapacity<<"-"<<EPICapacity<<"-"<<COLCapacity<<"-"<<RISCapacity<<endl;
        _completeddel(REAGENT_CAPACITY);
        _GroupReadParaCommder(AXIS_ORIGIN_X);
    }
    else
    {
        _writeFinish(REAGENT_CAPACITY);
        _sendWriteAxisOrder(AXIS_ORIGIN_X);
    }
}


void loadEquipmentPos::_recvOrininAxis(const QStringList hexArry)
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

        _completeddel(AXIS_ORIGIN_X);
        _GroupReadParaCommder(AXIS_ORIGIN_Y);
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

        _completeddel(AXIS_ORIGIN_Y);
        _GroupReadParaCommder(AXIS_CHN1_5_X);
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

        _completeddel(AXIS_CHN1_5_X);
        _GroupReadParaCommder(AXIS_CHN1_5_Y);
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

        _completeddel(AXIS_CHN1_5_Y);
        _GroupReadParaCommder(AXIS_CHN6_10_X);
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


        _completeddel(AXIS_CHN6_10_X);
        _GroupReadParaCommder(AXIS_CHN6_10_Y);
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

        _completeddel(AXIS_CHN6_10_Y);
        _GroupReadParaCommder(AXIS_CHN11_12_X);
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

        _completeddel(AXIS_CHN11_12_X);
        _GroupReadParaCommder(AXIS_CHN11_12_Y);
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
        _completeddel(AXIS_CHN11_12_Y);
        _GroupReadParaCommder(AXIS_CHN4_8_HANDSX);
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
        _completeddel(AXIS_CHN4_8_HANDSX);
        _GroupReadParaCommder(AXIS_CHN4_8_HANDSY);
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
        _completeddel(AXIS_CHN4_8_HANDSY);
        _GroupReadParaCommder(AXIS_CHN9_12_HANDSX);
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
        _completeddel(AXIS_CHN9_12_HANDSX);
        _GroupReadParaCommder(AXIS_CHN9_12_HANDSY);
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

        _completeddel(AXIS_CHN9_12_HANDSY);
        _GroupReadParaCommder(AXIS_TRAY_OFFSET_BLOODPINX);
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

        _completeddel(AXIS_TRAY_OFFSET_BLOODPINX);
        _GroupReadParaCommder(AXIS_TRAY_OFFSET_BLOODPINY);
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

        _completeddel(AXIS_TRAY_OFFSET_BLOODPINY);
        _GroupReadParaCommder(AXIS_TRAY_OFFSET_HANDSX);
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

        _completeddel(AXIS_TRAY_OFFSET_HANDSX);
        _GroupReadParaCommder(AXIS_TRAY_OFFSET_HANDSY);
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

        _completeddel(AXIS_TRAY_OFFSET_HANDSY);
        _GroupReadParaCommder(BLOODPINPARAOTHERDATA);
    }
    else
    {
        _writeFinish(AXIS_TRAY_OFFSET_HANDSY);
        _sendWriteAxisOrder(BLOODPINPARAOTHERDATA);
    }
}
void loadEquipmentPos::_recvBloodOtherdataAll(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        double max_value = 400;
        static const QString basickey = "InstrumentParameters";
        const QString bloodpindownmmKey = QString("%1/%2").arg(basickey).arg("DownHighGrabTestCups");
        const QString PPPConversionScaleKey = QString("%1/%2").arg(basickey).arg("PPPConversionScale");
        const QString NullBackValueKey =    QString("%1/%2").arg(basickey).arg("NullBackValue");
        const QString PhysicalHeightKey = QString("%1/%2").arg(basickey).arg("PhysicalHeight");
        const QString CleanReagentDetectionDownHighKey = QString("%1/%2").arg(basickey).arg("CleanReagentDetectionDownHigh");
        const QString NeedleInPlasmaModeKey = QString("%1/%2").arg(basickey).arg("NeedleInPlasmaMode");
        const QString AnaemiaDetectionDownHighKey = QString("%1/%2").arg(basickey).arg("AnaemiaDetectionDownHigh");
        const QString PRPkey = QString("%1/%2").arg(basickey).arg("PRPConvertTheratioColumn");
        const QString testhightoffsetkey = QString("%1/%2").arg(basickey).arg("TestHeightDifference");

        bool ok;
        QString hexstr;
        hexstr = hexArry[5]; //血样针下降高度
        quint8 bloodNeedleDownHeigh = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,bloodpindownmmKey, bloodNeedleDownHeigh);

        hexstr = hexArry[6];//ppp样本系数
        quint8 PPPratio = hexstr.toUInt(&ok,HEX_SWITCH);
        double conversionScale = static_cast<double>((PPPratio/255.0) * max_value)/100.0; // 显式转换
        loadParaData(false,m_ParaFilePath,PPPConversionScaleKey, conversionScale);

        hexstr = hexArry[7];//空回
        quint8 NullBackValueValue = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,NullBackValueKey,NullBackValueValue);

        hexstr = hexArry[8];//针到基准物高度
        quint8 PhysicalHeightValue = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,PhysicalHeightKey,PhysicalHeightValue);

        hexstr = hexArry[9];//样本针探测清洗液失败下降高度
        quint8 CleanReagentDetectionDownHighValue = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,CleanReagentDetectionDownHighKey,CleanReagentDetectionDownHighValue);

        hexstr = hexArry[10];//血浆模式下降高度
        quint8 NeedleInPlasmaModeVal = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,NeedleInPlasmaModeKey,NeedleInPlasmaModeVal);

        hexstr = hexArry[11];//贫血探测失败下降高度
        quint8 PPPDetectionDownHigh = hexstr.toUInt(&ok,HEX_SWITCH);
        loadParaData(false,m_ParaFilePath,AnaemiaDetectionDownHighKey,PPPDetectionDownHigh);

        hexstr = hexArry[12];//prp样本系数
        quint8 Prpratio = hexstr.toUInt(&ok,HEX_SWITCH);
        double valPrpratio = static_cast<double>((Prpratio/255.0) * max_value) / 100.0;
        loadParaData(false,m_ParaFilePath,PRPkey, valPrpratio);

        hexstr = hexArry[13];
        quint8 testHeightoffset = hexstr.toInt(&ok,HEX_SWITCH);
        double valoffsetheigh = static_cast<double>(testHeightoffset) / 10.0;// 显式转换
        loadParaData(false,m_ParaFilePath,testhightoffsetkey, valoffsetheigh);//测高偏移读上来/10
        QLOG_DEBUG()<<"读取到测高偏移:"<<valoffsetheigh<<endl;

        _completeddel(BLOODPINPARAOTHERDATA);
        _GroupReadParaCommder(PARAREAGENTPINDATA_I);
    }
    else
    {
        _writeFinish(BLOODPINPARAOTHERDATA);
        _sendWriteAxisOrder(PARAREAGENTPINDATA_I);
    }
}
void loadEquipmentPos::_recvReagentData(const QStringList hexArry)
{
	quint8 n = 5;
	QMap<quint8, QString> KeyList;
	KeyList.clear();
    if(!m_bReadorWrite)
    {
        KeyList.insert(n,"InstrumentParameters/SuckReagent_AAvolume");
        n++;
        KeyList.insert(n,"InstrumentParameters/SuckReagent_ADPAvolume");
        n++;
        KeyList.insert(n,"InstrumentParameters/SuckReagent_EPIvolume");
        n++;
        KeyList.insert(n,"InstrumentParameters/SuckReagent_COLvolume");
        n++;
        KeyList.insert(n,"InstrumentParameters/SuckReagent_RISvolume");

        n++;
        KeyList.insert(n,"InstrumentParameters/SpitReagent_AA_downHigh");
        n++;
        KeyList.insert(n,"InstrumentParameters/SpitReagent_ADP_downHigh");
        n++;
        KeyList.insert(n,"InstrumentParameters/SpitReagent_EPI_downHigh");
        n++;
        KeyList.insert(n,"InstrumentParameters/SpitReagent_COL_downHigh");
        n++;
        KeyList.insert(n,"InstrumentParameters/SpitReagent_RIS_downHigh");

        auto iter = KeyList.constBegin();
        while(iter != KeyList.constEnd())
        {
            quint8 data_ =  QString("%1").arg(hexArry.at(iter.key())).toInt(nullptr,HEX_SWITCH);
            loadParaData(false,m_ParaFilePath,iter.value(),data_);
            iter++;
        }
        _completeddel(PARAREAGENTPINDATA_I);
        _GroupReadParaCommder(PARAREAGENTPINDATA_II);
    }
    else
	{
        _writeFinish(PARAREAGENTPINDATA_I);
        _sendWriteAxisOrder(PARAREAGENTPINDATA_II);
    }
}

void loadEquipmentPos::_recvReagentDataOther(const QStringList hexArry)
{
    if(!m_bReadorWrite)
    {
        struct ParamConfig {int index;QString key; std::function<QVariant(const QString&,bool*)> converter;};

        // 配置列表（索引从5开始）
        const QVector<ParamConfig> params = {
            {5,  "WashReagentNeedleAbsorbCleanning",
                [](const QString& v,bool *ok) { return QVariant(v.toUShort(ok)); }},
            {6,  "CleanReagentDetectionDownHigh_reagneedle",
                [](const QString& v,bool *ok) { return QVariant(v.toUShort(ok)); }},
            {7,  "ReagentDetectionDownHigh",
                [](const QString& v,bool *ok) { return QVariant(v.toUShort(ok)); }},
            {8,  "SuckReagent_AA_Ratio",
                [](const QString& v,bool *ok) { 
				double value = v.toDouble(ok);
                return QVariant(*ok ? value / 100.0 : QVariant());
			}},
			{ 9,  "SuckReagent_ADP_Ratio",
				[](const QString& v, bool* ok) {
				double value = v.toDouble(ok);
                return QVariant(*ok ? value / 100.0 : QVariant());
			} },
			{ 10, "SuckReagent_EPI_Ratio",
				[](const QString& v, bool* ok) {
				double value = v.toDouble(ok);
                return QVariant(*ok ? value / 100.0 : QVariant());
			} },
			{ 11, "SuckReagent_COL_Ratio",
				[](const QString& v, bool* ok) {
				double value = v.toDouble(ok);
                return QVariant(*ok ? value / 100.0 : QVariant());
			} },
			{ 12, "SuckReagent_RIS_Ratio",
				[](const QString& v, bool* ok) {
				double value = v.toDouble(ok);
                return QVariant(*ok ? value / 100.0 : QVariant());
			} }
        };

        // 统一处理参数加载
        for (const auto& param : params) {
            if (hexArry.size() <= param.index) {
                qWarning() << "Hex array index out of range:" << param.index;
                continue;
            }

            bool ok = false;
            QVariant value = param.converter(hexArry[param.index], &ok);
			if (!ok || value.isNull()) {
				qWarning() << "Invalid conversion at index" << param.index
					<< "value:" << hexArry[param.index];
				continue;
			}

            loadParaData(false,
                        m_ParaFilePath,
                        QString("InstrumentParameters/%1").arg(param.key),
                        value);
        }

        _completeddel(PARAREAGENTPINDATA_II);
        _GroupReadParaCommder(PARALIMINTBOTTLE);//发送写主板参数0x19
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
        _completeddel(PARALIMINTBOTTLE);

        //next0x1a== CONTROLGRIPPERPARA 抓手控制参数
        _GroupReadParaCommder(CONTROLGRIPPERPARA);
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

        _completeddel(CONTROLGRIPPERPARA);//接收解析完控制抓手参数
        _GroupReadParaCommder(MAINBORD_REAGENT_AA_INFO); //next开始读主板试剂信息

    }else{

        //0x1a写完配置
        _writeFinish(CONTROLGRIPPERPARA);

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
    _initwritereagentParaDataI(_ExitParaFile,_ParaPath);

    //初始化写主板试剂信息到仪器
    initwriteMainReagNum();

    emit this->progresstotal(m_axiswriteequipment.size());//写入总条数
    QLOG_DEBUG() <<"写配置条数"<< m_axiswriteequipment.size()<<endl;

	QByteArray writedata_;
	QUIUtils::WriteEquipmentType(index_, EQUIPMENT_TYPED, writedata_);
	slotwritedataToEquip(writedata_);
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
     switch (indexEquipment) {
         case KS600:
         case KS800:
         case KS1200:
             configAxisPoint(originAxis, 50, 24);
             break;
         default:
             break;
     }
     SingletonAxis::GetInstance()->originPos(WRITE_OPERAT, originAxis);
     return originAxis;
}

QPoint loadEquipmentPos::initCleanLinqueoffsetBloodPin(const quint8 &indexEquipment)
{
    QPoint cleanZoneoffsetBloodPin(0,0);//清洗液offset血样针
    switch(indexEquipment) {
        case KS600:  configAxisPoint(cleanZoneoffsetBloodPin,180,634);  break;
        case KS800:  configAxisPoint(cleanZoneoffsetBloodPin,180,634);  break;
        case KS1200: configAxisPoint(cleanZoneoffsetBloodPin,180,634);  break;
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
        case KS600:  configAxisPoint(cleanZoneoffsetKttsPin,170,850);  break;
        case KS800:  configAxisPoint(cleanZoneoffsetKttsPin,170,850);  break;
        case KS1200: configAxisPoint(cleanZoneoffsetKttsPin,170,850);  break;
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
        case KS600:  configAxisPoint(throwsTubePos,5245,340);  break;
        case KS800:  configAxisPoint(throwsTubePos,5245,340);  break;
        case KS1200: configAxisPoint(throwsTubePos,5245,340);  break;
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
        case KS600:  configAxisPoint(firstReagentZone,60,1347);  break;
        case KS800:  configAxisPoint(firstReagentZone,60,1347);  break;
        case KS1200: configAxisPoint(firstReagentZone,60,1347);  break;
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

    configAxisPoint(firstHoleAxis,879,330);
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
    int chnnum = 0;
    switch(indexEquipment)
    {
        case KS600: chnnum = 4; break;
        case KS800: chnnum = 8; break;
        case KS1200: chnnum = 12; break;
        default: break;
    }

    configAxisPoint(firstHoleAxis,879,97);
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
    configAxisPoint(firstBloodZone,893,2483);

    switch(indexEquipment)
    {
        case KS600:
            QUIUtils::creatBloodSampleAxis(KS600, firstBloodZone,BloodZoneAxis);
        break;
        case KS800:

            QUIUtils::creatBloodSampleAxis(KS800, firstBloodZone,BloodZoneAxis);
        break;
        case KS1200:
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

            configAxisPoint(firstAxispos[0],895,1059);
            configAxisPoint(firstAxispos[1],2097,1057);
        break;
        case KS800:
            configAxisPoint(firstAxispos[0],895,1059);
            configAxisPoint(firstAxispos[1],2097,1057);
            configAxisPoint(firstAxispos[2],3293,1055);
        break;
        case KS1200:
            configAxisPoint(firstAxispos[0],895,1059);
            configAxisPoint(firstAxispos[1],2097,1057);
            configAxisPoint(firstAxispos[2],3293,1055);
            configAxisPoint(firstAxispos[3],4493,1053);
        break;
        default:break;
    }

	//QLOG_DEBUG() << "发散设置抓手试管 头坐标" << firstAxispos[0] << firstAxispos[1] << firstAxispos[2] << firstAxispos[3];
	quint8 keyhole = 0;
	const uint spaceTube = 150; //内部试管横 纵向间距相同
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
            configAxisPoint(firstAxispos[0],895,580);
            configAxisPoint(firstAxispos[1],2097,580);
        break;
        case KS800:
            configAxisPoint(firstAxispos[0],895,580);
            configAxisPoint(firstAxispos[1],2097,580);
            configAxisPoint(firstAxispos[2],3295,570);
        break;
        case KS1200:
            configAxisPoint(firstAxispos[0],895,580);
            configAxisPoint(firstAxispos[1],2097,580);
            configAxisPoint(firstAxispos[2],3295,570);
            configAxisPoint(firstAxispos[3],4485,570);
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

    quint16 dimmLed = ini.rConfigPara(MODULETARGETBASICVAL).toInt();
    bool bScanBarcode = ini.rConfigPara(SCANCODEBAR).toBool();
    bool buseRightReagent = ini.rConfigPara(USINGASIDEREAGENTLOC).toBool();
    bool bbootCathCups = ini.rConfigPara(INITGRABCUPS).toBool();

    QByteArray sendOrder;
    QUIUtils::_writeParaNumIOrder(sendOrder,moduledata,channelstate,bScanBarcode,
                                  buseRightReagent,bbootCathCups,dimmLed);

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

void loadEquipmentPos::_initwritereagentParaDataI(bool _bexit,QString _path)
{
    quint8 ReagentDownPinmm[5] = {0};
    quint8 ReagentPinSuckum[5] = {0};
    quint8 otherReagentData[8] = {0};
    QString writeKey = "InstrumentParameters/SuckReagent_AAvolume";
    loadParaData(_bexit,_path,writeKey,ReagentPinSuckum[0]);

    writeKey = "InstrumentParameters/SuckReagent_ADPvolume";
    loadParaData(_bexit,_path,writeKey,ReagentPinSuckum[1]);

    writeKey = "InstrumentParameters/SuckReagent_EPIvolume";
    loadParaData(_bexit,_path,writeKey,ReagentPinSuckum[2]);

    writeKey = "InstrumentParameters/SuckReagent_COLvolume";
    loadParaData(_bexit,_path,writeKey,ReagentPinSuckum[3]);

    writeKey = "InstrumentParameters/SuckReagent_RISvolume";
    loadParaData(_bexit,_path,writeKey,ReagentPinSuckum[4]);

     writeKey = "InstrumentParameters/SpitReagent_AA_downHigh";
     loadParaData(_bexit,_path,writeKey,ReagentDownPinmm[0]);

     writeKey = "InstrumentParameters/SpitReagent_ADP_downHigh";
     loadParaData(_bexit,_path,writeKey,ReagentDownPinmm[1]);

     writeKey = "InstrumentParameters/SpitReagent_EPI_downHigh";
     loadParaData(_bexit,_path,writeKey,ReagentDownPinmm[2]);

     writeKey = "InstrumentParameters/SpitReagent_COL_downHigh";
     loadParaData(_bexit,_path,writeKey,ReagentDownPinmm[3]);

     writeKey = "InstrumentParameters/SpitReagent_RIS_downHigh";
     loadParaData(_bexit,_path,writeKey,ReagentDownPinmm[4]);

     writeKey = "InstrumentParameters/WashReagentNeedleAbsorbCleanning";
     loadParaData(_bexit,_path,writeKey,otherReagentData[0]); //洗试剂针吸清洗液量

     writeKey = "InstrumentParameters/CleanReagentDetectionDownHigh_reagneedle";
     loadParaData(_bexit,_path,writeKey,otherReagentData[1]);  //试剂针清洗液探测失败高度

     writeKey = "InstrumentParameters/ReagentDetectionDownHigh";
     loadParaData(_bexit,_path,writeKey,otherReagentData[2]); //试剂针探测试剂失败高度

     double suckReagentRatio = 0.00;
     writeKey = "InstrumentParameters/SuckReagent_AA_Ratio";
     loadParaData(_bexit,_path,writeKey, suckReagentRatio );
     otherReagentData[3] = suckReagentRatio*100.0;

     writeKey = "InstrumentParameters/SuckReagent_ADP_Ratio";
     loadParaData(_bexit,_path,writeKey,suckReagentRatio);
     otherReagentData[4] = suckReagentRatio*100.0;

     writeKey = "InstrumentParameters/SuckReagent_EPI_Ratio";
     loadParaData(_bexit,_path,writeKey,suckReagentRatio);
     otherReagentData[5] = suckReagentRatio*100.0;

     writeKey = "InstrumentParameters/SuckReagent_COL_Ratio";
     loadParaData(_bexit,_path,writeKey,suckReagentRatio);
     otherReagentData[6] = suckReagentRatio*100.0;

     writeKey = "InstrumentParameters/SuckReagent_RIS_Ratio";
     loadParaData(_bexit,_path,writeKey,suckReagentRatio);
     otherReagentData[7] = suckReagentRatio*100.0;

     QByteArray sendOrder,sendOtherOrder;
     QUIUtils::_writeReagPinParaDataOrder(sendOrder,sendOtherOrder,ReagentPinSuckum,
                                          ReagentDownPinmm,otherReagentData);

     WRITEPARAMENTER* pPara = new WRITEPARAMENTER;
     pPara->_writeparaorder = sendOrder;
     pPara->_writefinish = false;
     m_axiswriteequipment.insert(PARAREAGENTPINDATA_I, pPara);

     WRITEPARAMENTER* potherPara = new WRITEPARAMENTER;
     potherPara->_writeparaorder = sendOtherOrder;
     potherPara->_writefinish = false;
     m_axiswriteequipment.insert(PARAREAGENTPINDATA_II, potherPara);



     //写入试剂整体限位
     QByteArray sendoutArry = initWriteParaintoEquipment();
     WRITEPARAMENTER *pBottleLimit = new WRITEPARAMENTER;
     pBottleLimit->_writeparaorder = sendoutArry;
     pBottleLimit->_writefinish = false;
     m_axiswriteequipment.insert(PARALIMINTBOTTLE,pBottleLimit);

     //写入抓手参数
     QByteArray gripperParaData = initWriteGripperPara();
     WRITEPARAMENTER* pgripperPara = new WRITEPARAMENTER;
     pgripperPara->_writeparaorder = gripperParaData;
     pgripperPara->_writefinish = false;
     m_axiswriteequipment.insert(CONTROLGRIPPERPARA, pgripperPara);

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
    return QUIUtils::writeGripperParaDataArry(Lessthanmax,bigthanmin,suckTime,true);
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


void loadEquipmentPos::slotwritedataToEquip(const QByteArray arry)
{
    if (!minitPort)  return;
    quint64 len = minitPort->write(arry);

    if(len != PROTOCOL_LENGTH)
        QLOG_ERROR()<<"发送命令失败"<<arry.toHex(' ').trimmed().toUpper()<<endl;

    return;
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
	if (minitPort)
	{
		if (minitPort->isOpen())
			minitPort->close();
		delete minitPort;
		minitPort = nullptr;
	}
    qDeleteAll(m_axiswriteequipment);
    m_axiswriteequipment.clear();


    qDeleteAll(mwriteAxismap);
    mwriteAxismap.clear();

}
