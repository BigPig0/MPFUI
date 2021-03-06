/// 
/// 实现Skia的文本操作
///
/// 文件名：SkTextOp.h
/// 作  者：汪荣
/// 日  期：2014-03-15
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
	///  通过换行符(\n或\r\n)计算文本的行数
	/// </summary>
	/// <param name="paint">文本绘制对象</param>
	/// <param name="text">文本对象</param>
	/// <param name="len">文本长度</param>
	/// <returns>文本行数</returns>
	static int ComputeTextLineCount(SkPaint& paint, const wchar_t* text, int len);

	/// <summary>
	///  通过换行符(\n或\r\n)和文本限制宽度计算文本的行数
	/// </summary>
	/// <param name="paint">文本绘制对象</param>
	/// <param name="w">文本限制宽度</param>
	/// <param name="text">文本对象</param>
	/// <param name="len">文本长度</param>
	/// <returns>文本行数</returns>
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
