#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/core/core.hpp>//OpenCV????????  
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/opencv.hpp>  
#define WIDTH 1920
#define HIGH 1080
#define NUM 1000
using namespace cv;

#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif 

#ifdef __cplusplus 
extern "C" {
#endif 
	/*Include ffmpeg header file*/
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 

#include <libavutil/imgutils.h>
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/error.h>
#ifdef __cplusplus 
}
#endif 

void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt_hevc, FILE *f)
{
	int ret;

	/* send the frame to the encoder */
	if (frame)
		printf("Send frame %3\n", frame->pts);

	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		exit(1);
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt_hevc);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			exit(1);
		}
		// pkt_hevc->stream_index = videoindex;
		// if (av_interleaved_write_frame(fmt_ctx, pkt_hevc) < 0) {
		// 	printf("av_interleaved_write_frame failed");
		// 	exit(1);
		// }
		printf("Write packet %3(size=%5d)\n", pkt_hevc->pts, pkt_hevc->size);
		fwrite(pkt_hevc->data, 1, pkt_hevc->size, f);
		av_packet_unref(pkt_hevc);
	}
}

int main()
{
	AVCodec *codec_hevc;
	AVCodecContext *codec_ctx_hevc = NULL;
	//const AVCodec *codec_wav;
	//AVCodecContext *codec_ctx_wav = NULL;
	// AVFormatContext *out_fmt_ctx = NULL;
	int videoindex = -1, audioindex = -1;
	const char *picname, *audioname, *outfilename;
	char filename_num[64];
	picname = "D:/workstation/temppic/lovestory";
	outfilename = "D:/a.h265";
	//img=img(Rect(0, 0, img.cols - 1, img.rows));
	//char filename[100] = "1.264";
	char *codec_name;
	int i, ret;
	int fps = 1; //????????
	FILE *f;
	AVFrame *frame;
	AVPacket *pkt_hevc;
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };


	// avformat_network_init();
	/* find the mpeg1video encoder */
	codec_hevc = avcodec_find_encoder(AV_CODEC_ID_HEVC);  //?????????

	codec_ctx_hevc = avcodec_alloc_context3(codec_hevc);  //????AVCodecContext????????????? ��???? ??????avcodec_open2()??
	if (!codec_ctx_hevc) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}

	pkt_hevc = av_packet_alloc();
	if (!pkt_hevc)
		exit(1);

	/* put sample parameters */
	codec_ctx_hevc->bit_rate = 400000;  //??��?bps
	/* resolution must be a multiple of two */
	codec_ctx_hevc->width = WIDTH;
	codec_ctx_hevc->height = HIGH;
	/* frames per second */
	codec_ctx_hevc->time_base.den = 25;
	codec_ctx_hevc->time_base.num = 1;
	codec_ctx_hevc->framerate.den = 1;
	codec_ctx_hevc->framerate.num = 25;
	codec_ctx_hevc->qmin = 10;
	codec_ctx_hevc->qmax = 51;

	codec_ctx_hevc->gop_size = 250;  //???????????????I??????????????????????? ?????10
	codec_ctx_hevc->max_b_frames = 3; //???B???????B??????��??????????B????????????????????
	codec_ctx_hevc->pix_fmt = AV_PIX_FMT_YUV420P;  //?????????????
	// codec_ctx_hevc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	if (codec_hevc->id == AV_CODEC_ID_HEVC)
		av_opt_set(codec_ctx_hevc->priv_data, "preset", "slow", 0);

	/* open it */
	ret = avcodec_open2(codec_ctx_hevc, codec_hevc, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open codec");
		exit(1);
	}

	fopen_s(&f, outfilename, "wb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", picname);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
	frame->format = codec_ctx_hevc->pix_fmt;
	frame->width = codec_ctx_hevc->width;
	frame->height = codec_ctx_hevc->height;

	ret = av_frame_get_buffer(frame, 0); //??????0
	if (ret < 0) {
		fprintf(stderr, "Could not allocate the video frame data\n");
		exit(1);
	}


	// init_rtsp_pusher(out_fmt_ctx, codec_ctx_hevc, codec_hevc);

	// avformat_write_header(out_fmt_ctx, NULL);

	//?????????????
	struct SwsContext *sws_ctx;
	enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_BGR24;
	enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_YUV420P;
	sws_ctx = sws_getContext(codec_ctx_hevc->width, codec_ctx_hevc->height, src_pix_fmt,
		codec_ctx_hevc->width, codec_ctx_hevc->height, dst_pix_fmt,
		SWS_POINT, NULL, NULL, NULL);

	/* encode 1 second of video */
	for (i = 1; i != NUM + 1; i++) {
		fflush(stdout);
		sprintf(filename_num, "%s%d.jpg", picname, i);
		Mat img = imread(filename_num), reimg;
		resize(img, reimg, Size(WIDTH, HIGH));
		//reimg = img;

		/* make sure the frame data is writable */
		ret = av_frame_make_writable(frame);
		if (ret < 0)
			exit(1);

		//RGB?YUV
		int imgwidthlen = reimg.step;
		uint8_t *reimgdata = new uint8_t[reimg.rows*reimg.step];
		memcpy(reimgdata, reimg.data, reimg.rows*reimg.step);
		//imshow("123",img);
		//waitKey();
		sws_scale(sws_ctx, &reimgdata,
			&imgwidthlen, 0, codec_ctx_hevc->height, frame->data, frame->linesize);
		// for (int m = 0; m < out_fmt_ctx->nb_streams; m++) {
		// 	if (out_fmt_ctx->streams[m]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
		// 		videoindex = m;
		// 		break;
		// 	}
		// }
		for (int j = 1; j < fps+1; j++) {
			frame->pts = (i-1)*fps+j;
		//	/* encode the image */
			encode(codec_ctx_hevc, frame, pkt_hevc, f);
		}
		img.release();
		reimg.release();
	}
	/* flush the encoder */
	encode(codec_ctx_hevc, NULL, pkt_hevc, f);

	/* add sequence end code to have a real MPEG file */
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);
	// av_write_trailer(out_fmt_ctx);

	avcodec_free_context(&codec_ctx_hevc);
	av_frame_free(&frame);
	av_packet_free(&pkt_hevc);

	// avformat_close_input(&out_fmt_ctx);

	return 0;
}