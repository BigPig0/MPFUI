/////////////////////////////////////////////////////
// VideoReader.cpp

#include "stdafx.h"
#include "VideoReader.h"
#include <time.h>

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

VideoInfo::~VideoInfo()
{
    if (audio_pkt != NULL)
    {
        av_free_packet(audio_pkt);
        av_packet_free(&audio_pkt);
    }
}

void VideoInfo::ClearVideos()
{
    suic::Mutex::Locker guard(vedioQueue.GetLock());
    AVPacket* packet = NULL;

    while (vedioQueue.PopItem(packet))
    {
        if (NULL != packet)
        {
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }
}

void VideoInfo::ClearAudios()
{
    suic::Mutex::Locker guard(audioQueue.GetLock());

    AVPacket* packet = NULL;

    while (audioQueue.PopItem(packet))
    {
        if (NULL != packet)
        {
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }
}

VideoReaderThr::VideoReaderThr(suic::String filename, suic::InvokeProxy* reflesh)
    : _filename(filename)
    , _reflesh(reflesh)
    , _decodeThr(NULL)
{
    
}

VideoReaderThr::~VideoReaderThr()
{
    if (NULL != _decodeThr)
    {
        _decodeThr->unref();
    }
}

void VideoReaderThr::StopPlay()
{
    SDL_PauseAudio(1);

    if (NULL != _decodeThr)
    {
        _decodeThr->Stop();
    }

    Stop();
}

void VideoReaderThr::PausePlay(bool bPause)
{
    if (IsPlaying())
    {
        _videoInfo.isPause = bPause;
        SDL_PauseAudio(bPause ? 1 : 0);
    }
}

bool VideoReaderThr::IsPlaying() const
{
    return !_videoInfo.quit;
}

bool VideoReaderThr::IsPause() const
{
    return _videoInfo.isPause;
}

void VideoReaderThr::SetPlayVolume(int volume)
{
    _videoInfo.volume = volume;
}

void VideoReaderThr::SetPlayProgress(float v)
{
    // 
    // ֻ���ڲ��Ź��������ò���Ч
    // 
    if (IsPlaying())
    {
        _videoInfo.seek_pos = _videoInfo.iDuration * v;
        _videoInfo.seek_req = true;
    }
}

void avcodec_get_frame_defaults(AVFrame *frame)
{
#if LIBAVCODEC_VERSION_MAJOR >= 55
     // extended_data should explicitly be freed when needed, this code is unsafe currently
     // also this is not compatible to the <55 ABI/API
    if (frame->extended_data != frame->data && 0)
        av_freep(&frame->extended_data);
#endif

    memset(frame, 0, sizeof(AVFrame));
    av_frame_unref(frame);
}

static int audio_decode_frame(VideoInfo *is, double *pts_ptr)
{
    int decodedDataSize = 0;
    int convertAudioSize = 0;
    int decoded_data_size = 0;
    
    // ������Ƶ֡�Ƿ���ȷ
    int iGotframe = 0;
    int wanted_nb_samples = 0;
    int resampled_data_size = 0;
    int n = 0;

    int64_t dec_channel_layout = 0;

    AVPacket *pkt = is->audio_pkt;
    
    double pts;

    for (;;) 
    {
        // 
        // audio_pkt_size����0��ʾ�Ѿ���ȡ��һ֡��Ƶ���ݣ����д���
        //
        while (is->audio_pkt_size > 0) 
        {

            // 
            // �Ѿ���ͣ
            // 
            if (is->isPause)
            {
                SDL_Delay(10);
                continue;
            }

            if (!is->audio_frame) 
            {
                if (!(is->audio_frame = av_frame_alloc())) 
                {
                    return AVERROR(ENOMEM);
                }
            } 
            else
            {
                avcodec_get_frame_defaults(is->audio_frame);
            }

            // 
            // ������Ƶ������������ֵ��ʾ�Ѿ��������������С
            // 
            decodedDataSize = avcodec_decode_audio4(is->audioStrm->codec, is->audio_frame, &iGotframe, pkt);

            if (decodedDataSize < 0) 
            {
                // 
                // ���������֡
                // 
                is->audio_pkt_size = 0;
                break;
            }

            // 
            // ����δ�������Ƶ������
            // 
            is->audio_pkt_data += decodedDataSize;
            is->audio_pkt_size -= decodedDataSize;

            if (iGotframe == 0)
            {
                continue;
            }

            // 
            // ����������������Ҫ�Ļ����С
            // 
            decoded_data_size = av_samples_get_buffer_size(NULL, is->audio_frame->channels, 
                is->audio_frame->nb_samples, (AVSampleFormat)is->audio_frame->format, 1);

            if (is->audio_frame->channel_layout == 0)
            {
                dec_channel_layout = av_get_default_channel_layout(is->audio_frame->channels);
            }
            else
            {
                if (is->audio_frame->channels == av_get_channel_layout_nb_channels(is->audio_frame->channel_layout))
                {
                    dec_channel_layout = is->audio_frame->channel_layout;
                }
                else
                {
                    dec_channel_layout = av_get_default_channel_layout(is->audio_frame->channels);
                }
            }

            wanted_nb_samples = is->audio_frame->nb_samples;

            if (is->audio_frame->format != is->audio_src_fmt
                    || dec_channel_layout != is->audio_src_channel_layout
                    || is->audio_frame->sample_rate != is->audio_src_freq
                    || (wanted_nb_samples != is->audio_frame->nb_samples && !is->audioSwrCtx)) 
            {
                if (NULL != is->audioSwrCtx)
                {
                    swr_free(&is->audioSwrCtx);
                }

                is->audioSwrCtx = swr_alloc_set_opts(NULL,
                        is->audio_tgt_channel_layout, (AVSampleFormat)is->audio_tgt_fmt, 
                        is->audio_tgt_freq, dec_channel_layout,
                        (AVSampleFormat)is->audio_frame->format, is->audio_frame->sample_rate, 0, NULL);

                if (!is->audioSwrCtx || swr_init(is->audioSwrCtx) < 0) 
                {
                    break;
                }

                is->audio_src_channel_layout = dec_channel_layout;
                is->audio_src_channels = is->audioStrm->codec->channels;
                is->audio_src_freq = is->audioStrm->codec->sample_rate;
                is->audio_src_fmt = is->audioStrm->codec->sample_fmt;
            }

            // 
            // �������ǿ��ԶԲ��������е��������ӻ��߼��٣�һ���������������ͬ��
            // 
            if (NULL != is->audioSwrCtx) 
            {
                const uint8_t **in = (const uint8_t **) is->audio_frame->extended_data;
                uint8_t *out[] = { is->audio_buf_temp };

                if (wanted_nb_samples != is->audio_frame->nb_samples) 
                {
                    if (swr_set_compensation(is->audioSwrCtx,
                            (wanted_nb_samples - is->audio_frame->nb_samples)
                             * is->audio_tgt_freq / is->audio_frame->sample_rate, 
                            wanted_nb_samples * is->audio_tgt_freq / is->audio_frame->sample_rate) < 0) 
                    {
                        break;
                    }
                }

                int out_count = sizeof(is->audio_buf_temp) / is->audio_tgt_channels / av_get_bytes_per_sample(is->audio_tgt_fmt);

                // 
                // ����Ƶ���ݽ��뵽������out
                // 
                convertAudioSize = swr_convert(is->audioSwrCtx, out, out_count, in, is->audio_frame->nb_samples);

                if (convertAudioSize < 0) 
                {
                    break;
                }

                if (convertAudioSize == out_count) 
                {
                    swr_init(is->audioSwrCtx);
                }

                is->audio_buf = is->audio_buf_temp;
                resampled_data_size = convertAudioSize * is->audio_tgt_channels * av_get_bytes_per_sample(is->audio_tgt_fmt);
            }
            else 
            {
                resampled_data_size = decoded_data_size;
                is->audio_buf = is->audio_frame->data[0];
            }

            pts = is->audioClock;
            *pts_ptr = pts;
            n = 2 * is->audioStrm->codec->channels;

            is->audioClock += (double) resampled_data_size / (double) (n * is->audioStrm->codec->sample_rate);

            if (is->seek_flag_audio)
            {
                // 
                // ������Ҫ�Ѻ���ת������
                // 
                int64_t iAudioTime = is->audioClock * 1000;

                // 
                // ��������ת �������ؼ�֡��Ŀ��ʱ����⼸֡
                // 
                if (iAudioTime < is->seekTime)
                {
                    break;
                }
                else
                {
                    is->seek_flag_audio = 0;
                }
            }

            // We have data, return it and come back for more later
            return resampled_data_size;
        }

        if (is->audio_pkt != NULL)
        {
            av_free_packet(is->audio_pkt);
            av_packet_free(&is->audio_pkt);
            is->audio_pkt = NULL;
        }
        
        if (is->quit)
        {
            return -1;
        }

        if (is->isPause == true)
        {
            SDL_Delay(10);
            continue;
        }

        if (!is->audioQueue.Pop(is->audio_pkt))
        {
            return -1;
        }

        // 
        // ��������ת��ˢ�»���
        // 
        if (NULL == is->audio_pkt)
        {
            avcodec_flush_buffers(is->audioStrm->codec);
            continue;
        }

        pkt = is->audio_pkt;

        // 
        // �յ�������ݣ�˵���ո�ִ�й���ת��������Ҫ�ѽ����������ݣ����һ��
        // 
        if (pkt->data == 0)
        {
            avcodec_flush_buffers(is->audioStrm->codec);
            av_free_packet(is->audio_pkt);
            is->audio_pkt = NULL;
        }
        else
        {
            is->audio_pkt_data = pkt->data;
            is->audio_pkt_size = pkt->size;

            // 
            // if update, update the audio clock w/pts
            // 
            if (pkt->pts != AV_NOPTS_VALUE) 
            {
                is->audioClock = av_q2d(is->audioStrm->time_base) * pkt->pts;
            }
        }
    }

    return 0;
}

/*
void audio_track_change(Uint8 *pBuffer, int iSize, int AudioChannel, int AudioBits )
{
  int i=0,w_pos=0,r_pos=0, n=0;

  n = iSize / 4; //ѭ������

  switch (AudioBits)
  {
  case 8:
    //��������ѡ��
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=1;
      break;             //������

    case 2:r_pos=1;
      w_pos=0;  
      break;             //������

    default:return ;           //������˫����ģʽ��

    }
    //ִ�������л�����  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,1);  
      w_pos+=2;
      r_pos+=2;
    }
    break;
  case 16:
    //��������ѡ��
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=2;
      break;             //������

    case 2:r_pos=2;
      w_pos=0;  
      break;             //������

    default:return ;           //������˫����ģʽ��

    }
    //ִ�������л�����  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,2);  
      w_pos+=4;
      r_pos+=4;
    }
    break;
  case 24:
    //��������ѡ��
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=3;
      break;             //������

    case 2:r_pos=3;
      w_pos=0;  
      break;             //������

    default:return;           //������˫����ģʽ��

    }
    //ִ�������л�����  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,3);  
      w_pos+=6;
      r_pos+=6;
    }
    break;
  case 32:
    //��������ѡ��
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=4;
      break;             //������

    case 2:r_pos=4;
      w_pos=0;  
      break;             //������

    default:return ;           //������˫����ģʽ��

    }
    //ִ�������л�����  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,4);  
      w_pos+=8;
      r_pos+=8;
    }
    break;
  }
  return ;		
}
*/

static void audio_callback(void *userdata, Uint8 *stream, int len) 
{
    VideoInfo *is = (VideoInfo*)userdata;

    // 
    // ��ǰ��Ҫ������SDL�������Ƶ���ݳ���
    // 
    int needCopyDataSize = 0;
    int audio_data_size = 0;

    double pts;

    // 
    // len����SDL�����SDL�������Ĵ�С������������δ�������Ǿ�һֱ�����������
    // 
    while (len > 0) 
    {
        // 
        // audio_buf_indexָʾ��ǰ��Ƶ�������ݵĿ�ʼλ��
        // audio_buf_sizeָʾ��ǰ��ȡ����Ƶ�������ݵ��ܳ���
        // ���audio_buf_index���ڵ���audio_buf_size��ʱ�򣬱�ʾ���е����ݶ��Ѿ�����SDL����
        // ��Ҫ�ٴζ�ȡ��Ƶ���ݵ�����
        // 
        if (is->audio_buf_index >= is->audio_buf_size) 
        {

            audio_data_size = audio_decode_frame(is, &pts);

            // 
            // audio_data_size < 0 ��ʾû�ܽ�������ݣ�����Ĭ�ϲ��ž���
            // 
            if (audio_data_size < 0) 
            {                
                // 
                // �������� 
                //
                is->audio_buf_size = 1024;

                if (is->audio_buf == NULL) 
                {
                    return;
                }
                
                if (is->audio_buf == &is->audio_buf_temp[0])
                {
                    memset(is->audio_buf, 0, is->audio_buf_size);
                }
            } 
            else 
            {
                is->audio_buf_size = audio_data_size;
            }

            is->audio_buf_index = 0;
        }

        // 
        // �鿴stream���ÿռ䣬����һ��copy�������ݣ�ʣ�µ��´μ���copy
        // 
        needCopyDataSize = is->audio_buf_size - is->audio_buf_index;

        // 
        // SDL����������̫С����������Ҫ���������ݳ��� 
        //
        if (needCopyDataSize > len) 
        {
            needCopyDataSize = len;
        }

        if (is->audio_buf == NULL) 
        {
            return;
        }

        SDL_memset(stream, 0, len);

        // �������ƺ���, gTrack 1 - ������, 2 - ������, 0 - ������
        // audio_track_change((uint8_t *)is->audio_buf + is->audio_buf_index, len1, gTrack, 16);    

        // 
        // ����Ƶ���ݿ�����SDL���壬׼������
        // 
        // memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, needCopyDataSize);
        SDL_MixAudio(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, needCopyDataSize, is->volume); 

        // 
        // �����»�����λ�� 
        //
        len -= needCopyDataSize;
        //stream += needCopyDataSize;
        is->audio_buf_index += needCopyDataSize;
    }
}

int VideoReaderThr::InitAudioComponent(VideoInfo *pVI, int streamIndex)
{
    AVCodecContext *codecCtx = pVI->aCodecCtx;
    AVCodec *codec = pVI->aCodec;

    SDL_AudioSpec wanted_spec;
    SDL_AudioSpec spec;
    int64_t wanted_channel_layout = 0;
    int wanted_nb_channels;

    // 
    // SDL֧�ֵ�������Ϊ 1, 2, 4, 6
    // �������ǻ�ʹ�����������������֧�ֵ�������Ŀ
    //
    const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };

    if (streamIndex < 0 || streamIndex >= pVI->formatCtx->nb_streams) 
    {
        return -1;
    }

    AVStream *pAudioStrm = pVI->formatCtx->streams[streamIndex];

    wanted_nb_channels = codecCtx->channels;

    if (!wanted_channel_layout 
        || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) 
    {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }

    // wanted_spec.channels = codecCtx->channels;
    wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.freq = codecCtx->sample_rate;

    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) 
    {
        return -1;
    }

    // ���庬����鿴��SDL�궨�塱����
    wanted_spec.format = AUDIO_S16SYS;

    // 
    // 0ָʾ����
    //
    wanted_spec.silence = 0;            
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;  // �Զ���SDL��������С
    wanted_spec.callback = audio_callback;        // ��Ƶ����Ĺؼ��ص�����
    wanted_spec.userdata = pVI;                    // ��������ص��������������


    while (SDL_OpenAudio(&wanted_spec, &spec) < 0)
    {
        fprintf(stderr, "SDL_OpenAudio(%d channels): %s\n", wanted_spec.channels, SDL_GetError());

        // FFMIN()��ffmpeg����ĺ꣬���ؽ�С����
        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)]; 

        if (!wanted_spec.channels)
        {
            fprintf(stderr, "No more channel to try\n");
            return -1;
        }
        
        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
    }

    // 
    // ���ʵ��ʹ�õ����ã�������spec����SDL_OpenAudio()��䣩
    // 
    if (spec.format != AUDIO_S16SYS) 
    {
        fprintf(stderr,"SDL advised audio format %d is not supported!\n",spec.format);
        return -1;
    }

    if (spec.channels != wanted_spec.channels) 
    {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) 
        {
            fprintf(stderr,"SDL advised channel count %d is not supported!\n",spec.channels);
            return -1;
        }
    }

    pVI->audio_hw_buf_size = spec.size;

    // 
    // �����úõĲ������浽��ṹ��
    //
    pVI->audio_src_fmt = pVI->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
    pVI->audio_src_freq = pVI->audio_tgt_freq = spec.freq;
    pVI->audio_src_channel_layout = pVI->audio_tgt_channel_layout = wanted_channel_layout;
    pVI->audio_src_channels = pVI->audio_tgt_channels = spec.channels;
    
    pAudioStrm->discard = AVDISCARD_DEFAULT;
    
    switch (codecCtx->codec_type) 
    {
    case AVMEDIA_TYPE_AUDIO:
        pVI->audioStrm = pVI->formatCtx->streams[streamIndex];
        pVI->audio_buf_size = 0;
        pVI->audio_buf_index = 0;
        SDL_PauseAudio(0);
        // SDL_PauseAudioDevice(pVI->audioID, 0);
        break;

    default:
        break;
    }

    return 0;
}

