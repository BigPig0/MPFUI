// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����datehelp.h
// ��  �ܣ����ڸ����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-02
// 
// ============================================================================

#ifndef _UIDATEHELP_H_
#define _UIDATEHELP_H_

#include <System/Windows/Object.h>

namespace suic
{

class DateHelp
{
public:

    static void DaysToYMD(int iDate, Uint16& iYear, Byte& iMonth, Byte& iDay);
    static int YMDToDays(int iYear, int iMonth, int iDay);
    static int MonthEndDay(int iYear, int iMonth);
    static bool IsLeapYear(int iYear);
};

}

#endif
