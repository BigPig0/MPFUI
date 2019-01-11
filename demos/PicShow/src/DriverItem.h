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

    suic::String ToString()
    {
        suic::String strName;
        if (_files.GetCount() > 0)
        {
            strName.Format(_U("%s (%d)"), GetName().c_str(), _files.GetCount());
        }
        else
        {
            strName = GetName();
        }
        return strName;
    }

    void OnSetExpanded(bool val);
    void InitSubFolders();

    int GetPicCount() const;
    suic::String GetPicPath();

    void PrevImage();
    void NextImage();

private:

    bool _subInited;
    int _currentIndex;

    suic::String _name;
    suic::String _path;

    suic::Array<suic::String> _files;
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
