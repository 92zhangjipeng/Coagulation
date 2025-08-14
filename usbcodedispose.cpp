#pragma execution_character_set("utf-8")

#include "usbcodedispose.h"
#include <QDebug>
#include <QBitArray>
#include <QString>
#include <QPoint>
#include <QDataStream>

UsbCodeDispose::UsbCodeDispose()
{


    StuConfig = (struct mainControl_FourByte*)malloc(sizeof(struct mainControl_FourByte)); //申请一个空间，把该空间地址给
    qDebug()<<"4BYTE长度"<<sizeof(*StuConfig);

    StuConfigFive = (struct mainControl_FiveByte*)malloc(sizeof(struct mainControl_FiveByte));
    StuReagentInfo = (struct MainBord_Reagent*)malloc(sizeof(struct MainBord_Reagent));

    Strut_MainBoard = (struct DisposeParameter*)malloc(sizeof(struct DisposeParameter));
    qDebug()<<"主板长度"<<sizeof(*Strut_MainBoard);

    Struct_bitMotor = (struct ReportMotor*)malloc(sizeof(struct ReportMotor));

    Struct_Motor = (struct Motor_Control*)malloc(sizeof(struct Motor_Control));

    StructTestmodeBit = (struct TestModeBityStruct*)malloc(sizeof(struct TestModeBityStruct));

    StructTestmode = (struct TestModeStruct*)malloc(sizeof(struct TestModeStruct));

}

UsbCodeDispose::~UsbCodeDispose()
{
    free(StuConfig);
    free(StuConfigFive);
    free(StuReagentInfo);
    free(Strut_MainBoard);
    free(Struct_bitMotor);
    free(Struct_Motor);
    free(StructTestmodeBit);
    free(StructTestmode);
}

char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
    return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
    return ch-'a'+10;
    else return (-1);
}

QByteArray QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i = 0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
        break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}


QByteArray intToByte(int leng,int number)
{
    QByteArray abyte0;

    abyte0.resize(leng);

    if(leng == 1)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
    }
    else if(leng == 2)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x000000ff & number) >> 8);
    }
    else if(leng == 4)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
        abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
        abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    }

    return abyte0;
}

uint BitArrayInvert(QBitArray array)
{
    uint value = 0;

    for(int i =0 ; i<array.size();++i)
    {
        value <<= 1;

        value += (int)array.at(i);
    }

    return value;
}
int byteAraryToInt(QByteArray arr,  Endian endian = LittileEndian)
{
    if (arr.size() < 4)
        return 0;

    int res = 0;

    // 小端模式
    if (endian == LittileEndian)
    {
        res = arr.at(0) & 0x000000FF;
        res |= (arr.at(1) << 8) & 0x0000FF00;
        res |= (arr.at(2) << 16) & 0x00FF0000;
        res |= (arr.at(3) << 24) & 0xFF000000;
    }

    // 大端模式
    else if (endian == BigEndian)
    {
        res = (arr.at(0) << 24) & 0xFF000000;
        res |= (arr.at(1) << 16) & 0x00FF0000;
        res |= arr.at(2) << 8 & 0x0000FF00;
        res |= arr.at(3) & 0x000000FF;
    }
    return res;
}

int bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

