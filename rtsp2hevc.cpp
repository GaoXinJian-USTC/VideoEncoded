#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/core/core.hpp>//OpenCV����ͷ�ļ�  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define WIDTH 720
#define HIGH 1280
#define NUM 80
#define SWS_POINT 0x10
using namespace cv;

#ifndef INT64_C
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif

#ifdef __cplusplus
extern "C" {
#endif
/*Include ffmpeg header file*/
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
//
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>

#ifdef __cplusplus
}
#endif


int main(int argc, char **argv) {
    AVFormatContext *ofmt_ctx = NULL;
    //AVPacket *pkt;
    const char *picname, *url;
    char in_filename[64];
    AVFrame *frame;
    struct SwsContext *sws_ctx;
    enum AVPixelFormat src = AV_PIX_FMT_BGR24;
    enum AVPixelFormat dst = AV_PIX_FMT_YUV420P;

    picname = argv[1];

    avformat_network_init();

    AVCodec *codec_hevc = avcodec_find_decoder(AV_CODEC_ID_HEVC);
    AVCodecContext *c_ctx_hevc = avcodec_alloc_context3(codec_hevc);

    //codeccontext��������
    c_ctx_hevc->bit_rate = 400000;
    c_ctx_hevc->width = WIDTH;
    c_ctx_hevc->height = HIGH;
    c_ctx_hevc->time_base.den = 25;
    c_ctx_hevc->time_base.num = 1;
    c_ctx_hevc->framerate.den = 1;
    c_ctx_hevc->framerate.num = 25;
    c_ctx_hevc->qmin = 10;
    c_ctx_hevc->qmax = 51;

    c_ctx_hevc->gop_size = 12;
    c_ctx_hevc->max_b_frames = 3;
    c_ctx_hevc->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(c_ctx_hevc->priv_data, "preset", "slow", 0);

    avcodec_open2(c_ctx_hevc, codec_hevc, NULL);

    //pkt = av_packet_alloc();

    frame = av_frame_alloc();
    frame->format = c_ctx_hevc->pix_fmt;
    frame->width = c_ctx_hevc->width;
    frame->height = c_ctx_hevc->height;
    av_frame_get_buffer(frame, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", url);
    av_opt_set(ofmt_ctx->priv_data, "rtsp_transport", "tcp", 0);
    ofmt_ctx->max_interleave_delta = 1000000;

    AVStream *video_s = avformat_new_stream(ofmt_ctx, codec_hevc);
    video_s->id = ofmt_ctx->nb_streams - 1;

    av_dump_format(ofmt_ctx, 0, ofmt_ctx->url, 1);

    avformat_write_header(ofmt_ctx, NULL);

    sws_ctx = sws_getContext(c_ctx_hevc->width, c_ctx_hevc->height,
                             src, c_ctx_hevc->width, c_ctx_hevc->height, dst,
                             SWS_POINT, NULL, NULL, NULL);

    for (int i = 1; i < NUM + 1; i++) {
        sprintf(in_filename, "%s%d.jpg", picname, i);
        Mat img = imread(in_filename);
        Mat resize_img;
        resize(img, resize_img, Size(WIDTH, HIGH));
        if (av_frame_make_writable(frame) < 0)
            exit(1);
        int imgwidthlen = resize_img.step;
        uint8_t *reimgdata = new uint8_t[resize_img.rows * resize_img.step];
        memcpy(reimgdata, resize_img.data, resize_img.rows * resize_img.step);

        sws_scale(sws_ctx, &reimgdata,
                  &imgwidthlen, 0, c_ctx_hevc->height, frame->data, frame->linesize);
        int ret;
        ret = avcodec_send_frame(c_ctx_hevc, frame);
        while (ret >= 0) {
            AVPacket pkt = {0};
            ret = avcodec_receive_packet(c_ctx_hevc, &pkt);
            if (ret == (-(EAGAIN)) || ret == AVERROR_EOF)
                break;
            av_packet_rescale_ts(&pkt, c_ctx_hevc->time_base, video_s->time_base);
            av_interleaved_write_frame(ofmt_ctx, &pkt);
            av_packet_unref(&pkt);
        }
    }
    av_write_trailer(ofmt_ctx);

    avcodec_free_context(&c_ctx_hevc);
    av_frame_free(&frame);
    sws_freeContext(sws_ctx);


    return 0;


}