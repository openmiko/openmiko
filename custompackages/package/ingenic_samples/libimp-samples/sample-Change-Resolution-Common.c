/*
 * sample-Change-Resolution-Common.c
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <imp/imp_log.h>
#include <imp/imp_system.h>

#include "sample-Change-Resolution-Common.h"

#define TAG "Sample-Common"

resolution_size_t resolution_size[] = {
	[RES_QCIF]	= {176, 144},	/* 176x144 */
	[RES_CIF]	= {352, 288},			/* 352x288 */
	[RES_DCIF]	= {528, 384},			/* 528x384 */
	[RES_HD1]	= {352, 576},			/* 352x576 */
	[RES_QVGA]	= {320, 240},			/* 320x240 */
	[RES_D1]	= {704, 576},				/* 704x576 */
	[RES_VGA]	= {640, 480},			/* 640x480 */
	[RES_960H]	= {960, 576},			/* 960x576 */
	[RES_720P]	= {1280, 720},			/* 1280x720 */
	[RES_960P]	= {1280, 960},			/* 1280x960 */
	[RES_1080P]	= {1920, 1080},			/* 1920x1080 */
};

static resolution_size_t sensor_resolution[] = {
	[SENSOR_OV9712]	= {1280, 720},
	[SENSOR_OV9732] = {1280, 720},
	[SENSOR_OV9750] = {1280, 720},
	[SENSOR_AR0141] = {1280, 720},
	[SENSOR_GC1004] = {1280, 720},
	[SENSOR_JXH42 ] = {1280, 720},
	[SENSOR_SC1035] = {1280, 960},
	[SENSOR_SC1045] = {1280, 720},
};

int sample_res_get_sensor_type(const char *sensor_type_string, sensor_type_t *sensor_type)
{
	if (!strncmp(sensor_type_string, "ov9712", sizeof("ov9712"))) {
		*sensor_type = SENSOR_OV9712;
	} else if (!strncmp(sensor_type_string, "ov9732", sizeof("ov9732"))) {
		*sensor_type = SENSOR_OV9732;
	} else if (!strncmp(sensor_type_string, "ov9750", sizeof("ov9750"))) {
		*sensor_type = SENSOR_OV9750;
	} else if (!strncmp(sensor_type_string, "ar0141", sizeof("ar0141"))) {
		*sensor_type = SENSOR_AR0141;
	} else if (!strncmp(sensor_type_string, "gc1004", sizeof("gc1004"))) {
		*sensor_type = SENSOR_GC1004;
	} else if (!strncmp(sensor_type_string, "jxh42", sizeof("jxh42"))) {
		*sensor_type = SENSOR_JXH42;
	} else if (!strncmp(sensor_type_string, "sc1035", sizeof("sc1035"))) {
		*sensor_type = SENSOR_SC1035;
	} else if (!strncmp(sensor_type_string, "sc1045", sizeof("sc1045"))) {
		*sensor_type = SENSOR_SC1045;
        } else if (!strncmp(sensor_type_string, "jxf22", sizeof("jxf22"))) {
                *sensor_type = SENSOR_JXF22;
        } else if (!strncmp(sensor_type_string, "jxf23", sizeof("jxf23"))) {
                *sensor_type = SENSOR_JXF23;
	}
        else {
		IMP_LOG_ERR(TAG, "Unsupported sensor type %s\n", sensor_type_string);
		return -1;
	}

	return 0;
}

