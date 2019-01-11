// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����plugmanager.h
// ��  �ܣ��������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-12
// 
// ============================================================================

#ifndef _UIPLUGMANAGER_H_
#define _UIPLUGMANAGER_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Types/Interface.h>
#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>

namespace suic
{

class PlugManager
{
public:

    static PlugManager* Ins();

    PlugManager();
    ~PlugManager();

    void AddPlug(const String& path);
    void RemovePlug(const String& path);
    bool ContainsPlug(const String& path);

    void LoadPlugXml(const String& xml);
    void InitPlugs();
    void ClearPlugs();

protected:

    bool _isInit;
    StringDic<HMODULE> _mapPlug;
};

}

#endif
