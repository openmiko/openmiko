/*
 * sample-common.h
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SAMPLE_COMMON_H__
#define __SAMPLE_COMMON_H__

#include <imp/imp_common.h>
#include <imp/imp_osd.h>
#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <unistd.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define SENSOR_FRAME_RATE_NUM		25
#define SENSOR_FRAME_RATE_DEN		1

#define SENSOR_JXF23

#if defined SENSOR_AR0141
#define SENSOR_NAME				"ar0141"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x10
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_OV7725
#define SENSOR_NAME				"ov7725"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x21
#define SENSOR_WIDTH			640
#define SENSOR_HEIGHT			480
#define CHN0_EN                 1
#define CHN1_EN                 0
#define CROP_EN					0
#elif defined SENSOR_OV9732
#define SENSOR_NAME				"ov9732"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x36
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_OV9750
#define SENSOR_NAME				"ov9750"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x36
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_OV9712
#define SENSOR_NAME				"ov9712"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x30
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_GC1004
#define SENSOR_NAME				"gc1004"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x3c
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_JXH42
#define SENSOR_NAME				"jxh42"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x30
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_SC1035
#define SENSOR_NAME				"sc1035"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x30
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			960
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#elif defined SENSOR_OV2710
#define SENSOR_NAME				"ov2710"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x36
#define SENSOR_WIDTH			1920
#define SENSOR_HEIGHT			1080
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1

#elif defined SENSOR_JXF22
#define SENSOR_NAME				"jxf22"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x40
#define SENSOR_WIDTH			1280
#define SENSOR_HEIGHT			720
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1

#elif defined SENSOR_JXF23
#define SENSOR_NAME				"jxf23"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x40
#define SENSOR_WIDTH			1920
#define SENSOR_HEIGHT			1080
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#endif


#define SENSOR_WIDTH_SECOND		640
#define SENSOR_HEIGHT_SECOND		360

#define NR_FRAMES_TO_SAVE		300
#define STREAM_BUFFER_SIZE		(1 * 1024 * 1024)

#define ENC_H264_CHANNEL		1
#define ENC_JPEG_CHANNEL		0

#define STREAM_FILE_PATH_PREFIX		"/tmp"
#define SNAP_FILE_PATH_PREFIX		"/tmp"

#define OSD_REGION_WIDTH		16
#define OSD_REGION_HEIGHT		34
#define OSD_REGION_WIDTH_SEC		8
#define OSD_REGION_HEIGHT_SEC   	18


#define SLEEP_TIME			1

#define FS_CHN_NUM			2 //MIN 1,MAX 2
#define IVS_CHN_ID          1

#define CH0_INDEX  0
#define CH1_INDEX  1
#define CHN_ENABLE 1
#define CHN_DISABLE 0

/*#define SUPPORT_RGB555LE*/

struct chn_conf{
	unsigned int index;//0 for main channel ,1 for second channel
	unsigned int enable;
	IMPFSChnAttr fs_chn_attr;
	IMPCell framesource_chn;
	IMPCell imp_encoder;
};

#define  CHN_NUM  ARRAY_SIZE(chn)

int sample_system_init();
int sample_system_exit();

int sample_framesource_streamon();
int sample_framesource_streamoff();

int sample_framesource_init();
int sample_framesource_exit();

int sample_encoder_init();
int sample_jpeg_init();
int sample_encoder_exit(void);

IMPRgnHandle *sample_osd_init(int grpNum);
int sample_osd_exit(IMPRgnHandle *prHandle,int grpNum);

int sample_do_get_h264_stream(int nr_frames);
int sample_get_h264_stream();
int sample_do_get_jpeg_snap(void);
int sample_get_jpeg_snap();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_COMMON_H__ */