int sample_res_get_sensor_info(sensor_type_t sensor_type, IMPSensorInfo *sensor_info)
{
	memset(sensor_info, 0, sizeof(IMPSensorInfo));
	switch (sensor_type) {
		case SENSOR_OV9712:
			memcpy(sensor_info->name, "ov9712", sizeof("ov9712"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "ov9712", sizeof("ov9712"));
			sensor_info->i2c.addr = 0x30;
			break;
		case SENSOR_OV9732:
			memcpy(sensor_info->name, "ov9732", sizeof("ov9732"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "ov9732", sizeof("ov9732"));
			sensor_info->i2c.addr = 0x36;
			break;
		case SENSOR_OV9750:
			memcpy(sensor_info->name, "ov9750", sizeof("ov9750"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "ov9750", sizeof("ov9750"));
			sensor_info->i2c.addr = 0x36;
			break;
		case SENSOR_AR0141:
			memcpy(sensor_info->name, "ar0141", sizeof("ar0141"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "ar0141", sizeof("ar0141"));
			sensor_info->i2c.addr = 0x10;
			break;
		case SENSOR_GC1004:
			memcpy(sensor_info->name, "gc1004", sizeof("gc1004"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "gc1004", sizeof("gc1004"));
			sensor_info->i2c.addr = 0x3c;
			break;
		case SENSOR_JXH42:
			memcpy(sensor_info->name, "jxh42", sizeof("jxh42"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "jxh42", sizeof("jxh42"));
			sensor_info->i2c.addr = 0x30;
			break;
		case SENSOR_SC1035:
			memcpy(sensor_info->name, "sc1035", sizeof("sc1035"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "sc1035", sizeof("sc1035"));
			sensor_info->i2c.addr = 0x30;
			break;
		case SENSOR_SC1045:
			memcpy(sensor_info->name, "sc1045", sizeof("sc1045"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "sc1045", sizeof("sc1045"));
			sensor_info->i2c.addr = 0x30;
			break;
		case SENSOR_JXF22:
			memcpy(sensor_info->name, "jxf22", sizeof("jxf22"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "jxf22", sizeof("jxf22"));
			sensor_info->i2c.addr = 0x40;
			break;
		case SENSOR_JXF23:
			memcpy(sensor_info->name, "jxf23", sizeof("jxf23"));
			sensor_info->cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
			memcpy(sensor_info->i2c.type, "jxf23", sizeof("jxf23"));
			sensor_info->i2c.addr = 0x40;
			break;
		default:
			IMP_LOG_ERR(TAG, "unsupport sensor type:%d\n", sensor_type);
			return -1;
	}

	return 0;
}

int sample_res_system_init(IMPSensorInfo *sensor_info)
{
	int ret = 0;

	ret = IMP_ISP_Open();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "failed to ISPOpen\n");
		goto err_IMP_ISP_Open;
	}

	ret = IMP_ISP_AddSensor(sensor_info);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to AddSensor\n");
		goto err_IMP_ISP_AddSensor;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
		goto err_IMP_ISP_EnableSensor;
	}

	ret = IMP_System_Init();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to System Init\n");
		goto err_IMP_System_Init;
	}

	IMP_LOG_DBG(TAG, "ImpSystemInit success\n");

	return 0;

err_IMP_System_Init:
	IMP_ISP_DisableSensor();
err_IMP_ISP_EnableSensor:
	IMP_ISP_DelSensor(sensor_info);
err_IMP_ISP_AddSensor:
	IMP_ISP_Close();
err_IMP_ISP_Open:
	return -1;
}

int sample_res_system_exit(IMPSensorInfo *sensor_info)
{
	IMP_System_Exit();
	IMP_ISP_DisableSensor();
	IMP_ISP_DelSensor(sensor_info);
	IMP_ISP_Close();

	return 0;
}

int sample_res_framesource_param_default(IMPFSChnAttr *fschn_attr, sensor_type_t sensor_type, resolution_size_t size,
		int i_fps_num, int i_fps_den, int vbnum)
{
	if ((sensor_type > SENSOR_TYPEEND) || (size.width > sensor_resolution[sensor_type].width)
			|| (size.height > sensor_resolution[sensor_type].height)) {
		IMP_LOG_ERR(TAG, "Unsupported sensor_type=%d,sensor_resolution(%ux%u)\n", sensor_type, size.width, size.height);
		goto err_unsupported_infsparam;
	}
	int enable_scaler = ((size.width < sensor_resolution[sensor_type].width) && (size.height < sensor_resolution[sensor_type].height)) ? 1 : 0;
	int crop_width = sensor_resolution[sensor_type].width;
	int crop_height = sensor_resolution[sensor_type].height;

	if ((sensor_resolution[sensor_type].width == size.width) || (sensor_resolution[sensor_type].height == size.height)) {
		crop_width = size.width;
		crop_height = size.height;
	}

	fschn_attr->pixFmt = PIX_FMT_NV12;
	fschn_attr->outFrmRateNum = i_fps_num;
	fschn_attr->outFrmRateDen = i_fps_den;
	fschn_attr->nrVBs = vbnum;

	fschn_attr->crop.enable = 1;
	fschn_attr->crop.top = 0;
	fschn_attr->crop.left = 0;
	fschn_attr->crop.width = crop_width;
	fschn_attr->crop.height = crop_height;

	fschn_attr->scaler.enable = enable_scaler;
	fschn_attr->scaler.outwidth = size.width;
	fschn_attr->scaler.outheight = size.height;

	fschn_attr->picWidth = size.width;
	fschn_attr->picHeight = size.height;

	return 0;

err_unsupported_infsparam:
	return -1;
}

int sample_res_framesource_init(int chn_num, IMPFSChnAttr *imp_chn_attr, bool is_create_channel)
{
	int ret = 0;

	if (is_create_channel == true) {
		/* create channel chn_num */
		ret = IMP_FrameSource_CreateChn(chn_num, imp_chn_attr);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error!\n", chn_num);
			goto err_IMP_FrameSource_CreateChn;
		}
	}

	ret = IMP_FrameSource_SetChnAttr(chn_num, imp_chn_attr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(%d) error: %d\n", ret, chn_num);
		goto err_IMP_FrameSource_SetChnAttr;
	}

	/* Check channel chn_num attr */
	IMPFSChnAttr imp_chn_attr_check;
	ret = IMP_FrameSource_GetChnAttr(chn_num, &imp_chn_attr_check);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_GetChnAttr(%d) error: %d\n", ret, chn_num);
		goto err_IMP_FrameSource_GetChnAttr;
	}

	return 0;

err_IMP_FrameSource_GetChnAttr:
err_IMP_FrameSource_SetChnAttr:
	if (is_create_channel == true) {
		IMP_FrameSource_DestroyChn(chn_num);
	}
err_IMP_FrameSource_CreateChn:
	return -1;
}

int sample_res_framesource_exit(int chn_num, bool is_destroy_channel)
{
	int ret = 0;

	if (is_destroy_channel == true) {
		/*Destroy channel i*/
		ret = IMP_FrameSource_DestroyChn(chn_num);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", chn_num, ret);
			return -1;
		}
	}
	return 0;
}

int sample_res_framesource_streamon(int chn_num)
{
	int ret = 0;
	/* Enable channels */
	ret = IMP_FrameSource_EnableChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn_num);
		return -1;
	}

	return 0;
}

