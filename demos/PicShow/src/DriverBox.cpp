/////////////////////////////////////////////////////
// DriverBox.cpp

#include "stdafx.h"
#include "DriverBox.h"

DriverBox::DriverBox()
{
}

DriverBox::~DriverBox()
{
}


void DriverBox::LoadDriver()
{
    // 
    // ��ȡ���̷���
    // 
    /*size_t alldriver =: :GetLogicalDriveStrings(0, NULL);     
    _TCHAR *driverstr;

    // 
    //����ռ�
    // 
    driverstr = new _TCHAR[alldriver + sizeof(_T(""))];

    if (GetLogicalDriveStrings(alldriver, driverstr) != alldriver - 1)
    {
        return;
    }

    _TCHAR *pdriverstr=driverstr;
    size_t driversize=strlen(pdriverstr);
    HTREEITEM disktree;

    // 
    // ѭ����ʾ����ͼ��
    // 
    while (driversize > 0)
    {
        SHGetFileInfo(pdriverstr,0,&fileinfo,sizeof(fileinfo),SHGFI_ICON);
        imindex=m_ImageList.Add(fileinfo.hIcon);
        disktree=m_Tree.InsertItem(pdriverstr,imindex,imindex,TVI_ROOT,TVI_LAST);
        pdriverstr+=driversize+1;
        driversize=strlen(pdriverstr);
    }*/
}