//////////////////////////////  主板信息 /////////////////////////////////////
/// \brief UsbCodeDispose::_ReadorWriteMainBoard
/// \param Write
/// \param Type_Reagent
/// \param Reagent_remain
/// \return
///
QByteArray UsbCodeDispose::Group_MainboardOrder(uint Byet_mode4,uint Byet_mode5,uint Byet_mode8, unsigned short Value)
{
    QByteArray MainBordOrder;
    MainBordOrder.resize(SizeofMainBord);
    MainBordOrder.fill(0);

    Strut_MainBoard->slave_addr = 0x05;         //从机地址
    Strut_MainBoard->func_code =  0x16;          //功能码
    Strut_MainBoard->cmd_num =    0x00;          //命令编码
    Strut_MainBoard->cmd_size =   0x00;         //剩余缓存命令条数
    Strut_MainBoard->temperature_rep = 0;

    Strut_MainBoard->Control_Para4 = Byet_mode4;
    Strut_MainBoard->Control_Para5 = Byet_mode5;
    Strut_MainBoard->Control_Para8 = Byet_mode8;

    Strut_MainBoard->Reagent_remain = Value;

    int length = 1;
    QByteArray MainBoard_snap;
    MainBoard_snap.clear();

    MainBoard_snap  = intToByte(length,Strut_MainBoard->slave_addr);  //从机地址
    MainBordOrder.replace(0,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->func_code);  //功能码
    MainBordOrder.replace(1,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->cmd_num);    //命令编号
    MainBordOrder.replace(2,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->cmd_size);  //缓存命令
    MainBordOrder.replace(3,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->Control_Para4);
    MainBordOrder.replace(4,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->Control_Para5);
    MainBordOrder.replace(5,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(2,Strut_MainBoard->temperature_rep);
    MainBordOrder.replace(6,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(length,Strut_MainBoard->Control_Para8);
    MainBordOrder.replace(8,MainBoard_snap.size(),MainBoard_snap);

    MainBoard_snap  = intToByte(4,Strut_MainBoard->date_Reagent);
    MainBordOrder.replace(9,MainBoard_snap.size(),MainBoard_snap);


    int len_intVar = sizeof(Strut_MainBoard->Reagent_remain );
    MainBoard_snap.resize(len_intVar);
    memcpy(MainBoard_snap.data(), &Strut_MainBoard->Reagent_remain, len_intVar);
    MainBordOrder.replace(13,len_intVar,MainBoard_snap);

    return MainBordOrder;
}
QByteArray UsbCodeDispose::_ReadorWriteMainBoard(bool Write, quint8 Type_Reagent, unsigned short Reagent_remain)
{
    QBitArray mainboard_1byte;
    mainboard_1byte.resize(Max_Bit);
    mainboard_1byte.fill(false);

    QBitArray mainboard_2byte;
    mainboard_2byte.resize(Max_Bit);
    mainboard_2byte.fill(false);

    QBitArray mainboard_3byte;
    mainboard_3byte.resize(Max_Bit);
    mainboard_3byte.fill(false);

    if(Write)
    {
        //试剂种类
        StuReagentInfo->reagetn_type = Type_Reagent;
        QString Bit0,Bit1,Bit2;
        QString byte = QString("%1").arg(StuReagentInfo->reagetn_type,3 ,2, QLatin1Char('0'));
        Bit0 = byte.at(0);
        Bit1 = byte.at(1);
        Bit2 = byte.at(2);
        mainboard_3byte.setBit(Bit_5,Bit0.toUInt());
        mainboard_3byte.setBit(Bit_6,Bit1.toUInt());
        mainboard_3byte.setBit(Bit_7,Bit2.toUInt());
        //Strut_MainBoard->Reagent_remain = Reagent_remain;
    }
    else
    {
       StuConfig->type  = true;
       mainboard_1byte.setBit(Bit_7,StuConfig->type);
    }

    uint Byet_mode4 = BitArrayInvert(mainboard_1byte);
    uint Byet_mode5 = BitArrayInvert(mainboard_2byte);
    uint Byet_mode8 = BitArrayInvert(mainboard_3byte);

    return Group_MainboardOrder(Byet_mode4,Byet_mode5,Byet_mode8,Reagent_remain);
}

QByteArray UsbCodeDispose::_GetReagentTypecapacity(quint8 type)
{
    QBitArray mainboard_1byte;
    mainboard_1byte.resize(Max_Bit);
    mainboard_1byte.fill(false);

    QBitArray mainboard_2byte;
    mainboard_2byte.resize(Max_Bit);
    mainboard_2byte.fill(false);

    QBitArray mainboard_3byte;
    mainboard_3byte.resize(Max_Bit);
    mainboard_3byte.fill(false);

    StuConfig->type  = true;
    mainboard_1byte.setBit(Bit_7,StuConfig->type);

    QString byte = QString("%1").arg(type,4 ,2, QLatin1Char('0'));
    QString Bit0 = byte.at(0);
    QString Bit1 = byte.at(1);
    QString Bit2 = byte.at(2);
    QString Bit3 = byte.at(3);
    mainboard_3byte.setBit(Bit_4,Bit0.toUInt());
    mainboard_3byte.setBit(Bit_5,Bit1.toUInt());
    mainboard_3byte.setBit(Bit_6,Bit2.toUInt());
    mainboard_3byte.setBit(Bit_7,Bit3.toUInt());
    uint Byet_mode4 = BitArrayInvert(mainboard_1byte);
    uint Byet_mode5 = BitArrayInvert(mainboard_2byte);
    uint Byet_mode8 = BitArrayInvert(mainboard_3byte);

    return Group_MainboardOrder(Byet_mode4,Byet_mode5,Byet_mode8,0);
}

QByteArray UsbCodeDispose::SetConsumableRemaining(int State, quint8 Types, unsigned short Caption)
{
    QBitArray mainboard_1byte;
    mainboard_1byte.resize(Max_Bit);
    mainboard_1byte.fill(false);

    QBitArray mainboard_2byte;
    mainboard_2byte.resize(Max_Bit);
    mainboard_2byte.fill(false);

    QBitArray mainboard_3byte;
    mainboard_3byte.resize(Max_Bit);
    mainboard_3byte.fill(false);

    QString byte = QString("%1").arg(Types,4 ,2, QLatin1Char('0'));
    QString Bit0 = byte.at(0);
    QString Bit1 = byte.at(1);
    QString Bit2 = byte.at(2);
    QString Bit3 = byte.at(3);
    mainboard_3byte.setBit(Bit_4,Bit0.toUInt());
    mainboard_3byte.setBit(Bit_5,Bit1.toUInt());
    mainboard_3byte.setBit(Bit_6,Bit2.toUInt());
    mainboard_3byte.setBit(Bit_7,Bit3.toUInt());
    QString statebit = QString("%1").arg(State,4 ,2, QLatin1Char('0'));
    Bit0 = statebit.at(0);
    Bit1 = statebit.at(1);
    Bit2 = statebit.at(2);
    Bit3 = statebit.at(3);
    mainboard_3byte.setBit(Bit_0,Bit0.toUInt());
    mainboard_3byte.setBit(Bit_1,Bit1.toUInt());
    mainboard_3byte.setBit(Bit_2,Bit2.toUInt());
    mainboard_3byte.setBit(Bit_3,Bit3.toUInt());

    uint Byet_mode4 = BitArrayInvert(mainboard_1byte);
    uint Byet_mode5 = BitArrayInvert(mainboard_2byte);
    uint Byet_mode8 = BitArrayInvert(mainboard_3byte);

    return Group_MainboardOrder(Byet_mode4,Byet_mode5,Byet_mode8,Caption);
}


QByteArray UsbCodeDispose::_buzzerControl(bool open)
{
    QBitArray mainboard_1byte;
    mainboard_1byte.resize(Max_Bit);
    mainboard_1byte.fill(false);

    QBitArray mainboard_2byte;
    mainboard_2byte.resize(Max_Bit);
    mainboard_2byte.fill(false);

    QBitArray mainboard_3byte;
    mainboard_3byte.resize(Max_Bit);
    mainboard_3byte.fill(false);

    mainboard_2byte.setBit(Bit_5,open);

    uint Byet_mode4 = BitArrayInvert(mainboard_1byte);
    uint Byet_mode5 = BitArrayInvert(mainboard_2byte);
    uint Byet_mode8 = BitArrayInvert(mainboard_3byte);

    return Group_MainboardOrder(Byet_mode4,Byet_mode5,Byet_mode8,0);
}


/*************************测试模组 温度、风扇开关 *****************************/
QByteArray UsbCodeDispose::_SetTestModeInfo(quint8 indexTestMode, quint8 temperature,quint8 indexfan, bool open)
{
    QByteArray TestModeCmd;
    TestModeCmd.resize(SizeofTestMode);
    TestModeCmd.fill(0);
    switch(indexTestMode)
    {
        case TestModeMotorGroup1:
                StructTestmode->slave_addr = Module_one;
        break;
        case TestModeMotorGroup2:
                StructTestmode->slave_addr = Module_two;
        break;
        case TestModeMotorGroup3:
                StructTestmode->slave_addr = Module_three;
        break;
        default:
            break;
    }
    StructTestmode->func_code = 0x17;
    StructTestmode->cmd_num = 0;
    StructTestmode->cmd_size = 0;
    StructTestmode->temperature_rep = temperature;

    QBitArray modebit;
    modebit.resize(Max_Bit);
    modebit.fill(0);
    if(indexfan == 0){
        StructTestmodeBit->motor1_status = false;
        StructTestmodeBit->motor2_status = false;
        StructTestmodeBit->motor3_status = false;
        StructTestmodeBit->motor4_status = false;
        StructTestmodeBit->motor5_status = false;
        StructTestmodeBit->motor6_status = false;
        StructTestmodeBit->motor7_status = false;
        StructTestmodeBit->motor8_status = false;
        StructTestmodeBit->motor9_status = false;
        StructTestmodeBit->motor10_status = false;
        StructTestmodeBit->motor11_status = false;
        StructTestmodeBit->motor12_status = false;
    }else if(indexfan == 1){
         StructTestmodeBit->motor1_status = open;
    }else if(indexfan == 2)
        StructTestmodeBit->motor2_status = open;
    else if(indexfan == 3)
        StructTestmodeBit->motor3_status = open;
    else if(indexfan == 4)
        StructTestmodeBit->motor4_status = open;
    else if(indexfan == 5)
        StructTestmodeBit->motor5_status = open;
    else if(indexfan == 6)
        StructTestmodeBit->motor6_status = open;
    else if(indexfan == 7)
        StructTestmodeBit->motor7_status = open;
    else if(indexfan == 8)
        StructTestmodeBit->motor8_status = open;
    else if(indexfan == 9)
        StructTestmodeBit->motor9_status = open;
    else if(indexfan == 10)
        StructTestmodeBit->motor10_status = open;
    else if(indexfan == 11)
        StructTestmodeBit->motor11_status = open;
    else if(indexfan == 12)
        StructTestmodeBit->motor12_status = open;

    if(TestModeMotorGroup1 == indexTestMode){
        modebit.setBit(Bit_0,StructTestmodeBit->motor4_status);
        modebit.setBit(Bit_1,StructTestmodeBit->motor3_status);
        modebit.setBit(Bit_2,StructTestmodeBit->motor2_status);
        modebit.setBit(Bit_3,StructTestmodeBit->motor1_status);
    }else if(TestModeMotorGroup2 == indexTestMode){
        modebit.setBit(Bit_0,StructTestmodeBit->motor8_status);
        modebit.setBit(Bit_1,StructTestmodeBit->motor7_status);
        modebit.setBit(Bit_2,StructTestmodeBit->motor6_status);
        modebit.setBit(Bit_3,StructTestmodeBit->motor5_status);
    }else if(TestModeMotorGroup3 == indexTestMode){
        modebit.setBit(Bit_0,StructTestmodeBit->motor12_status);
        modebit.setBit(Bit_1,StructTestmodeBit->motor11_status);
        modebit.setBit(Bit_2,StructTestmodeBit->motor10_status);
        modebit.setBit(Bit_3,StructTestmodeBit->motor9_status);
    }


    modebit.setBit(Bit_4,StructTestmodeBit->heap_status);
    StructTestmodeBit->cmd_status = 0;
    QString byte = QString("%1").arg(StructTestmodeBit->cmd_status,2 ,2, QLatin1Char('0'));
    QString tmp = byte.at(0);
    QString tmp1 = byte.at(1);
    modebit.setBit(Bit_5,tmp.toUInt());
    modebit.setBit(Bit_6,tmp1.toUInt());
    StructTestmodeBit->type = false;
    modebit.setBit(Bit_7,StructTestmodeBit->type);

    uint Byet_mode = BitArrayInvert(modebit);
    StructTestmode->DataBity = Byet_mode;

    int Length = 1;
    QByteArray tmparry;
    tmparry.clear();

    tmparry  = intToByte(Length,StructTestmode->slave_addr);  //从机地址
    TestModeCmd.replace(0,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,StructTestmode->func_code);
    TestModeCmd.replace(1,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,StructTestmode->cmd_num);
    TestModeCmd.replace(2,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,StructTestmode->cmd_size);
    TestModeCmd.replace(3,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,StructTestmode->DataBity);
    TestModeCmd.replace(4,tmparry.size(),tmparry);

    tmparry  = intToByte(2,StructTestmode->temperature_rep);
    TestModeCmd.replace(5,tmparry.size(),tmparry);

    return TestModeCmd;
}





void UsbCodeDispose::GroupIndex_4_byte(quint8 Spotr_axis, quint8 Sport_mode,quint8 cmd_state,quint8 cmd_type)
{
    QBitArray Motor_byte;
    Motor_byte.resize(Max_Bit); //协议下标 第 4 个字节
    const int Binary = 2;
    QString invert_bit;
    QString bit_axis_1,bit_axis_2,bit_axis_3;
    //运动的电机
    invert_bit = QString("%1").arg(Spotr_axis,3 ,Binary, QLatin1Char('0'));
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);
    bit_axis_3 = invert_bit.at(2);
    Motor_byte.setBit(Bit_0,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_1,bit_axis_2.toUInt());
    Motor_byte.setBit(Bit_2,bit_axis_3.toUInt());
    //运动模式
    invert_bit = QString("%1").arg(Sport_mode,Binary ,Binary, QLatin1Char('0'));
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);
    Motor_byte.setBit(Bit_3,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_4,bit_axis_2.toUInt());

    Motor_byte.setBit(Bit_5,cmd_state);
    Motor_byte.setBit(Bit_6,cmd_state);
    Motor_byte.setBit(Bit_7,cmd_type);

    uint Byet_= BitArrayInvert(Motor_byte);
    Struct_Motor->controldata =  Byet_;
    return;
}

