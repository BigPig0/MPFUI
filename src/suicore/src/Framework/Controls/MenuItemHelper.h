// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// MenuItem.cpp

#include <Framework/Controls/Menu.h>

namespace suic
{

class TrackingMenuOp : public Object
{
public:

    TrackingMenuOp();

    static TrackingMenuOp* Ins();

    void TrackContextMenu(Popup* popup, Element* menu, int x, int y);
    void TrackMenuPopup(Popup* popup, Element* item, Element* menu);
    void TrackSubMenuPopup(Popup* popup, Element* menu);

    void SetTrackingMenu(Popup* popup, Element* menu);

    bool HandleMouseDown(Handle hwnd, bool& bOverTree);
    bool HandleMouseUp(Handle hwnd, bool& bOverTree);
    bool HandleMouseMove(MessageParam* mp, bool& bOverTree);
    bool IsMouseOverOwer();

    MenuItem* GetMenuItem(Element* elem);

    void OnRootPopupClosing(Object* source, EventArg* e);
    bool OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt);

    void ClosePopup(Element* elem);
    void CloseTopPopup();

    void HideAllPopup();
    void CloseAllPopup(bool bAsync);

    Element* HitTestPopup(Handle hwnd, bool& bOver, bool& bOverTree);
    bool IsMouseCapture() const;
    void SetMouseCapture(bool value);

    void AddPopupMenuItem(Popup* popup);

    int GetTrackPopupCount() 
    {
        return _popups.GetCount();
    }

    Element* GetTrackingMenu()
    {
        return _trackingMenu;
    }

    Popup* GetRootPopup() const
    {
        return _popup;
    }

    Popup* GetTopPopup() const
    {
        if (_popups.GetCount() > 0)
        {
            return (Popup*)_popups.GetItem(_popups.GetCount() - 1);
        }
        else
        {
            return _popup;
        }
    }

    Element* GetTrackingMenuItem()
    {
        return _trackingMenuItem;
    }

    MenuItem* GetFocusMenuItem()
    {
        return _focusMenuItem;
    }

    bool IsInTracking()
    {
        return (NULL != _trackingMenu);
    }

protected:

    Popup* _popup;
    XArray _popups;

    // �����ĸ��˵�����
    Element* _trackingMenu;
    Element* _trackingMenuItem;
    MenuItem* _focusMenuItem;
    bool _mouseCapture;
};

#define GetMenuPopup() TrackingMenuOp::Ins()

}
