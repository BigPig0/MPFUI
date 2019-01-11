// ======================================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

#include <System/Windows/CoreTool.h>
#include <Framework/Controls/Window.h>
#include <Framework/Animation/Animation.h>
#include <Framework/Animation/Storyboard.h>

namespace suic
{

Transform::Transform()
    : radio(0)
    , alpha(1)
    , widRadio(1)
    , heiRadio(1)
    , frame(NULL)
    , Object(false)
{
}

Transform::~Transform()
{
    if (frame)
    {
        frame->__unref();
    }
}

void Transform::SetBitmap(Bitmap* bmp)
{
    SetRefObject(frame, bmp);
}

bool Transform::TransformPoint(Point inPoint, Point& result)
{
    return false;
}

bool Transform::TransformRect(Rect inRect, Rect& result)
{
    return false;
}

//-----------------------------------------------------
//

AnimateNotifyArg::AnimateNotifyArg(int iOper)
    : _oper(iOper)
{
}

bool AnimateNotifyArg::IsStart()
{
    return (_oper == 1);
}

bool AnimateNotifyArg::IsFinish()
{
    return (_oper != 1 && _oper != 2);
}

bool AnimateNotifyArg::IsPause()
{
    return (_oper == 2);
}

//-----------------------------------------------------
//

Animate::Animate()
    : _state(0)
    , _restore(false)
    , _target(NULL)
    , _cacheLayer(true)
    , _interpolatedTime(0)
    , _offsetTime(0)
    , _stayAnimate(false)
{
}

Animate::Animate(FrameworkElement* target)
    : _state(0)
    , _restore(false)
    , _target(target)
    , _cacheLayer(true)
    , _interpolatedTime(0)
    , _offsetTime(0)
    , _stayAnimate(false)
{
}

Animate::~Animate()
{
}

Float Animate::GetInterpolation()
{
    if (_interpolatedTime < 0)
    {
        _interpolatedTime = 0;
    }
    else if (_interpolatedTime > 1.0)
    {
        _interpolatedTime = 1.0;
    }

    return _interpolatedTime;
}

Float Animate::ComputeAnimateSpeed()
{
    Float dRatio = _offsetTime / (GetDuration() * 1000);
    Float localSpeed = ComputeInterpolation(dRatio);
    return localSpeed;
}

Float Animate::ComputeInterpolation(Float t)
{
    Float resolveDuration = GetDuration() * 1000;
    Float localSpeed = 1.0;
    Float userAcceleration = GetAccelerationRatio(); 
    Float userDeceleration = GetDecelerationRatio();
    Float transitionTime = userAcceleration + userDeceleration;

    if (t >= 1)
    {
        _interpolatedTime = 1;
        //_currentTime = resolveDuration;
        return 1.0;
    }

    // �޼��ٺͼ���
    if (transitionTime == 0)
    { 
        localSpeed = 1;
    }
    else
    {
        Float maxRate = 2 / (2 - transitionTime); 

        // ���ڼ��ٽ׶�
        if (t < userAcceleration) 
        {
            localSpeed = maxRate * t / userAcceleration; 
            t = maxRate * t * t / (2 * userAcceleration);
        }
        // �������ٽ׶�
        else if (t <= (1 - userDeceleration))
        { 
            localSpeed = maxRate;
            t = maxRate * (t - userAcceleration / 2); 
        }
        // ���ڼ��ٽ׶�
        else
        {
            Float tc = 1 - t;
            localSpeed = maxRate * tc / userDeceleration; 
            t = 1 - maxRate * tc * tc / (2 * userDeceleration); 
        }

        // �����ѹ�ʱ��ֵ
        //_currentTime = t * resolveDuration + 0.5;
    } 

    _interpolatedTime = t;

    return localSpeed;
}

void Animate::SetRestoreLayered(bool val)
{
    _restore = val;
}

bool Animate::IsFinish()
{
    return (!IsRun() && !IsPause());
}

bool Animate::IsRun()
{
    return _state == 1;
}

bool Animate::IsPause()
{
    return _state == 2;
}

void Animate::Start()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Start(this);
    }
    _state = 1;
}

void Animate::Stop()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Stop(this);
    }
    _state = 0;
}

void Animate::Pause()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Pause(this);
    }
}

FrameworkElement* Animate::GetTarget()
{
    return _target;
}

void Animate::SetTarget(FrameworkElement* target)
{
    _target = target;
}