void VideoReaderThr::DoSeekFrameReq()
{
    if (_videoInfo.seek_req)
    {
        int stream_index = -1;
        int64_t seek_target = _videoInfo.seek_pos;

        if (_videoInfo.videoIndex >= 0)
        {
            stream_index = _videoInfo.videoIndex;
        }
        else if (_videoInfo.audioIndex >= 0)
        {
            stream_index = _videoInfo.audioIndex;
        }

        AVRational aVRational = {1, AV_TIME_BASE};
        
        if (stream_index >= 0)
        {
            // 
            // seek_target��λ�Ǻ��룬������Ҫת����FFMpegʱ�䵥λ
            // 
            seek_target = av_rescale_q(seek_target, aVRational, _videoInfo.GetStream(stream_index)->time_base);
        }

        // 
        // ��λ֡��ָ��ʱ��
        // 
        if (av_seek_frame(_videoInfo.formatCtx, stream_index, seek_target, AVSEEK_FLAG_BACKWARD) < 0) 
        {
            fprintf(stderr, "%s: error while seeking\n", _videoInfo.formatCtx->filename);
        } 
        else 
        {
            if (_videoInfo.audioIndex >= 0) 
            {
                _videoInfo.ClearVideos();
                _videoInfo.vedioQueue.Add(NULL);
            }

            if (_videoInfo.videoIndex >= 0) 
            {
                _videoInfo.ClearAudios();
                _videoInfo.audioQueue.Add(NULL);
            }
        }

        _videoInfo.seek_req = false;

        // 
        // ת������
        // 
        _videoInfo.seekTime = _videoInfo.seek_pos / 1000000.0;
        _videoInfo.seek_flag_audio = true;
        _videoInfo.seek_flag_video = true;
    }
}

