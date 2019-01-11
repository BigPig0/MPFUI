// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// uiroutedevent.cpp


#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/EventManager.h>

namespace suic
{

RoutedEvent::RoutedEvent(const String& name, RoutingStrategyType routStrategy, RTTIOfInfo* handlerType, RTTIOfInfo* ownerType)
{
    _name = name;
    _routStrategy = routStrategy; 
    _ownerType = ownerType;
    _handlerType = handlerType;
    _hashCode = suic::DpProperty::CalcHashCode(_name, _ownerType);

    _globalIndex = EventControl::GetNextGlobalIndex(this);
}

RoutingStrategyType RoutedEvent::GetRoutingStrategy() const
{
    return _routStrategy;
}

RTTIOfInfo* RoutedEvent::GetOwnerType() const
{
    return _ownerType;
}

RTTIOfInfo* RoutedEvent::GetHandlerType() const
{
    return _handlerType;
}

String RoutedEvent::GetName() const
{
    return _name;
}

int RoutedEvent::GetIndex() const
{
    return _globalIndex;
}

int RoutedEvent::GetHashCode() const
{
    return _hashCode;
}

}