Float Animate::GetInterval()
{
    //
    // ����ʱ�们������һ֡
    //
    _offsetTime += GetFrameRate() * GetSpeedRatio() + 0.5;

    // 
    // ���㶯���ٶ��Ѿ���ֵ
    //
    ComputeAnimateSpeed();

    return GetFrameRate();
}

bool Animate::CanAnimate()
{
    return true;
}

bool Animate::GetTransform(Transform* trans)
{
    bool isrun = IsRun();
    DoTransform(GetInterpolation(), trans);
    return isrun;
}

void Animate::DoTransform(Float interTime, Transform* trans)
{
}

void Animate::OnAnimate(FrameworkElement* owner, Rect* rcvalid)
{
    // �������ʱֹͣ
    if (GetInterpolation() >= 1)
    {
        ++_repeatcount;

        switch (GetRepeatMode())
        {
        case Timeline::RepeatDuration:
            Stop();
            break;

        case Timeline::IterationCount:
            Stop();
            break;

        case Timeline::Forever:
            _repeatcount = 0;
            _offsetTime = 0.0;
            break;

        default:
            ;
        }
    }

    // ����ж����㷨���������㷨��Ŀ��Ԫ����
    if (_target)
    {
        *rcvalid = suic::GetRenderBound(_target);
    }
}

void Animate::OnStart(FrameworkElement* owner)
{
    Window* targetWnd(dynamic_cast<Window*>(_target));
    _offsetTime = -GetFrameRate();
    _state = 1;

    if (_restore && targetWnd)
    {
        if (!targetWnd->AllowsTransparency())
        {
            targetWnd->SetAllowsTransparency(true);
        }
        else
        {
            _restore = false;
        }
    }

    NotifyAnimate(_state);
}

void Animate::OnFinish(FrameworkElement* owner)
{
    _state = 0;
    Window* targetWnd(dynamic_cast<Window*>(_target));

    if (_restore && targetWnd)
    {
        targetWnd->SetAllowsTransparency(false);
    }
    NotifyAnimate(_state);
}

void Animate::OnPause(FrameworkElement* owner)
{
    _state = 2;
    NotifyAnimate(_state);
}

void Animate::OnResume(FrameworkElement* owner)
{
    if (_state == 2)
    {
        _state = 1;
        NotifyAnimate(_state);
    }
}

void Animate::NotifyAnimate(int state)
{
    if (AnimateNotify.safe_bool())
    {
        AnimateNotifyArg e(state);
        AnimateNotify(this, e);
    }
}

void Animate::AddAnimateNotify(AnimateNotifyHandler handler)
{
    AnimateNotify += handler;
}

void Animate::RemoveAnimateNotify()
{
    AnimateNotify.clear();
}

//========================================================
// AnimateGroup

AnimateGroup::AnimateGroup()
{

}

AnimateGroup::~AnimateGroup()
{

}

bool AnimateGroup::IsRun()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        if (_anis[i]->IsRun())
        {
            return true;
        }
    }
    return false;
}

bool AnimateGroup::IsPause()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        if (_anis[i]->IsPause())
        {
            return true;
        }
    }
    return false;
}

void AnimateGroup::Start()
{
    if (!GetTarget())
    {
        return;
    }

    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 1;
    }
    _state = 1;

    GetTarget()->GetStoryBoard()->Start(this);
}

void AnimateGroup::Stop()
{
    if (!GetTarget())
    {
        return;
    }

    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 1;
    }
    _state = 0;
    GetTarget()->GetStoryBoard()->Stop(this);
    GetTarget()->WriteViewFlag(ViewFlags::IsInAnimating, false);
    GetTarget()->SetValue(FrameworkElement::AnimageTagProperty, NULL);
}

void AnimateGroup::Pause()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 2;
    }
    _state = 2;
}

bool AnimateGroup::GetTransform(Transform* trans)
{
    UIMutex::Locker sync(_locker);
    int iEndCount = 0;

    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->GetTransform(trans);
        if (!_anis[i]->IsRun())
        {
            ++iEndCount;
        }
    }

    if (iEndCount != _anis.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AnimateGroup::DoTransform(Float interTime, Transform* trans)
{
}

void AnimateGroup::OnAnimate(FrameworkElement* owner, Rect* rcvalid)
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        Animate* ani = _anis[i];

        // 
        // ������Ч����������
        //
        if (ani && ani->IsRun() && ani->CanAnimate()
            && ani->CheckInterval((Float)30))
        {
            Rect rect;
            ani->_interval = ani->GetInterval();
            ani->OnAnimate(owner, &rect);

            rcvalid->UnionOnly(&rect);
        }
    }
}

}