void VideoReaderThr::Run()
{
    suic::Mulstr fname(_filename.c_str());

    AVFormatContext *pFormatCtx = NULL;

    // 
    // ��Ƶ��ؽ�����
    // 
    AVCodecContext *pVCodecCtx = NULL;
    AVCodec *pVCodec = NULL;

    // 
    // ��Ƶ��ؽ�����
    // 
    AVCodecContext *pACodecCtx = NULL;
    AVCodec *pACodec = NULL;

    int audioStream = -1;
    int videoStream = -1;
    int i = 0;

    av_register_all();

    //
    // ����AVFormatContext.
    // 
    pFormatCtx = avformat_alloc_context();

    // 
    // ����Ƶ�ļ�
    // 
    if (avformat_open_input(&pFormatCtx, fname.c_str(), NULL, NULL) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) 
    {
        printf("Could't find stream infomation.\n");
        return;
    }

    // ��ȡ��Ƶ�ܳ���
    if (pFormatCtx->duration != AV_NOPTS_VALUE) 
    {
        _videoInfo.iDuration = pFormatCtx->duration + 5000;
    }

    //
    // ������Ƶ����Ƶ��Ϣ
    // ѭ��������Ƶ�а���������Ϣ��
    // 
    for (i = 0; i < pFormatCtx->nb_streams; i++) 
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }

        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO  && audioStream < 0)
        {
            audioStream = i;
        }
    }

    _videoInfo.formatCtx = pFormatCtx;

    // ���videoStreamΪ-1 ˵��û���ҵ���Ƶ��
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }

    // 
    // û�������������л���
    // 
    if (audioStream == -1) {
        printf("Didn't find a audio stream.\n");
    }

    _videoInfo.videoIndex = videoStream;
    _videoInfo.audioIndex = audioStream;

    // 
    // �����Ƶ��Ϣ
    // 
    av_dump_format(pFormatCtx, 0, fname.c_str(), 0); 

    // 
    // ������Ƶ������
    // 
    pVCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pVCodec = avcodec_find_decoder(pVCodecCtx->codec_id);

    _videoInfo.vCodecCtx = pVCodecCtx;
    _videoInfo.vCodec = pVCodec;

    if (pVCodec == NULL) 
    {
        printf("PCodec not found.\n");
        return;
    }

    // 
    // ����Ƶ������
    // 
    if (avcodec_open2(pVCodecCtx, pVCodec, NULL) < 0) 
    {
        printf("Could not open video codec.\n");
        return;
    }

    _videoInfo.vrate = _videoInfo.GetVideoStrm()->r_frame_rate;
    if (_videoInfo.vrate.num / _videoInfo.vrate.den > 100) 
    {
        _videoInfo.vrate.num = 21;
        _videoInfo.vrate.den = 1;
    }

    float iRate = _videoInfo.vrate.num / _videoInfo.vrate.den;

    _videoInfo.tickframe = 1000 / iRate;
    _videoInfo.ticksleep = _videoInfo.tickframe;

    if (audioStream != -1)
    {
        //
        // ������Ƶ������
        //
        pACodecCtx = pFormatCtx->streams[audioStream]->codec;
        pACodec = avcodec_find_decoder(pACodecCtx->codec_id);

        _videoInfo.aCodec = pACodec;
        _videoInfo.aCodecCtx = pACodecCtx;

        // 
        // ����Ƶ������
        // 
        if (avcodec_open2(pACodecCtx, pACodec, NULL) < 0) {
            printf("Could not open audio codec.\n");
            return;
        }

        _videoInfo.audioStrm = pFormatCtx->streams[audioStream];

        // 
        // ��ʼ����Ƶ���Ż�����SDL��
        // 
        InitAudioComponent(&_videoInfo, audioStream);
    }

    _decodeThr = new VideoDecodeThr(&_videoInfo, _reflesh);
    _decodeThr->ref();
    _decodeThr->Start();

    // 
    // ����һ��packet ������Ŷ�ȡ����Ƶ
    //
    AVPacket *packet = NULL;
    int ySize = pVCodecCtx->width * pVCodecCtx->height;

    _reflesh->PostInvoker(1, NULL);

    for (;;)
    {
        // 
        // �߳��Ѿ��˳�
        //
        if (IsStopped())
        {
            // 
            // �ȴ������߳��˳�����ʹ�ñ��̷߳������Դ��
            //
            _decodeThr->Stop();
            _decodeThr->Join();
            break;
        }

        DoSeekFrameReq();

        if (_videoInfo.isPause)
        {
            Sleep(100);
            continue;
        }

        if (_videoInfo.vedioQueue.IsFull() ||
            _videoInfo.audioQueue.IsFull())
        {
            printf("�������ݣ���ʱ\n");
            SDL_Delay(100);
            continue;
        }

        //
        // ����һ��packet���������洢����
        //
        packet = av_packet_alloc();

        // 
        // ��ȡһ֡��Ƶ�������ݷŵ�packet������
        //
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            // 
            // �ͷŸոշ�����ڴ�
            // 
            av_packet_free(&packet);

            if ((pFormatCtx->pb->error) == 0)
            {
                SDL_Delay(100); //���ǳ��������Ƕ�����
                //
                // ��ȡ���������ñ�־
                //
                _videoInfo.readFinished = true;

                // �Ƚ����߳̽���
                _decodeThr->Join();
                break;
            }
            else
            {
                // �����ˣ�������
                continue; 		
            }
        }

        // 
        // ������Ƶ����������Ƶ��������У�׼����ʾ
        // 
        if (packet->stream_index == videoStream)
        {
            _videoInfo.vedioQueue.Add(packet);
        }
        else if( packet->stream_index == audioStream)
        {
            // 
            // ��Ƶ���ݰ�ѹ�����
            // 
            _videoInfo.audioQueue.Add(packet);
        }
        else
        {
            // 
            // û�ж�Ӧ�Ĳ������ͣ��ͷŰ�
            // 
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }

    // 
    // �ͷ���Դ��׼���˳�
    // 
    avcodec_close(pACodecCtx);
    avcodec_close(pVCodecCtx);
    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    SDL_CloseAudio();

    _reflesh->PostInvoker(0, NULL);
}

