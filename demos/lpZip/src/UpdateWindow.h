// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����UpdateWindow.h
// ��  �ܣ�������½��洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-16
// 
// ============================================================================

#ifndef _UPDATEWINDOW_H_
#define _UPDATEWINDOW_H_

class UpdateAppData : public  suic::NotifyPropChanged
{
public:

    BeginMember(UpdateAppData, suic::NotifyPropChanged)
        MemberString(Version)
    EndMember()

    RTTIOfClass(UpdateAppData)

    DefineString(Version);
};

class UpdateAppWindow : public suic::Window
{
public:

    // �¼�ӳ��
    BeginRoutedEvent(UpdateAppWindow, suic::Window)
        MemberRouted(OnClickUpdate)
    EndRoutedEvent()

    UpdateAppWindow();
    ~UpdateAppWindow();

private:

    void UpdateWindowTitle();

private:

    void OnClickUpdate(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // �����Լ��Ŀؼ�����
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    UpdateAppData* _updateData;
};

#endif
