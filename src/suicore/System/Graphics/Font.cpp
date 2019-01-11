// ======================================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// uifont.cpp

#include <System/Graphics/Font.h>
#include <System/Tools/Map.h>
#include <System/Tools/Debug.h>
#include <System/Interop/System.h>

#include <System/Graphics/SuiTypeface.h>

namespace suic
{

static const DWORD cccsMost = 16;
static const DWORD QUICKCRCSEARCHSIZE = 31;
static DWORD g_dwAgeNext = 0;

static CTypeface _rgccs[cccsMost];

struct
{
    CTypeface* fcrs;
    Byte crs;
}quickCrcSearch[QUICKCRCSEARCHSIZE + 1];

void __suiInitFontCache()
{
    for (int i = 0; i < cccsMost; i++ )
    {
        _rgccs[i].font = 0;
        _rgccs[i].dwage = 0;
        _rgccs[i].refcount = 0;
        _rgccs[i].valid = false;
        _rgccs[i].crs = 0;
    }
}

int __CalcuCfcrs(const Font* font)
{
    int iSize = font->GetSize();
    Char* family = font->GetFamily().c_str();

    suic::Byte bCrc = 0;
    suic::Byte* pb = NULL;
    suic::Byte* pend = (suic::Byte*)&iSize;
    suic::Char* pFaceName = NULL;
    int effects = 0;
    
    // Italic=0x0001, Underline=0x0002, StrikeOut=0x0004, Bold=0x0008
    if (font->GetItalic())
    {
        effects |= FontStyle::Italic;
    }

    if (font->GetUnderline())
    {
        effects |= FontStyle::Underline;
    }

    if (font->GetWeight() > 0)
    {
        effects |= FontStyle::Bold;
    }

    for (pb = (BYTE*)&iSize; pb < pend; pb++)
    {
        bCrc ^= *pb;
    }

    for (pb = (BYTE*)&effects; pb < pend; pb++)
    {
        bCrc ^= *pb;
    }

    pend = (suic::Byte*)&family;

    /*for (pb = (suic::Byte*)&cs.charSet; pb < pend; pb++)
    {
        bCrc ^= *pb;
    }*/

    pend = (suic::Byte*)&family + sizeof(font->GetFamily().Length());

    for (pb = (suic::Byte*)&family
        , pFaceName = (suic::Char*)&family; 
        *pFaceName && pb < pend; pFaceName++)
    {
        bCrc ^= *pb++;
        bCrc ^= *pb++;
    }

    if (!bCrc )
    {
        bCrc++;
    }

    return bCrc;
}

CTypeface* __InitNewCcs(const Font *const font)
{
    DWORD dwAgeOldest = 0xffffffff;
    CTypeface *fcrs = NULL;
    const CTypeface * const pccsMost = &_rgccs[cccsMost - 1];
    CTypeface *pccsOldest = NULL;

    //
    // �ҵ����в���ʱ�������������ʽ
    //
    for (fcrs = &_rgccs[0]; fcrs <= pccsMost && fcrs->valid; fcrs++)
    {
        if (fcrs->refcount == 0 && fcrs->dwage < dwAgeOldest)
        {
            dwAgeOldest = fcrs->dwage;
            pccsOldest = fcrs;
        }
    }

    // ����û���ҵ�,����ʹ�������
    if (fcrs > pccsMost)
    {
        fcrs = pccsOldest;
        if (!fcrs)
        {
            Debug::Assert(FALSE, "FontCache::InitNewCcs oldest entry is NULL");
            return NULL;
        }
    }

    if (fcrs->font)
    {
        ::DeleteObject(HandleToHdc(fcrs->font));
    }

    // ���µ�������ʽ��ʼ��

    HDC dc = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);

    ZeroMemory(&fcrs->lf, sizeof(LOGFONT));

    fcrs->lf.lfCharSet = (BYTE)::GetTextCharsetInfo (dc, NULL, 0);
    _tcsncpy(fcrs->lf.lfFaceName, font->GetFamily().c_str(), font->GetFamily().Length());

    fcrs->lf.lfEscapement = font->GetAngle() * 10;
    fcrs->lf.lfItalic = font->GetItalic();
    fcrs->lf.lfHeight = font->GetSize();
    fcrs->lf.lfWeight = font->GetWeight();
    fcrs->lf.lfUnderline = font->GetUnderline();

    // ����ɾ����
    if (font->GetType() & 0x0004)
    {
        fcrs->lf.lfStrikeOut = 1;
    }

    fcrs->font = (Handle)(DWORD_PTR)::CreateFontIndirect(&fcrs->lf);

    //-----------------------------------------------------------

    Mulstr strFontName(font->GetFamily().c_str());
    SkTypeface::Style sty = SkTypeface::kNormal;

    if (fcrs->lf.lfWeight > 0)
    {
        sty = SkTypeface::kBold;
    }
    else if (fcrs->lf.lfItalic > 0)
    {
        sty = SkTypeface::kItalic;
    }
    else if (fcrs->lf.lfWeight > 0 && fcrs->lf.lfItalic > 0)
    {
        sty = SkTypeface::kBoldItalic;
    }
    fcrs->typeface = SkTypeface::CreateFromName(strFontName.c_str(), sty);

