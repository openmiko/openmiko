/*
 * sample-Change-Resolution-Common.c
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SAMPLE_CHANGE_RESOLUTION_COMMON_H__
#define __SAMPLE_CHANGE_RESOLUTION_COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <imp/imp_common.h>
#include <imp/imp_isp.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

typedef enum resolution_type {
	RES_QCIF	= 0,	/* 176x144 */
	RES_CIF,			/* 352x288 */
	RES_DCIF,			/* 528x384 */
	RES_HD1,			/* 352x576 */
	RES_QVGA,			/* 320x240 */
	RES_D1,				/* 704x576 */
	RES_VGA,			/* 640x480 */
	RES_960H,			/* 960x576 */
	RES_720P,			/* 1280x720 */
	RES_960P,			/* 1280x960 */
	RES_1080P,			/* 1920x1080 */
} resolution_type_t;

typedef struct resolution_size {
	uint32_t	width;
	uint32_t	height;
} resolution_size_t;

typedef enum sensor_type {
	SENSOR_OV9712 = 0,
	SENSOR_OV9732,
	SENSOR_OV9750,
	SENSOR_AR0141,
	SENSOR_GC1004,
	SENSOR_JXH42,
	SENSOR_SC1035,
	SENSOR_SC1045,
	SENSOR_JXF22,
	SENSOR_JXF23,
	SENSOR_TYPEEND,
} sensor_type_t;

typedef struct stream_thread_param {
	int				enc_chn_num;
	char			*path;
	IMPPayloadType	enType;
	int				nr_frames;
} stream_thread_param_t;

extern resolution_size_t resolution_size[];

extern int sample_res_get_sensor_type(const char *sensor_type_string, sensor_type_t *sensor_type);
extern int sample_res_get_sensor_info(sensor_type_t sensor_type, IMPSensorInfo *sensor_info);
extern int sample_res_system_init(IMPSensorInfo *sensor_info);
extern int sample_res_system_exit(IMPSensorInfo *sensor_info);
extern int sample_res_framesource_param_default(IMPFSChnAttr *fschn_attr, sensor_type_t sensor_type, resolution_size_t size,
		int i_fps_num, int i_fps_den, int vbnum);
extern int sample_res_framesource_init(int chn_num, IMPFSChnAttr *imp_chn_attr, bool is_create_group);
extern int sample_res_framesource_exit(int chn_num, bool is_destroy_group);
extern int sample_res_framesource_streamon(int chn_num);
extern int sample_res_framesource_streamoff(int chn_num);
extern int sample_res_encoder_param_default(IMPEncoderCHNAttr *chnAttr, IMPPayloadType type, IMPEncoderRcMode mode,
		resolution_size_t size, int outFrmRateNum, int outFrmRateDen, int outBitRate);
extern int sample_res_encoder_init(uint32_t grp_num, uint32_t chn_num, IMPEncoderCHNAttr *chnAttr, bool is_create_group);
extern int sample_res_encoder_exit(uint32_t grp_num, uint32_t chn_num, bool is_destroy_group);
extern int sample_res_do_get_stream(int enc_chn_num, char *path, IMPPayloadType enType, int nr_frames);
extern void *sample_res_do_get_stream_thread_func(void *args);

#endif
