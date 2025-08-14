
#ifdef Q_OS_UNIX
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#else
#include <QBitArray>
#include <QDataStream>
#include <qt_windows.h>
#endif

#include <qDebug>
#include "instrumentcoordinatetable.h"