    //-----------------------------------------------------------

    ::DeleteDC(dc);

    // ���ü�1
    fcrs->ref();
    if (fcrs->typeface)
    {
        fcrs->typeface->unref();
    }

    return fcrs;
}

CTypeface* __FindFontCrs(Font* font, Byte crs)
{
    CTypeface* fcrs = NULL;
    int hashKey = crs & QUICKCRCSEARCHSIZE;

    // �ȴ����ȱ���Ŀ��ٲ��ұ��ж������Ƿ�
    // �Ѿ����棬�������ֱ�ӷ���
    // ��Ӧ������У��ͱ�����ͬ
    if (crs == quickCrcSearch[hashKey].crs)
    {
        // �õ������������Ϣ
        fcrs = quickCrcSearch[hashKey].fcrs;
        if (fcrs && fcrs->crs == crs && fcrs->valid 
            && fcrs->size == font->GetSize())
        {
            fcrs->ref();
            return fcrs;
        }
    }

    const CTypeface* const pccsMost = &_rgccs[cccsMost - 1];

    quickCrcSearch[hashKey].crs = crs;

    for (fcrs = &_rgccs[0]; fcrs <= pccsMost; fcrs++)
    {
        if (fcrs->crs == crs && fcrs->valid && fcrs->size == font->GetSize())
        {
            fcrs->dwage = g_dwAgeNext++;
            fcrs->ref();

            quickCrcSearch[hashKey].fcrs = fcrs;
            return fcrs;
        }
    }

    fcrs = __InitNewCcs(font);
    fcrs->crs = crs;
    fcrs->size = font->GetSize();
    fcrs->valid = true;
    quickCrcSearch[hashKey].fcrs = fcrs;

    return fcrs;
}

ImplementTypeofClass(Font, Resource);

Font::Font()
    : _size(12)
    , _italic(0)
    , _underline(0)
    , _weight(0)
    , _name(_T("����"))
    , _angle(0)
    , _type(0)
    , _crs(0)
    , _h(0)
{
}

Font::~Font()
{
    if (_h)
    {
        ::DeleteObject(HandleToHdc(_h));
    }
}

Font* Font::Clone() const
{
    return const_cast<Font*>(this);
}

void Font::Reset()
{
    _crs = 0;
    if (_h)
    {
        ::DeleteObject(HandleToHdc(_h));
        _h = 0;
    }
}

Handle Font::GetHandle()
{
    return _h;
}

Typeface* Font::GetTypeface()
{
    if (_crs <= 0)
    {
        _crs = __CalcuCfcrs(this);
    }

    // �ҵ������е�����
    CTypeface* fcrs = __FindFontCrs(this, _crs);
    return fcrs;
}

//Handle Font::GetFont()
//{
//    if (_crs <= 0)
//    {
//        _crs = __CalcuCfcrs(this);
//    }
//
//    // �ҵ������е�����
//    CFontCrs* fcrs = __FindFontCrs(this, _crs);
//
//    if (fcrs == NULL)
//    {
//        return NULL;
//    }
//    else
//    {
//        return fcrs->font;
//    }
//}

inline void Font::SetSize(Int32 size)
{
    if (_size != size)
    {
        _size = size;
        _crs = 0;
    }
}

inline void Font::SetFamily(String name)
{
    if (name.Equals(_name))
    {
        _name = name;
        _crs = 0;
    }
}

inline void Font::SetWeight(Int32 weight)
{
    if (_weight != weight)
    {
        if (weight != 0)
        {
            _weight = 900;
        }
        else
        {
            _weight = 0;
        }
        _crs = 0;
    }
}

inline void Font::SetItalic(Byte value)
{
    if (_italic != value)
    {
        _italic = value;
        _crs = 0;
    }
}

inline void Font::SetUnderline(Byte value)
{
    if (_underline != value)
    {
        _underline = value;
        _crs = 0;
    }
}

inline void Font::SetAngle(int val)
{
    if (_angle != val)
    {
        _angle = val;
        _crs = 0;
    }
}

inline void Font::SetType(Uint32 val)
{
    _type = val;
}

String Font::ToString()
{
    String strXml;
//
//    strXml = _T("<Font ");
//
//    strXml += _T("Key=\"");
//    strXml += _key;
//    strXml += _T("\" ");
//
//    strXml += _T("\" Family=\"");
//    strXml += _name;
//    strXml += _T("\" ");
//
//    strXml += _T("\" Size=\"");
//    strXml += String((Int32)_size).ToString();
//    strXml += _T("\" ");
//
//    strXml += _T("\" Weight=\"");
//    strXml += String((Int32)_weight).ToString();
//    strXml += _T("\" ");
//
//    strXml += _T("\" Italic=\"");
//    strXml += String((Int32)_italic).ToString();
//    strXml += _T("\" ");
//
//    strXml += _T("\" Underline=\"");
//    strXml += String((Int32)_underline).ToString();
//    strXml += _T("\" ");
//
//    strXml = _T("/>");
//
    return strXml;
}

}
