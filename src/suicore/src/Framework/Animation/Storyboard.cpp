// =============================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// =============================================================

#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Storyboard.h>

/*--------�ڲ�ʹ��ͷ�ļ�--------*/
#include <System/Interop/Packaged.h>
#include <System/Windows/HwndObject.h>

namespace suic
{

const int UIANIMATE_INTERVAL = 30;

StoryBoard::StoryBoard()
    : _owner(NULL)
    , _state(0)
    , _timerid(0)
{
}

StoryBoard::~StoryBoard()
{
}

void StoryBoard::SetOwner(FrameworkElement* owner)
{
    _owner = owner;
}

FrameworkElement* StoryBoard::GetOwner()
{
    return _owner;
}

void StoryBoard::Start(Animate* ani)
{
    if (!ani->CanAnimate())
    {
        return;
    }

    // �������û��������������
    InternalStart();

    // ��⶯���Ƿ����
    UIMutex::Locker sync(_locker);
    if (!InternalContainsAnimate(ani))
    {
        _anis.push_back(ani);
    }

    if (ani->_state != 1)
    {
        ani->_state = 1;
        ani->OnStart(_owner);
    }
}

void StoryBoard::Pause(Animate* ani)
{
    UIMutex::Locker sync(_locker);
    if (InternalContainsAnimate(ani))
    {
        // ��ͣ����
        if (ani->_state == 1)
        {
            ani->_state = 2;
            ani->OnPause(_owner);
        }
    }
}

void StoryBoard::Stop(Animate* ani)
{
    UIMutex::Locker sync(_locker);
    if (InternalContainsAnimate(ani))
    {
        if (ani->_state != 0)
        {
            ani->_state = 0;
            ani->OnFinish(_owner);
        }
    }
}

void StoryBoard::CheckStoppedAnimate()
{
    for (;;)
    {
        bool bLoop = false;

        for (int i = 0; i < GetCount(); ++i)
        {
            AnimatePtr ani = GetAnimate(i);
            AnimateGroup* anigrp(dynamic_cast<AnimateGroup*>(ani.get()));

            // ������
            if (anigrp)
            {
            }
            else if (ani->IsFinish())
            {
                FrameworkElement* target(ani->GetTarget());
                if (target)
                {
                    target->WriteViewFlag(ViewFlags::IsInAnimating, false);
                    target->SetValue(target->AnimageTagProperty, NULL);
                }

                bLoop = true;
                _anis.erase(_anis.begin() + i);
                break;
            }
        }

        if (!bLoop)
        {
            break;
        }
    }

    if (_anis.size() == 0)
    {
        InternalStop();
    }
}

void StoryBoard::OnTimeout(int id, Object*)
{
    UIMutex::Locker sync(_locker);

    for (int i = 0; i < GetCount(); ++i)
    {
        Rect rect;
        AnimatePtr ani = GetAnimate(i);
        AnimateGroup* anigrp(dynamic_cast<AnimateGroup*>(ani.get()));
        
        // ������
        if (anigrp)
        {
            anigrp->OnAnimate(_owner, &rect);
        }

        // 
        // ������Ч����������
        //
        else if (ani && ani->IsRun() && ani->CanAnimate()
            && ani->CheckInterval((Float)UIANIMATE_INTERVAL))
        {
            ani->_interval = ani->GetInterval();
            ani->OnAnimate(_owner, &rect);
        }

        if (!rect.IsZero())
        {
            _owner->InvalidateRect(&rect, false);
        }
    }

    // ����˳��Ķ���
    CheckStoppedAnimate();
}

void StoryBoard::Resume(Animate* ani)
{
    UIMutex::Locker sync(_locker);
    if (InternalContainsAnimate(ani))
    {
        if (ani->_state == 2)
        {
            ani->_state = 1;
            ani->OnResume(_owner);
        }
    }
}

void StoryBoard::Add(Animate* ani)
{
    if (!ani->CanAnimate())
    {
        return;
    }

    UIMutex::Locker sync(_locker);
    if (!InternalContainsAnimate(ani))
    {
        _anis.push_back(ani);
    }
}

void StoryBoard::Remove(Animate* pAni)
{
    UIMutex::Locker sync(_locker);
    _anis.remove(pAni);
    if (_anis.size() == 0)
    {
        InternalStop();
    }
}

int StoryBoard::GetCount()
{
    UIMutex::Locker sync(_locker);
    return (int)_anis.size();
}

Animate* StoryBoard::GetAnimate(int index)
{
    UIMutex::Locker sync(_locker);
    return _anis[index].get();
}

void StoryBoard::Clear()
{
    UIMutex::Locker sync(_locker);
    _anis.clear();

    if (_owner)
    {
        //_owner->KillTimer(_timerid);
    }
}

bool StoryBoard::ContainsAnimate(Animate* ani)
{
    UIMutex::Locker sync(_locker);
    return InternalContainsAnimate(ani);
}

bool StoryBoard::InternalContainsAnimate(Animate* ani)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (ani == GetAnimate(i))
        {
            return true;
        }
    }

    return false;
}

void StoryBoard::InternalStart()
{
    if (0 == _timerid && _owner)
    {
        /*TimerGroup* tgp(GetTimerGroup(_owner, UI_SYSTIMERID, UI_SYSINTERVAL));
        tgp->AddTimer(UI_SYSINTERVAL, this, TimerGroup::TimerNormal);
        _timerid = UI_SYSTIMERID;
        _owner->WriteViewFlag(ViewFlags::IsInAnimating, true);*/
    }
}

void StoryBoard::InternalStop()
{
    if (0 != _timerid && _owner)
    {
        /*TimerGroup* tgp(GetTimerGroup(_owner, UI_SYSTIMERID, UI_SYSINTERVAL));
        tgp->RemoveTimer(this);
        _timerid = 0;
        _owner->WriteViewFlag(ViewFlags::IsInAnimating, false);*/
    }
}

}