int sample_res_framesource_streamoff(int chn_num)
{
	int ret = 0;
	/* Enable channels */
	ret = IMP_FrameSource_DisableChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn_num);
		return -1;
	}

	return 0;
}

int sample_res_encoder_param_default(IMPEncoderCHNAttr *chnAttr, IMPPayloadType type, IMPEncoderRcMode mode,
		resolution_size_t size, int outFrmRateNum, int outFrmRateDen, int outBitRate)
{
	IMPEncoderAttr *encAttr = &chnAttr->encAttr;
	IMPEncoderRcAttr *rcAttr = &chnAttr->rcAttr;

	if ((type != PT_H264) && (type != PT_JPEG)) {
		IMP_LOG_ERR(TAG, "unsupported encode type:%d, we only support h264 or jpeg\n", type);
		return -1;
	}

	if (type == PT_H264 && ((mode != ENC_RC_MODE_H264FIXQP) && (mode != ENC_RC_MODE_H264CBR))) {
		IMP_LOG_ERR(TAG, "unsupported h264 rcmode:%d, we only support cbr or fixqp\n", mode);
		return -1;
	}

	memset(chnAttr, 0, sizeof(IMPEncoderCHNAttr));
	encAttr->enType = type;
	encAttr->bufSize = 0;
	encAttr->profile = 0;
	encAttr->picWidth = size.width;
	encAttr->picHeight = size.height;

	if (type == PT_H264) {
		if (mode == ENC_RC_MODE_H264FIXQP) {
			rcAttr->rcMode = ENC_RC_MODE_H264FIXQP;
			rcAttr->attrH264FixQp.maxGop = outFrmRateNum * 2 / outFrmRateDen;
			rcAttr->attrH264FixQp.outFrmRate.frmRateNum = outFrmRateNum;
			rcAttr->attrH264FixQp.outFrmRate.frmRateDen = outFrmRateDen;
			rcAttr->attrH264FixQp.qp = 30;
		} else if (mode == ENC_RC_MODE_H264CBR) {
			rcAttr->rcMode = ENC_RC_MODE_H264CBR;
			rcAttr->attrH264Cbr.maxGop = outFrmRateNum * 2 / outFrmRateDen;
			rcAttr->attrH264Cbr.outFrmRate.frmRateNum = outFrmRateNum;
			rcAttr->attrH264Cbr.outFrmRate.frmRateDen = outFrmRateDen;
			rcAttr->attrH264Cbr.outBitRate = outBitRate;
			rcAttr->attrH264Cbr.maxQp = 35;
			rcAttr->attrH264Cbr.minQp = 20;
			rcAttr->attrH264Cbr.maxFPS = 30;
			rcAttr->attrH264Cbr.minFPS = 1;
			rcAttr->attrH264Cbr.IBiasLvl = 0;
			rcAttr->attrH264Cbr.FrmQPStep = 3;
			rcAttr->attrH264Cbr.GOPQPStep = 15;
			rcAttr->attrH264Cbr.AdaptiveMode = false;
			rcAttr->attrH264Cbr.GOPRelation = false;
		}
	} else if (type == PT_JPEG) {
		encAttr->profile = 2;
	}

	return 0;
}

