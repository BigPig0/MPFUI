// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Track.cpp

#include <Framework/Controls/Track.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/RangeBase.h>
#include <Framework/Controls/ScrollBar.h>

#include <System/Windows/VisualStates.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/ResourceParser.h>
#include <System/Types/Numeric.h>

#include <math.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// Track

ImplementRTTIOfClass(Track, FrameworkElement)

DpProperty* Track::ValueProperty;
DpProperty* Track::MaximumProperty;
DpProperty* Track::MinimumProperty;

DpProperty* Track::OrientationProperty;
DpProperty* Track::ViewportSizeProperty;

DpProperty* Track::IsDirectionReversedProperty;

Track::Track()
    : _density(0)
    , _thumb(NULL)
    , _thumbCenterOffset(0)
    , _decreaseButton(NULL)
    , _increaseButton(NULL)
{
    for (int i = 0; i < 3; ++i)
    {
        _visualChildren[i] = NULL;
    }
}

Track::~Track()
{
    if (_thumb)
    {
        _thumb->unref();
    }
    if (_decreaseButton)
    {
        _decreaseButton->unref();
    }
    if (_increaseButton)
    {
        _increaseButton->unref();
    }
}

void Track::StaticInit()
{
    if (ValueProperty == NULL)
    {
        RangeBase::StaticInit();

        ValueProperty = RangeBase::ValueProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        MaximumProperty = RangeBase::MaximumProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(1), PropMetadataOptions::AffectsArrange));
        MinimumProperty = RangeBase::MinimumProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));

        ViewportSizeProperty = DpProperty::Register(_T("ViewportSize"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsArrange));

        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsArrange));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);

        IsDirectionReversedProperty = DpProperty::Register(_T("IsDirectionReversed"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsArrange));
    }
}

static void CoerceLength(int& componentLength, int trackLength)
{
    if (componentLength < 0)
    {
        componentLength = 0;
    }
    else if (componentLength > trackLength)
    {
        componentLength = trackLength;
    }
}

int Track::GetValueFromDistance(int horizontal, int vertical)
{
    int num = IsDirectionReversed() ? ((int) (-1)) : ((int) 1);

    if (GetOrientation() == Orientation::Horizontal)
    {
        return (int)((num * horizontal) * _density + 0.5);
    }
    else
    {
        return (int)(((-1.0 * num) * vertical) * _density + 0.5);
    }
}

int Track::GetValueFromPoint(Point pt)
{
    int num = 0;

    if (GetOrientation() == Orientation::Horizontal)
    {
        num = (int)(GetValue() + GetValueFromDistance(pt.x - _thumbCenterOffset, pt.y - (GetRenderSize().Height() * 0.5)));
    }
    else
    {
        num = (int)(GetValue() + GetValueFromDistance(pt.x - (GetRenderSize().Width() * 0.5), pt.y - _thumbCenterOffset));
    }

    return max(GetMinimum(), min(GetMaximum(), num));
}

void Track::UpdateComponent(Element* oldValue, Element* newValue)
{
    if (oldValue != newValue)
    {
        if (oldValue != NULL)
        {
            RemoveVisualChild(oldValue);
        }

        int index = 0;

        while (index < 3)
        {
            if (_visualChildren[index] == NULL)
            {
                break;
            }
            if (_visualChildren[index] == oldValue)
            {
                while ((index < 2) && (_visualChildren[index + 1] != NULL))
                {
                    _visualChildren[index] = _visualChildren[index + 1];
                    index++;
                }
            }
            else
            {
                index++;
            }
        }

        _visualChildren[index] = newValue;
        AddVisualChild(newValue);

        if (IsInitialized())
        {
            newValue->BeginInit();
            newValue->EndInit();
        }

        InvalidateMeasure();
        InvalidateArrange();
    }
}

void Track::AddChild(Object* obj)
{

}

int Track::GetVisualChildrenCount()
{
    int count = 0;

    if (NULL != _decreaseButton)
    {
        ++count;
    }

    if (NULL != _increaseButton)
    {
        ++count;
    }

    if (NULL != _thumb)
    {
        ++count;
    }

    return count;
}

Element* Track::GetVisualChild(int index)
{
    return _visualChildren[index];
}

