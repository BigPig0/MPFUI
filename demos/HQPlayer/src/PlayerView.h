// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����PlayerView.h
// ��  �ܣ����Ž���Ŀؼ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-01-02
// 
// ============================================================================

#ifndef _PLAYERVIEW_H_
#define _PLAYERVIEW_H_

#include <src/Video/VideoReader.h>

class PlayerView : public suic::FrameworkElement
{
public:

    PlayerView();
    ~PlayerView();

    void PostRender(BmpInfo* bmp);

protected:

    void Dispose();

    void OnRender(suic::Drawing* drawing);

private:

    void DrawVideo(suic::Drawing* drawing, suic::Bitmap* bmp);

private:

    BmpInfo* _bmp;
};

#endif
