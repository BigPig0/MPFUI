// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����WndHelper.h
// ��  �ܣ����ڲ�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-09-23
// 
// ============================================================================

#ifndef _UIWNDHELPER_H_
#define _UIWNDHELPER_H_

#include <Framework/Controls/Control.h>

namespace suic
{

class WndHelper
{
public:

    WndHelper(Element* p);
    virtual ~WndHelper();

    bool OpenClipboard();
    void CloseClipboard();

    int PasteText(String & text);
    int CutText(String & text);
    int CopyText(const String & text);

protected:

    Element* _owner;
};

};

#endif