VideoDecodeThr::VideoDecodeThr(VideoInfo* vInfo, suic::InvokeProxy* reflesh)
    : _videoInfo(vInfo)
    , _reflesh(reflesh)
{
}

VideoDecodeThr::~VideoDecodeThr()
{
}

double VideoDecodeThr::SynchronizeVideo(AVFrame *srcFrame, double pts)
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
    frameDelay += srcFrame->repeat_pict * (frameDelay * 0.5);

    _videoInfo->videoClock += frameDelay;

    return pts;
}

unsigned long long GetCurrentTimeMsec()  
{  
#ifdef __OS_WIN32  
    struct timeval tv;  
    time_t clock;  
    struct tm tm;  
    SYSTEMTIME wtm;  

    GetLocalTime(&wtm);  
    tm.tm_year = wtm.wYear - 1900;  
    tm.tm_mon = wtm.wMonth - 1;  
    tm.tm_mday = wtm.wDay;  
    tm.tm_hour = wtm.wHour;  
    tm.tm_min = wtm.wMinute;  
    tm.tm_sec = wtm.wSecond;  
    tm.tm_isdst = -1;  
    clock = mktime(&tm);  
    tv.tv_sec = clock;  
    tv.tv_usec = wtm.wMilliseconds * 1000;  
    return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);  
