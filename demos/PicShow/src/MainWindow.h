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

#include "DriverItem.h"

class MainWindow : public suic::Window
{
public:

    BeginRoutedEvent(MainWindow, suic::Window)
        MemberRouted(OnClickPrevImage)
        MemberRouted(OnClickNextImage)
        MemberRouted(OnTreeViewSelectedItemChanged)
    EndRoutedEvent()

    MainWindow();
    ~MainWindow();

    void ShowImage(DriverItem* pItem);

    void OnLoaded(suic::LoadedEventArg* e);

protected:

    void OnClickPrevImage(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickNextImage(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnTreeViewSelectedItemChanged(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    // �����Լ��Ŀؼ�����
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void OnRefleshTimer(suic::Object* sender, suic::EventArg* e);

private:

    DriverGroup* _driverGrp;
    suic::TreeView* _tvDrivers;

    // DriverLevel* _driverLevel;

    // ��ʱ��������ˢ�½����б�
    suic::AssignerTimer* _timer;
};

#endif
