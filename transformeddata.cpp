#include "transformeddata.h"

transformedData::transformedData()
{

}
//QString bitArray2String(QBitArray array)
//{
//    uint value = 0;
//    int num = array.size();
//    for (uint i = 0; i < num; ++i )
//    {
//        value <<= 1;
//        value += (int)array.at(i);
//    }
//    //qDebug() << value <<__FILE__<<__LINE__; //转化出的整形
//    QString str;
//    str.setNum(value, 10);
//    return str;
//}
QByteArray transformedData::quint32ToQByteArray(quint32 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
        output[1] = (uchar)((0x00ff0000 & input) >> 16);
        output[0] = (uchar)((0xff000000 & input) >> 24);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
        output[2] = (uchar)((0x00ff0000 & input) >> 16);
        output[3] = (uchar)((0xff000000 & input) >> 24);
    }
    return output;
}

quint32 transformedData::QByteArrayToquint32(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
        output |= (input[1] << 16) & 0x00FF0000;
        output |= (input[0] << 24) & 0xFF000000;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
        output |= (input[2] << 16) & 0x00FF0000;
        output |= (input[3] << 24) & 0xFF000000;
    }
    return output;
}

QByteArray transformedData::qint32ToQByteArray(qint32 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
        output[1] = (uchar)((0x00ff0000 & input) >> 16);
        output[0] = (uchar)((0xff000000 & input) >> 24);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
        output[2] = (uchar)((0x00ff0000 & input) >> 16);
        output[3] = (uchar)((0xff000000 & input) >> 24);
    }
    return output;
}
qint32 transformedData::QByteArrayToqint32(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
        output |= (input[1] << 16) & 0x00FF0000;
        output |= (input[0] << 24) & 0xFF000000;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
        output |= (input[2] << 16) & 0x00FF0000;
        output |= (input[3] << 24) & 0xFF000000;
    }
    return output;
}

QByteArray transformedData::quint16ToQByteArray(quint16 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
    }
    return output;
}

quint16 transformedData::QByteArrayToquint16(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
    }
    return output;
}

QByteArray transformedData::qint16ToQByteArray(qint16 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
    }
    return output;
}
qint16 transformedData::QByteArrayToqint16(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
    }
    return output;
}

QByteArray transformedData::quint64ToQByteArray(quint64 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        QByteArray _output;
        _output.resize(sizeof(input));
        memcpy(_output.data(), &input, sizeof(input));
        for (int i = 0; i < _output.size(); i++)
        {
            output[i] = _output[_output.size() - i - 1];
        }
    }
    else
    {
        output.resize(sizeof(input));
        memcpy(output.data(), &input, sizeof(input));
    }
    return output;
}
quint64 transformedData::QByteArrayToquint64(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        QByteArray _input;
        for (int i = 0; i < input.size(); i++)
        {
            _input[i] = input[input.size() - i - 1];
        }
        memcpy(&output, _input, sizeof(output));
    }
    else
    {
        memcpy(&output, input, sizeof(output));
    }
    return output;
}


QByteArray transformedData::qint64ToQByteArray(qint64 input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        QByteArray _output;
        _output.resize(sizeof(input));
        memcpy(_output.data(), &input, sizeof(input));
        for (int i = 0; i < _output.size(); i++)
        {
            output[i] = _output[_output.size() - i - 1];
        }
    }
    else
    {
        output.resize(sizeof(input));
        memcpy(output.data(), &input, sizeof(input));
    }
    return output;
}
qint64 transformedData::QByteArrayToqint64(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        QByteArray _input;
        for (int i = 0; i < input.size(); i++)
        {
            _input[i] = input[input.size() - i - 1];
        }
        memcpy(&output, _input, sizeof(output));
    }
    else
    {
        memcpy(&output, input, sizeof(output));
    }
    return output;
}


int transformedData::QByteArrayToint(QByteArray input)
{
    int output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
        output |= (input[1] << 16) & 0x00FF0000;
        output |= (input[0] << 24) & 0xFF000000;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
        output |= (input[2] << 16) & 0x00FF0000;
        output |= (input[3] << 24) & 0xFF000000;
    }
    return output;
}
QByteArray transformedData::intToQByteArray(int input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
        output[1] = (uchar)((0x00ff0000 & input) >> 16);
        output[0] = (uchar)((0xff000000 & input) >> 24);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
        output[2] = (uchar)((0x00ff0000 & input) >> 16);
        output[3] = (uchar)((0xff000000 & input) >> 24);
    }
    return output;
}

uint transformedData::QByteArrayTouint(QByteArray input)
{
    uint output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
        output |= (input[1] << 16) & 0x00FF0000;
        output |= (input[0] << 24) & 0xFF000000;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
        output |= (input[2] << 16) & 0x00FF0000;
        output |= (input[3] << 24) & 0xFF000000;
    }
    return output;
}
QByteArray transformedData::uintToQByteArray(uint input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
        output[1] = (uchar)((0x00ff0000 & input) >> 16);
        output[0] = (uchar)((0xff000000 & input) >> 24);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
        output[2] = (uchar)((0x00ff0000 & input) >> 16);
        output[3] = (uchar)((0xff000000 & input) >> 24);
    }
    return output;
}



