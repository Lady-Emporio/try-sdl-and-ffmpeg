
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>


extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>

#include "sky.h"
//const int SCREEN_WIDTH = 640;
//const int SCREEN_HEIGHT = 480;



void saveFrame(AVFrame *avFrame, int width, int height, int frameIndex);



int64_t FrameToPts(AVStream* pavStream, int frame) 
{
	return (int64_t(frame) * pavStream->r_frame_rate.den *  pavStream -> time_base.den) /
(int64_t(pavStream->r_frame_rate.num) *
	pavStream->time_base.num);
}


int main(int argc, char *argv[]) {
	if (TTF_Init() == -1) {
		e("ttf init");
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		e("sdl init");
	}
	SDL_Surface* screen_surface = NULL;
	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Hello, SDL 2!", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (window == NULL) {
		e("sdl window");
	}
	screen_surface = SDL_GetWindowSurface(window);

	AVFormatContext *pFormatCtx = NULL;
	const char * fileName = "C:/Users/al/Desktop/1/mp4/YP-1R-01x02.mkv";
	// Эта функция читает заголовок файла и сохраняет информацию о формате файла в структуре AVFormatContext
	//int 	avformat_open_input (AVFormatContext **ps, const char *filename, AVInputFormat *fmt, AVDictionary **options)
	if (avformat_open_input(&pFormatCtx, fileName, NULL, NULL) != 0) {
		e("avformat_open_input");
	}

	// Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		e("Couldn't find stream information");
	}

	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, fileName, 0);


	int i;
	AVCodecContext * pCodecCtxOrig = NULL;
	AVCodecContext * pCodecCtx = NULL;
	// Find the first video stream
	int videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		// check the General type of the encoded data to match // AVMEDIA_TYPE_VIDEO
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) // [5]
		{
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1)
	{
		e("didn't find a video stream");
	}


	AVCodec * pCodec = NULL;
	// Find the decoder for the video stream
	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id); // [6]
	if (pCodec == NULL)
	{
		e("Unsupported codec!\n");
	}

	pCodecCtxOrig = avcodec_alloc_context3(pCodec); // [7]
	if (avcodec_parameters_to_context(pCodecCtxOrig, pFormatCtx->streams[videoStream]->codecpar) < 0) {
		e("avcodec_parameters_to_context");
	}


	pCodecCtx = avcodec_alloc_context3(pCodec); // [7]
	if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) != 0) {
		e("Could not copy codec context.\n");
	}


	// Open codec
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		e("Could not open codec.\n");
	}


	////////////////////////////////////////////////////////// Зачем начало
	// Now we need a place to actually store the frame:
	AVFrame * pFrame = NULL;
	// Allocate video frame
	pFrame = av_frame_alloc();  // [9]
	if (pFrame == NULL)
	{
// Could not allocate frame
e("Could not allocate frame.\n");
	}
	////////////////////////////////////////////////////////// Зачем конец

	AVFrame * pFrameRGB = NULL;
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
	{
		// Could not allocate frame
		e("Could not allocate frame.\n");
	}

	uint8_t * buffer = NULL;
	int numBytes;
	numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 32);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));    // [11]

	av_image_fill_arrays( // [12]
		pFrameRGB->data,
		pFrameRGB->linesize,
		buffer,
		AV_PIX_FMT_RGB24,
		pCodecCtx->width,
		pCodecCtx->height,
		32
	);


	struct SwsContext * sws_ctx = NULL;

	AVPacket * pPacket = av_packet_alloc();
	if (pPacket == NULL)
	{
		e("Could not alloc packet,\n");
	}

	sws_ctx = sws_getContext(   // [13]
		pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		AV_PIX_FMT_RGB24,   // sws_scale destination color scheme
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);



	SDL_Renderer *renderer;
	SDL_Texture *texture;

	std::vector< SDL_Texture *> mass;


	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		e("! render");
	};

	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_STREAMING,
		1920,
		1080);
	if (!texture) {
		e("! texture");
	}


	bool USE_STOP_LIMIT = false;
	int passSec = 10;
	int fps = 24;
	int maxFramesToDecode = passSec * fps;
	i = 0;
	int ret;


	//seconds=frames * time_base(fps);// .avcodec defaults to a value of 1, 000, 000 fps(so a pos of 2 seconds will be timestamp of 2000000).
	int seconds = 1000000;

	SDL_Event event;
	//// [14]
	bool isNeedSeek = true;
	int nextPos = 1;

	//while (av_read_frame(pFormatCtx, pPacket) >= 0)
	while (true)
	{
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				e("exit");
			}
			if (event.type == SDL_KEYDOWN) {
				
				if (event.key.keysym.sym == SDLK_RIGHT) {
					//if (av_seek_frame(pFormatCtx, videoStream,
					nextPos += 21;
					if(av_seek_frame(pFormatCtx, videoStream, nextPos, 0) < 0){
						e("av_seek_frame");
					}
					std::cout << nextPos << std::endl;
					isNeedSeek=true;
				}
				if (event.key.keysym.sym == SDLK_LEFT) {

					//if (av_seek_frame(pFormatCtx, videoStream,
					nextPos -= 21;
					if (av_seek_frame(pFormatCtx, videoStream, nextPos, AVSEEK_FLAG_BACKWARD) < 0) {
						e("av_seek_frame");
					}
					std::cout << nextPos << std::endl;
					isNeedSeek = true;

				}
			}
		}

		if (!isNeedSeek) {
			continue;
		}
		if (av_read_frame(pFormatCtx, pPacket) < 0) {
			continue;
		}
		else {
			isNeedSeek = false;
		}
		//else {
			//double  pts = av_frame_get_best_effort_timestamp(pFrame);
			//std::cout << "good read " << pts << std::endl;

		//метку времени декодирования(DTS — от decoding time stamp) 
		//метку времени представления(PTS — от presentation time stamp).
			std::cout <<"nextPos: "<< nextPos<<" .Presentation timestamp: '" << pPacket->pts << "'. Decompression timestamp: '" << pPacket->dts<<"'."<< std::endl;
			nextPos = pPacket->pts + 1;
		//}
		


		// Is this a packet from the video stream?
		if (pPacket->stream_index == videoStream)
		{
			// Decode video frame
			// avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pPacket);
			// Deprecated: Use avcodec_send_packet() and avcodec_receive_frame().
			ret = avcodec_send_packet(pCodecCtx, pPacket);    // [15]
			if (ret < 0)
			{
				// could not send packet for decoding
				printf("Error sending packet for decoding.\n");

				// exit with eror
				continue;
				return 1;
			}

			while (ret >= 0)
			{
				ret = avcodec_receive_frame(pCodecCtx, pFrame);   // [15]

				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				{
					// EOF exit loop
					break;
				}
				else if (ret < 0)
				{
					// could not decode packet
					printf("Error while decoding.\n");

					// exit with error
					return -1;
				}

				// Convert the image from its native format to RGB
				sws_scale(  // [16]
					sws_ctx,
					(uint8_t const * const *)pFrame->data,
					pFrame->linesize,
					0,
					pCodecCtx->height,
					pFrameRGB->data,
					pFrameRGB->linesize
				);

				// Save the frame to disk
				if (++i <= maxFramesToDecode || !USE_STOP_LIMIT)
				{
					// save the read AVFrame into ppm file
					//saveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);











					//########################


					sws_scale(sws_ctx,
						pFrame->data,
						pFrame->linesize,
						0,
						pCodecCtx->height,
						pFrameRGB->data,
						pFrameRGB->linesize);

					SDL_UpdateTexture(texture, NULL, pFrameRGB->data[0], pFrameRGB->linesize[0]);

					

					SDL_RenderClear(renderer);
					SDL_RenderCopy(renderer, texture, NULL, NULL);

					SDL_Texture *image = renderText( std::to_string(pPacket->pts), renderer);
					renderTexture(image, renderer, 0, 0, 100, 100);

					SDL_RenderPresent(renderer);




					//########################
				}
				else
				{
					break;
				}
			}

			if (i > maxFramesToDecode &&USE_STOP_LIMIT)
			{
				// exit loop and terminate
				break;
			}
		}

		// Free the packet that was allocated by av_read_frame
		// [FFmpeg-cvslog] avpacket: Replace av_free_packet with
		// av_packet_unref
		// https://lists.ffmpeg.org/pipermail/ffmpeg-cvslog/2015-October/094920.html
		av_packet_unref(pPacket);
	}



	return 0;
}
#include <fstream>
void saveFrame(AVFrame *avFrame, int width, int height, int frameIndex)
{
	std::cout << "save file" << std::endl;
	FILE * pFile;
	//char szFilename[32];
	std::string szFilename = "C:/Users/al/Desktop/1/mp4/1/frame" + std::to_string(frameIndex) + ".ppm";
	int  y;

	 // Open file
	pFile = fopen(szFilename.c_str(), "wb");
	if (pFile == NULL)
	{
		return;
	}

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y = 0; y < height; y++)
	{
		fwrite(avFrame->data[0] + y * avFrame->linesize[0], 1, width * 3, pFile);
	}

	// Close file
	fclose(pFile);
}
