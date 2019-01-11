// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����internaloper.h
// ��  �ܣ���װ�ڲ��Ĳ�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-12
// 
// ============================================================================

#ifndef _UIINTERNALOPER_H_
#define _UIINTERNALOPER_H_

#include <System/Tools/Array.h>
#include <System/Windows/HwndObject.h>
#include <Framework/Controls/Window.h>

/*--------�ڲ�ʹ��ͷ�ļ�--------*/
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

void suiInternalClear();

void RecursiveLoaded(Element* sender);
void RecursiveUnloaded(Element* sender);
void RecursiveLoadedWithFocused(Element* sender, ElementPtr& focused);

}

#endif

