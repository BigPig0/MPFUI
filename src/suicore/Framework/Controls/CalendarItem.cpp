// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Calendar.cpp

#include <Framework/Controls/CalendarItem.h>
#include <System/Tools/Debug.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(CalendarItem, Control)

CalendarItem::CalendarItem()
{
}

void CalendarItem::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    if (_previousButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnPreviousButtonClick);
        _previousButton->RemoveClick(&e);
    }
    if (_nextButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnNextButtonClick);
        _nextButton->RemoveClick(&e);
    }
    if (_headerButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnHeaderButtonClick);
        _headerButton->RemoveClick(&e);
    }

    _monthView = DynamicCast<Panel>(GetTemplateChild("PART_MonthView"));
    _yearView = DynamicCast<Panel>(GetTemplateChild("PART_YearView"));
    _previousButton = DynamicCast<Button>(GetTemplateChild("PART_PreviousButton"));
    _nextButton = DynamicCast<Button>(GetTemplateChild("PART_NextButton"));
    _headerButton = DynamicCast<Button>(GetTemplateChild("PART_HeaderButton"));
    _disabledVisual = DynamicCast<FrameworkElement>(GetTemplateChild("PART_DisabledVisual"));
    _dayTitleTemplate = NULL;
}

void CalendarItem::OnPreviousButtonClick(Element* sender, RoutedEventArg* e)
{
}

void CalendarItem::OnNextButtonClick(Element* sender, RoutedEventArg* e)
{
}

void CalendarItem::OnHeaderButtonClick(Element* sender, RoutedEventArg* e)
{
}

}
