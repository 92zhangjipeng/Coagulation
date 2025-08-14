#ifndef CCRASHSTACK_H
#define CCRASHSTACK_H
#include <qt_windows.h>
#include <QString>

class ccrashstack
{
private:
    PEXCEPTION_POINTERS m_pException;
private:

public:
    ccrashstack(PEXCEPTION_POINTERS pException);

};

#endif // CCRASHSTACK_H