short transformedData::QByteArrayToshort(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
    }
    return output;
}
QByteArray transformedData::shortToQByteArray(short input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
    }
    return output;
}

ushort transformedData::QByteArrayToushort(QByteArray input)
{
    qint64 output;
    if (isEndianTyped)
    {
        output = input[3] & 0x000000FF;
        output |= (input[2] << 8) & 0x0000FF00;
    }
    else
    {
        output = input[0] & 0x000000FF;
        output |= (input[1] << 8) & 0x0000FF00;
    }
    return output;
}
QByteArray transformedData::ushortToQByteArray(ushort input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
    }
    return output;
}

float transformedData::QByteArrayTofloat(QByteArray input)
{
    float output;
    if (isEndianTyped)
    {
        QByteArray _input;
        for (int i = 0; i < input.size(); i++)
        {
            _input[i] = input[input.size() - i - 1];
        }
        memcpy(&output, _input, sizeof(output));
    }
    else
    {
        memcpy(&output, input, sizeof(output));
    }
    return output;
}
QByteArray transformedData::floatToQByteArray(float input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        QByteArray _output;
        _output.resize(sizeof(input));
        memcpy(_output.data(), &input, sizeof(input));
        for (int i = 0; i < _output.size(); i++)
        {
            output[i] = _output[_output.size() - i - 1];
        }
    }
    else
    {
        output.resize(sizeof(input));
        memcpy(output.data(), &input, sizeof(input));
    }
    return output;
}

double transformedData::QByteArrayTodouble(QByteArray input)
{
    double output;
    if (isEndianTyped)
    {
        QByteArray _input;
        for (int i = 0; i < input.size(); i++)
        {
            _input[i] = input[input.size() - i - 1];
        }
        memcpy(&output, _input, sizeof(output));
    }
    else
    {
        memcpy(&output, input, sizeof(output));
    }
    return output;
}
QByteArray transformedData::doubleToQByteArray(double input)
{
    QByteArray output;
    if (isEndianTyped)
    {
        QByteArray _output;
        _output.resize(sizeof(input));
        memcpy(_output.data(), &input, sizeof(input));
        for (int i = 0; i < _output.size(); i++)
        {
            output[i] = _output[_output.size() - i - 1];
        }
    }
    else
    {
        output.resize(sizeof(input));
        memcpy(output.data(), &input, sizeof(input));
    }
    return output;
}


QBitArray transformedData::transformedbit( bool (&configbit)[8])
{
    int length = 8;
    QBitArray outdata(length);
    outdata.fill(false);
    int end = length - 1;
    for(int i = 0; i < length ; i++ )
    {
        outdata.setBit(i,configbit[end -i]);
    }
    return outdata;
}


//QByteArray  transformedData::modulSendArry(int indexModule,quint8 indexChannel,const bool runmotor,const bool bAddHot)
//{
//    QByteArray backArry;
//    //从机地址
//    QString hexstr = QString::number(indexModule, 16);
//    std::string hexchar =  hexstr.toStdString();
//    backArry.push_back(hexchar.c_str());
//    //功能码
//    backArry.push_back(0x17);
//    //命令编号
//    hexstr = QString::number(1, 16);
//    hexchar =  hexstr.toStdString();
//    backArry.push_back(hexchar.c_str());
//    //缓存的命令容量
//    hexstr = QString::number(1, 16);
//    hexchar =  hexstr.toStdString();
//    backArry.push_back(hexchar.c_str());
//    //模组小马达开、关
//    bool statebit[8]{};
//    if(indexChannel == 4 || indexChannel == 8 || indexChannel == 12)
//    {
//        statebit[0] = runmotor;
//    }
//    else if(indexChannel == 3 || indexChannel == 7 || indexChannel == 11)
//    {
//        statebit[1] = runmotor;
//    }
//    else if(indexChannel == 2 || indexChannel == 6 || indexChannel == 10)
//    {
//        statebit[2] = runmotor;
//    }
//    else if(indexChannel == 1 || indexChannel == 5 || indexChannel == 9)
//    {
//        statebit[3] = runmotor;
//    }
//    else if(indexChannel == 0)
//    {
//        statebit[0] = false;
//        statebit[1] = false;
//        statebit[2] = false;
//        statebit[3] = false;
//    }
//    else if(indexChannel > 12)
//    {
//        statebit[0] = true;
//        statebit[1] = true;
//        statebit[2] = true;
//        statebit[3] = true;
//    }
//    statebit[4] = bAddHot; //加热
//    statebit[5] = false;
//    statebit[6] = false;
//    statebit[7] = false;
//    QBitArray controlbity = transformedbit(&statebit);
//    QString bithex =  bitArray2String(controlbity);
//    hexchar =  bithex.toStdString();
//    backArry.push_back(hexchar.c_str());
//    //温度数据
//    quint16 temp = 0;
//    backArry.push_back(quint16ToQByteArray(temp));
//    //通道数据
//    for(int k = 0; k < 4; k++)
//        backArry.push_back(quint16ToQByteArray(temp));
//    return backArry;
//}








