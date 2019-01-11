// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ProgressBar.cpp

#include <Framework/Controls/ProgressBar.h>

namespace suic
{

ImplementRTTIOfClass(ProgressBar, RangeBase)
;
DpProperty* ProgressBar::IsIndeterminateProperty;
DpProperty* ProgressBar::OrientationProperty;

ProgressBar::ProgressBar()
    : _glow(NULL)
    , _track(NULL)
    , _indicator(NULL)
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::StaticInit()
{
    if (NULL == OrientationProperty)
    {
        RangeBase::StaticInit();

        IsIndeterminateProperty = DpProperty::Register(_U("IsIndeterminate"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));
        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsMeasure));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);
        RangeBase::MaximumProperty->OverrideMetadata(RTTIType(), new PropMetadata(new OFloat(100.0f)));
    }
}

String ProgressBar::ToString() const
{
    return String();
}

bool ProgressBar::IsIndeterminate()
{
    return false;
}

Orientation ProgressBar::GetOrientation()
{
    return (Orientation)DpObject::GetValue(OrientationProperty)->ToInt();
}

void ProgressBar::SetOrientation(Orientation val)
{
    DpObject::SetValue(OrientationProperty, OrientationBox::From(val));
}

void ProgressBar::UpdateAnimation()
{
    UpdateLayout();
}

void ProgressBar::SetProgressBarIndicatorLength()
{
    if ((_track != NULL) && (_indicator != NULL))
    {
        Float minimum = GetMinimum();
        Float maximum = GetMaximum();
        Float fVal = GetValue();
        Float fIndicator = 1.0f;
        
        if (IsIndeterminate() || (maximum <= minimum)) 
        {
            fIndicator = 1.0f;
        }
        else
        {
            fIndicator = ((fVal - minimum) / (maximum - minimum));
        }

        if (GetOrientation() == Orientation::Horizontal)
        {
            _indicator->SetWidth(fIndicator * _track->GetActualWidth());
        }
        else
        {
            _indicator->SetHeight(fIndicator * _track->GetActualHeight());
        }

        //
        // ��������ǿ��ˢ�²���
        //
        UpdateArrange();

        UpdateAnimation();
    }
}

void ProgressBar::SetProgressBarGlowElementBrush()
{
}

void ProgressBar::OnApplyTemplate()
{
    RangeBase::OnApplyTemplate();

    if (_track != NULL)
    {
        RoutedEventHandler h(this, &ProgressBar::OnTrackSizeChanged);
        _track->RemoveSizeChanged(&h);
    }

    _track = RTTICast<FrameworkElement>(GetTemplateChild("PART_Track"));
    _indicator = RTTICast<FrameworkElement>(GetTemplateChild("PART_Indicator"));
    _glow = RTTICast<FrameworkElement>(GetTemplateChild("PART_GlowRect"));

    if (_track != NULL)
    {
        _track->AddSizeChanged(new RoutedEventHandler(this, &ProgressBar::OnTrackSizeChanged));
    }

    if (IsIndeterminate())
    {
        SetProgressBarGlowElementBrush();
    }
}

void ProgressBar::OnTrackSizeChanged(DpObject* sender, RoutedEventArg* e)
{
    SetProgressBarIndicatorLength();
}

void ProgressBar::OnValueChanged(Float oldValue, Float newValue)
{
    RangeBase::OnValueChanged(oldValue, newValue);

    //
    // ���½�����ָʾ�ؼ�
    //
    SetProgressBarIndicatorLength();
}

void ProgressBar::OnMaximumChanged(Float oldMaximum, Float newMaximum)
{
}

void ProgressBar::OnMinimumChanged(Float oldMinimum, Float newMinimum)
{
}

};
