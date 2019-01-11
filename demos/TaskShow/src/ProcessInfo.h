// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����ProcessInfo.h
// ��  �ܣ������б���Ϣ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-02
// 
// ============================================================================

#ifndef _PROCESSINFO_H_
#define _PROCESSINFO_H_

class ProcessItem : public suic::NotifyPropChanged
{
public:

    BeginMember(ProcessItem, suic::NotifyPropChanged)
        MemberString(Name)
        MemberString(UserName)
        MemberInt(ProcessID)
    EndMember()

    RTTIOfClass(ProcessItem)

    DefineString(Name);
    DefineString(UserName);
    DefineInt(ProcessID);
};

class ProcessInfo
{
public:

    ProcessInfo();
    ~ProcessInfo();

    void RefleshProcessInfo();

    suic::ObservableCollection* GetProcessColl()
    {
        return _processColl;
    }

private:

    suic::ObservableCollection *_processColl;
};

#endif
