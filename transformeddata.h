#ifndef TRANSFORMEDDATA_H
#define TRANSFORMEDDATA_H

#include <QBitArray>
#include <QByteArray>
#include <QString>
//https://blog.csdn.net/hiwoshixiaoyu/article/details/121783070

#define isEndianTyped  false //ture为大端；false为小端

class transformedData
{
public:
    transformedData();
public:
   QByteArray  quint32ToQByteArray(quint32 input);      //quint32转QbyteArray
   quint32     QByteArrayToquint32(QByteArray input);   //QbyteArray 转quint32

   QByteArray  qint32ToQByteArray(qint32 input);
   qint32      QByteArrayToqint32(QByteArray input);

   QByteArray  quint16ToQByteArray(quint16 input);
   quint16     QByteArrayToquint16(QByteArray input);

   QByteArray  qint16ToQByteArray(qint16 input);
   qint16      QByteArrayToqint16(QByteArray input);

   QByteArray  quint64ToQByteArray(quint64 input);
   quint64     QByteArrayToquint64(QByteArray input);

   QByteArray qint64ToQByteArray(qint64 input);
   qint64     QByteArrayToqint64(QByteArray input);

   QByteArray intToQByteArray(int input);
   int        QByteArrayToint(QByteArray input);

   QByteArray  uintToQByteArray(uint input);
   uint        QByteArrayTouint(QByteArray input);

   QByteArray  shortToQByteArray(short input);
   short       QByteArrayToshort(QByteArray input);

   QByteArray  ushortToQByteArray(ushort input);
   ushort      QByteArrayToushort(QByteArray input);

   QByteArray  floatToQByteArray(float input);
   float       QByteArrayTofloat(QByteArray input);

   QByteArray  doubleToQByteArray(double input);
   double      QByteArrayTodouble(QByteArray input);

   //比特位组合成字节
   QBitArray   transformedbit(bool (&configbit)[8]);


   //QByteArray  modulSendArry(int indexModule, quint8 indexChannel, const bool runmotor, const bool bAddHot);


};

#endif // TRANSFORMEDDATA_H
