// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����SettingWindow.h
// ��  �ܣ�������ý��洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-16
// 
// ============================================================================

#ifndef _SETTINGWINDOW_H_
#define _SETTINGWINDOW_H_

class SettingData : public  suic::NotifyPropChanged
{
public:

    BeginMember(SettingData, suic::NotifyPropChanged)
        MemberString(Version)
    EndMember()

    RTTIOfClass(SettingData)

    DefineString(Version);
};

class ZipSetWindow : public suic::Window
{
public:

    // �¼�ӳ��
    BeginRoutedEvent(ZipSetWindow, suic::Window)
        MemberRouted(OnClickApply)
        MemberRouted(OnClickCancel)
        MemberRouted(OnClickOk)
    EndRoutedEvent()

    ZipSetWindow();
    ~ZipSetWindow();

private:

    void OnClickApply(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickOk(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // �����Լ��Ŀؼ�����
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    SettingData* _settingData;
};

#endif
