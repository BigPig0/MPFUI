// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����UnzipWindow.h
// ��  �ܣ���ѹ���洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-16
// 
// ============================================================================

#ifndef _UNZIPWINDOW_H_
#define _UNZIPWINDOW_H_

#include "DriverItem.h"
#include <Framework/Controls/Menu.h>

class UnzipData : public  suic::NotifyPropChanged
{
public:

    BeginMember(UnzipData, suic::NotifyPropChanged)
        MemberInt(Second)
        MemberFloat(UnzipProgress)
        MemberGetInt(UnzipVisible)
        MemberGetInt(UnzippingVisible)
        MemberGetString(UnzipSpeed)
        MemberString(UnzipFile)
        MemberString(CurrentUnzipFile)
        MemberString(UnzipDir)
    EndMember()

    RTTIOfClass(UnzipData)

    DefineInt(Second);
    DefineFloat(UnzipProgress);
    DefineString(UnzipFile);
    DefineString(CurrentUnzipFile);
    DefineString(UnzipDir);

    UnzipData()
    {
        SetSecond(0);

        _unzipSpeed = 0.1f;
        _unzipVisible = suic::Visibility::Visible;
        _unzippingVisible = suic::Visibility::Collapsed;
    }

    void DoUnzip()
    {
        _unzipVisible = suic::Visibility::Collapsed;
        _unzippingVisible = suic::Visibility::Visible;
        // 
        // ֪ͨ���½���
        // 
        NotifyChanged("UnzipVisible");
        NotifyChanged("UnzippingVisible");
    }

    int GetUnzipVisible()
    {
        return _unzipVisible;
    }

    int GetUnzippingVisible()
    {
        return _unzippingVisible;
    }

    suic::String GetUnzipSpeed()
    {
        suic::String strSpeed;
        strSpeed.Format(_U("%.2fMB/s"), _unzipSpeed);
        return strSpeed;
    }

    void SetUnzipSpeed(suic::Float val)
    {
        _unzipSpeed = val;
        NotifyChanged("UnzipSpeed");
    }

private:
    
    int _unzipVisible;
    int _unzippingVisible;

    suic::Float _unzipSpeed;
};

class UnzipWindow : public suic::Window
{
public:

    // �¼�ӳ��
    BeginRoutedEvent(UnzipWindow, suic::Window)
        MemberRouted(OnClickChangeDir)
        MemberRouted(OnClickUnzip)
        MemberRouted(OnClickPause)
        MemberRouted(OnClickCancel)
        MemberRouted(OnCheckedDown)
    EndRoutedEvent()

    UnzipWindow();
    ~UnzipWindow();

private:

    void UpdateWindowTitle();

private:

    void OnClickChangeDir(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickUnzip(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickPause(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnCheckedDown(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // �����Լ��Ŀؼ�����
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    UnzipData* _unzipData;

    suic::String _selectedDir;
    DriverGroup* _driverGrp;
};

#endif