void UsbCodeDispose::GroupIndex_5_byte(bool notuse ,bool cleanMachine, bool bLearn,quint8 Areas ,quint8 Offset_Z)
{
    QBitArray Motor_byte;
    Motor_byte.resize(Max_Bit); //协议下标 第 5个字节
    const int Binary = 2;
    QString invert_bit;
    QString bit_axis_1,bit_axis_2;

    Motor_byte.setBit(Bit_0,notuse); //未使用
    //清洗
    Motor_byte.setBit(Bit_1,cleanMachine); //隔膜泵
    Motor_byte.setBit(Bit_2,cleanMachine); //开关泵
    //吸样标志
    Motor_byte.setBit(Bit_3,bLearn);     //吸样标志
    //运动区域
    invert_bit = QString("%1").arg(Areas,Binary,Binary,QLatin1Char('0'));//转为2进制
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);
    Motor_byte.setBit(Bit_4,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_5,bit_axis_2.toUInt());

    invert_bit = QString("%1").arg(Offset_Z,Binary ,Binary, QLatin1Char('0'));
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);



    Motor_byte.setBit(Bit_6,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_7,bit_axis_2.toUInt());

    uint Byet_= BitArrayInvert(Motor_byte);
    Struct_Motor->controldata_2 =  Byet_;
    return;
}
///
/// \brief UsbCodeDispose::GroupIndex_5_byte_Zaxis
/// \param Action_state               抓取状态
/// \param Optocoupler                光耦
/// \param Liquid_level_detection     液面探测
/// \param not_use
/// \param Coordinate_partitioning    分区
/// \param Relative_position          相对坐标
///
void UsbCodeDispose::GroupIndex_5_byte_Zaxis(bool Action_state ,bool Optocoupler,bool  Liquid_level_detection, bool not_use,quint8 Coordinate_partitioning ,quint8 Relative_position)
{
    QBitArray Motor_byte;
    Motor_byte.resize(Max_Bit); //协议下标 第 5个字节
    Motor_byte.fill(false);
    const int Binary = 2;
    QString invert_bit;
    QString bit_axis_1,bit_axis_2;
    Motor_byte.setBit(Bit_0,Action_state); //抓手抓取成功，液面未探测到/ 抓手抓取失败，液面未探测成功
    Motor_byte.setBit(Bit_1,Optocoupler); //在光耦、脱离光耦
    Motor_byte.setBit(Bit_2,Liquid_level_detection); //液面探测
    Motor_byte.setBit(Bit_3,not_use);
    //运动区域
    invert_bit = QString("%1").arg(Coordinate_partitioning,Binary,Binary,QLatin1Char('0'));//转为2进制
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);
    Motor_byte.setBit(Bit_4,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_5,bit_axis_2.toUInt());
    //相对位置
    invert_bit = QString("%1").arg(Relative_position,Binary ,Binary, QLatin1Char('0'));
    bit_axis_1 = invert_bit.at(0);
    bit_axis_2 = invert_bit.at(1);
    Motor_byte.setBit(Bit_6,bit_axis_1.toUInt());
    Motor_byte.setBit(Bit_7,bit_axis_2.toUInt());
    uint Byet_= BitArrayInvert(Motor_byte);
    Struct_Motor->controldata_2 =  Byet_;
    return;
}

QByteArray UsbCodeDispose::Mindfunction_Axis_Z()
{
    QByteArray ParameterCode;
    ParameterCode.resize(SizeofXorYMotorBord);
    ParameterCode.fill(0);

    int Length = 1;
    QByteArray tmparry;
    tmparry.clear();

    GroupIndex_4_byte(Struct_bitMotor->Axisnum,Struct_bitMotor->operation_mode,0 , 0);
    //从机地址
    tmparry  = intToByte(Length,Struct_Motor->slave_addr);
    ParameterCode.replace(0,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->func_code);
    ParameterCode.replace(1,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->cmd_num);
    ParameterCode.replace(2,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->cmd_size);
    ParameterCode.replace(3,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->controldata);
    ParameterCode.replace(4,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->controldata_2);
    ParameterCode.replace(5,tmparry.size(),tmparry);
    /*控制Z轴复位的速度*/
    tmparry  = intToByte(4,Struct_Motor->SpotrStep);
    ParameterCode.replace(6,tmparry.size(),tmparry);
    /*抓取失败再次抓取的高度*/
    tmparry = intToByte(4,Struct_Motor->SportStepY);
    ParameterCode.replace(10,tmparry.size(),tmparry);

    return ParameterCode;
}

QByteArray UsbCodeDispose::Mindfunction(bool clean,bool Learn, bool XYtogether)
{    
    GroupIndex_4_byte(Struct_bitMotor->Axisnum,Struct_bitMotor->operation_mode,0 , 0);
    GroupIndex_5_byte(false,clean,Learn,Struct_bitMotor->area_num,Struct_bitMotor->sport_asix);

    QByteArray ParameterCode;
    ParameterCode.resize(SizeofXorYMotorBord);
    ParameterCode.fill(0);

    int Length = 1;
    QByteArray tmparry;
    tmparry.clear();

    tmparry  = intToByte(Length,Struct_Motor->slave_addr);  //从机地址
    ParameterCode.replace(0,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->func_code);
    ParameterCode.replace(1,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->cmd_num);
    ParameterCode.replace(2,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->cmd_size);
    ParameterCode.replace(3,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->controldata);
    ParameterCode.replace(4,tmparry.size(),tmparry);

    tmparry  = intToByte(Length,Struct_Motor->controldata_2);
    ParameterCode.replace(5,tmparry.size(),tmparry);

    tmparry  = intToByte(4,Struct_Motor->SpotrStep);
    ParameterCode.replace(6,tmparry.size(),tmparry);

    if(XYtogether)
    {
        tmparry = intToByte(4,Struct_Motor->SportStepY);
        ParameterCode.replace(10,tmparry.size(),tmparry);
    }
    return ParameterCode;
}


//-------------XY移动
QByteArray UsbCodeDispose::_DLLXYMovePoint_(int XPoint,int YPoint,int &indexcode, int offsetZ,int Area)
{
    QByteArray TaskLinkedList;
    TaskLinkedList.clear();
    Struct_Motor->slave_addr = 0x05;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = XPoint;
    Struct_Motor->SportStepY = YPoint;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = LoactionMode;
    Struct_bitMotor->Axisnum = XYjoint;     //4 XY一起动
    Struct_bitMotor->sport_asix =  offsetZ; //XY移动相对与哪跟针
    Struct_bitMotor->area_num = Area;       //测试区
    TaskLinkedList =  Mindfunction(false,false,true);
    indexcode++;
    return TaskLinkedList;
}
//-------------XY 复位
QByteArray UsbCodeDispose:: _DLLXYMoveReposition_(int &indexcode, int Speed)
{
    QByteArray TaskReposition;
    TaskReposition.clear();
    Struct_Motor->slave_addr = 0x05;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = Speed;
    Struct_Motor->SportStepY = Speed; //负速度速度
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = Negativespeed;
    Struct_bitMotor->Axisnum = XYjoint; //4 XY一起动
    Struct_bitMotor->sport_asix =  0; //XY移动相对与哪跟针
    Struct_bitMotor->area_num = 0;    //测试区
    TaskReposition =  Mindfunction(false,false,true);
    indexcode++;
    return TaskReposition;
}


QByteArray UsbCodeDispose::_DLLZDownward(int IndexZ, int IndexArea, int downmm, int &indexcode, bool LinqueTest,uint FailedLinque)
{
    QByteArray Zmotoraction;
    Zmotoraction.clear();
    Struct_Motor->slave_addr = 0x04;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0x00;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = SpeedMode::LoactionMode;//位置模式
    Struct_bitMotor->Axisnum = IndexZ;
    //液面探测标志
    quint8 offsetdownNeddle = 0; //下针的偏移
    switch(Struct_bitMotor->Axisnum)
    {
        case 0: offsetdownNeddle = 2; break;
        case 1: offsetdownNeddle = 1; break;
        case 2: offsetdownNeddle = 0; break;
    default:
        break;
    }
    GroupIndex_5_byte_Zaxis(false,false,LinqueTest,false,IndexArea,offsetdownNeddle);
    if(LinqueTest == true)
    {
         Struct_Motor->SpotrStep = FailedLinque;
    }
    else
    {
        Struct_Motor->SpotrStep = downmm;
    }
    Struct_bitMotor->area_num =  IndexArea;
    Zmotoraction = Mindfunction_Axis_Z();
    indexcode++;
    return Zmotoraction;
}


