// TrackInfo.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

// 避免 ssize_t 引发的错误
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <iostream>  
#include <vlc/vlc.h>
#include <vlc/libvlc_media.h>

using namespace std;

// 获取轨道信息
void reading(libvlc_media_t *media)
{
	// 用于存储媒体轨迹的信息
	libvlc_media_track_t **tracks;
	unsigned tracksCount;

	// 获取轨道信息
	tracksCount = libvlc_media_tracks_get(media, &tracks);
	if (tracksCount > 0) {
		for (unsigned i = 0; i < tracksCount; i++) {
			libvlc_media_track_t *track = tracks[i];

			cout << "i_type: " << track->i_type << endl;
			cout << "i_codec: " << track->i_codec << endl;
			cout << "i_original_fourcc: " << track->i_original_fourcc << endl;

			// 编解码器
			const char* codec = libvlc_media_get_codec_description(track->i_type, track->i_codec);
			const char* fourcc_codec = libvlc_media_get_codec_description(track->i_type, track->i_original_fourcc);

			if (codec != NULL)
				cout << "track codec: " << codec << endl;

			if (fourcc_codec != NULL)
				cout << "fourcc codec: " << fourcc_codec << endl;

			cout << "i_profile: " << track->i_profile << endl;
			cout << "i_level: " << track->i_level << endl;

			if (track->psz_language != NULL)
				cout << "psz_language: " << track->psz_language << endl;
			if (track->psz_description != NULL)
				cout << "psz_description: " << track->psz_description << endl;

			// 音频轨道
			if (track->i_type == libvlc_track_audio) {
				libvlc_audio_track_t *audioTrack = track->audio;
				cout << "i_channels: " << audioTrack->i_channels << endl;
				cout << "i_rate: " << audioTrack->i_rate << endl;
			}
			else if (track->i_type == libvlc_track_video) {  // 视频轨道
				libvlc_video_track_t *videoTrack = track->video;

				// 视频宽度、高度
				cout << "i_width: " << videoTrack->i_width << endl;
				cout << "i_height: " << videoTrack->i_height << endl;
				cout << "i_sar_num: " << videoTrack->i_sar_num << endl;
				cout << "i_sar_den: " << videoTrack->i_sar_den << endl;
				cout << "i_frame_rate_num: " << videoTrack->i_frame_rate_num << endl;
				cout << "i_frame_rate_den: " << videoTrack->i_frame_rate_den << endl;

				// 帧率
				if (videoTrack->i_sar_num > 0) {
					double frameRate = videoTrack->i_frame_rate_num / (double)videoTrack->i_frame_rate_den;
					cout << "frame_rate: " << frameRate << endl;
				}

				// 像素宽高比
				if (videoTrack->i_sar_num > 0) {
					double sar = videoTrack->i_sar_num / (double)videoTrack->i_sar_den;
					cout << "sar: " << sar << endl;
				}
			}
			cout << "--------------------" << endl;
		}
		libvlc_media_tracks_release(tracks, tracksCount);
	}
}

int main()
{
	libvlc_instance_t *vlcInstance;
	libvlc_media_t *media;

	// 创建并初始化 libvlc 实例
	vlcInstance = libvlc_new(0, NULL);

	// 创建一个 media，参数是一个媒体资源位置（例如：有效的 URL）。 
	media = libvlc_media_new_path(vlcInstance, "F:\\Videos\\Detective Chinatown.rmvb");

	// 解析媒体以从中读取信息
	libvlc_media_parse(media);

	// 获取轨道信息
	reading(media);

	// 现在，不需要保留 media 了
	libvlc_media_release(media);

	// 释放 libvlc 实例
	libvlc_release(vlcInstance);

	getchar();

	return 0;
}