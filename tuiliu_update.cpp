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

//    const char *inUrl = R"(D:\projects\audio_encode\demo.mp4)";
    const char *inUrl = R"(D:\projects\audio_encode\h265.mkv)";
//    const char *outUrl = "rtp://localhost/live";
    const char *outUrl = "rtp://localhost/live";
    int frame_index = 0;
    const int apts = 1000;
    int64_t cur_pts_v=0,cur_pts_a=0;
//    const char *outUrl = "rtmp://localhost/live";

    //初始化所有封装和解封装 flv mp4 mov mp3
    //av_register_all();

    //初始化网络库
    avformat_network_init();
    AVFormatContext *ictx = NULL;

    //打开文件，解封文件头
    int re = avformat_open_input(&ictx, inUrl, 0, 0);//
    if (re != 0)
    {
        return XError(re);
    }
    cout << "open file " << inUrl << " Success." << endl;

    //获取音频视频流信息 ,h264 flv
    re = avformat_find_stream_info(ictx, 0);
    if (re != 0)
    {
        return XError(re);
    }
    av_dump_format(ictx, 0, inUrl, 0);

    //输出流

    //创建输出流上下文
    AVFormatContext *octx = NULL;
    re = avformat_alloc_output_context2(&octx, NULL, "rtsp", outUrl);
    av_opt_set(octx->priv_data, "rtsp_transport", "tcp", 0);
    //octx->max_interleave_delta = 1000000;
//    re = avformat_alloc_output_context2(&octx, 0, "flv", outUrl);
    if (!octx)
    {
        return XError(re);
    }
    cout << "octx create success!" << endl;

    //配置输出流
    //遍历输入的AVStream
    for (int i = 0; i < ictx->nb_streams; i++)
    {
        //创建输出流
        AVCodec *codec = avcodec_find_decoder(ictx->streams[i]->codecpar->codec_id);
        AVStream *out = avformat_new_stream(octx, codec);

        //AVStream *out = avformat_new_stream(octx, );
        if (!out)
        {
            return XError(0);
        }
        //复制配置信息,同于MP4
        //re = avcodec_copy_context(out->codec, ictx->streams[i]->codec);
        re = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
        //out->codec->codec_tag = 0;
    }
    av_dump_format(octx, 0, outUrl, 1);
    //

    //rtmp推流

    //打开io
    re = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
    if (!octx->pb)
    {
        return XError(re);
    }

    //写入头信息
    //av_opt_set(octx->priv_data, "rtsp_transport", "tcp", 0);
    re = avformat_write_header(octx, 0);
    cout<<re<<endl;
    if (re < 0)
    {
        return XError(re);
    }
    cout << "avformat_write_header " << re << endl;
    AVPacket pkt;
    long long startTime = av_gettime(); // Get the current time in microseconds.
    for (;;)
    {
        int frame_a = 0;
        int frame_v = 0;
        re = av_read_frame(ictx, &pkt);
        ictx
        if (re != 0)
        {
            cout<<"re status:"<<re<<endl;
            break;
        }
//        cout << pkt.pts << " " << flush;
        //计算转换pts dts
        for(int i =0;i<2;i++){
            AVRational time_base1=ictx->streams[i]->time_base;
        if(i==0){

//        //Duration between 2 frames (us)
            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ictx->streams[i]->r_frame_rate); //frame_rate = 25,两帧之间有多少个时间单位
            //Parameters
            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE); //1000000
            pkt.dts=pkt.pts;
            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE); //Duration of this packet in AVStream->time_base units, 0 if unknown.
//
            AVRational time_base=ictx->streams[i]->time_base;
            AVRational time_base_q={1,AV_TIME_BASE};
//        int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t dts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - startTime;
            if (dts_time > now_time)
                av_usleep(dts_time-now_time);
            AVRational itime = ictx->streams[pkt.stream_index]->time_base;
            AVRational otime = octx->streams[pkt.stream_index]->time_base;
//		/* copy packet */
//		//转换PTS/DTS（Convert PTS/DTS）
            pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, itime, otime);
            pkt.pos = -1;

        }
        else{

//        //Duration between 2 frames (us)
            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ictx->streams[i]->r_frame_rate); //frame_rate = 25,两帧之间有多少个时间单位
            //Parameters
            pkt.pts=0; //1000000
            pkt.dts=pkt.pts * frame_index;
            pkt.duration=1024; //Duration of this packet in AVStream->time_base units, 0 if unknown.
//
            AVRational time_base=ictx->streams[i]->time_base;
            AVRational time_base_q={1,AV_TIME_BASE};
//        int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t dts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - startTime;
            if (dts_time > now_time)
                av_usleep(dts_time-now_time);
            AVRational itime = ictx->streams[pkt.stream_index]->time_base;
            AVRational otime = octx->streams[pkt.stream_index]->time_base;
//		/* copy packet */
//		//转换PTS/DTS（Convert PTS/DTS）
            pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, itime, otime);
            pkt.pos = -1;
        }
}
}
		//Print to Screen
		if(pkt.stream_index==0){
//			printf("Send %8d video frames to output URL\n",frame_index);
			frame_index++;
		}
        re = av_interleaved_write_frame(octx, &pkt);

    long long endTime = av_gettime();
    cout << "total time " <<endTime-startTime<< endl;
    getchar();
    return 0;
}