QByteArrayList UsbCodeDispose::_DLLZBackorigin(int IndexZ, int ZbackArea,int &indexcode)
{
    QByteArrayList ZbackArray;
    ZbackArray.clear();
    Struct_Motor->slave_addr = 0x04;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = ZMOVESPEED;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = SpeedMode::Negativespeed;
    Struct_bitMotor->sport_asix =  0;
    Struct_Motor->SportStepY = 0; //复位时再次下探高度为0
    Struct_bitMotor->area_num = ZbackArea;
    if(IndexZ == -1){
        for(int i = 0 ;i <3 ;i++){
            Struct_bitMotor->Axisnum = ZMotortongs + i;
            QByteArray Zmotoraction = Mindfunction_Axis_Z();
            indexcode++;
            Struct_Motor->cmd_num = indexcode%255;
            ZbackArray.push_back(Zmotoraction);
        }
    }
    else if(IndexZ == ZMotortongs)
    {
        Struct_bitMotor->Axisnum = ZMotortongs;
        QByteArray Zmotoraction = Mindfunction_Axis_Z();
        ZbackArray.push_back(Zmotoraction);
    }
    else if(IndexZ == ZMotorReagent)
    {
        Struct_bitMotor->Axisnum = ZMotorReagent;
        QByteArray Zmotoraction = Mindfunction_Axis_Z();
        ZbackArray.push_back(Zmotoraction);
    }
    else if(IndexZ == ZMotorBlood)
    {
        Struct_bitMotor->Axisnum = ZMotorBlood;
        QByteArray Zmotoraction = Mindfunction_Axis_Z();
        ZbackArray.push_back(Zmotoraction);
    }
    indexcode++;
    return ZbackArray;
}




/*正常的机器复位*/
QByteArrayList UsbCodeDispose::Dll_TaskFinishedreposition(int &indexcode,bool Loaction_Mode)
{
    QByteArrayList Repositioncode;
    Repositioncode.clear();
    Repositioncode =  _DLLZBackorigin(-1,TRAY_TESTCHANNEL,indexcode);  //所有Z复位
    if(!Loaction_Mode)
        Repositioncode.push_back(_DLLXYMoveReposition_(indexcode,MACHINEBACK_SPEED));   //XY负速度模式
    else
        Repositioncode.push_back(_DLLXYMovePoint_(0,0,indexcode,0,0));
    return Repositioncode;
}
//清洗后复位
QByteArrayList UsbCodeDispose::Dll_CleannedResetback(int indexcode,bool Loaction_Mode,int movespeed)
{
    QByteArrayList Repositioncode;
    Repositioncode.clear();
    if(!Loaction_Mode)
        Repositioncode.push_back(_DLLXYMoveReposition_(indexcode,movespeed));   //XY负速度模式
    else
        Repositioncode.push_back(_DLLXYMovePoint_(0,0,indexcode,0,0));
    return Repositioncode;
}

//回原点位置不清洗-- 开机复位
QByteArrayList UsbCodeDispose::_DLLMachineBackOrigin(QPoint Cleanlaction,int &indexcode)
{
    //先Z 复位 - XY复位 --XY 到清洗位 - 血样针跟试剂针洗针 - 大小泵速度模式 归0 - 拉泵位置模式到7200 -
    QByteArrayList MachineOrigin;
    MachineOrigin.clear();

    MachineOrigin =  _DLLZBackorigin(-1,TRAY_TESTCHANNEL,indexcode);  //所有Z复位
    MachineOrigin.push_back(_DLLXYMoveReposition_(indexcode,MACHINEBACK_SPEED));   //XY负速度模式
    MachineOrigin.push_back(_DLLXYMovePoint_(0,0,indexcode,0,0));  //XY位置模式
    MachineOrigin.push_back(_DLLXYMovePoint_(Cleanlaction.x(),Cleanlaction.y(),indexcode,0,0)); // xy移动到清洗位
    MachineOrigin.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,indexcode,false,CLEANING_DOWNHIGN));
    MachineOrigin.push_back(BenNegativespeedBackOrigin(0,BIG_SAMALL_BENBACK_SPEED,indexcode,false));
    MachineOrigin.push_back(BigBenActive(false,7200,indexcode,true));
    MachineOrigin.push_back(BigBenActive(false,0,indexcode,false));
    MachineOrigin.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_TESTCHANNEL,indexcode).first());

    MachineOrigin.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,indexcode,false,CLEANING_DOWNHIGN));
    MachineOrigin.push_back(BenNegativespeedBackOrigin(1,BIG_SAMALL_BENBACK_SPEED,indexcode,false));
    MachineOrigin.push_back(SmallBenActive(false,7200,true,indexcode));
    MachineOrigin.push_back(SmallBenActive(false,0,false,indexcode));
    MachineOrigin.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,indexcode).first());

    return MachineOrigin;
}


/*泵负速度模式复位   index = 0 大泵  1 小泵*/
QByteArray UsbCodeDispose::BenNegativespeedBackOrigin(int Index ,int speed,int &CodeNum, bool cleanuse)
{
    Struct_Motor->slave_addr = 0x05;
    Struct_Motor->cmd_num = CodeNum%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = speed;
    Struct_bitMotor->type = false;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = Negativespeed;
    if(Index == 0)
        Struct_bitMotor->Axisnum = PlungerBig;
    else if(Index == 1)
         Struct_bitMotor->Axisnum = PlungerSamll;
    Struct_bitMotor->sport_asix =  0;
    Struct_bitMotor->area_num =  0;
    QByteArray BenCode;
    BenCode.clear();
    if(cleanuse == false)
        BenCode = Mindfunction(false,false,false);
    else
        BenCode = Mindfunction(true,false,false);
    CodeNum++;
    return BenCode;
}



/**************移动到贫血吸样***********************/
QByteArrayList UsbCodeDispose::ActiveAnaemiaSingle(QPoint HolePos,QPoint EmptyPos,int emptydown,int miter,int &CodeNum,int FaileLinuuehigh)
{
    //在血样区吸贫血   emptydown(在试管区下血样针高度) FaileLinuuehigh(液面探测失败下降高度)
    QByteArrayList PoorBloodsucking;
    PoorBloodsucking.clear();
    QByteArrayList  Tmparry;

    int XBlood = HolePos.x();
    int YBlood = HolePos.y();
    PoorBloodsucking.push_back(_DLLXYMovePoint_(XBlood,YBlood,CodeNum,BloodNddle,TRAY_BLOOD));

    PoorBloodsucking.push_back(BigBenActive(false,BIG_BEN_INHALE_ARI,CodeNum,false));  //运动到位置先吸一段空气

    PoorBloodsucking.push_back(_DLLZDownward(ZMotorBlood,TRAY_BLOOD,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh)); //0 位置模式 == 液面探测

    int suck_ul = miter * BIG_BEN_RATIO + BIG_BEN_INHALE_ARI;
    qDebug()<<"贫血液面探测吸取的量:"<< suck_ul <<"ul";

    PoorBloodsucking.push_back(BigBenActive(true,suck_ul,CodeNum,false)); //吸血样 标志变

    Tmparry.clear();
    Tmparry = _DLLZBackorigin(ZMotorBlood,TRAY_BLOOD,CodeNum);
    for(int t = 0 ; t< Tmparry.size(); t++)
        PoorBloodsucking.push_back(Tmparry.at(t));

    XBlood = EmptyPos.x();
    YBlood = EmptyPos.y();

    PoorBloodsucking.push_back(_DLLXYMovePoint_(XBlood,YBlood,CodeNum,BloodNddle,TRAY_TESTENPTY));
    PoorBloodsucking.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTENPTY,emptydown,CodeNum,false,emptydown));

    int spit_ul =  BIG_BEN_INHALE_ARI/2;
    PoorBloodsucking.push_back(BigBenActive(false,spit_ul,CodeNum, false));
    qDebug()<<"贫血液面探测吐取的量:"<< spit_ul <<"ul";
    Tmparry.clear();
    Tmparry =_DLLZBackorigin(ZMotorBlood,TRAY_TESTENPTY,CodeNum);
    for(int i = 0 ; i<Tmparry.size(); i++)
        PoorBloodsucking.push_back(Tmparry.at(i));

    return PoorBloodsucking;
}