#else  
    struct timeval tv;  
    gettimeofday(&tv,NULL);  
    return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);  
#endif  
}

void VideoDecodeThr::Run()
{
    // ������Ƶ���
    AVFrame *pFrame = NULL;
    AVFrame *pFrameRGB = NULL;

    // ������rgb����
    uint8_t *pRgbBuffer;

    AVPacket *pPacket = NULL;

    // ���ڽ�������Ƶ��ʽת��
    struct SwsContext *pImgConvertCtx;

    int iRet = 0;
    int iGotPicture = 0;
    int iNumBytes = 0;

    double videoPts = 0; // ��ǰ��Ƶ��pts
    double audioPts = 0; // ��Ƶpts

    // av_register_all();

    // ��Ƶ������
    AVCodecContext *pCodecCtx = _videoInfo->vCodecCtx;

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    // ����ĳɽ�������YUV����ת����RGB32
    pImgConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);

    iNumBytes = avpicture_get_size(AV_PIX_FMT_BGRA, pCodecCtx->width, pCodecCtx->height);

    pRgbBuffer = (uint8_t *) av_malloc(iNumBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, pRgbBuffer, AV_PIX_FMT_BGRA,
            pCodecCtx->width, pCodecCtx->height);

    static int index = 0;
    unsigned long long startTime = GetCurrentTimeMsec();

    //_playThread = new VideoPlayThread(_videoInfo, _reflesh);
    //_playThread->Start();

    for (;;)
    {
        // �߳��Ѿ��˳�
        if (IsStopped())
        {
            //_playThread->Stop();
            break;
        }

        if (_videoInfo->isPause)
        {
            Sleep(100);
            continue;
        }

        pPacket = NULL; 

        if (!_videoInfo->vedioQueue.Pop(pPacket))
        {
            if (_videoInfo->readFinished)
            {
                // 
                // ��������û�������Ҷ�ȡ�����
                //
                _videoInfo->quit = true;
                break;
            }
            else
            {
                // 
                // ����ֻ����ʱû�����ݶ���
                //
                Sleep(5);
                continue;
            }
        }

        if (NULL == pPacket)
        {
            _videoInfo->ticklast = 0;
            avcodec_flush_buffers(_videoInfo->GetVideoStrm()->codec);
            continue;
        }

        startTime = GetCurrentTimeMsec();

        //
        // ������Ƶ������
        // 
        iRet = avcodec_decode_video2(pCodecCtx, pFrame, &iGotPicture, pPacket);

        if (iRet < 0) 
        {
            suic::Debug::Trace(suic::String("decode error.\n").c_str());
            av_free_packet(pPacket);
            av_packet_free(&pPacket);
            continue;
        }

        int64_t aPts = _videoInfo->audioClock * 1000;
        int64_t vPts = av_rescale_q(av_frame_get_best_effort_timestamp(pFrame), 
            _videoInfo->GetVideoStrm()->time_base, TIMEBASE_MS);
        DWORD tickcur = GetTickCount();
        int tickdiff = tickcur - _videoInfo->ticklast;
        int64_t avDiff = aPts - vPts - _videoInfo->tickavdiff;

        _videoInfo->ticklast = tickcur;

        if (_videoInfo->seek_flag_video)
        {
            // 
            // ��������ת �������ؼ�֡��Ŀ��ʱ����⼸֡
            //
            if (vPts < _videoInfo->seekTime)
            {
                av_free_packet(pPacket);
                av_packet_free(&pPacket);
                continue;
            }
            else
            {
                _videoInfo->seek_flag_video = false;
            }
        }

        if (tickdiff - _videoInfo->tickframe >  2) 
        {
            _videoInfo->ticksleep--;
        }

        if (tickdiff - _videoInfo->tickframe < -2) 
        {
            _videoInfo->ticksleep++;
        }

        if (aPts > 0 && vPts > 0) 
        {
            if (avDiff > 5) 
            {
                _videoInfo->ticksleep -= 2;
            }

            if (avDiff < -5) 
            {
                _videoInfo->ticksleep += 2;
            }
        }

        if (_videoInfo->ticksleep < 0) 
        {
            _videoInfo->ticksleep = 0;
        }
        
        if (_videoInfo->ticksleep > 0) 
        {
            Sleep(_videoInfo->ticksleep);
        }

        if (iGotPicture != 0)
        {
            // 
            // ת��ͼ���ʽ
            //
            sws_scale(pImgConvertCtx, (uint8_t const * const *) pFrame->data, 
                pFrame->linesize, 0, pCodecCtx->height, 
                pFrameRGB->data, pFrameRGB->linesize);

            // 
            // �Ӹ����������Ⱦ
            //
            BmpInfo* bmp = new BmpInfo();
            bmp->ref();
            bmp->bmp.Create(pCodecCtx->width, pCodecCtx->height, pRgbBuffer, 32);
            //bmp->bmp.SetConfig(pCodecCtx->width, pCodecCtx->height, 32);
            //bmp->bmp.SetPixels(pRgbBuffer);
            bmp->curDuration = vPts;

            // 
            // ת��������
            // 
            bmp->duration = _videoInfo->iDuration / 1000;

            _reflesh->PostInvoker(0, bmp);

            /*PlayInfo* pInfo = new PlayInfo();
            pInfo->bmpInfo = bmp;
            pInfo->videoPts = videoPts;
            pInfo->repeat_pict = pFrame->repeat_pict;

            _playThread->AddPlayInfo(pInfo);*/
        }

        av_free_packet(pPacket);
        av_packet_free(&pPacket);
    }

    //
    // �ͷ������Դ���������˳�
    // 
    av_free(pFrame);
    av_free(pFrameRGB);
    av_free(pRgbBuffer);
    sws_freeContext(pImgConvertCtx);

    // 
    // �����߳��˳������ñ�־��֪ͨ�����߳��˳�
    // 
    _videoInfo->decodeFinished = true;

    //_playThread->Join();
}
