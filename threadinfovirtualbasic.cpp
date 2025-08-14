#include "threadinfovirtualbasic.h"

using namespace std;

threadInfoVirtualBasic::threadInfoVirtualBasic(QString function,int datalength)
    :m_DataLength(datalength),m_FunctionCode(function)
{

}

 void threadInfoVirtualBasic::SetFunction(QString &function)
 {
     m_FunctionCode = function;

 }
 QString threadInfoVirtualBasic::GetFunction()
 {
     return m_FunctionCode;
 }
 void threadInfoVirtualBasic::SetDataLength(int &datalength)
 {
     m_DataLength = datalength;
 }
 int threadInfoVirtualBasic::GetDataLength()
 {
     return m_DataLength;
 }
