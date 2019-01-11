// ���ڿƼ���Ȩ���� 2010-2020
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// MPF�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
// ====================================================================

#include <Framework/Controls/RowDefinition.h>

namespace suic
{

ImplementRTTIOfClass(RowDefinition, DefinitionBase)

DpProperty* RowDefinition::MaxHeightProperty;
DpProperty* RowDefinition::MinHeightProperty;
DpProperty* RowDefinition::HeightProperty;

void RowDefinition::StaticInit()
{
    DefinitionBase::StaticInit();

    if (NULL == HeightProperty)
    {
        int iFlag = PropMetadataOptions::AffectsMeasure | PropMetadataOptions::AffectsParentMeasure;

        MaxHeightProperty = DpProperty::Register(_U("MaxHeight"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(Numeric::MeasureInt), iFlag, &RowDefinition::OnMaxHeightPropChanged));
        MinHeightProperty = DpProperty::Register(_U("MinHeight"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(0), iFlag, &RowDefinition::OnMinHeightPropChanged));
        HeightProperty = DpProperty::Register(_U("Height"), RTTIType(), GridSize::RTTIType()
            , DpPropMemory::GetPropMeta(new GridSize(GridSizeType::Star, 1.0f), iFlag, &RowDefinition::OnHeightPropChanged));

        HeightProperty->SetConvertValueCb(GridSizeConvert::Convert);
    }
}

void RowDefinition::OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    GridSize* gSize = RTTICast<GridSize>(e->GetNewValue());

    rowDef->SetUserSize(gSize, false);
}

void RowDefinition::OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    rowDef->SetUserMinSize(e->GetNewValue()->ToFloat(), false);
}

void RowDefinition::OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    rowDef->SetUserMaxSize(e->GetNewValue()->ToFloat(), false);
}

RowDefinition::RowDefinition()
    : DefinitionBase(false)
{
}

RowDefinition::~RowDefinition()
{

}

RowDefinitions::RowDefinitions()
{
}

RowDefinitions::~RowDefinitions()
{

}

}
