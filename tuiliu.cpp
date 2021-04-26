//
// Created by dell on 2021/4/14.
//

//
// Created by dell on 2021/4/11.
//

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include <libavutil/opt.h>
}
#include <iostream>
using namespace std;
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

int XError(int errNum)
{
    char buf[1024] = { 0 };
    av_strerror(errNum, buf, sizeof(buf));
    cout << buf << endl;
    getchar();
    return -1;
}
static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

int main(int argc, char *argv[])
{
    const char *inUrl = R"(D:\projects\audio_encode\h265.mkv)";
    AVPacket pkt;
    AVOutputFormat *ofmt = NULL;
    int videoindex_v=-1,videoindex_out=-1;
    int audioindex_a=-1,audioindex_out=-1;
    int64_t cur_pts_v=0,cur_pts_a=0;
    int ret;
    int frame_index=0;
    AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL,*ofmt_ctx = NULL,*ictx = NULL;
    const char *in_filename_v = "D:\\projects\\audio_encode\\a.h265";
    const char *in_filename_a = "D:\\projects\\audio_encode\\ls.wav";

    const char *out_filename = "rtp://localhost/live";
//    av_register_all();
    //Input get video info
//    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0) {
//        printf( "Could not open input file.");
//        return -99;
//    }
//    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {
//        printf( "Failed to retrieve input stream information");
//        return -98;
//    }
//    //get audio info
//    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0) {
//        printf( "Could not open input file.");
//        return -97;
//    }
//    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {
//        printf("Failed to retrieve input stream information");
//        return -96;
//    }
//####################
    int re = avformat_open_input(&ictx, inUrl, 0, 0);//
    if (re != 0)
    {
        return XError(re);
    }
    cout << "open file " << inUrl << " Success." << endl;

    //获取音频视频流信息 ,h264 flv
    //re = avformat_find_stream_info(ictx, 0);

//    ifmt_ctx_a = ictx->streams[1];
//    ifmt_ctx_v = ictx->streams[0];
//####################
    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
    avformat_network_init();
    //Output
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", out_filename);
    av_opt_set(ofmt_ctx->priv_data, "rtsp_transport", "tcp", 0);
//    AVDictionary* options = NULL;
//    av_dict_set(&options, "buffer_size", "192000", 0);
    if (!ofmt_ctx) {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return -95;
    }
    ofmt = ofmt_ctx->oformat;

    for (int i = 0; i < ifmt_ctx_v->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_v->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
            videoindex_v=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                return -94;
            }
            videoindex_out=out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar) < 0) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -93;
            }
//            out_stream->codec->codec_tag = 0;
//            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//            break;
        }
    }

    for (int i = 0; i < ifmt_ctx_a->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_a->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            AVStream *in_stream = ifmt_ctx_a->streams[i];
            AVCodec *codec = avcodec_find_decoder(ofmt_ctx->streams[i]->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, codec);
            audioindex_a=i;
            if (!out_stream) {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                return -92;
            }
            audioindex_out=out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar) < 0) {
                printf( "Failed to copy context from input to output stream codec context\n");
                return -91;
            }
//            out_stream->codec->codec_tag = 0;
//            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//            break;
        }
    }

    printf("==========Output Information==========\n");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
            printf( "Could not open output file '%s'", out_filename);
            return -90;
        }
    }
    //Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf( "Error occurred when opening output file\n");
        return -90;
    }

    long long startTime = av_gettime(); // Get the current time in microseconds.
    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        //if a before b ,v before a return -1
        if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0){
            ifmt_ctx=ifmt_ctx_v;
            stream_index=videoindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){
                do{
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];
                    if(pkt.stream_index==videoindex_v){
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate); //相邻两帧的持续时间单位秒
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_v=pkt.pts;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }else{
                break;
            }
        }else{
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){
                do{
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a){
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }else{
                break;
            }
        }
        int64_t pts_time = av_rescale_q(pkt.dts, ofmt_ctx->streams[0]->time_base, {1,AV_TIME_BASE});
        int64_t now_time = av_gettime() - startTime;
        if (pts_time > now_time)
            av_usleep(pts_time-now_time);

        //FIX:Bitstream Filter
//#if USE_H264BSF
//        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
//#endif
//#if USE_AACBSF
//        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
//#endif


        //Convert PTS/DTS convert data a based time_base c to based time_base c
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

        printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf( "Error muxing packet\n");
            break;
        }
        if(pkt.stream_index==0){
            printf("Send %8d video frames to output URL\n",frame_index);
            frame_index++;
        }
        av_free_packet(&pkt);

    }
    av_write_trailer(ofmt_ctx);
    getchar();
    return 0;
}