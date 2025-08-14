#include "movements.h"
#include <QBitArray>
#include <cglobal.h>
#include <QsLog/include/QsLog.h>

movements::movements(QObject *parent) : QObject(parent)
{
    if(!mMotorDataStu)
        mMotorDataStu = new  MotorPkg;
}

movements::~movements()
{
    if(mMotorDataStu)
        delete mMotorDataStu;
    mMotorDataStu = nullptr;
}


static QByteArray bitsToBytes(const QBitArray &bits)
{
    QByteArray bytes;
    bytes.resize(bits.count() / 8 + ((bits.count() % 8)? 1: 0));
    bytes.fill(0x00);
    for (int b = 0; b < bits.count(); ++b)
        bytes[b / 8] = ( bytes.at(b / 8) | ((bits[b] ? 1: 0) << (7 - (b % 8))));
    return bytes;
}

static QByteArray intSwitchToBytes(int leng,int number)
{
    QByteArray abyte0;
    abyte0.resize(leng);
    switch(leng)
    {
        case 1:
            abyte0[0] = (uchar) (0x000000ff & number);
        break;
        case 2:
            abyte0[0] = (uchar) (0x000000ff & number);
            abyte0[1] = (uchar) ((0x000000ff & number) >> 8);
        break;
        case 4:
            abyte0[0] = (uchar) (0x000000ff & number);
            abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
            abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
            abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
        break;
    default:break;
    }
    return abyte0;
}


void movements::ControlsAxisMovement(quint8 &cmdnum, quint8 buffsize, QPoint xyMotorMovedLoc,QByteArray &outputArry)
{
    mMotorDataStu->slavea_ddr = MAIN_CONTROL;
    mMotorDataStu->fucn_code =  MOTOR_FUNCTIONCODE;
    mMotorDataStu->cmd_num = cmdnum;
    cmdnum++;
    mMotorDataStu->cmd_size = buffsize;

    mMotorDataStu->RunMotorMode.type = 0;  //命令写
    mMotorDataStu->RunMotorMode.cmd_status = 0; //等待
    mMotorDataStu->RunMotorMode.operation_mode = 0; //位置模式
    mMotorDataStu->RunMotorMode.axis = 4; //xy

    mMotorDataStu->controlMotorByt.sportAsix = 0; //相对位置
    mMotorDataStu->controlMotorByt.indexArea = 0;
    mMotorDataStu->controlMotorByt.gipperState = 0;
    mMotorDataStu->controlMotorByt.downMode = 0;
    mMotorDataStu->controlMotorByt.statePump = 0;
    mMotorDataStu->controlMotorByt.zAxisState = 0;

    mMotorDataStu->MotorRangex = xyMotorMovedLoc.x();
    mMotorDataStu->MotorRangey = xyMotorMovedLoc.y();
    mMotorDataStu->Fault = 0;

    outputArry.clear();
    creatSendCode(outputArry);
    QLOG_DEBUG()<<"XYLoc:["<<outputArry.toHex(' ').trimmed().toUpper()<<"]"<<endl;
    return;
}

void  movements::ControlBloodNeedlesMovement(quint8 &cmdnum, quint8 buffsize,int MovedLoc,
                                               bool LocMode,QByteArray &outputArry)
{
    int downMode = 0,speedMode = 0;
    mMotorDataStu->slavea_ddr = Z_AXIS;
    mMotorDataStu->fucn_code =  MOTOR_FUNCTIONCODE;
    mMotorDataStu->cmd_num = cmdnum;
    cmdnum++;
    mMotorDataStu->cmd_size = buffsize;

    mMotorDataStu->RunMotorMode.type = 0;  //命令写
    mMotorDataStu->RunMotorMode.cmd_status = 0; //等待
    (LocMode)? speedMode = 0: speedMode = 1;
    mMotorDataStu->RunMotorMode.operation_mode = speedMode; //位置模式
    mMotorDataStu->RunMotorMode.axis = MOTOR_BLOOD_INDEX; //血样针


    mMotorDataStu->controlMotorByt.sportAsix = 0; //相对位置
    mMotorDataStu->controlMotorByt.indexArea = 0;
    mMotorDataStu->controlMotorByt.gipperState = 0;
    (LocMode)? downMode = 0 : downMode = 1;
    mMotorDataStu->controlMotorByt.downMode = downMode;
    mMotorDataStu->controlMotorByt.statePump = 0;
    mMotorDataStu->controlMotorByt.zAxisState = 0;

    mMotorDataStu->MotorRangex = MovedLoc; //液面探测失败高度
    mMotorDataStu->MotorRangey = 0;
    mMotorDataStu->Fault = 0;

    outputArry.clear();
    creatSendCode(outputArry);
    QLOG_DEBUG()<<"血样针Loc:["<<outputArry.toHex(' ').trimmed().toUpper()<<"]"<<endl;
    return;
}

