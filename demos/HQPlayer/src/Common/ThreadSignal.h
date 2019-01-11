// ���ڿƼ���Ȩ���� 2010-2021
// 
// �ļ�����ThreadSignal.h
// ��  �ܣ��̰߳�ȫ���С�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-01-02
// 
// ============================================================================

#ifndef __XPSIGNAL_H
#define __XPSIGNAL_H

namespace core
{

class Signal
{
public:

	Signal(bool bSignal=true);
	Signal(bool bMenuReset,bool bSignal);

	~Signal();

	operator HANDLE() const;

	bool Wait(int iTimeout=INFINITE);

    /**
    * ��  �ܣ��ȴ��ڲ����ⲿ�¼���
    * 
    * ��  ����
    *         _Other - �ⲿ�¼�
    *         iTimeout - ��ʱʱ��
    *	
    * ����ֵ����������¼�����,����1;����ⲿ�¼�����,����2;��ʱ����3;����-1
    * ��  ������
    */
    int Wait(core::Signal& _Other, int iTimeout=INFINITE);

	void Notify();

	void Reset();

private:

	HANDLE signal_cond;
};

}

#endif
