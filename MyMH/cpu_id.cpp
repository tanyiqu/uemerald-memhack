#include "widget.h"
#include <QDebug>

QString Widget::get_cpuId()
{
    unsigned long s1,s2,s3,s4;
    char string[128];
    char szCpuId[1024];
    char p1[128], p2[128];
    unsigned int eax = 0;
    unsigned int ebx,ecx,edx;
    #if 1
    asm volatile
    (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "0"(0)
    );
    snprintf(szCpuId, 5, "%s", (char *)&ebx);
    snprintf(szCpuId+4, 5, "%s", (char *)&edx);
    snprintf(szCpuId+8, 5, "%s", (char *)&ecx);
    #endif

    asm volatile
    (
        "movl $0x01 , %%eax ; \n\t"
        "xorl %%edx , %%edx ;\n\t"
        "cpuid ;\n\t"
        "movl %%edx ,%0 ;\n\t"
        "movl %%eax ,%1 ; \n\t"
        :"=m"(s1),"=m"(s2)
    );

    char cpuID[20];
    memset(cpuID,0,20);
    sprintf((char *)p1, "%08X%08X", s1, s2);
    snprintf(szCpuId+12, 20, "%s", (char *)p1);
    memcpy(cpuID,p1,17);

    QString CPUID(cpuID);
//    qDebug() << QString(cpuID);
    //将CPUID重置为乱码
    QString str1 = CPUID.mid(0,8);
    QString str2 = CPUID.mid(8,8);

//    qDebug() << str1 << str2;
    u32 n1 = str1.toUInt(nullptr,16);
    u32 n2 = str2.toUInt(nullptr,16);
    n1 ^= 0x654ab6c6;
    n2 ^= 0xb4a5c5ff;
//    qDebug() << n1 << n2;
    str1 = QString::number(n1,16);while(str1.length()<8)str1.insert(0,"0");
    str2 = QString::number(n2,16);while(str2.length()<8)str2.insert(0,"0");
    CPUID = str1 + str2;
    CPUID = CPUID.toUpper();
    return CPUID;

}
