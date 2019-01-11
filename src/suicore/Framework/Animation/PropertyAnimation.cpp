// =============================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// =============================================================

#include <Framework/Animation/PropertyAnimation.h>

namespace suic
{

PropertyAnimate::PropertyAnimate(DpProperty* property)
    : _property(property)
    , _relayout(false)
{
}

PropertyAnimate::~PropertyAnimate()
{

}

void PropertyAnimate::SetProp(DpProperty* prop)
{
    _property = const_cast<DpProperty*>(prop);
}

DpProperty* PropertyAnimate::GetProp()
{
    return _property;
}

}
