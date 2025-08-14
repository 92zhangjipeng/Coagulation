#include "infovirtual.h"
using namespace std;

InfoVirtual::InfoVirtual(QString function,int datalength,QString temp)
    :threadInfoVirtualBasic(function,datalength)
{
    SetFunction(temp);
}
void InfoVirtual::SetFunction_send(QString code_send)
{
    m_temp = code_send;
}
QString InfoVirtual::GetFunction_send()
{
    return m_temp;
}
QString InfoVirtual::RecveFunctionCode()
{

    return "";
}
