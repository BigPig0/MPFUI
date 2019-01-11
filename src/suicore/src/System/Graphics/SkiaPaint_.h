// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SkiaPaint.h
// ��  �ܣ�Skia����������Ϣ��
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISKIAPAINT_H_
#define _UISKIAPAINT_H_

#include <System/Graphics/DrawInfo.h>
#include <Skia/core/SkPaint.h>
#include <Skia/core/SkTypeface.h>

#define CalcLineSpace(fm) (fm.fBottom - fm.fTop + fm.fLeading + 0.4)
#define DrawInfoToSkia(p) (((SkDrawMeta*)((p)->GetDrawMeta()))->GetPaint())

namespace suic
{

class SkDrawMeta : public DrawMeta
{
public:

    SkDrawMeta()
    {
    }

    ~SkDrawMeta()
    {
    }

    SkPaint& Paint()
    {
        return p;
    }

    void SetSingleLine(bool val)
    {

    }

    void SetTextAlign(int val)
    {

    }

    Size MeasureText(const Char* buff, int len)
    {
        Size size;
        SkPaint::FontMetrics fm;

        Paint().setTextEncoding(SkPaint::kUTF16_TextEncoding);
        Paint().getFontMetrics(&fm);

        size.cx = (int)(Paint().measureText(buff, len * 2) + 0.4);
        size.cy = CalcLineSpace(fm);

        return size;
    }

    Size MeasureText(Float w, const Char* text, int size);
    Size MeasureText(Float w, const Char* text, int size, int& realCount);

    SkPaint& GetPaint()
    {
        return p;
    }

private:

    SkPaint p;
};

}

#endif