/**************移动到富血吸样***********************/
QByteArrayList UsbCodeDispose::ActiveRichbloodSingle(QPoint HolePos,QList<QPoint> EmptyPos,int testhigt,int emptydown,
                                                     int miter, quint8 Number,int  Securityvalue ,int &CodeNum)
{
    QByteArrayList RichBloodsucking;
    RichBloodsucking.clear();
    QByteArrayList  Tmparry;
    int XBlood = HolePos.x();
    int YBlood = HolePos.y();

    RichBloodsucking.push_back(_DLLXYMovePoint_(XBlood,YBlood,CodeNum,BloodNddle,TRAY_BLOOD));

    RichBloodsucking.push_back(BigBenActive(false,BIG_BEN_INHALE_ARI,CodeNum,false));  //运动到位置先吸一段空气

    RichBloodsucking.push_back(_DLLZDownward(ZMotorBlood,TRAY_BLOOD,testhigt,CodeNum,false,testhigt));

    int suck_ul = Number * miter * BIG_BEN_RATIO + BIG_BEN_INHALE_ARI ;//吸取的富血样本量: 样本个数 * 单个量 * 72
    qDebug()<<"富血吸取的量step:"<< suck_ul <<"ul"<<__FILE__<<__LINE__;

    //int totalmiter = Number * miter;

    RichBloodsucking.push_back(BigBenActive(true,suck_ul,CodeNum,false));

    Tmparry.clear();
    Tmparry = _DLLZBackorigin(ZMotorBlood,TRAY_BLOOD,CodeNum);

    for(int t =0 ; t<Tmparry.size(); t++)
        RichBloodsucking.push_back(Tmparry.at(t));

    //在空试管区吐出贫血
    int  spit_ul = 0;
    for(int i = 0 ; i < EmptyPos.size() ;i++)
    {
        QPoint EmptyTubePos = EmptyPos.at(i);

        XBlood =  EmptyTubePos.x();
        YBlood =  EmptyTubePos.y();

        RichBloodsucking.push_back(_DLLXYMovePoint_(XBlood,YBlood,CodeNum,BloodNddle,TRAY_TESTENPTY));
        RichBloodsucking.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTENPTY,emptydown,CodeNum,false,emptydown));

        if(i == 0)
           spit_ul = suck_ul - ((i+1)*miter * BIG_BEN_RATIO) - Securityvalue;
        else if(i +1 == EmptyPos.size())
        {
           spit_ul = BIG_BEN_INHALE_ARI/2;
        }
        else
        {
           spit_ul = spit_ul - (miter * BIG_BEN_RATIO);
        }

        //int Ml = (Number -(i+1))*totalmiter/Number;//单个富血的量

        qDebug()<<"单个富血的量步数:"<< spit_ul<<__FILE__<<__LINE__;
        RichBloodsucking.push_back(BigBenActive(false, spit_ul , CodeNum, false));
        Tmparry.clear();
        Tmparry =_DLLZBackorigin(ZMotorBlood,TRAY_TESTENPTY,CodeNum);
        for(int k = 0 ; k< Tmparry.size() ; k++)
            RichBloodsucking.push_back(Tmparry.at(k));
    }
    return RichBloodsucking;
}







//// 测试调试位置发送移动目标位置
/// \brief UsbCodeDispose::XYLocation
/// \param XYPos       XY 坐标
/// \param RelativeZ   下那根Z
/// \param Areas       区域
/// \param high        下降高度
/// \return
///
QByteArrayList UsbCodeDispose::XYLocation(QPoint XYPos, uint RelativeZ, uint Areas, int high, int &Codenum,int FaileLinuuehigh)
{
    QByteArrayList backCommand;
    backCommand.clear();
    int XPos = XYPos.x();
    int YPos = XYPos.y();
    backCommand = _DLLZBackorigin(-1,ADJUSTMODE,Codenum); //Z全部复位
    backCommand.push_back(_DLLXYMovePoint_(XPos,YPos,Codenum,0,Areas));
    if(high == 0)
    {
        backCommand.push_back(_DLLZDownward(RelativeZ,ADJUSTMODE,high,Codenum,true,FaileLinuuehigh)); //0 位置模式 == 液面探测
    }
    else
    {
        backCommand.push_back(_DLLZDownward(RelativeZ,ADJUSTMODE,high,Codenum,false,high));
    }
    return backCommand;
}


//调试模式丢杯测试
QByteArrayList UsbCodeDispose::AdjustModeThrowTube(QPoint XYPos, uint RelativeZ, uint Areas, int high, int &Codenum)
{
    QByteArrayList backCommand;
    backCommand.clear();
    int XPos = XYPos.x();
    int YPos = XYPos.y();
    backCommand = _DLLZBackorigin(-1,ADJUSTMODE,Codenum); //Z全部复位
    backCommand.push_back(_DLLXYMovePoint_(XPos,YPos,Codenum,0,Areas));
    backCommand.push_back(_DLLZDownward(RelativeZ,ADJUSTMODE,high,Codenum,false,high)); //丢杯得时候用位置模式
    //往后退丢杯
    int movebackY = std::abs(YPos - PUTTUBEMOVE);
    backCommand.push_back(_DLLXYMovePoint_(XPos,movebackY,Codenum,Hand,TRAY_TESTCHANNEL));
    return backCommand;
}

//////////
/// \brief UsbCodeDispose::EmptyTube2TestMode ---- 空试管移动到测试通道
/// \param EmptyTubeHole -----空试管区孔号
/// \param TestModeHole  -----测试通道孔号
/// \param downHeight    ------试管夹下降高度
/// \param CodeNum       ------命令编号
/// \param MidTestCode   ------输出编码
///
QByteArrayList UsbCodeDispose::EmptyTube2TestMode(QPoint EmptyTubeHole,QPoint TestModeHole ,int downHeight,int &CodeNum)
{
    QByteArrayList TastEmptyTube2ChannelCommed;
    TastEmptyTube2ChannelCommed.clear();

    int XPosEmptyarea = EmptyTubeHole.x();
    int YPosEmptyarea = EmptyTubeHole.y();
    int XPosTestarea =  TestModeHole.x();
    int YPosTestarea =  TestModeHole.y();

    /*************XY移动到空试管区(抓手)****************/
    TastEmptyTube2ChannelCommed.push_back(_DLLXYMovePoint_(XPosEmptyarea,YPosEmptyarea,CodeNum,Hand,TRAY_TESTENPTY));
    TastEmptyTube2ChannelCommed.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTENPTY,downHeight,CodeNum,false,downHeight));
    TastEmptyTube2ChannelCommed.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,CodeNum).first());

    TastEmptyTube2ChannelCommed.push_back(_DLLXYMovePoint_(XPosTestarea,YPosTestarea,CodeNum,Hand,TRAY_TESTCHANNEL));
    TastEmptyTube2ChannelCommed.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downHeight,CodeNum,false,downHeight)); //到测试通道丢杯
    int movebackY = std::abs(YPosTestarea - PUTTUBEMOVE);
    qDebug()<<"Y后退丢杯坐标"<<YPosTestarea<<"绝对值:"<<movebackY;
    TastEmptyTube2ChannelCommed.push_back(_DLLXYMovePoint_(XPosTestarea,movebackY,CodeNum,Hand,TRAY_TESTCHANNEL));
    TastEmptyTube2ChannelCommed.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,CodeNum).first());
    return TastEmptyTube2ChannelCommed;
}



/////
/// \brief UsbCodeDispose::ThrowTestChannelTube
/// \param Channel
///  丢杯命令
QByteArrayList UsbCodeDispose::ThrowTestChannelTube(QPoint ChannelPos, QPoint DiscardCupPos, int downhigh, int &CodeNum)
{
    QByteArrayList ThrowTestcommand;
    ThrowTestcommand.clear();

    int XDiscardCup = DiscardCupPos.x();
    int YDiscardCup = DiscardCupPos.y();
    int XPosTestarea =  ChannelPos.x();
    int YPosTestarea =  ChannelPos.y();

    ThrowTestcommand.push_back(_DLLXYMovePoint_(XPosTestarea,YPosTestarea,CodeNum,Hand,TRAY_TESTCHANNEL));
    ThrowTestcommand.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downhigh,CodeNum,false,downhigh));
    ThrowTestcommand.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,CodeNum).first());

    ThrowTestcommand.push_back(_DLLXYMovePoint_(XDiscardCup,YDiscardCup,CodeNum,Hand,THROWCUP_AREA));
    ThrowTestcommand.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downhigh,CodeNum,false,downhigh));
    int movebackY = std::abs(YDiscardCup - PUTTUBEMOVE);
    qDebug()<<"弃杯动作Y后退丢杯坐标"<<YPosTestarea<<"绝对值:"<<movebackY;
    ThrowTestcommand.push_back(_DLLXYMovePoint_(XDiscardCup,movebackY,CodeNum,Hand,THROWCUP_AREA));
    ThrowTestcommand.push_back(_DLLZBackorigin(ZMotortongs,THROWCUP_AREA,CodeNum).first());

    return ThrowTestcommand;
}



