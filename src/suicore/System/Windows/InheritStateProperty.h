// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����InheritStateProperty.h
// ��  �ܣ�ʵ�ֺ��Ŀ��״̬����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIINHERITSTATEPROPERTY_H_
#define _UIINHERITSTATEPROPERTY_H_

#include <System/Windows/Element.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class InheritStateProperty
{
public:

    InheritStateProperty(DpProperty* dp, int cacheFlag, int changedFlag);

    virtual void FireState(Element* sender, bool oldValue) = 0;
    virtual void DoRelative(Element* oldElem, Element* newElem);

    void OnOriginalValueChanged(Element* oldElem, Element* newElem);

protected:

    DpProperty* _dp;
    int _cacheFlag;
    int _changedFlag;

};

class MouseOverProperty : public InheritStateProperty
{
public:

    MouseOverProperty();

    void FireState(Element* sender, bool oldValue);
    void DoRelative(Element* oldElem, Element* newElem);

protected:

    void HandleRelativeMouse(Element* elem);
};

class MouseCaptureProperty : public InheritStateProperty
{
public:

    MouseCaptureProperty();

    void FireState(Element* sender, bool oldValue);
};

class FocusWithinProperty : public InheritStateProperty
{
public:

    FocusWithinProperty();

    void FireState(Element* sender, bool oldValue);
};

}

#endif
