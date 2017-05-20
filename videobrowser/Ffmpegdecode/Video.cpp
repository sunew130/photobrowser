
#include "Video.h"
#include "VideoDisplay.h"

extern "C"{

#include <libswscale/swscale.h>

}

extern bool quit;

VideoState::VideoState()
{
    video_ctx        = NULL;
    stream_index     = -1;
    stream           = NULL;
#if 0
    window           = NULL;
    bmp              = NULL;
    renderer         = NULL;
#endif
    frame            = NULL;
    displayFrame     = NULL;

    videoq           = new PacketQueue();

    frame_timer      = 0.0;
    frame_last_delay = 0.0;
    frame_last_pts   = 0.0;
    video_clock      = 0.0;
}

VideoState::~VideoState()
{
    qDebug()<<"clean video queue";
    delete videoq;
    // close（codec）
    avcodec_close(video_ctx);

    av_frame_free(&frame);
    free(displayFrame->data[0]);
    av_frame_free(&displayFrame);

    if(frameq.clearQueue())
    {
        qDebug()<<"clean frame queue";
    }

}

void VideoState::video_play(MediaState *media)
{
    int width = VGA_WIDTH;
    int height = VGA_HEIGHT;

#if 0
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
#endif

    frame = av_frame_alloc();
    displayFrame = av_frame_alloc();

    displayFrame->format = AV_PIX_FMT_YUV420P;
    displayFrame->width = width;
    displayFrame->height = height;

    int numBytes = avpicture_get_size((AVPixelFormat)displayFrame->format,displayFrame->width, displayFrame->height);
    buffer = (uint8_t*)malloc(numBytes * sizeof(uint8_t));

    avpicture_fill((AVPicture*)displayFrame, buffer, (AVPixelFormat)displayFrame->format, displayFrame->width, displayFrame->height);

    SDL_CreateThread(decode, "", this);

    schedule_refresh(media, 40); // start display
}

double VideoState::synchronize(AVFrame *srcFrame, double pts)
{
    double frame_delay;

    if (pts != 0)
        video_clock = pts; // Get pts,then set video clock to it
    else
        pts = video_clock; // Don't get pts,set it to video clock

    frame_delay = av_q2d(stream->codec->time_base);
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

    video_clock += frame_delay;

    return pts;
}


int  decode(void *arg)
{
    VideoState *video = (VideoState*)arg;

    AVFrame *frame = av_frame_alloc();

    AVPacket packet;
    double pts;

    while (!quit)
    {
        video->videoq->deQueue(&packet, true);

        int ret = avcodec_send_packet(video->video_ctx, &packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            continue;

        ret = avcodec_receive_frame(video->video_ctx, frame);
        if (ret < 0 && ret != AVERROR_EOF)
            continue;

        if ((pts = av_frame_get_best_effort_timestamp(frame)) == AV_NOPTS_VALUE)
            pts = 0;

        pts *= av_q2d(video->stream->time_base);

        pts = video->synchronize(frame, pts);

        frame->opaque = &pts;

        if (video->frameq.nb_frames >= FrameQueue::capacity)
            SDL_Delay(500 * 2);

        video->frameq.enQueue(frame);

        av_frame_unref(frame);
    }

    qDebug()<<"close decode";
    av_frame_free(&frame);

    return 0;
}

