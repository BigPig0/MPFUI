// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����uidwmapi.h
// ��  �ܣ���װwin7��areoЧ����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIDWMAPI_H_
#define _UIDWMAPI_H_

#include <System/Windows/Object.h>

namespace suic
{

class UIDwmApi
{
public:

    static void EnabledComposition(bool enable);
    static bool IsCompositionEnabled();
    static void ExtendFrameIntoClientArea(HWND hwnd, Rect rect);

    static bool GetRenderNcWindow(HWND hwnd);
    static void SetRenderNcWindow(HWND hwnd, bool enable);

    static void EnableBlurBehindWindow(HWND hwnd, bool enable);
    static void EnableWindowThumbnail(HWND hwnd, bool enable);

    static bool IsValid();

private:

    UIDwmApi();
};

}

#endif
