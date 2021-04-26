//
// Created by dell on 2021/4/11.
//

#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>


#define __STDC_CONSTANT_MACROS


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
};


//封装格式
//#include "libavformat/avformat.h"
//#include "libavcodec/avcodec.h"
//#include "libswscale/swscale.h"
//#include "libswresample/swresample.h"


int main (void)
{
    //1.注册组件
    av_register_all();
    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();


    //2.打开输入音频文件该函数用于打开多媒体数据并且获取一些信息
    if (avformat_open_input(&pFormatCtx, "D:\\BaiduNetdiskDownload\\demo.wav", NULL, NULL) != 0) {
        printf("%s", "da kai shu ru wen j shi bai");
        return -1;
    }
    //3.获取音频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("%s", "huo qu yin pin shi bai");
        return -1;
    }


    //音频解码，需要找到对应的AVStream所在的pFormatCtx->streams的索引位置
    int audio_stream_idx = -1;
    unsigned int i = 0;
    for (; i < pFormatCtx->nb_streams; i++) {
        //根据类型判断是否是音频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = i;
            break;
        }
    }
    //4.获取解码器
    //根据索引拿到对应的流,根据流拿到解码器上下文
    AVCodecContext *pCodeCtx = pFormatCtx->streams[audio_stream_idx]->codec;
    //再根据上下文拿到编解码id，通过该id拿到解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
    if (pCodec == NULL) {
        printf("%s", "cont decode");
        return -1;
    }
    //5.打开解码器
    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        printf("%s", "encoder cant open");
        return -1;
    }
    //编码数据，
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //解压缩数据
    AVFrame *frame = av_frame_alloc();


    //frame->16bit 44100 PCM 统一音频采样格式与采样率
    SwrContext *swrCtx = swr_alloc();
    //重采样设置选项-----------------------------------------------------------start
    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = pCodeCtx->sample_fmt;
    //输出的采样格式 16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入的采样率
    int in_sample_rate = pCodeCtx->sample_rate;
    //输出的采样率
    int out_sample_rate = 44100;
    //输入的声道布局
    uint64_t in_ch_layout = pCodeCtx->channel_layout;
    //输出的声道布局
    uint64_t out_ch_layout = AV_CH_LAYOUT_MONO;


    swr_alloc_set_opts(swrCtx, out_ch_layout, out_sample_fmt, out_sample_rate, in_ch_layout, in_sample_fmt,
                       in_sample_rate, 0, NULL);
    swr_init(swrCtx);
    //重采样设置选项-----------------------------------------------------------end
    //获取输出的声道个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    //存储pcm数据
    uint8_t *out_buffer = (uint8_t *) av_malloc(2 * 44100);
    FILE *fp_pcm = fopen("out.pcm", "wb");
    int ret, got_frame, framecount = 0;
    //6.一帧一帧读取压缩的音频数据AVPacket
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
            //解码AVPacket->AVFrame
            ret = avcodec_decode_audio4(pCodeCtx, frame, &got_frame, packet);
            if (ret < 0) {
                printf("%s", "decode finish");
            }
            //非0，正在解码
            if (got_frame) {
                printf("decode %d fps\n", framecount++);
                swr_convert(swrCtx, &out_buffer, 2 * 44100, (const uint8_t**)frame->data, frame->nb_samples);
                //获取sample的size
                int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples,
                                                                 out_sample_fmt, 1);
                //写入文件进行测试
                fwrite(out_buffer, 1, out_buffer_size, fp_pcm);
            }
        }
        av_free_packet(packet);
    }
    fclose(fp_pcm);
    av_frame_free(&frame);
    av_free(out_buffer);
    swr_free(&swrCtx);
    avcodec_close(pCodeCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}