/******************清洗的时候动作**********************/
QByteArrayList UsbCodeDispose::XYMotorMovingBackOrigin(int Index,QPoint bloodPos,QPoint renagenpos,
                                                       QPoint originpos,int BloodmmL,int Reagent_ul,int &CodeNum,int FaileLinuuehigh)
{
    //清洗
    QByteArrayList TaskLinkedList;
    TaskLinkedList.clear();
    int Xblood = bloodPos.x();
    int Yblood = bloodPos.y();
    int Xreagent = renagenpos.x();
    int Yreagent = renagenpos.y();

    int suck_ul = 0; //吸取量
    if(Index == 0)
    {
        //2根针移动到吸试剂  清洗
        TaskLinkedList.push_back(_DLLXYMovePoint_(Xreagent,Yreagent,CodeNum,ReagentNeddle,TRAY_TESTCHANNEL));
        TaskLinkedList.push_back(SmallBenActive(false,SMALL_BEN_INHALE_ARI,false, CodeNum));  //运动到位置先吸一段空气
        TaskLinkedList.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh));       //**(吸试剂液面探测)
        suck_ul = Reagent_ul * SMALL_BEN_RATIO + SMALL_BEN_INHALE_ARI;
        TaskLinkedList.push_back(SmallBenActive(true,suck_ul,false,CodeNum));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,CodeNum).first());

        TaskLinkedList.push_back(_DLLXYMovePoint_(Xblood,Yblood,CodeNum,BloodNddle,TRAY_TESTCHANNEL));
        TaskLinkedList.push_back(BigBenActive(false,BIG_BEN_INHALE_ARI,CodeNum,false));  //运动到位置先吸一段空气
        TaskLinkedList.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTCHANNEL,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh));       //**(吸试剂液面探测)
        suck_ul =  BloodmmL * BIG_BEN_RATIO + BIG_BEN_INHALE_ARI;
        TaskLinkedList.push_back(BigBenActive(true,suck_ul,CodeNum,false)); //吸清洗剂
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_TESTCHANNEL,CodeNum).first());

        //移动要原点继续清洗
        TaskLinkedList.push_back(_DLLXYMovePoint_(originpos.x(),originpos.y(),CodeNum,0,0));  //原点清洗孔
        TaskLinkedList.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,CodeNum,false,CLEANING_DOWNHIGN));
        TaskLinkedList.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,CodeNum,false,CLEANING_DOWNHIGN));
        TaskLinkedList.push_back(BigBenActive(false,0,CodeNum,true));
        TaskLinkedList.push_back(SmallBenActive(false,0,true,CodeNum));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_TESTCHANNEL,CodeNum).first());
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,CodeNum).first());
    }
    else if(Index == 1)
    {
        TaskLinkedList.push_back(_DLLXYMovePoint_(Xblood,Yblood,CodeNum,BloodNddle,TRAY_TESTCHANNEL));
        TaskLinkedList.push_back(BigBenActive(false,BIG_BEN_INHALE_ARI,CodeNum,false));  //运动到位置先吸一段空气
        TaskLinkedList.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTCHANNEL,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh));       //**(吸试剂液面探测)
        suck_ul =  BloodmmL * BIG_BEN_RATIO + BIG_BEN_INHALE_ARI;
        TaskLinkedList.push_back(BigBenActive(true,suck_ul,CodeNum,false));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_TESTCHANNEL,CodeNum).first());
        TaskLinkedList.push_back(_DLLXYMovePoint_(originpos.x(),originpos.y(),CodeNum,0,0));
        TaskLinkedList.push_back(_DLLZDownward(ZMotorBlood,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,CodeNum,false,CLEANING_DOWNHIGN));
        TaskLinkedList.push_back(BigBenActive(false,0/*BIG_BEN_INHALE_ARI/2*/,CodeNum,true));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_TESTCHANNEL,CodeNum).first());

    }
    else if(Index == 2)
    {
        TaskLinkedList.push_back(_DLLXYMovePoint_(Xreagent,Yreagent,CodeNum,ReagentNeddle,TRAY_TESTCHANNEL));
        TaskLinkedList.push_back(SmallBenActive(false,SMALL_BEN_INHALE_ARI,false,CodeNum));  //运动到位置先吸一段空气
        TaskLinkedList.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh));       //**(吸试剂液面探测)
        suck_ul =  Reagent_ul * SMALL_BEN_RATIO + SMALL_BEN_INHALE_ARI;
        TaskLinkedList.push_back(SmallBenActive(true,suck_ul,false,CodeNum));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,CodeNum).first());
        TaskLinkedList.push_back(_DLLXYMovePoint_(originpos.x(),originpos.y(),CodeNum,0,0));
        TaskLinkedList.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,CLEANING_DOWNHIGN,CodeNum,false,CLEANING_DOWNHIGN));
        TaskLinkedList.push_back(SmallBenActive(false,0/*SMALL_BEN_INHALE_ARI/2*/,true,CodeNum));
        TaskLinkedList.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,CodeNum).first());
    }
    TaskLinkedList.push_back(_DLLXYMoveReposition_(CodeNum,CLEANNIG_BACK_ORIGIN_SPEED)); //复位原点
    return TaskLinkedList;
}

QByteArrayList UsbCodeDispose::EmptyTube2TestModeaddReagent(QPoint EmptyPos, QPoint ChannlPos,int Tongsdown, int &tmpnum)
{
    QByteArrayList TaskLinkedList;
    TaskLinkedList.clear();
    int XEmpty = EmptyPos.x();
    int YEmpty = EmptyPos.y();
    int XChannel = ChannlPos.x();
    int YChannel = ChannlPos.y();
    TaskLinkedList.push_back(_DLLXYMovePoint_(XEmpty,YEmpty,tmpnum,Hand,TRAY_TESTENPTY));
    TaskLinkedList.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTENPTY,Tongsdown,tmpnum,false,Tongsdown));
    TaskLinkedList.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,tmpnum).first());
    TaskLinkedList.push_back(_DLLXYMovePoint_(XChannel,YChannel,tmpnum,Hand,TRAY_TESTCHANNEL));
    TaskLinkedList.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,Tongsdown,tmpnum,false,Tongsdown));
    int movebackY = std::abs(YChannel - PUTTUBEMOVE);
    qDebug()<<"弃杯动作Y后退丢杯坐标"<<YChannel<<"绝对值:"<<movebackY;
    TaskLinkedList.push_back(_DLLXYMovePoint_(XChannel,movebackY,tmpnum,Hand,TRAY_TESTCHANNEL));
    TaskLinkedList.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,tmpnum).first());
    return TaskLinkedList;
}



/**************吸试剂**********************/
QByteArrayList UsbCodeDispose::Aspiratereagent(QPoint reagent, int milliliter, int &CodeNum,int FaileLinuuehigh)
{
    int  XPos = reagent.x();
    int  YPos = reagent.y();
    QByteArrayList AspiratereagentCommand;
    AspiratereagentCommand.clear();

    AspiratereagentCommand.push_back(_DLLXYMovePoint_(XPos,YPos,CodeNum,ReagentNeddle,TRAY_TESTCHANNEL));

    AspiratereagentCommand.push_back(SmallBenActive(false,BIG_BEN_INHALE_ARI,false,CodeNum));  //运动到位置先吸一段空气

    AspiratereagentCommand.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,FaileLinuuehigh,CodeNum,true,FaileLinuuehigh)); //(吸试剂液面探测)

    int suck_ul = milliliter * SMALL_BEN_RATIO + BIG_BEN_INHALE_ARI;
    qDebug()<<"吸取试剂量step:"<< suck_ul <<"ul"<<__FILE__<<__LINE__;

    AspiratereagentCommand.push_back(SmallBenActive(true,suck_ul,false,CodeNum));

    AspiratereagentCommand.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,CodeNum).first());
    return AspiratereagentCommand;
}

/**************吐试剂**********************/
QByteArrayList UsbCodeDispose::Spitreagent2Channel(QPoint channelpos,int downmm,int &tmpnum)
{
    int  XPos = channelpos.x();
    int  YPos = channelpos.y();
    QByteArrayList AspiratereagentCommand;
    AspiratereagentCommand.clear();
    AspiratereagentCommand.push_back(_DLLXYMovePoint_(XPos,YPos,tmpnum,ReagentNeddle,TRAY_TESTCHANNEL));
    AspiratereagentCommand.push_back(_DLLZDownward(ZMotorReagent,TRAY_TESTCHANNEL,downmm,tmpnum,false,downmm));
    AspiratereagentCommand.push_back(SmallBenActive(false, BIG_BEN_INHALE_ARI/2 , false,tmpnum));
    AspiratereagentCommand.push_back(_DLLZBackorigin(ZMotorReagent,TRAY_TESTCHANNEL,tmpnum).first());
    return AspiratereagentCommand;
}


