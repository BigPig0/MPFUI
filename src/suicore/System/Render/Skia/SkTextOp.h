/// 
/// ʵ��Skia���ı�����
///
/// �ļ�����SkTextOp.h
/// ��  �ߣ�����
/// ��  �ڣ�2014-03-15
///  
/// ===============================================================
///

#ifndef _UISKTEXTOP_H_
#define _UISKTEXTOP_H_

#include <core/SkCanvas.h>
#include <core/SkBitmap.h>

#define CalcLineSpace(fm) (fm.fBottom - fm.fTop + fm.fLeading + 0.4)

class SkTextOp
{
public:

    enum TextFormat
    {
        tSingleLine=1,
        tTextCenter=2,
        tTextVCenter=4,
        tWrapText=8,
        tEllipsisText=0x10,
        tPathEllipsisText=0x20,
    };

	/// <summary>
	///  ͨ�����з�(\n��\r\n)�����ı�������
	/// </summary>
	/// <param name="paint">�ı����ƶ���</param>
	/// <param name="text">�ı�����</param>
	/// <param name="len">�ı�����</param>
	/// <returns>�ı�����</returns>
	static int ComputeTextLineCount(SkPaint& paint, const wchar_t* text, int len);

	/// <summary>
	///  ͨ�����з�(\n��\r\n)���ı����ƿ�ȼ����ı�������
	/// </summary>
	/// <param name="paint">�ı����ƶ���</param>
	/// <param name="w">�ı����ƿ��</param>
	/// <param name="text">�ı�����</param>
	/// <param name="len">�ı�����</param>
	/// <returns>�ı�����</returns>
	static int ComputeWrapTextLineCount(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len);
    static int ComputeTextLines(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len);

    static void DrawText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt);

	static SkScalar DrawSingleText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len);
	static SkPoint DrawLinesText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len);
	static SkPoint DrawWrapText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len);

	static SkScalar DrawEllipsisText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt);
	static SkScalar DrawPathEllipsisText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt);

private:

	static void CoereYCor(SkPaint& paint, SkScalar& y);
	static SkScalar DrawSingle(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar y, SkScalar w, SkScalar h, const wchar_t* text, int len);
	static int ComputeWrapTextLines(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len);
	static void DrawLimitText(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar w, const wchar_t* text, int len, SkScalar lineSpace, SkPoint& offset);
	static SkScalar DoDotTextOnly(SkCanvas* canvas, SkPaint& paint, const wchar_t* dotText, SkScalar x, SkScalar y, SkScalar w, SkScalar dotSize);
	static SkScalar InnerDrawEllipsisText(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar y, SkScalar w, const wchar_t* text, int len);
};

#endif
