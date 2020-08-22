/*
 * sample-OSD.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_log.h>
#include <imp/imp_encoder.h>
#include <imp/imp_osd.h>
#include <imp/imp_utils.h>

#include "sample-common.h"

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
#endif

#define TAG "Sample-OSD"

extern struct chn_conf chn[];

int grpNum = 0;
IMPRgnHandle *prHander;

static int osd_show(void)
{
	int ret;

	ret = IMP_OSD_ShowRgn(prHander[0], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() timeStamp error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[1], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Logo error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[2], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[3], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Rect error\n");
		return -1;
	}


	return 0;
}

static void *update_thread(void *p)
{
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0;
	void *dateData = NULL;
	uint32_t *data = p;
	IMPOSDRgnAttrData rAttrData;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	while(1) {
			int penpos_t = 0;
			int fontadv = 0;

			time(&currTime);
			currDate = localtime(&currTime);
			memset(DateStr, 0, 40);
			strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
			for (i = 0; i < 20; i++) {
				switch(DateStr[i]) {
					case '0' ... '9':
						dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
						fontadv = gBgramap[DateStr[i] - '0'].width;
						penpos_t += gBgramap[DateStr[i] - '0'].width;
						break;
					case '-':
						dateData = (void *)gBgramap[10].pdata;
						fontadv = gBgramap[10].width;
						penpos_t += gBgramap[10].width;
						break;
					case ' ':
						dateData = (void *)gBgramap[11].pdata;
						fontadv = gBgramap[11].width;
						penpos_t += gBgramap[11].width;
						break;
					case ':':
						dateData = (void *)gBgramap[12].pdata;
						fontadv = gBgramap[12].width;
						penpos_t += gBgramap[12].width;
						break;
					default:
						break;
				}
#ifdef SUPPORT_RGB555LE
				for (j = 0; j < OSD_REGION_HEIGHT; j++) {
					memcpy((void *)((uint16_t *)data + j*20*OSD_REGION_WIDTH + penpos_t),
							(void *)((uint16_t *)dateData + j*fontadv), fontadv*2);
				}
#else
				for (j = 0; j < OSD_REGION_HEIGHT; j++) {
					memcpy((void *)((uint32_t *)data + j*20*OSD_REGION_WIDTH + penpos_t),
							(void *)((uint32_t *)dateData + j*fontadv), fontadv*4);
				}

#endif
			}
			rAttrData.picData.pData = data;
			IMP_OSD_UpdateRgnAttrData(prHander[0], &rAttrData);

			sleep(1);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int ret;

	/* only show OSD in chn0 */
	chn[0].enable = 1;
	chn[1].enable = 0;

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}

	ret = IMP_Encoder_CreateGroup(chn[0].index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(0) error !\n");
		return -1;
	}

	/* Step.3 Encoder init */
	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	if (IMP_OSD_CreateGroup(0) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateGroup(0) error !\n");
		return -1;
	}

	/* Step.4 OSD init */
	prHander = sample_osd_init(grpNum);
	if (prHander <= 0) {
		IMP_LOG_ERR(TAG, "OSD init failed\n");
		return -1;
	}

	/* Step.5 Bind */
	IMPCell osdcell = {DEV_ID_OSD, 0, 0};
	ret = IMP_System_Bind(&chn[0].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel0 and OSD failed\n");
		return -1;
	}

	ret = IMP_System_Bind(&osdcell, &chn[0].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
		return -1;
	}

	/* Step.6 Create OSD bgramap update thread */
	pthread_t tid;
#ifdef SUPPORT_RGB555LE
	uint32_t *timeStampData = malloc(20 * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * 2);
#else
	uint32_t *timeStampData = malloc(20 * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * 4);
#endif
	if (timeStampData == NULL) {
		IMP_LOG_ERR(TAG, "valloc timeStampData error\n");
		return -1;
	}

	ret = pthread_create(&tid, NULL, update_thread, timeStampData);
	if (ret) {
		IMP_LOG_ERR(TAG, "thread create error\n");
		return -1;
	}

	/* Step.7 Stream On */
	IMP_FrameSource_SetFrameDepth(0, 0);
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	/* Step.8 Get stream */
	ret = sample_do_get_h264_stream(NR_FRAMES_TO_SAVE);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}

	/* Exit sequence as follow */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMPStreamOff failed\n");
		return -1;
	}

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	free(timeStampData);

	/* Step.b UnBind */
	ret = IMP_System_UnBind(&osdcell, &chn[0].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind OSD and Encoder failed\n");
		return -1;
	}

	ret = IMP_System_UnBind(&chn[0].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource and OSD failed\n");
		return -1;
	}

	/* Step.c OSD exit */
	ret = sample_osd_exit(prHander,grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD exit failed\n");
		return -1;
	}

	/* Step.d Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.e FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.f System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