//开机自检 丢杯
QByteArrayList  UsbCodeDispose::_DLL_Grabcup_ThrowCup(QVector<QPoint> AllChannelPos,QPoint ThrowTube ,int downmm,int &codenum)
{
    QByteArrayList Grab_Throw_Cupe;
    Grab_Throw_Cupe.clear();
    auto iter = AllChannelPos.begin();
    QPoint ChannelPos;
    int movebackY = 0;
    movebackY = std::abs(ThrowTube.y() - PUTTUBEMOVE);
    Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(ThrowTube.x(),ThrowTube.y(),codenum,Hand,TRAY_TESTCHANNEL));
    Grab_Throw_Cupe.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downmm,codenum,false,downmm));
    Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(ThrowTube.x(),movebackY,codenum,Hand,TRAY_TESTCHANNEL));
    Grab_Throw_Cupe.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,codenum).first());

    while(iter != AllChannelPos.end()){
        ChannelPos = *iter;
        Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(ChannelPos.x(),ChannelPos.y(),codenum,Hand,TRAY_TESTCHANNEL));
        Grab_Throw_Cupe.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downmm,codenum,false,downmm));
        Grab_Throw_Cupe.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,codenum).first());
        //丢杯
        Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(ThrowTube.x(),ThrowTube.y(),codenum,Hand,TRAY_TESTCHANNEL));
        Grab_Throw_Cupe.push_back(_DLLZDownward(ZMotortongs,TRAY_TESTCHANNEL,downmm,codenum,false,downmm));
        Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(ThrowTube.x(),movebackY,codenum,Hand,TRAY_TESTCHANNEL));
        Grab_Throw_Cupe.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,codenum).first());
        iter++;
    }
    Grab_Throw_Cupe.push_back(_DLLXYMoveReposition_(codenum,MACHINEBACK_SPEED));   //XY负速度模式
    Grab_Throw_Cupe.push_back(_DLLXYMovePoint_(0,0,codenum,0,0));  //XY位置模式
    return Grab_Throw_Cupe;
}





/*-------------------------------*/
QByteArray  UsbCodeDispose::DLL_XYMoveSpecifiedPosition(const QPoint Position, const int offset_Z, const int Area ,int &indexcode)
{
    QByteArray TaskLinkedList;
    TaskLinkedList.clear();
    Struct_Motor->slave_addr = 0x05;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = Position.x();
    Struct_Motor->SportStepY = Position.y();
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = LoactionMode;
    Struct_bitMotor->Axisnum = XYjoint;     //4 XY一起动
    Struct_bitMotor->sport_asix =  offset_Z; //XY移动相对与哪跟针
    Struct_bitMotor->area_num = Area;       //测试区
    TaskLinkedList =  Mindfunction(false,false,true);
    indexcode++;
    return TaskLinkedList;
}
////
/// \brief UsbCodeDispose::DLL_ZMoveSpecifiedPosition
/// \param TypeZ         下降的针 下标
/// \param downhigt      非液面探测下降高度
/// \param Area          区域
/// \param indexcode     命令编号
/// \param LinqueTest    液面探测标志
/// \param Failedhigh    探测失败 下降高度
/// \return
///

QByteArray  UsbCodeDispose::DLL_ZMoveSpecifiedPosition(const int TypeZ ,const int downhigt ,const int Area,int &indexcode, bool LinqueTest,int Failedhigh)
{
    QByteArray Zmotoraction;
    Zmotoraction.clear();
    Struct_Motor->slave_addr = 0x04;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0x00;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = SpeedMode::LoactionMode; //位置模式
    Struct_bitMotor->Axisnum = TypeZ;
    //液面探测标志
    quint8 offsetdownNeddle = 0; //下针的偏移
    switch(Struct_bitMotor->Axisnum)
    {
        case 0: offsetdownNeddle = 2; break;
        case 1: offsetdownNeddle = 1; break;
        case 2: offsetdownNeddle = 0; break;
    default:
        break;
    }
    GroupIndex_5_byte_Zaxis(false,false,LinqueTest,false,Area,offsetdownNeddle);
    if(LinqueTest == true)
    {
         Struct_Motor->SpotrStep = Failedhigh;
    }
    else
    {
        Struct_Motor->SpotrStep = downhigt;
    }
    Struct_bitMotor->area_num =  Area;
    Zmotoraction = Mindfunction_Axis_Z();
    indexcode++;
    return Zmotoraction;
}

QByteArray  UsbCodeDispose::DLL_ZAxis_Reset(const int IndexZ, const int FailedHigh,const int ZbackArea,int &indexcode)
{
    QByteArray ZbackArray;
    ZbackArray.clear();
    Struct_Motor->slave_addr = 0x04;
    Struct_Motor->func_code = 0x17;
    Struct_Motor->cmd_num = indexcode%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = ZMOVESPEED;
    Struct_Motor->SportStepY = FailedHigh; /*抓手抓取失败,再次抓取的高度*/
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = SpeedMode::Negativespeed;
    Struct_bitMotor->sport_asix =  IndexZ;
    Struct_bitMotor->area_num = ZbackArea; //下针区域
    Struct_bitMotor->Axisnum = IndexZ;
    ZbackArray = Mindfunction_Axis_Z();
    indexcode++;
    return ZbackArray;
}

QByteArray UsbCodeDispose::BigBenActive(bool suck, int milliliter, int &CodeNum, bool cleanuse)
{
    Struct_Motor->slave_addr = 0x05;
    Struct_Motor->cmd_num = CodeNum%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = milliliter;
    Struct_bitMotor->type = false;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = LoactionMode;
    Struct_bitMotor->Axisnum = PlungerBig;
    Struct_bitMotor->sport_asix =  0;
    Struct_bitMotor->area_num =  0;
    QByteArray BenCode;
    BenCode.clear();
    BenCode = Mindfunction(cleanuse,suck,false);
    CodeNum++;
    return BenCode;
}

/*100ML 的泵开始吸、吐*/
QByteArray UsbCodeDispose::SmallBenActive(bool suck, int milliliter, bool clranning, int &CodeNum)
{
    Struct_Motor->slave_addr = 0x05;
    if(CodeNum%255 == 0)    CodeNum = 1;
    Struct_Motor->cmd_num = CodeNum%255;
    Struct_Motor->cmd_size = 0;
    Struct_Motor->SpotrStep = milliliter;
    Struct_bitMotor->type = false;
    Struct_bitMotor->cmd_status = 0;
    Struct_bitMotor->operation_mode = LoactionMode;
    Struct_bitMotor->Axisnum = PlungerSamll;
    Struct_bitMotor->sport_asix =  0;
    Struct_bitMotor->area_num =  0;
    QByteArray BenCode;
    BenCode.clear();
    if(!clranning)
        BenCode = Mindfunction(false,suck,false);
    else
        BenCode = Mindfunction(true,suck,false);
    CodeNum++;
    return BenCode;
}


/////////////////////////////////////////质控动作////////////////////////////////////////////////////////////////////////
/*清洗血样针*/
QByteArrayList UsbCodeDispose::QualityControlCleanBloodNeedle(int Indexcode,QPoint BloodNeele2Clean,QPoint OriginPos,int CleanlinqueMm,int failLinquehigh)
{
    QByteArrayList QualityCleanBloodNeedle; //吸蒸馏水
    QualityCleanBloodNeedle.clear();
    int indexZ = 0;
    for(;indexZ < 3 ; indexZ++)
    {
        QByteArray backcode = DLL_ZAxis_Reset(indexZ,0,0,Indexcode);
        QualityCleanBloodNeedle.push_back(backcode);
    }
    //清洗血样针
    QByteArrayList CleanNeddleArry = XYMotorMovingBackOrigin(1,BloodNeele2Clean,QPoint(0,0),OriginPos,CleanlinqueMm,0,Indexcode,failLinquehigh);
    for(int i = 0 ; i < CleanNeddleArry.size() ; i++ )
    {
        QualityCleanBloodNeedle.push_back(CleanNeddleArry.at(i));
    }
    return QualityCleanBloodNeedle;
}

