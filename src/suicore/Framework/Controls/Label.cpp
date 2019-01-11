// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Label.cpp

#include <Framework/Controls/Label.h>
#include <System/Tools/Math.h>

namespace suic
{

ImplementRTTIOfClass(Label, ContentControl)

Label::Label()
{
}

Label::~Label()
{
}

void Label::StaticInit()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;
        ContentControl::StaticInit();
        IsEnabledProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));
    }
}

Label* Label::Clone()
{
    Label* pObj(DynamicCast<Label>(ContentControl::Clone()));
    return pObj;
}

}
