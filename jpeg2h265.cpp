#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/core/core.hpp>//OpenCV包含头文件
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define WIDTH 352
#define HIGH 280
#define NUM 10
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
char av_error[AV_ERROR_MAX_STRING_SIZE] = {0};
#define av_err2str(errnum) av_make_error_string(av_error, AV_ERROR_MAX_STRING_SIZE, errnum)


int amain() {
    AVFormatContext *ofmt_ctx = NULL;
    //AVPacket *pkt;
    int ret;
    const char *picname, *url;
    url = "rtp://192.168.10.56:554/push1";
    char in_filename[64];
    AVFrame *frame;
    struct SwsContext *sws_ctx;
    enum AVPixelFormat src = AV_PIX_FMT_BGR24;
    enum AVPixelFormat dst = AV_PIX_FMT_YUV420P;

    picname = "D:\\projects\\c++\\IMAGES_0\\000000000";

    avformat_network_init();

    AVCodec *codec_hevc = avcodec_find_decoder(AV_CODEC_ID_HEVC); //find a decoder
    AVCodecContext *c_ctx_hevc = avcodec_alloc_context3(codec_hevc);

    //codeccontext参数设置
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
    av_frame_get_buffer(frame, 0); //Check if the frame data is writable.
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", url); //Allocate an AVFormatContext for an output format.
    av_opt_set(ofmt_ctx->priv_data, "rtsp_transport", "tcp", 0);
    ofmt_ctx->max_interleave_delta = 1000000;

    AVStream *video_s = avformat_new_stream(ofmt_ctx, codec_hevc);
    avcodec_parameters_from_context(video_s->codecpar, c_ctx_hevc);
    video_s->id = ofmt_ctx->nb_streams - 1;
//    if (avformat_write_header(ofmt_ctx, NULL)){
//        std::cout<<"Call avformat_write_header function failed.\n";
//        return 0;
//    }
    std::cout<<"Call avformat_write_header function failed.\n";
    //av_dump_format(ofmt_ctx, 0, ofmt_ctx->url, 1);// Print detailed information about the input or output format, such as
  //  duration, bitrate, streams, container, programs, metadata, side data,* codec and time base.
    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, url, AVIO_FLAG_WRITE);
        if (ret < 0)
            fprintf(stderr, "Could not open '%s'  %d: %s\n", url, ret, av_err2str(ret));
    }


    sws_ctx = sws_getContext(c_ctx_hevc->width, c_ctx_hevc->height,
                             src, c_ctx_hevc->width, c_ctx_hevc->height, dst,
                             SWS_POINT, NULL, NULL, NULL);

    for (int i = 1; i < 7 + 1; i++) {
        sprintf(in_filename, "%s%d.jpg", picname, i);
        Mat img = imread(in_filename);
        Mat resize_img;
        resize(img, resize_img, Size(WIDTH, HIGH));
//        imshow("demo",resize_img);
//        waitKey(0);
        if (av_frame_make_writable(frame) < 0)
            exit(1);
        int imgwidthlen = resize_img.step; //resize_img.step 矩阵第一行元素的字节数
        uint8_t *reimgdata = new uint8_t[resize_img.rows * resize_img.step];//_t 表示这些数据类型是通过typedef定义的,无符号字符型
        memcpy(reimgdata, resize_img.data, resize_img.rows * resize_img.step);//即从源source中拷贝n个字节到目标destin中
        sws_scale(sws_ctx, &reimgdata,
                  &imgwidthlen, 0, c_ctx_hevc->height, frame->data, frame->linesize);//在srcSlice中缩放图像切片，并将缩放后的切片放入dst中的图像中
        ret = avcodec_send_frame(c_ctx_hevc, frame);//Supply a raw video or audio frame to the encoder. Use avcodec_receive_packet() to retrieve buffered output packets.
        while (ret >= 0) {
            AVPacket pkt = {0};
            ret = avcodec_receive_packet(c_ctx_hevc, &pkt);//如果成功会返回一个包含压缩数据的AVPacket。
            if (ret == (-(EAGAIN)) || ret == AVERROR_EOF)
                break;
            av_packet_rescale_ts(&pkt, c_ctx_hevc->time_base, video_s->time_base);//av_packet_rescale_ts()用于将 AVPacket 中各种时间值从一种时间基转换为另一种时间基。
//            av_interleaved_write_frame(ofmt_ctx, &pkt); // Write an uncoded frame to an output media file.
            av_packet_unref(&pkt);
//            av_usleep(1000);
        }
        img.release();
        resize_img.release();
    }
    av_write_trailer(ofmt_ctx);//av_write_trailer()用于输出文件尾

    avcodec_free_context(&c_ctx_hevc);
    av_frame_free(&frame);
    sws_freeContext(sws_ctx);


    return 0;


}