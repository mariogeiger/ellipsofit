/*

        Copyright (c) 2010 by Mario Geiger <mario.geiger@epfl.ch>

     ***************************************************************************
     *                                                                         *
     *   This program is free software; you can redistribute it and/or modify  *
     *   it under the terms of the GNU General Public License as published by  *
     *   the Free Software Foundation; either version 3 of the License, or     *
     *   (at your option) any later version.                                   *
     *                                                                         *
     ***************************************************************************
    */

#include "mplatfnt.h"



#include <qglobal.h>
#if defined(Q_OS_WIN)
#  include <windows.h>
#elif defined(Q_OS_LINUX)
#  include <unistd.h>
#elif defined(Q_OS_MAC)
#  include <mach/mach.h>
#  include <mach/machine.h>
#elif defined(Q_OS_OS2)
#define INCL_DOSMISC
#include <os2.h>

#ifndef QSV_NUMPROCESSORS
/*The QSV_NUMPROCESSORS is only defined in the SMP toolkit,
 *and since the SMP toolkit is always at least 6 months behind
 *the normal toolkit you will probably not want use it.
 */
#define QSV_NUMPROCESSORS 26
#endif
#endif

int ncpu()
{
    int cpu(1);
#if defined(Q_OS_WIN)
    SYSTEM_INFO si;
    memset( &si, 0, sizeof(si));
    GetSystemInfo( &si);
    cpu = si.dwNumberOfProcessors;
#elif defined(Q_OS_LINUX)
    cpu = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(Q_OS_MAC)
    kern_return_t kr;
    struct host_basic_info hostinfo;
    unsigned int count;

    count = HOST_BASIC_INFO_COUNT;
    kr = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostinfo, &count);
    if(kr == KERN_SUCCESS)
    {
        cpu = hostinfo.avail_cpus;
        // totalMemory = hostinfo.memory_size; //in bytes
    }
#elif defined(Q_OS_OS2)
    ULONG CPUs;
    APIRET rc;
    CPUs = 0;
    rc = DosQuerySysInfo(QSV_NUMPROCESSORS,
                         QSV_NUMPROCESSORS,
                         &CPUs,
                         sizeof(CPUs));
    /*We have to guard against running under a non-SMP OS/2
   *that does not support index 26, and that index 26 (as
   *far as I remember) has previously been used for
   *something else
   */

    if(rc!=0 || CPUs<1 || CPUs>64)
        CPUs = 1;
    cpu = CPUs;
#endif
    if(cpu < 1) cpu = 1;

    return cpu;
}




#include <QString>
#if defined(Q_OS_WIN)
#  include <windows.h>
#  include <lmcons.h>
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_OS2)
#  include <stdlib.h>
#endif

QString username()
{
#if defined(Q_OS_WIN)
    QString userName;

#if defined(UNICODE)
    if (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based)
    {
        TCHAR winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
        DWORD winUserNameSize = sizeof(winUserName);
        GetUserName(winUserName, &winUserNameSize);
        userName = QString::fromUtf16((ushort*)winUserName);
    } else
#endif
    {
        char winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
        DWORD winUserNameSize = sizeof(winUserName);
        GetUserNameA(winUserName, &winUserNameSize);
        userName = QString::fromLocal8Bit(winUserName);
    }
    return userName;
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_OS2)
    return QString(getenv("USER"));
#endif
}
