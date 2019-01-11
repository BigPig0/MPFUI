// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����MainWindow.h
// ��  �ܣ������洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "ZipInfo.h"

#include "ZipWindow.h"
#include "UnzipWindow.h"

#include "UpdateWindow.h"
#include "SettingWindow.h"

#include <Framework/Controls/Menu.h>

class ZipListView : public suic::ListView
{
public:

    RTTIOfClass(ZipListView)

    void RenderChildren(suic::Drawing* drawing);
};

class CompareInfo : public suic::Object
{
public:

    int iDesc;
    int iColumn;
    suic::String name;

    CompareInfo() 
        : suic::Object(false)
    {
        iColumn = 0;
        iDesc = -1;
    }
};

class ZipEntryCompare
{
public:

    static int Compare(suic::ItemEntry* t1, suic::ItemEntry* t2, suic::Object* flag)
    {
        CompareInfo* ci = (CompareInfo*)flag;
        ZipEntry* zt1 = (ZipEntry*) t1->GetItem();
        ZipEntry* zt2 = (ZipEntry*) t2->GetItem();

        int iRet = 0;
        
        if (ci->name.Equals("����"))
        {
            iRet = zt1->GetName().Compare(zt2->GetName());
        }
        else if (ci->name.Equals("����"))
        {
            iRet = zt1->GetZipType().Compare(zt2->GetZipType());
        }

        return iRet * ci->iDesc;
    }
};

class ZipFileColl : public suic::ObservableCollection
{
public:

    ZipFileColl()
    {
    }
};

class MainWindow : public suic::Window
{
public:

    // �¼�ӳ��
    BeginRoutedEvent(MainWindow, suic::Window)
        MemberRouted(OnClickAddToButton)
        MemberRouted(OnClickUnzipButton)
        MemberRouted(OnClickOnOneUnzipButton)
        MemberRouted(OnClickDeleteButton)
        MemberRouted(OnClickPwdButton)
        MemberRouted(OnClickUpLevelButton)
        MemberRouted(OnClickIconModeButton)
        MemberRouted(OnClickListModeButton)
    EndRoutedEvent()

    MainWindow();
    ~MainWindow();

private:

    void OnClickAddToButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickUnzipButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickOnOneUnzipButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickDeleteButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickPwdButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickUpLevelButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickIconModeButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickListModeButton(suic::DpObject* sender, suic::RoutedEventArg* e);
    
    void OnListViewColomnClicked(suic::Element* sender, suic::RoutedEventArg* e);
    void OnListViewItemDbClck(suic::Element* sender, suic::MouseButtonEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // �����Լ��Ŀؼ�����
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void AddTestData();

    void HideListViewSortIcon();
    void SortListViewColumn(suic::GridViewColumnHeader* colHeader);

    void UnzipFile(suic::String strPath);

    void OnRefleshTimer(suic::Object* sender, suic::EventArg* e);
    void OnMainMenuClick(suic::Element* sender, suic::RoutedEventArg* e);

private:

    ZipFileColl* _zipColl;
    ZipListView* _zipListView;

    suic::Menu* _mainMenu;
    suic::ListBox* _zipListBox;

    ZipInfo* _zipInfo;
    suic::AssignerTimer* _timer;
};

#endif