void  movements::ControlBloodreposition(quint8 &cmdnum, quint8 buffsize,int MovedSpeed,QByteArray &outputArry)
{
    mMotorDataStu->slavea_ddr = Z_AXIS;
    mMotorDataStu->fucn_code =  MOTOR_FUNCTIONCODE;
    mMotorDataStu->cmd_num = cmdnum;
    cmdnum++;
    mMotorDataStu->cmd_size = buffsize;

    mMotorDataStu->RunMotorMode.type = 0;  //命令写
    mMotorDataStu->RunMotorMode.cmd_status = 0; //等待
    mMotorDataStu->RunMotorMode.operation_mode = 2; //负速度模式复位
    mMotorDataStu->RunMotorMode.axis = MOTOR_BLOOD_INDEX; //血样针

    mMotorDataStu->controlMotorByt.sportAsix = 0; //相对位置
    mMotorDataStu->controlMotorByt.indexArea = 0;
    mMotorDataStu->controlMotorByt.gipperState = 0;
    mMotorDataStu->controlMotorByt.downMode = 0;
    mMotorDataStu->controlMotorByt.statePump = 0;
    mMotorDataStu->controlMotorByt.zAxisState = 0;

    mMotorDataStu->MotorRangex = MovedSpeed; //复位速度
    mMotorDataStu->MotorRangey = 0;
    mMotorDataStu->Fault = 0;

    outputArry.clear();
    creatSendCode(outputArry);
    QLOG_DEBUG()<<"血样针复位Loc:["<<outputArry.toHex(' ').trimmed().toUpper()<<"]"<<endl;
    return;
}



void  movements::ControlReagentNeedlesMovement(quint8 &cmdnum, quint8 buffsize,int MovedLoc,
                                                  bool LocMode,QByteArray &outputArry)
{
    int downMode = 0,speedMode = 0;
    mMotorDataStu->slavea_ddr = Z_AXIS;
    mMotorDataStu->fucn_code =  MOTOR_FUNCTIONCODE;
    mMotorDataStu->cmd_num = cmdnum;
    cmdnum++;
    mMotorDataStu->cmd_size = buffsize;

    mMotorDataStu->RunMotorMode.type = 0;  //命令写
    mMotorDataStu->RunMotorMode.cmd_status = 0; //等待
    (LocMode)? speedMode = 0: speedMode = 1;
    mMotorDataStu->RunMotorMode.operation_mode = speedMode; //位置模式
    mMotorDataStu->RunMotorMode.axis = MOTOR_REAGNET_INDEX; //试剂针

    mMotorDataStu->controlMotorByt.sportAsix = 0; //相对位置
    mMotorDataStu->controlMotorByt.indexArea = 0;
    mMotorDataStu->controlMotorByt.gipperState = 0;
    (LocMode)? downMode = 0 : downMode = 1;
    mMotorDataStu->controlMotorByt.downMode = downMode;
    mMotorDataStu->controlMotorByt.statePump = 0;
    mMotorDataStu->controlMotorByt.zAxisState = 0;

    mMotorDataStu->MotorRangex = MovedLoc; //液面探测失败高度、下降高度
    mMotorDataStu->MotorRangey = 0;
    mMotorDataStu->Fault = 0;

    outputArry.clear();
    creatSendCode(outputArry);
    QLOG_DEBUG()<<"试剂针Loc:["<<outputArry.toHex(' ').trimmed().toUpper()<<"]"<<endl;
    return;
}


void movements::ControlTongsMovement(quint8 &cmdnum, quint8 buffsize,int MovedLoc,bool GrabCup,QByteArray &outputArry)
{
    int GrabtheCups = 0;
    mMotorDataStu->slavea_ddr = Z_AXIS;
    mMotorDataStu->fucn_code =  MOTOR_FUNCTIONCODE;
    mMotorDataStu->cmd_num = cmdnum;
    cmdnum++;
    mMotorDataStu->cmd_size = buffsize;

    mMotorDataStu->RunMotorMode.type = 0;  //命令写
    mMotorDataStu->RunMotorMode.cmd_status = 0; //等待
    mMotorDataStu->RunMotorMode.operation_mode = 0; //位置模式
    mMotorDataStu->RunMotorMode.axis = MOTOR_HANDS_INDEX; //抓手

    mMotorDataStu->controlMotorByt.sportAsix = 0; //相对位置
    mMotorDataStu->controlMotorByt.indexArea = 0;
    (!GrabCup)? GrabtheCups = 0 : GrabtheCups = 1;
    mMotorDataStu->controlMotorByt.gipperState = GrabtheCups;

    mMotorDataStu->controlMotorByt.downMode = 0;
    mMotorDataStu->controlMotorByt.statePump = 0;
    mMotorDataStu->controlMotorByt.zAxisState = 0;

    mMotorDataStu->MotorRangex = MovedLoc; //液面探测失败高度、下降高度
    mMotorDataStu->MotorRangey = 0;
    mMotorDataStu->Fault = 0;

    outputArry.clear();
    creatSendCode(outputArry);
    QLOG_DEBUG()<<"抓手Loc:["<<outputArry.toHex(' ').trimmed().toUpper()<<"]"<<endl;
    return;
}


