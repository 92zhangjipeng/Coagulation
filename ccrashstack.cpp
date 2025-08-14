#include "ccrashstack.h"
#include <tlhelp32.h>
#include <stdio.h>

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

//#include<base/constants.h>


ccrashstack::ccrashstack(PEXCEPTION_POINTERS pException)
{
    m_pException = pException;
}





