// ======================================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// TextPaint.cpp

#include <System/Graphics/TextPaint.h>
#include <System/Tools/Map.h>
#include <System/Interop/System.h>

namespace suic
{

TextPaint::TextPaint()
    : _textGlowColor(COLOR_NULL)
    , _vertTextAlign(0)
    , _ellipsis(false)
    , _singleLine(true)
    , _textStyle(TextStyle::Normal_TextStyle)
{
}

TextPaint::~TextPaint()
{
}

/*inline Handle TextPaint::GetFont() const
{
    if (_fcache)
    {
        return ((CTypeface*)_fcache)->font;
    }
    else
    {
        return _font;
    }
}

inline void TextPaint::SetTypeface(Typeface* fcrs)
{
    if (_fcache)
    {
        ((CTypeface*)_fcache)->unref();
    }
    _fcache = fcrs;
}

Typeface* TextPaint::GetTypeface() const
{
    return _fcache;
}*/

}
