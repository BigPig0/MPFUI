// ���ڿƼ���Ȩ���� 2010-2021
// 
// �ļ�����VideoPlay.h
// ��  �ܣ������洰�ڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-01-02
// 
// ============================================================================

#ifndef _VIDEOPLAY_H_
#define _VIDEOPLAY_H_

class VideoPlay : public suic::Thread
{
public:

    VideoPlay();
    ~VideoPlay();

    void Run();
};

#endif
