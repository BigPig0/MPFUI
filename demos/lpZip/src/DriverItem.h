// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����DriverItem.h
// ��  �ܣ�Ŀ¼�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-02
// 
// ============================================================================

#ifndef _DRIVERITEM_H_
#define _DRIVERITEM_H_

#include "FileFinder.h"
#include "ShellHelper.h"

class DriverItem : public suic::HierarchicalItem
{
public:

    BeginMember(DriverItem, suic::HierarchicalItem)
        MemberGetString(Name)
    EndMember()

    DriverItem(suic::String name, suic::String path, bool subInited);
    DriverItem(suic::String name, suic::String path);

    suic::String GetName()
    {
        return _name;
    }

    suic::String GetFilePath()
    {
        return _path;
    }

    suic::String ToString()
    {
        return GetName();
    }

    void OnSetExpanded(bool val);
    void InitSubFolders();

    bool IsLeafItem()
    {
        return (_subInited && 0 == GetChildren()->GetCount());
    }

private:

    bool _subInited;

    suic::String _name;
    suic::String _path;
};

class DriverGroup : public suic::ObservableCollection
{
public:

    DriverGroup()
    {
    }

    void InitRootItems();

private:

    void InitMyFolder(LPSHELLFOLDER lsfDesk, int clsid);
    void InitMyComputer(LPSHELLFOLDER lsfDesk);
    void InitChildDrivers(DriverItem* pParent, LPSHELLFOLDER lpsf, LPITEMIDLIST itemIdl, LPENUMIDLIST lpe);

    suic::String GetName(LPITEMIDLIST itemIdl);
    suic::String GetPath(LPSHELLFOLDER lpsf, LPITEMIDLIST itemIdl);
};

#endif
