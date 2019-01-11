/////////////////////////////////////////////////////
// VideoPlayThread.cpp

#include "stdafx.h"
#include "VideoPlayThread.h"

VideoPlayThread::VideoPlayThread(VideoInfo* vInfo, suic::InvokeProxy* reflesh)
{
    _videoInfo = vInfo;
    _reflesh = reflesh;
}

VideoPlayThread::~VideoPlayThread()
{
}

double VideoPlayThread::SynchronizeVideo(double pts, int repeat_pict)
{
    double frameDelay;

    if (pts != 0) 
    {
        _videoInfo->videoClock = pts;
    } 
    else 
    {
        pts = _videoInfo->videoClock;
    }

    // 
    // ����ʱ��
    // 
    frameDelay = av_q2d(_videoInfo->GetVideoStrm()->codec->time_base);
    frameDelay += repeat_pict * (frameDelay * 0.5);

    _videoInfo->videoClock += frameDelay;

    return pts;
}

void VideoPlayThread::Run()
{
    for (;;)
    {
        if (_playInfo.IsEmpty())
        {
            if (_videoInfo->quit)
            {
                break;
            }
            Sleep(10);
            continue;
        }

        PlayInfo* playItem = NULL;

        _playInfo.Pop(playItem);

        double videoPts = playItem->videoPts;

        videoPts *= av_q2d(_videoInfo->GetVideoStrm()->time_base);
        videoPts = SynchronizeVideo(videoPts, playItem->repeat_pict);

        if (_videoInfo->seek_flag_video)
        {
            // ��������ת �������ؼ�֡��Ŀ��ʱ����⼸֡
           if (videoPts < _videoInfo->seekTime)
           {
               delete playItem;
               continue;
           }
           else
           {
               _videoInfo->seek_flag_video = false;
           }
        }
        
        double delay = videoPts - _videoInfo->frame_last_pts;
        if (delay <= 0 || delay >= 1.0)
        {
            delay = _videoInfo->frame_last_delay;
        }

        _videoInfo->frame_last_delay = delay;
        _videoInfo->frame_last_pts = videoPts;

        double ref_clock  = _videoInfo->audioClock;
        double diff = videoPts - ref_clock;
        double threshold = (delay > SYNC_THRESHOLD) ? delay : SYNC_THRESHOLD;

        if (fabs(diff) < NOSYNC_THRESHOLD) // ��ͬ��
        {
            if (diff <= -threshold) // ���ˣ�delay��Ϊ0
            {
                delay = 0;
            }
            else if (diff >= threshold) // ���ˣ��ӱ�delay
            {
                delay *= 2;
            }
        }

        if (delay > 0)
        {
            Sleep(delay * 1000);
        }

        if (playItem->bmpInfo != 0)
        {

            // 
            // �Ӹ����������Ⱦ
            //
            BmpInfo* bmp = playItem->bmpInfo;
            bmp->ref();
            _reflesh->PostInvoker(0, bmp);
        }

        delete playItem;
    }
}

void VideoPlayThread::AddPlayInfo(PlayInfo* playInfo)
{
    _playInfo.Add(playInfo);
}