bool Track::OnReadingChild(IXamlNode* node)
{
    String name(node->GetName());

    if (node->HasNext())
    {
        IXamlNode* child = node->Current();

        if (name.Equals(_U("Track.DecreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _decreaseButton = RTTICast<RepeatButton>(elem.get());

            if (_decreaseButton)
            {
                _decreaseButton->ref();
            }

            UpdateComponent(NULL, _decreaseButton);
            return true;
        }
        else if (name.Equals(_U("Track.IncreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _increaseButton = RTTICast<RepeatButton>(elem.get());

            if (_increaseButton)
            {
                _increaseButton->ref();
            }

            UpdateComponent(NULL, _increaseButton);

            return true;
        }
        else if (name.Equals(_U("Track.Thumb")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _thumb = RTTICast<Thumb>(elem.get());

            if (_thumb)
            {
                _thumb->ref();
            }
            UpdateComponent(NULL, _thumb);
            return true;
        }
    }

    return false;
}

void Track::BindToTemplatedParent(DpProperty* target, DpProperty* source)
{
    if (!HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSourceRef(new RelativeSource(RelativeSourceMode::TemplatedParent));
        binding->SetPath(PropertyPath(source->GetName()));
        SetBinding(target, binding);
    }
}

void Track::BindChildToTemplatedParent(FrameworkElement* elem, DpProperty* target, DpProperty* source)
{
    if ((elem != NULL) && !elem->HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSource(GetTemplatedParent());
        binding->SetPath(PropertyPath(source->GetName()));
        elem->SetBinding(target, binding);
    }
}

void Track::OnPreApplyTemplate()
{
    FrameworkElement::OnPreApplyTemplate();
    RangeBase* templatedParent = RTTICast<RangeBase>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        BindToTemplatedParent(MinimumProperty, RangeBase::MinimumProperty);
        BindToTemplatedParent(MaximumProperty, RangeBase::MaximumProperty);
        BindToTemplatedParent(ValueProperty, RangeBase::ValueProperty);

        if (templatedParent->GetRTTIType() ==  ScrollBar::RTTIType())
        {
            BindToTemplatedParent(ViewportSizeProperty, ScrollBar::ViewportSizeProperty);
            BindToTemplatedParent(OrientationProperty, ScrollBar::OrientationProperty);
        }
        else if (templatedParent->GetRTTIType() ==  Slider::RTTIType())
        {
            BindToTemplatedParent(OrientationProperty, Slider::OrientationProperty);
            BindToTemplatedParent(IsDirectionReversedProperty, Slider::IsDirectionReversedProperty);
            BindChildToTemplatedParent(GetDecreaseButton(), RepeatButton::DelayProperty, Slider::DelayProperty);
            BindChildToTemplatedParent(GetDecreaseButton(), RepeatButton::IntervalProperty, Slider::IntervalProperty);
            BindChildToTemplatedParent(GetIncreaseButton(), RepeatButton::DelayProperty, Slider::DelayProperty);
            BindChildToTemplatedParent(GetIncreaseButton(), RepeatButton::IntervalProperty, Slider::IntervalProperty);
        }
    }
}

void Track::OnArrange(const Size& arrangeSize)
{
    // �ϱ�ҳ��С
    int upPageSize = 0;
    // �����С
    int thumbSize = 0;
    // �±�ҳ��С
    int downPageSize = 0;

    int iOrien = GetOrientation();
    bool isVertical = iOrien == Orientation::Vertical;
    int iViewSize = GetViewportSize();
    int d = max(0, iViewSize);
    DpObject* fe = GetTemplatedParent();

    if (iViewSize < 0)
    {
        ComputeSliderLengths(arrangeSize, isVertical, upPageSize, thumbSize, downPageSize);
    }
    else if (!ComputeScrollBarLengths(arrangeSize, isVertical, d, upPageSize, thumbSize, downPageSize))
    {
        return;
    }

    int dx = 0;
    int dy = 0;
    Size size = arrangeSize;
    bool isDirectionReversed = IsDirectionReversed();

    if (isVertical)
    {
        CoerceLength(upPageSize, arrangeSize.Height());
        CoerceLength(downPageSize, arrangeSize.Height());
        CoerceLength(thumbSize, arrangeSize.Height());
        dy = isDirectionReversed ? (upPageSize + thumbSize) : 0;
        size.cy = downPageSize;

        if (_increaseButton != NULL)
        {
            _increaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }

        dy = isDirectionReversed ? 0.0 : (downPageSize + thumbSize);
        size.cy = upPageSize;

        if (_decreaseButton != NULL)
        {
            _decreaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }

        dy = isDirectionReversed ? upPageSize : downPageSize;
        size.cy = thumbSize;

        if (_thumb != NULL)
        {
            _thumb->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }
        _thumbCenterOffset = dy + (thumbSize * 0.5);
        return ;
    }

    CoerceLength(upPageSize, arrangeSize.Width());
    CoerceLength(downPageSize, arrangeSize.Width());
    CoerceLength(thumbSize, arrangeSize.Width());
    dx = isDirectionReversed ? (downPageSize + thumbSize) : 0;
    size.cx = upPageSize;

    if (_decreaseButton != NULL)
    {
        _decreaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    dx = isDirectionReversed ? 0 : (upPageSize + thumbSize);
    size.cx = downPageSize;

    if (_increaseButton != NULL)
    {
        _increaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    dx = isDirectionReversed ? downPageSize : upPageSize;
    size.cx = thumbSize;

    if (_thumb != NULL)
    {
        _thumb->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    _thumbCenterOffset = dx + (thumbSize * 0.5);
}

Size Track::OnMeasure(const Size& constraintSize)
{
    Size desiredSize;

    if (GetThumb() != NULL)
    {
        GetThumb()->Measure(constraintSize);
        desiredSize = GetThumb()->GetDesiredSize();
    }

    if (GetViewportSize() > 0)
    {
        if (GetOrientation() == Orientation::Vertical)
        {
            desiredSize.cy = 0;
        }
        else
        {
            desiredSize.cx = 0;
        }
    }

    return desiredSize;
}

void Track::ComputeSliderLengths(Size arrangeSize, bool isVertical, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength)
{
    Float height = 0;
    Float minimum = GetMinimum();
    Float numLength = Math::Max(0, GetMaximum() - minimum);
    // ������ĵ�ǰֵ
    Float numPos = Math::Min(numLength, GetValue() - minimum);

    if (isVertical)
    {
        height = arrangeSize.Height();
        thumbLength = (GetThumb() == NULL) ? 0 : GetThumb()->GetDesiredSize().Height();
    }
    else
    {
        height = arrangeSize.Width();
        thumbLength = (GetThumb() == NULL) ? 0 : GetThumb()->GetDesiredSize().Width();
    }

    CoerceLength(thumbLength, height);
    Float trackLength = height - thumbLength;
    decreaseButtonLength = ((Float)(trackLength * numPos) / (Float)numLength);
    CoerceLength(decreaseButtonLength, trackLength);
    increaseButtonLength = trackLength - decreaseButtonLength;
    CoerceLength(increaseButtonLength, trackLength);

    _density = (Float)numLength / (Float)trackLength;
}

bool Track::ComputeScrollBarLengths(Size arrangeSize, bool isVertical, int viewportSize, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength)
{
    int height = 0;
    // �������Thumb��С
    int thumbRealLen = 0;
    Float minimum = GetMinimum();
    // �������߼���С
    // GetMaximum()��ʾ���Թ������߼��߶ȣ��ܵ��߼��߶ȼ������Ӹ߶ȣ�
    Float numLength = Math::Max(0, (GetMaximum() - minimum));
    // ��ǰ���ٵ��߼�λ��
    Float numPos = Math::Min(numLength, (GetValue() - minimum));
    // �����ܵ��߼��߶ȣ�����viewportSize���Ӹ߶ȣ�
    int numLogicLen = Math::Max(0, numLength) + viewportSize;

    if (numLogicLen == 0)
    {
        return false;
    }

    if (isVertical)
    {
        height = arrangeSize.Height();
        thumbRealLen = 16;

        if (thumbRealLen < _thumb->GetDesiredSize().Height())
        {
            thumbRealLen = _thumb->GetDesiredSize().Height();
        }
    }
    else
    {
        height = arrangeSize.Width();
        thumbRealLen = 16;

        if (thumbRealLen < _thumb->GetDesiredSize().Width())
        {
            thumbRealLen = _thumb->GetDesiredSize().Width();
        }
    }

    // ���㻬��Ĵ�С
    // ����height�൱��page���߼��߶�һҳ�Ĵ�С��
    // height��Track�ĳߴ磬viewportSize�ǹ������ߴ�
    thumbLength = (int)((Float) (height * viewportSize) / (Float) numLogicLen);
    // У����С�����ܴ��ڲ��ִ�С
    CoerceLength(thumbLength, height);
    // ������Сֵ����С��Ĭ�ϻ����С��һ��
    thumbLength = max(thumbRealLen, thumbLength);

    // ������Ч������
    bool bLessZero = (int)numLength <= 0;
    bool bGreateMax = thumbLength > height;

    if (bLessZero || bGreateMax)
    {
        _thumbCenterOffset = 0;
        _density = 0;
        decreaseButtonLength = 0;
        increaseButtonLength = 0;
        return false;
    }

    // ����ҳ�ܴ�С�����Ӵ�С���������С��
    int trackLength = height - (int)thumbLength;
    // ���ϱ߻����С
    decreaseButtonLength = (int)((Float) (trackLength * numPos) / (Float) numLength);
    CoerceLength(decreaseButtonLength, trackLength);
    // ���±߻����С
    increaseButtonLength = trackLength - decreaseButtonLength;
    CoerceLength(increaseButtonLength, trackLength);

    _density = (Float)numLength / (Float)trackLength;

    return true;
}

void Track::SetThumb(Thumb* val)
{
    UpdateComponent(_thumb, val);
    SETREFOBJ(_thumb, val);
}

void Track::SetDecreaseButton(RepeatButton* val)
{
    UpdateComponent(_decreaseButton, val);
    SETREFOBJ(_decreaseButton, val);
}

void Track::SetIncreaseButton(RepeatButton* val)
{
    UpdateComponent(_increaseButton, val);
    SETREFOBJ(_increaseButton, val);
}

RepeatButton* Track::GetDecreaseButton() const
{
    return _decreaseButton;
}

RepeatButton* Track::GetIncreaseButton() const
{
    return _increaseButton;
}

}
