#ifndef INFOVIRTUAL_H
#define INFOVIRTUAL_H
#include <QString>
#include "threadinfovirtualbasic.h"
using namespace std;

class InfoVirtual : public threadInfoVirtualBasic
{
public:
    InfoVirtual(QString,int,QString temp ="");
    virtual ~InfoVirtual(){}
    void SetFunction_send(QString);
    QString GetFunction_send();
    virtual QString RecveFunctionCode() override;
private:
    QString m_temp;

};

#endif // INFOVIRTUAL_H
