// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DataTemplate.cpp

#include <Framework/Controls/DataTemplate.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(DataTemplate, FrameworkTemplate)

DataTemplate::DataTemplate()
    : _dataType(NULL)
    , _triggers(NULL)
{
    _dataType = ContentPresenter::RTTIType();
}

DataTemplate::DataTemplate(RTTIOfInfo* targetType) 
{
    _dataType = targetType;
    _triggers = NULL;
}

DataTemplate::~DataTemplate()
{

}

TriggerCollection* DataTemplate::GetTriggers()
{
    if (!_triggers)
    {
        _triggers = new TriggerCollection();
    }
    return _triggers;
}

void DataTemplate::ValidateTemplatedParent(FrameworkElement* templatedParent)
{ 
    if (templatedParent == NULL)
    { 
        throw ArgumentNullException(_T("templatedParent"), __FILELINE__);
    }

    // ����ģ�������һ��ContentPresenter 
    if (!(templatedParent->GetRTTIType()->InheritFrom(ContentPresenter::RTTIType())))
    { 
        throw ArgumentException(templatedParent->GetRTTIType()->typeName, _U("TemplateTargetTypeMismatch"), __FILELINE__); 
    }
}

void DataTemplate::ReadDataTemplateTriggers(const TemplateReadInfo* info, IXamlNode* node)
{

}

}
