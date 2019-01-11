// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// WindowHelper.cpp

#include <Framework/Controls/WindowHelper.h>
#include <System/Windows/RenderTarget.h>
#include <System/Windows/CoreTool.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// WndHelper
// 
WndHelper::WndHelper(Element* p) 
    : _owner(p)
{
}

WndHelper::~WndHelper()
{
}

bool WndHelper::OpenClipboard()
{
    VisualHostPtr pHost = VisualHost::GetVisualHost(_owner);

    if (!pHost)
    {
        return false;
    }

    HWND realwnd = HANDLETOHWND(pHost->GetHandle());

    if (::IsWindow(realwnd))
    {
        if (::OpenClipboard(realwnd))
        {
            return true;
        }
    }

#ifdef _DEBUG
    LPVOID lpMsgBuf;
    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        ::GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
        );
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Info"), MB_OK | MB_ICONINFORMATION );
    ::LocalFree( lpMsgBuf );
#endif

    return false;
}

void WndHelper::CloseClipboard()
{
    ::CloseClipboard();
}

int WndHelper::PasteText(String & text)
{
    // ���ȴ������Լ���������ݸ�ʽ
    String UI_EDIT_CFMSDEVCOLSEL(_T("UI_EDIT_BINARY"));
    CLIPFORMAT uCF_MSDEVColumnSelect = (CLIPFORMAT)::RegisterClipboardFormat(UI_EDIT_CFMSDEVCOLSEL.c_str());
    HANDLE hMemText = ::GetClipboardData(uCF_MSDEVColumnSelect);
    
    if (hMemText)
    {
        int nSize = (int)::GlobalSize(hMemText);
        BYTE * arColBlk = (BYTE*)::GlobalLock(hMemText);
    }

    HGLOBAL hmem = NULL;
    if (::IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        hmem = ::GetClipboardData(CF_UNICODETEXT);
    }

    if (hmem)
    {
        Char * pClipData = (Char *)::GlobalLock(hmem);
        DWORD dwLen = (DWORD)::GlobalSize(hmem);

        text = String(pClipData, (dwLen / 2) - 1);

#ifdef _DEBUG
        _CrtCheckMemory();
#endif
        ::GlobalUnlock(hmem);
    }

    return 0;
}

int WndHelper::CutText(String & text)
{
    int nRet = CopyText(text);
    if (0 == nRet)
    {
        ::EmptyClipboard();
    }
    return nRet;
}

int WndHelper::CopyText(const String & text)
{
    ::EmptyClipboard();

    // ����unicode�������洢�ַ�
    Uint32 dataSize = (Uint32)text.Length() * sizeof(Char);

    HGLOBAL hmem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSize + sizeof(TCHAR));
    LPVOID lpClipData = (LPVOID)::GlobalLock(hmem);

    if (NULL != lpClipData)
    {
        memcpy(lpClipData, text.c_str(), dataSize);
        ::ZeroMemory(((BYTE *)lpClipData + dataSize), sizeof(TCHAR));
    }

    int uCodePage = 0;

    // ������Ҫ���ֽ���
    int nLen  = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpClipData, -1, NULL, 0, NULL, NULL);

    HGLOBAL hMBCSMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (dataSize + 2));
    LPSTR lpMBCSSource = (LPSTR)::GlobalLock(hMBCSMem);

    int nBytes = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpClipData, -1, lpMBCSSource, nLen, NULL, NULL);

    assert(nBytes <= (int)nLen);

    lpMBCSSource[nBytes] = _T('\0');

    ::GlobalUnlock(hmem);
    ::GlobalUnlock(hMBCSMem);

    // ���Ǵ洢��unicode�ı�
    ::SetClipboardData(CF_UNICODETEXT, hmem);
    ::SetClipboardData(CF_TEXT, hMBCSMem);
    ::SetClipboardData(CF_OEMTEXT, hMBCSMem);

    return 0;
}

};
