// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����cliprectmanager.h
// ��  �ܣ���װ��Ч�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UICLIPRECTMANAGER_H_
#define _UICLIPRECTMANAGER_H_

#include <System/Tools/Array.h>
#include <System/Types/Interface.h>
#include <System/Tools/SSE.h>

#define RECTMULITY(r) (r.Width() * r.Height())

namespace suic
{

const int MAXCLIPCOUNT = 1;

class HwndObject;
class InvManager
{
public:

    InvManager();
    ~InvManager();

    void Invalidate();
    void SetHwndObject(HwndObject* target);

    static void InvalidateElement(Element* elem, const Rect* lprc, bool force);

    bool PopClip(Rect& rect);
    bool PeekClip(Rect& rect);
    int AddClip(Rect rect);
    void CombineClips();
    int GetClipCount() const;
    int GetUpdateTick() const;

    void OnTimeout(int id, Object*);

protected:

    Uint32 _lastUpdate;
    int _curIndex;
    int _topIndex;
    Rect _rects[MAXCLIPCOUNT];
    HwndObject* _target;
    Mutex _mutex;
};

}

#endif
