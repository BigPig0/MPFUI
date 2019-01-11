// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����threadrender.h
// ��  �ܣ�Ĭ�ϻ��Ʒ�ʽ�����У�
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-12
// 
// ============================================================================

#ifndef _THREADRENDER_H_
#define _THREADRENDER_H_

#include <System/Windows/HwndObject.h>

namespace suic
{

struct RenderItem
{
    FrameworkElement* elem;
    Rect clip;
};

class RenderSplitUnit
{
public:

    Vector<RenderItem> rdItem;
};

class RenderSplit
{
public:

    // �ü�����
    Rect splitClip[8];
    RenderSplitUnit rdInfo[8];
};

class ThreadRender
{
public:

    static void RenderCanvas(FrameworkElement* root, Handle hdc, Bitmap* canvas, fRect clip);
    static void RenderVisualTree(FrameworkElement* elem, Drawing * drawing, fRect clip);
    static void RenderChild(FrameworkElement* elem, Drawing* drawing);

protected:

    static void DispatchRender(FrameworkElement* elem, Drawing * drawing);
    static void SplitRenderArea(FrameworkElement* elem, fRect clip, RenderSplit& rdInfo);
};

}

#endif