QStringList Char2String(char *data, int size)
{
    QByteArray byArr;
    QString back_data;
    byArr.resize(size);
    for (int i = 0; i<size; i++)
        byArr[i] = data[i];
    QDataStream out(&byArr, QIODevice::ReadWrite);
    while (!out.atEnd())
    {
        qint8 outchar = 0;
        out >> outchar;
        QString str = QString("%1").arg(outchar & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString((" "));
        back_data += str;
    }
    back_data.simplified();
    QStringList outlist = back_data.split(" ");
    if (outlist.last().isEmpty())
        outlist.removeLast();
    return outlist;
}

void bytearry_2_String(QByteArray arry,QStringList &Machinedata){

    char buf[15];//数组
    int len_array = arry.size();
    int len_buf = sizeof(buf);
    int len = qMin(len_array, len_buf);
    memcpy(buf, arry, len);// 转化
    Machinedata = Char2String(buf, 15);
    return;
}

/*到血样孔吸吸蒸馏水 || 贫血 || 富血*/
QByteArrayList UsbCodeDispose::QualityControlRunActive(int Indexcode,int downspeed,int downZhigh,QPoint WaterPos,QPoint TestTubePos,int usemilliliter)
{
    QByteArrayList QualityWaterabsorption; //吸蒸馏水 || 贫血 || 富血
    QualityWaterabsorption.clear();

    QByteArray  Singleactionarry;
    Singleactionarry.clear();
    QualityWaterabsorption.push_back(DLL_XYMoveSpecifiedPosition(WaterPos,TRAY_BLOOD,BloodNddle,Indexcode));
    //先吸空气
    QualityWaterabsorption.push_back(BigBenActive(true,BIG_BEN_INHALE_ARI,Indexcode,false));

    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotorBlood,downspeed,TRAY_BLOOD,Indexcode,true,downspeed);  //液面探测
    QualityWaterabsorption.push_back(Singleactionarry);
    QStringList outprintcode;
    outprintcode.clear();
    bytearry_2_String(Singleactionarry,outprintcode);
    qDebug()<<"液面探测code："<<outprintcode<<__LINE__;
    int suck_ul =  usemilliliter * BIG_BEN_RATIO + BIG_BEN_INHALE_ARI;
    qDebug()<<"质控吸的量步数："<<suck_ul<<"量"<<usemilliliter<<"*"<<BIG_BEN_RATIO<<__LINE__;
    QualityWaterabsorption.push_back(BigBenActive(true,suck_ul,Indexcode,false));
    QualityWaterabsorption.push_back(_DLLZBackorigin(ZMotorBlood,TRAY_BLOOD,Indexcode).first());

    //把吸了蒸馏水的注到测试管中
    Singleactionarry = DLL_XYMoveSpecifiedPosition(TestTubePos,TRAY_TESTENPTY,BloodNddle,Indexcode);
    QualityWaterabsorption.push_back(Singleactionarry);
    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotorBlood,downZhigh,TRAY_BLOOD,Indexcode,false,downZhigh);
    QualityWaterabsorption.push_back(Singleactionarry);
    Singleactionarry = BigBenActive(false,0,Indexcode,false); //吐出蒸馏水
    QualityWaterabsorption.push_back(Singleactionarry);
    Singleactionarry =_DLLZBackorigin(ZMotorBlood,TRAY_TESTENPTY,Indexcode).first();
    QualityWaterabsorption.push_back(Singleactionarry);

    return  QualityWaterabsorption;
}
//到空试管 -- 把空试管夹到测试通道
QByteArrayList UsbCodeDispose::QualityControl2Channel(int Indexcode,QPoint TestTubePos,QPoint TestChannelPos,int downHandsmm)
{
    QByteArrayList QualityTestTubemoveChannel; //把试管夹到测试通道
    QualityTestTubemoveChannel.clear();
    QByteArray  Singleactionarry;
    Singleactionarry = DLL_XYMoveSpecifiedPosition(TestTubePos,TRAY_TESTENPTY,Hand,Indexcode);
    QualityTestTubemoveChannel.push_back(Singleactionarry);
    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotortongs,downHandsmm,TRAY_TESTENPTY,Indexcode,false,downHandsmm);
    QualityTestTubemoveChannel.push_back(Singleactionarry);
    Singleactionarry = _DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,Indexcode).first();
    QualityTestTubemoveChannel.push_back(Singleactionarry);
    //到测试通道放试管
    Singleactionarry = DLL_XYMoveSpecifiedPosition(TestChannelPos,TRAY_TESTCHANNEL,Hand,Indexcode);
    QualityTestTubemoveChannel.push_back(Singleactionarry);
    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotortongs,downHandsmm,TRAY_TESTCHANNEL,Indexcode,false,downHandsmm);
    QualityTestTubemoveChannel.push_back(Singleactionarry);

    int movebackY = std::abs(TestChannelPos.y() - PUTTUBEMOVE);
    QPoint PutbackTube(TestChannelPos.x(),movebackY);
    Singleactionarry = DLL_XYMoveSpecifiedPosition(PutbackTube,TRAY_TESTCHANNEL,Hand,Indexcode);
    QualityTestTubemoveChannel.push_back(Singleactionarry);
    Singleactionarry = _DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,Indexcode).first();
    QualityTestTubemoveChannel.push_back(Singleactionarry);

    return QualityTestTubemoveChannel;
}

//把测试通道中的杯子丢掉
QByteArrayList UsbCodeDispose::QualityControThrowTube(int Indexcode,QPoint TestChannelPos,QPoint ThrowtubePos,int downHandsmm)
{
    QByteArrayList QualityTestTubeThrow; //把试管夹丢掉
    QualityTestTubeThrow.clear();
    QByteArray  Singleactionarry;
    Singleactionarry = DLL_XYMoveSpecifiedPosition(TestChannelPos,TRAY_TESTCHANNEL,Hand,Indexcode);
    QualityTestTubeThrow.push_back(Singleactionarry);
    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotortongs,downHandsmm,TRAY_TESTCHANNEL,Indexcode,false,downHandsmm);
    QualityTestTubeThrow.push_back(Singleactionarry);
    Singleactionarry = _DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,Indexcode).first();
    QualityTestTubeThrow.push_back(Singleactionarry);


    Singleactionarry = DLL_XYMoveSpecifiedPosition(ThrowtubePos,TRAY_TESTCHANNEL,Hand,Indexcode);
    QualityTestTubeThrow.push_back(Singleactionarry);
    Singleactionarry = DLL_ZMoveSpecifiedPosition(ZMotortongs,downHandsmm,TRAY_TESTCHANNEL,Indexcode,false,downHandsmm);
    QualityTestTubeThrow.push_back(Singleactionarry);
    int movebackY = std::abs(ThrowtubePos.y() - PUTTUBEMOVE);
    QPoint PutbackTube(ThrowtubePos.x(),movebackY);
    Singleactionarry = DLL_XYMoveSpecifiedPosition(PutbackTube,TRAY_TESTCHANNEL,Hand,Indexcode);
    QualityTestTubeThrow.push_back(Singleactionarry);
    Singleactionarry = _DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,Indexcode).first();
    QualityTestTubeThrow.push_back(Singleactionarry);
    return QualityTestTubeThrow;
}

//试管区试管夹到测试通道(抓手) 抓手不下
QByteArrayList UsbCodeDispose::EmptyTubeMove2Channel(int Indexcode, const QPoint TubePoint,const QPoint ChannelPoint,const int downMM)
{
    QByteArrayList Tubemove2channelarrycode;
    Tubemove2channelarrycode.clear();
    Tubemove2channelarrycode.push_back(DLL_XYMoveSpecifiedPosition(TubePoint,TRAY_TESTCHANNEL,Hand,Indexcode));
    Tubemove2channelarrycode.push_back(DLL_ZMoveSpecifiedPosition(ZMotortongs,downMM,TRAY_TESTCHANNEL,Indexcode,false,downMM));
    Tubemove2channelarrycode.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,Indexcode).first());
    Tubemove2channelarrycode.push_back(DLL_XYMoveSpecifiedPosition(ChannelPoint,TRAY_TESTCHANNEL,Hand,Indexcode));
    return Tubemove2channelarrycode;
}

//抓手到测试通道把试管放下
QByteArrayList UsbCodeDispose::HandsPutitdown2Channel(int Indexcode,const QPoint ChannelPoint,const int downMM)
{
    QByteArrayList HandsPutdownTube;
    HandsPutdownTube.clear();
    HandsPutdownTube.push_back(DLL_ZMoveSpecifiedPosition(ZMotortongs,downMM,TRAY_TESTCHANNEL,Indexcode,false,downMM));
    int movebackY = std::abs(ChannelPoint.y() - PUTTUBEMOVE);
    QPoint PutbackTube(ChannelPoint.x(),movebackY);
    HandsPutdownTube.push_back(DLL_XYMoveSpecifiedPosition(PutbackTube,TRAY_TESTCHANNEL,Hand,Indexcode));
    HandsPutdownTube.push_back( _DLLZBackorigin(ZMotortongs,TRAY_TESTENPTY,Indexcode).first());
    return HandsPutdownTube;
}
//抓手到测试通道夹起杯子移动到另一通道不下抓手
QByteArrayList UsbCodeDispose::HandsFromAchanne2Bchannel(int Indexcode,const QPoint FromChannelPoint,const QPoint EndChannelPoint,const int downMM)
{
    QByteArrayList HandsMoveChannelTube;
    HandsMoveChannelTube.clear();
    HandsMoveChannelTube.push_back(DLL_XYMoveSpecifiedPosition(FromChannelPoint,TRAY_TESTCHANNEL,Hand,Indexcode));
    HandsMoveChannelTube.push_back(DLL_ZMoveSpecifiedPosition(ZMotortongs,downMM,TRAY_TESTCHANNEL,Indexcode,false,downMM));
    HandsMoveChannelTube.push_back(_DLLZBackorigin(ZMotortongs,TRAY_TESTCHANNEL,Indexcode).first());
    HandsMoveChannelTube.push_back(DLL_XYMoveSpecifiedPosition(EndChannelPoint,TRAY_TESTCHANNEL,Hand,Indexcode));
    return HandsMoveChannelTube;
}
