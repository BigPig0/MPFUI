// ���ڿƼ���Ȩ���� 2010-2021
// 
// �ļ�����AudioPlayThread.h
// ��  �ܣ������洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-01-02
// 
// ============================================================================

#ifndef _AUDIOPLAYTHREAD_H_
#define _AUDIOPLAYTHREAD_H_

class AudioPlayThread : public suic::Thread
{
public:

    AudioPlayThread();
    ~AudioPlayThread();

    void Run();
};

#endif
