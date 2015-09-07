#include<stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef __cplusplus
extern "C"
{
#endif

#include<libavformat/avformat.h>
#include<libavutil/mathematics.h>
#include<libavutil/time.h>

#ifdef __cplusplus
}
#endif

#define USE_H264BSF 0

int main(int argc,char **argv)
{
	AVOutputFormat *ofmt = NULL;
	AVFormatContext *ifmt_ctx = NULL,*ofmt_ctx = NULL;
	AVPacket pkt;
	const char *in_filename,*out_filename;	
	int ret,i;
	int video_index=-1;
	int frame_index=0;
	in_filename="rtsp://192.168.1.138:8554/ch1";
	out_filename="receive.flv";

	av_register_all();
	avformat_network_init();

	if((ret=avformat_open_input(&ifmt_ctx,in_filename,0,0))<0)
	{
		printf("Could not open input file.\n");
		goto end;
	}
	if((ret=avformat_find_stream_info(ifmt_ctx,0))<0)
	{
		printf("Failed to retrieve input stream information\n");
		goto end;
	}

	for(i=0;i<ifmt_ctx->nb_streams;i++)
		if(ifmt_ctx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			video_index=i;
			break;
		}

	av_dump_format(ifmt_ctx,0,in_filename,0);

	//output
	//avformat_alloc_output_context2(&ofmt_ctx,NULL,NULL,out_filename);
	int temp_ret;
	//temp_ret = avformat_alloc_output_context2(&ofmt_ctx,NULL,NULL,out_filename);
	//avformat_alloc_output_context2(&ofmt_ctx,NULL,/*"mpeg"*/"flv",out_filename);
	avformat_alloc_output_context2(&ofmt_ctx,NULL,NULL,out_filename);
	//printf("%d\n",temp_ret);
	
	if(!ofmt_ctx)
	{
		printf("Could not create output context\n");
		ret=AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for(i=0;i<ifmt_ctx->nb_streams;i++)
	{	//Create output AVStream according to input AVStream
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx,in_stream->codec->codec);
		if(!out_stream)
		{
			printf("Failed allocating output stream.\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}

	//Copy the settings of AVCodecContext
	ret = avcodec_copy_context(out_stream->codec,in_stream->codec);
	if(ret<0)
	{
		printf("Failed to copy context from input to output stream codec context\n");
		goto end;
	}
	out_stream->codec->codec_tag = 0;

	if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	}

	//Dump format--------------------
	av_dump_format(ofmt_ctx,0,out_filename,1);
	// open output URL
	if(!(ofmt->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&ofmt_ctx->pb,out_filename,AVIO_FLAG_WRITE);
		if(ret<0)
		{
			printf("Could not open output URL '%s'",out_filename);
			goto end;
		}
	}

	//write file header
	ret = avformat_write_header(ofmt_ctx,NULL);
	if(ret < 0)
	{
		printf("Error occured when opening output URL\n");
		goto end;
	}

#if USE_H264BSF
	AVBitStreamFilterContext *h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif

	while(1)
	{
		AVStream *in_stream,*out_stream;
		//Get an AVPacket
		ret = av_read_frame(ifmt_ctx,&pkt);
		if(ret<0)
			break;

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		//copy packet
		//convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts,in_stream->time_base,out_stream->time_base,(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));  
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);  
       	 	pkt.pos = -1;  

		//Print to Screen
		if(pkt.stream_index==video_index)
		{
			printf("Receive %8d video frames from input URL\n",frame_index);
			frame_index++;

#if USE_H264BSF
	 av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0); 
#endif
		
		}
		ret = av_interleaved_write_frame(ofmt_ctx,&pkt);
		if(ret < 0)
		{
			printf("Error muxing packet\n");
			break;
		}

		av_free_packet(&pkt);

	}

#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif

	//Write file trailer
	av_write_trailer(ofmt_ctx);

end:
	avformat_close_input(&ifmt_ctx);
	//Close input
	if(ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if(ret<0 && ret != AVERROR_EOF)
	{
		printf("Error occured.\n");
		return -1;
	}
	return 0;
	
}












