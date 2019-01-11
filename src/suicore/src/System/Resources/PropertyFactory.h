// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����PropertyFactory.h
// ��  �ܣ���ȡ��������Դ������ֵ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-08-02
// 
// ============================================================================

#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include <System/Tools/Collection.h>
#include <System/Resources/Style.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class IPropReader : public Object
{
public:

    virtual ~IPropReader() {}

    virtual void ReadProp(RTTIOfInfo* owner, const String& prop, IXamlNode* node, ObjectPtr& obj) = 0;
};

struct AttachDpInfo
{
    DpProperty* dp;
    RTTIOfInfo* rttiInfo;
};

class PropFactory
{
public:

    static void ReadProp(RTTIOfInfo* owner, const String& prop, String val, ObjectPtr& obj);
    static void ReadProp(RTTIOfInfo* owner, const String& prop, IXamlNode* node, ObjectPtr& obj);

    static void RegisterPropReader(const String& prop, IPropReader* val);
    static IPropReader* FindPropReader(const String& prop);

    static bool ConvertAttachProp(const String& name, AttachDpInfo& attachDb);

    static void Clear();
    
private:

    PropFactory();

    static StringAutoDic<Object> _props;
};

}

#endif
