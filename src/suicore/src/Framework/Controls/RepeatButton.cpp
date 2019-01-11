// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// RepeatButton.cpp

#include <Framework/Controls/RepeatButton.h>
#include <System/Windows/CoreTool.h>
#include <System/Input/MouseDevice.h>

namespace suic
{

ImplementRTTIOfClass(RepeatButton, ButtonBase)

DpProperty* RepeatButton::DelayProperty;
DpProperty* RepeatButton::IntervalProperty;

RepeatButton::RepeatButton()
{
    _timer = new AssignerTimer();
    _timer->ref();
}

RepeatButton::~RepeatButton()
{
    if (_timer)
    {
        _timer->unref();
    }
}

void RepeatButton::StaticInit()
{
    if (NULL == DelayProperty)
    {
        ButtonBase::StaticInit();
        DelayProperty = DpProperty::Register("Delay", Integer::RTTIType(), RTTIType(), DpPropMemory::GetPropMeta(new Integer(MouseDevice::Current()->GetDoubleDeltaTime() * 1.2)));
        IntervalProperty = DpProperty::Register("Interval", Integer::RTTIType(), RTTIType(), DpPropMemory::GetPropMeta(new Integer(MouseDevice::Current()->GetDoubleDeltaTime() * 0.2)));
    }
}

void RepeatButton::OnInitialized(EventArg* e)
{
    // 
    // ���û�������¼��ַ�����
    //
    Element::OnInitialized(e);
}

void RepeatButton::OnUnloaded(LoadedEventArg* e)
{
    ButtonBase::OnUnloaded(e);
    _timer->Stop();
}

void RepeatButton::OnRender(suic::Drawing * drawing)
{
    // 
    // ֱ�ӵ��û�����л���
    //
    ButtonBase::OnRender(drawing);
}

void RepeatButton::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    _timer->SetTick(EventHandler(this, &RepeatButton::OnTick));
    _timer->SetInterval(GetDelay());
    _timer->Start();
    ButtonBase::OnMouseLeftButtonDown(e);
}

void RepeatButton::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    _timer->Stop();
    ButtonBase::OnMouseLeftButtonUp(e);
}

void RepeatButton::OnTick(Object* sender, EventArg* e)
{
    _timer->SetInterval(GetInterval());

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        Point pt = CalcuCusorPoint(this);
        Rect rect(0, 0, GetRenderSize().cx, GetRenderSize().cy);

        pt = PointFromScreen(pt);
        OnRepeated(rect.PointIn(pt));
    }
}

void RepeatButton::OnRepeated(bool bMouseIn)
{
    if (bMouseIn)
    {
        OnClick();
    }
}

/////////////////////////////////////////////////////
//

int RepeatButton::GetDelay()
{
    return GetValue(DelayProperty)->ToInt();
}

void RepeatButton::SetDelay(int iDelay)
{
    SetValue(DelayProperty, new Integer(iDelay));
}

int RepeatButton::GetInterval()
{
    return GetValue(IntervalProperty)->ToInt();
}

void RepeatButton::SetInterval(int iInterval)
{
    SetValue(IntervalProperty, new Integer(iInterval));
}

}