QByteArray movements::Convert4thbytes()
{
    int totalBit = 8;
    int length = 1;
    QBitArray controlByte;
    controlByte.resize(totalBit);
    controlByte.fill(false);

    controlByte.setBit(7,mMotorDataStu->RunMotorMode.type);
    length = 2;
    QString tmpStr = QString("%1").arg(mMotorDataStu->RunMotorMode.cmd_status,length ,2, QLatin1Char('0'));
    controlByte.setBit(5,tmpStr.at(0).toLatin1());
    controlByte.setBit(6,tmpStr.at(1).toLatin1());

    length = 2;
    tmpStr = QString("%1").arg(mMotorDataStu->RunMotorMode.operation_mode,length ,2, QLatin1Char('0'));
    controlByte.setBit(3,tmpStr.at(0).toLatin1());
    controlByte.setBit(4,tmpStr.at(1).toLatin1());

    length = 3;
    tmpStr = QString("%1").arg(mMotorDataStu->RunMotorMode.axis,length ,2, QLatin1Char('0'));
    controlByte.setBit(0,tmpStr.at(0).toLatin1());
    controlByte.setBit(1,tmpStr.at(1).toLatin1());
    controlByte.setBit(2,tmpStr.at(2).toLatin1());
    QByteArray byte4th = bitsToBytes(controlByte);
    return byte4th;
}
QByteArray movements::Convert5thbytes()
{
    int totalBit = 8;
    int length = 0;
    QBitArray controlByte;
    controlByte.resize(totalBit);
    controlByte.fill(false);

    controlByte.setBit(7,mMotorDataStu->controlMotorByt.zAxisState);
    controlByte.setBit(6,mMotorDataStu->controlMotorByt.statePump);
    controlByte.setBit(5,mMotorDataStu->controlMotorByt.downMode);
    controlByte.setBit(4,mMotorDataStu->controlMotorByt.gipperState);
    length = 2;
    QString bitStr =  QString("%1").arg(mMotorDataStu->controlMotorByt.indexArea,length ,2, QLatin1Char('0'));
    controlByte.setBit(2,bitStr.at(0).toLatin1());
    controlByte.setBit(3,bitStr.at(1).toLatin1());

    bitStr =  QString("%1").arg(mMotorDataStu->controlMotorByt.sportAsix,length ,2, QLatin1Char('0'));
    controlByte.setBit(0,bitStr.at(0).toLatin1());
    controlByte.setBit(1,bitStr.at(1).toLatin1());
	QByteArray byte5th = bitsToBytes(controlByte);
    return byte5th;
}
void movements::creatSendCode(QByteArray &datasend)
{
    int Length = 1;
    int index = 0;
    datasend.resize(15);
    datasend.fill(0);

    QByteArray  monomial = intSwitchToBytes(Length,mMotorDataStu->slavea_ddr);
    datasend.replace(index,Length,monomial);
    index++;

    monomial = intSwitchToBytes(Length,mMotorDataStu->fucn_code);
    datasend.replace(index,Length,monomial);
    index++;

    monomial = intSwitchToBytes(Length,mMotorDataStu->cmd_num);
    datasend.replace(index,Length,monomial);
    index++;

    monomial = intSwitchToBytes(Length,mMotorDataStu->cmd_size);
    datasend.replace(index,Length,monomial);
    index++;

    monomial = Convert4thbytes();
    datasend.replace(index,Length,monomial);
    index++;

    monomial = Convert5thbytes();
    datasend.replace(index,Length,monomial);
    index++;

    Length = 4;
    monomial = intSwitchToBytes(Length,mMotorDataStu->MotorRangex);
    datasend.replace(index,Length,monomial);
    index = index + Length;

    monomial = intSwitchToBytes(Length,mMotorDataStu->MotorRangey);
    datasend.replace(index,Length,monomial);
    index = index + Length;

    Length = 1;
    monomial = intSwitchToBytes(Length,mMotorDataStu->Fault);
    datasend.replace(index,Length,monomial);
    return;
}


