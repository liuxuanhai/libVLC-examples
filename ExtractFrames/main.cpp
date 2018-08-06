// 避免 ssize_t 引发的错误
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <windows.h>
#include <vlc/vlc.h>
#include <QImage>
#include <QMutex>
#include <QCoreApplication>

// 定义输出视频的分辨率
#define VIDEO_WIDTH   640
#define VIDEO_HEIGHT  480

struct context {
	QMutex mutex;
	uchar *pixels;
};

static void *lock(void *opaque, void **planes)
{
	struct context *ctx = (context *)opaque;
	ctx->mutex.lock();

	// 告诉 VLC 将解码的数据放到缓冲区中
	*planes = ctx->pixels;

	return NULL;
}

// 获取 argb 图片并保存到文件中
static void unlock(void *opaque, void *picture, void *const *planes)
{
	Q_UNUSED(picture);

	struct context *ctx = (context *)opaque;
	unsigned char *data = (unsigned char *)*planes;
	static int frameCount = 1;

	QImage image(data, VIDEO_WIDTH, VIDEO_HEIGHT, QImage::Format_ARGB32);
	image.save(QString("frame_%1.png").arg(frameCount++));

	ctx->mutex.unlock();
}

static void display(void *opaque, void *picture)
{
	Q_UNUSED(picture);

	(void)opaque;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mediaPlayer;
	libvlc_media_t *media;

	// 等待 20 秒
	int waitTime = 1000 * 20;

	struct context ctx;
	ctx.pixels = new uchar[VIDEO_WIDTH * VIDEO_HEIGHT * 4];
	memset(ctx.pixels, 0, VIDEO_WIDTH * VIDEO_HEIGHT * 4);

	// 创建并初始化 libvlc 实例
	vlcInstance = libvlc_new(0, NULL);

	// 创建一个 media，参数是一个媒体资源位置（例如：有效的 URL）。
	media = libvlc_media_new_location(vlcInstance, "rtsp://184.72.239.149/vod/mp4:BigBuckBunny_175k.mov");

	libvlc_media_add_option(media, ":avcodec-hw=none");

	// 创建一个 media player 播放环境
	mediaPlayer = libvlc_media_player_new_from_media(media);

	// 现在，不需要保留 media 了
	libvlc_media_release(media);

	// 设置回调，用于提取帧或者在屏幕中显示。
	libvlc_video_set_callbacks(mediaPlayer, lock, unlock, display, &ctx);
	libvlc_video_set_format(mediaPlayer, "RGBA", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * 4);

	// 播放 media player
	libvlc_media_player_play(mediaPlayer);

	// 让它播放一会
	Sleep(waitTime);

	// 停止播放
	libvlc_media_player_stop(mediaPlayer);

	// 释放 media player
	libvlc_media_player_release(mediaPlayer);

	// 释放 libvlc 实例
	libvlc_release(vlcInstance);

	return a.exec();
}