int sample_res_encoder_init(uint32_t grp_num, uint32_t chn_num, IMPEncoderCHNAttr *chnAttr, bool is_create_group)
{
	int ret = 0;

	/* Creat Encoder Group */
	if (is_create_group == true) {
		ret = IMP_Encoder_CreateGroup(grp_num);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error: %d\n", grp_num, ret);
			goto err_IMP_Encoder_CreateGroup;
		}
	}

	/* Create Channel */
	ret = IMP_Encoder_CreateChn(chn_num, chnAttr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error: %d\n", chn_num, ret);
		goto err_IMP_Encoder_CreateChn;
	}

	/* Resigter Channel */
	ret = IMP_Encoder_RegisterChn(grp_num, chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n", grp_num, chn_num, ret);
		goto err_IMP_Encoder_RegisterChn;
	}

	return 0;

err_IMP_Encoder_RegisterChn:
	IMP_Encoder_DestroyChn(chn_num);
err_IMP_Encoder_CreateChn:
	if (is_create_group == true) {
		IMP_Encoder_DestroyGroup(grp_num);
	}
err_IMP_Encoder_CreateGroup:
	return -1;
}

int sample_res_encoder_exit(uint32_t grp_num, uint32_t chn_num, bool is_destroy_group)
{
	IMP_Encoder_UnRegisterChn(chn_num);
	IMP_Encoder_DestroyChn(chn_num);
	if (is_destroy_group == true) {
		IMP_Encoder_DestroyGroup(grp_num);
	}
	return 0;
}

static int save_stream(int fd, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++) {
		ret = write(fd, (void *)stream->pack[i].virAddr,
					stream->pack[i].length);
		if (ret != stream->pack[i].length) {
			IMP_LOG_ERR(TAG, "stream write error:%s\n", strerror(errno));
			return -1;
		}
	}

	return 0;
}

static int sample_res_get_save_stream_file_name(int chn_num, char *path, IMPPayloadType enType)
{
	int ret = 0;
	time_t now;
	struct tm *now_tm;
	char now_str[32];
	char stream_path[128];

	time(&now);
	now_tm = localtime(&now);
	strftime(now_str, 40, "%Y%m%d%I%M%S", now_tm);

	sprintf(stream_path, "%s%schn%d-stream-%s.%s", path != NULL ? path : "", path == NULL ? "" : "/", chn_num, now_str, (enType == PT_JPEG ? "jpeg" : "h264"));
	IMP_LOG_INFO(TAG, "%s(%d):Open Stream file %s\n", __func__, __LINE__, stream_path);

	ret = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);

	return ret;
}

int sample_res_do_get_stream(int enc_chn_num, char *path, IMPPayloadType enType, int nr_frames)
{
	int ret, i;
	IMPEncoderStream stream;
	int stream_fd = sample_res_get_save_stream_file_name(enc_chn_num, path, enType);

	if (stream_fd < 0) {
		IMP_LOG_ERR(TAG, "sample_res_get_save_stream_file_name(%d,%s,%d) failed\n", enc_chn_num, path, enType);
		goto err_sample_res_get_save_stream_file_name;
	}

	/* H264 Channel start receive picture */
	ret = IMP_Encoder_StartRecvPic(enc_chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", enc_chn_num);
		goto err_IMP_Encoder_StartRecvPic;
	}

	for (i = 0; (nr_frames != -1) ? (i < nr_frames) : 1; i++) {
		/* Polling H264/jpeg Stream, set timeout as 1000msec */
		ret = IMP_Encoder_PollingStream(enc_chn_num, 1000);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "Polling stream timeout, enc_chn_num=%d\n", enc_chn_num);
			continue;
		}

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		/* Get H264/jpeg Stream */
		ret = IMP_Encoder_GetStream(enc_chn_num, &stream, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream() failed\n");
			goto err_IMP_Encoder_GetStream;
		}

		ret = save_stream(stream_fd, &stream);
		if (ret < 0) {
			close(stream_fd);
			goto err_save_stream;
		}

		IMP_Encoder_ReleaseStream(enc_chn_num, &stream);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	}

	close(stream_fd);

	IMP_Encoder_StopRecvPic(enc_chn_num);

	return 0;

err_save_stream:
	IMP_Encoder_ReleaseStream(enc_chn_num, &stream);
err_IMP_Encoder_GetStream:
	IMP_Encoder_StopRecvPic(enc_chn_num);
err_IMP_Encoder_StartRecvPic:
	close(stream_fd);
err_sample_res_get_save_stream_file_name:
	return -1;
}

void *sample_res_do_get_stream_thread_func(void *args)
{
	int ret = 0;
	stream_thread_param_t *param = (stream_thread_param_t *)args;
	ret = sample_res_do_get_stream(param->enc_chn_num, param->path, param->enType, param->nr_frames);

	return (void *)ret;
}
