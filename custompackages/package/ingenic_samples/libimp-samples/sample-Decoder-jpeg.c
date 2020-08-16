#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include <imp/imp_system.h>
#include <imp/imp_log.h>
#include <imp/imp_decoder.h>

#define TAG		"JPEGD"
#define JPEGFILENAME		"1280x720.jpeg_1"

static int sampe_decoder_system_init(void)
{
	return IMP_System_Init();
}

static int sampe_decoder_system_exit(void)
{
	return IMP_System_Exit();
}

static int sample_decoder_jpeg_init(void)
{
	int ret = 0;

	IMPDecoderCHNAttr attr = {
		.decAttr = {
			.decType		= PT_JPEG,
			.maxWidth		= 1280,
			.maxHeight		= 720,
			.pixelFormat	= PIX_FMT_NV12,
			.nrKeepStream	= 2,
			.frmRateNum		= 25,
			.frmRateDen		= 1,
		},
	};

	ret = IMP_Decoder_CreateChn(0, &attr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_CreateChn failed\n");
		goto err_IMP_Decoder_CreateChn;
	}

	return 0;

err_IMP_Decoder_CreateChn:
	return -1;
}

static int sample_decoder_jpeg_exit(void)
{
	int ret = 0;

	IMP_LOG_ERR(TAG, "sample_decoder_jpeg_exit start\n");
	ret = IMP_Decoder_DestroyChn(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_DestroyChn failed\n");
		goto err_IMP_Decoder_DestroyChn;
	}
	IMP_LOG_ERR(TAG, "sample_decoder_jpeg_exit end\n");

	return 0;

err_IMP_Decoder_DestroyChn:
	return -1;
}

int sample_decoder_get_frame(void)
{
	int ret;
	time_t now;
	struct tm *now_tm;
	char now_str[32];
	char videopath[128];
	int videofd = -1;
	IMPFrameInfo *frame = NULL;

	time(&now);
	now_tm = localtime(&now);
	strftime(now_str, 40, "%Y%m%d%I%M%S", now_tm);
	sprintf(videopath, "video-%s.nv12", now_str);

	videofd = open(videopath, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (videofd < 0) {
		IMP_LOG_ERR(TAG, "Open Video file %s failed:%s\n", videopath, strerror(errno));
		goto err_open_videopath;
	}
	IMP_LOG_DBG(TAG, "Open Video file %s OK\n", videopath);

	/* jpegd Channel start receive picture */
	ret = IMP_Decoder_StartRecvPic(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_StartRecvPic(%d) failed\n", 0);
		goto err_IMP_Decoder_StartRecvPic;
	}

retry_pooling_point:
	/* jpegd pooling stream */
	ret = IMP_Decoder_PollingFrame(0, 1000000);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_PollingFrame(%d) failed\n", 0);
		goto retry_pooling_point;
	}

	ret = IMP_Decoder_GetFrame(0, &frame);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_GetFrame(%d) failed\n", 0);
		goto err_IMP_Decoder_GetFrame;
	}

	ret = write(videofd, (void *)frame->virAddr, frame->size);
	if (ret != frame->size) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_GetFrame(%d) failed\n", 0);
		goto err_write_frame;
	}

	ret = IMP_Decoder_ReleaseFrame(0, frame);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_ReleaseFrame(%d) failed\n", 0);
		goto err_IMP_Decoder_ReleaseFrame;
	}

	ret = IMP_Decoder_StopRecvPic(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_StopRecvPic(%d) failed\n", 0);
		goto err_IMP_Decoder_StopRecvPic;
	}

	close(videofd);

	return 0;

err_IMP_Decoder_StopRecvPic:
err_IMP_Decoder_ReleaseFrame:
err_write_frame:
	IMP_Decoder_ReleaseFrame(0, frame);
err_IMP_Decoder_GetFrame:
	IMP_Decoder_StopRecvPic(0);
err_IMP_Decoder_StartRecvPic:
	close(videofd);
err_open_videopath:
	return -1;
}

void *do_get_frame(void *arg)
{
	return (void *)sample_decoder_get_frame();
}

int sample_decoder_send_stream(void)
{
	int jfd = -1;
	char *jpegbuf = NULL;
	struct stat statbuf;
	int ret = 0, filesize = 0;
	IMPDecoderStream stream;

	jfd = open(JPEGFILENAME, O_RDONLY);
	if (jfd < 0) {
		IMP_LOG_ERR(TAG, "open %s failed\n", JPEGFILENAME);
		goto err_open_jpeg_file;
	}

	ret = fstat(jfd, &statbuf);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "fstat %s failed\n", JPEGFILENAME);
		goto err_fstat_jpeg_file;
	}

	filesize = statbuf.st_size;
	jpegbuf = malloc(filesize);
	if (jpegbuf == NULL) {
		IMP_LOG_ERR(TAG, "malloc jpegbuf %d failed\n", filesize);
		goto err_malloc_jpegbuf;
	}

	ret = read(jfd, jpegbuf, filesize);
	if (ret != filesize) {
		IMP_LOG_ERR(TAG, "read file:%s %d failed\n", JPEGFILENAME, filesize);
		goto err_read_jpeg_file;
	}

	stream.decoderNal.i_payload = filesize;
	stream.decoderNal.p_payload = (uint8_t *)jpegbuf;
	stream.decoderNal.timeStamp = 123454;
	IMP_LOG_INFO(TAG, "IMP_Decoder_SendStreamTimeout start\n");
	ret = IMP_Decoder_SendStreamTimeout(0, &stream, 2000);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Decoder_SendStreamTimeout failed\n");
		goto err_IMP_Decoder_SendStreamTimeout;
	}
	IMP_LOG_INFO(TAG, "IMP_Decoder_SendStreamTimeout end\n");

	return 0;

err_IMP_Decoder_SendStreamTimeout:
err_read_jpeg_file:
	free(jpegbuf);
err_malloc_jpegbuf:
err_fstat_jpeg_file:
	close(jfd);
err_open_jpeg_file:
	return -1;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	pthread_t tid = 0;

	ret = sampe_decoder_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sampe_decoder_system_init failed\n", 0);
		goto err_sampe_decoder_system_init;
	}

	ret = sample_decoder_jpeg_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_decoder_jpeg_init failed\n", 0);
		goto err_sample_decoder_jpeg_init;
	}

	ret = pthread_create(&tid, NULL, do_get_frame, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "pthread_create do_get_frame failed\n", 0);
		goto err_pthread_create_do_get_frame;
	}

	ret = sample_decoder_send_stream();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_decoder_send_stream failed\n", 0);
		goto err_sample_decoder_send_stream;
	}

	//pthread_cancel(tid);
	pthread_join(tid, NULL);

	sample_decoder_jpeg_exit();
	sampe_decoder_system_exit();

	return 0;

err_sample_decoder_send_stream:
	pthread_cancel(tid);
	pthread_join(tid, NULL);
err_pthread_create_do_get_frame:
	sample_decoder_jpeg_exit();
err_sample_decoder_jpeg_init:
	sampe_decoder_system_exit();
err_sampe_decoder_system_init:
	return -1;
}
