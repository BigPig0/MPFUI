// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����TiXmlWrapper.h
// ��  �ܣ�ʵ�����ԡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-08
// 
// ============================================================================

#ifndef _TIXMLWRAPPER_H_
#define _TIXMLWRAPPER_H_

#include <System/Windows/Object.h>
#include <System/Types/XamlNode.h>
#include <System/Types/StringCore.h>
#include <System/Markup/rapidxml/rapidxml_utils.hpp>

typedef rapidxml::xml_document<char> XmlDoc;
typedef rapidxml::xml_node<char> XmlNode;

namespace suic
{

class TiXmlWrapper
{
public:

    TiXmlWrapper();

    virtual ~TiXmlWrapper();
    
    XmlNode* GetRoot();

    bool LoadMemory(const Byte* data, int size);
    bool LoadUri(ResContext& resCtx, const String& uri);
    bool Save(String path);

    void Close();

    int GetLineNumber() const
    {
        return _xmlDoc.get_linenumber();
    }

protected:

    bool LoadAsXml(const Byte* xml, int iSize);

protected:

    XmlDoc _xmlDoc;
    XmlNode* _root;
    ByteStream _content;
};

}

#endif
