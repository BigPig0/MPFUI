// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================
/////////////////////////////////////////////////////////////////////////
// InheritStateProperty.cpp

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/InheritStateProperty.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

InheritStateProperty::InheritStateProperty(DpProperty* dp, int cacheFlag, int changedFlag)
    : _dp(dp)
    , _cacheFlag(cacheFlag)
    , _changedFlag(changedFlag)
{
}

void InheritStateProperty::DoRelative(Element* oldElem, Element* newElem)
{

}

void InheritStateProperty::OnOriginalValueChanged(Element* oldElem, Element* newElem)
{
    // ��һ��������������뿪��Ԫ��״̬(IsMouseOverChange)
    Element* pElem = oldElem;

    while (pElem != NULL)
    {
        // 
        // ����״̬�ı�
        //
        pElem->WriteFlag(_changedFlag, true);
        // 
        // ȡ�������ͣ��־
        //
        pElem->WriteFlag(_cacheFlag, false);

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    // �ڶ�������ӵ������Ԫ��״̬(IsMouseOverChange)
    pElem = newElem;
    while (pElem != NULL)
    {
        // 
        // ����״̬�ı�
        //
        pElem->WriteFlag(_changedFlag, !pElem->ReadFlag(_changedFlag));

        // 
        // ȡ�������ͣ��־
        //
        pElem->WriteFlag(_cacheFlag, true);

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    //DoRelative(oldElem, newElem);

    Element* pRoot = oldElem ? VisualTreeOp::GetVisualRoot(oldElem) : VisualTreeOp::GetVisualRoot(newElem);

    // �����������״̬IsMouseOverChange�����ı��Ԫ��
    // ������Ӧ��MouseEnter��MouseLeave�¼�
    pElem = oldElem;
    while (pElem != NULL)
    {
        if (pElem->ReadFlag(_changedFlag))
        {
            pElem->WriteFlag(_changedFlag, false);
            pElem->WriteFlag(_cacheFlag, false);
            // ����״̬�ı�
            pElem->SetValue(_dp, Boolean::False);
            FireState(pElem, true);
        }

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    // ������
    pElem = newElem;
    while (pElem != NULL)
    {
        if (pElem->ReadFlag(_changedFlag))
        {
            pElem->WriteFlag(_changedFlag, false);
            pElem->WriteFlag(_cacheFlag, true);

            if (pElem->IsEnabled())
            {
                // ����״̬�ı�
                pElem->SetValue(_dp, Boolean::True);
                FireState(pElem, false);
            }
        }

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }
}

//===========================================================
// MouseOverProperty
MouseOverProperty::MouseOverProperty()
    : InheritStateProperty(Element::IsMouseOverProperty
    , CoreFlags::IsMouseOverWithinCache
    , CoreFlags::IsMouseOverWithinChanged)
{
}

void MouseOverProperty::FireState(Element* sender, bool oldValue)
{
    Point pt = __GetCusorPoint(sender);
    MouseButtonEventArg e(sender, pt);

    if (oldValue)
    {
        e.SetRoutedEvent(sender->MouseLeaveEvent);
    }
    else
    {
        e.OnOverrideOriginalSource(MouseDevice::GetMouseOver());
        e.SetRoutedEvent(sender->MouseEnterEvent);
    }

    sender->RaiseEvent(&e);
}

void MouseOverProperty::HandleRelativeMouse(Element* elem)
{
}

void MouseOverProperty::DoRelative(Element* oldElem, Element* newElem)
{
}

//===========================================================
// MouseCaptureProperty
MouseCaptureProperty::MouseCaptureProperty()
    : InheritStateProperty(Element::IsMouseCaptureWithinProperty
    , CoreFlags::IsMouseCapturedWithinCache
    , CoreFlags::IsMouseCapturedWithinChanged)
{
}

void MouseCaptureProperty::FireState(Element* sender, bool oldValue)
{
    sender->OnMouseCaptureWithinChanged(!oldValue);
}

//===========================================================
// FocusWithinProperty
FocusWithinProperty::FocusWithinProperty()
    : InheritStateProperty(Element::IsKeyboardFocusWithinProperty
    , CoreFlags::IsKeyboardFocusWithinCache
    , CoreFlags::IsKeyboardFocusWithinChanged)
{

}

void FocusWithinProperty::FireState(Element* sender, bool oldValue)
{
    if (sender->IsFocusable())
    {
        sender->OnKeyboardFocusWithinChanged(!oldValue);
    }
}

}
