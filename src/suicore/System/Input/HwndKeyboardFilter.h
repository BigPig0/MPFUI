// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����HwndKeyboardFilter.h
// ��  �ܣ�������Ŀ�ļ�/ֵ����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDKEYBOARDFILTER_H_
#define _UIHWNDKEYBOARDFILTER_H_

#include <System/Windows/HwndObject.h>
#include <Framework/Controls/Control.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

class HwndKeyboardFilter;
class HwndKeyboardFilter
{
public:

    HwndKeyboardFilter(HwndObject* objPtr);
    ~HwndKeyboardFilter();

    bool OnFilterMessage(Object* sender, MessageParam* mp);
    void SetHwndObject(HwndObject* obj) { _hwndObj = obj; }

protected:

    Element* GetKeyboardRoot(Element* rootElement);
    Element* GetFocusedElement(Element* rootElement);

    bool Process_WM_KEYDOWN(Element* rootElement, MessageParam* mp);
    bool Process_WM_KEYUP(Element* rootElement, MessageParam* mp);

    void Process_WM_KILLFOCUS(Element* rootElement, MessageParam* mp);
    void Process_WM_SETFOCUS(Element* rootElement, MessageParam* mp);

    void Process_WM_HOTKEY(Element* rootElement, MessageParam* mp);

protected:

    HwndObject* _hwndObj;
    bool _calledFocus;
};

}

#endif