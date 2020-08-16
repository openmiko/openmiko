/*
 * sample-Change-Resolution.c
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

/*
 * 此例目的是将输出码流960p+D1切换成输出720+VGA码流
 * 对第一路码流960p=>720p有两种实现:
 * 第一种是encoder不缩放，输出960p码流时framesource和encoder均使用960p的图像，720p时均使用720p的图像；
 * 第二种是framesource固定输出960p,encoder输出960p码流时直接编码输出，输出720p码流时通过将960p缩放成720p输出；
 * 若定义CHANGE_FS_RESOLUTION宏，则采用第一种实现方法，否则使用第二种实现方法；
 * 推荐采用第一种实现，因第一种从960p到720p使用的是原图像硬件裁剪的方法,不用占用额外的资源，第二种从960p到720p的缩放
 * 使用的是纯软件的缩放，会额外占用内存和较多的cpu资源
 *
 * 对第二路码流D1=>VGA实现思路是：
 * 输出D1码流时encoder channel直接绑定第一路framesource(输出960p的图像)，encoder通过缩放成D1尺寸的图像编码输出；
 * 输出VGA码流时直接将D1 channel先反注册后毁掉，并创建VGA channel和第二路编码Group，将其注册到第二路编码group，绑定到
 * framesource第二路通道(VGA通道)。
 *
 * 因第二路framesource(VGA通道)可能绑定有智能识别的算法，所以固定输出为VGA大小。
 */

#include <unistd.h>
#include <string.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-Change-Resolution-Common.h"

#define TAG "sample-Change-Resolution"
#define STREAM_CNT		2	/* 定义此范例有两路码流 */
#define CHANGE_FS_RESOLUTION	/* 若定义，则使用960p=>720p的第二种实现方法，决定是否改变framesource通道输出尺寸 */

int main(int argc, char *argv[])
{
	int i, ret;
	sensor_type_t sensor_type;
	IMPSensorInfo sensor_info;
	IMPFSChnAttr fschn_attr[STREAM_CNT];
	resolution_size_t fs_size[] = {resolution_size[RES_960P], resolution_size[RES_VGA]};
	resolution_size_t enc_size[] = {resolution_size[RES_960P], resolution_size[RES_D1]};
	int i_fps_num[] = {25, 25};
	int i_fps_den[] = {1, 1};
	int vbnum[] = {3, 4};
	IMPEncoderCHNAttr encchn_attr[STREAM_CNT];
	IMPPayloadType encchn_type[] = {PT_H264, PT_H264};
	int outBitRate[] = {2000, 1000};
	pthread_t stream_tid[] = {-1, -1};

	if (argc < 2) {
		IMP_LOG_ERR(TAG, "Usage:%s sensor_type, sensor_type support ov9712,ov9713,ov9750,ar0141,gc1004,jxh42,sc1035,sc1045\n", argv[0]);
		return -1;
	}

	// 1、从外部命令行获得sensor的类型
	ret = sample_res_get_sensor_type(argv[1], &sensor_type);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_get_sensor_type failed\n");
		return -1;
	}

	// 2、根据sensor类型获取sensor注册信息
	ret = sample_res_get_sensor_info(sensor_type, &sensor_info);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_get_sensor_info failed\n");
		return -1;
	}

	// 3、初始化系统
	ret = sample_res_system_init(&sensor_info);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_system_init failed\n");
		return -1;
	}

	// 4、创建两路framesource，第一路960p，第二路VGA
	memset(fschn_attr, 0, sizeof(fschn_attr));
	for (i = 0; i < STREAM_CNT; i++) {
		// 4.1、获取framesource的通道参数
		ret = sample_res_framesource_param_default(&fschn_attr[i], sensor_type, fs_size[i], i_fps_num[i], i_fps_den[i], vbnum[i]);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "sample_res_framesource_param_default(%d) failed\n", i);
			return -1;
		}

		// 4.2、初始化framesource，其中参数true为需要创建framesource channel
		ret = sample_res_framesource_init(i, &fschn_attr[i], true);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "sample_res_framesource_init(%d) failed\n", i);
			return -1;
		}
	}

	// 5、创建两个编码channel，因两者共用第一路framesource，所以只需要一个编码group即可，故在创建第一路encoder channel时，同时
	// 创建第一路编码group，第二路encoder channel直接注册到第一路编码group上
	memset(encchn_attr, 0, sizeof(encchn_attr));
	for (i = 0; i < STREAM_CNT; i++) {
		// 5.1、获取编码channel参数
		ret = sample_res_encoder_param_default(&encchn_attr[i], encchn_type[i], ENC_RC_MODE_H264CBR,
				enc_size[i], i_fps_num[i], i_fps_den[i], outBitRate[i]);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "sample_res_encoder_param_default(%d) failed\n", i);
			return -1;
		}
		// 5.2 初始化encoder，其中第二路encoder channel不需额外创建编码group，直接注册到第一路编码group上即可
		ret = sample_res_encoder_init(0, i, &encchn_attr[i], i == 0 ? true : false);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "sample_res_encoder_init(%d) failed\n", i);
			return -1;
		}
	}

	IMPCell framesource_channel0_output0 = {DEV_ID_FS, 0, 0};
	IMPCell encoder_grp0 = {DEV_ID_ENC, 0, 0};
	IMPCell framesource_channel1_output0 = {DEV_ID_FS, 1, 0};
	IMPCell encoder_grp1 = {DEV_ID_ENC, 1, 0};

	// 6、绑定第一路编码group到第一路framesource上
	ret = IMP_System_Bind(&framesource_channel0_output0, &encoder_grp0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_channel0_output0, &encoder0) failed\n", i);
		return -1;
	}

	// 7、第一次启动framesource前必须EnableTuning。
	ret = IMP_ISP_EnableTuning();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_ISP_EnableTuning failed\n");
		return -1;
	}

	// 8、启动两路framesource
	for (i = 0; i < STREAM_CNT; i++) {
		ret = sample_res_framesource_streamon(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "sample_res_framesource_streamon(%d) failed\n", i);
			return -1;
		}
	}

	// 9、获取第一路960p和第二路D1码流5s
	stream_thread_param_t stream_param[2] = {
		[0] = {
			.enc_chn_num		= 0,
			.path				= NULL,
			.enType				= PT_H264,
			.nr_frames			= -1,
		},
		[1] = {
			.enc_chn_num		= 1,
			.path				= NULL,
			.enType				= PT_H264,
			.nr_frames			= -1,
		},
	};
	for (i = 0; i < STREAM_CNT; i++) {
		ret = pthread_create(&stream_tid[i], NULL, sample_res_do_get_stream_thread_func, &stream_param[i]);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "pthread_create stream_tid[%d] failed\n", i);
			return -1;
		}
	}

	sleep(5);


	// 10、停掉两路获取码流的线程
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		pthread_cancel(stream_tid[i]);
	}

	// 11、等两路获取码流的线程完全退出后销毁两路编码的channel
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		pthread_join(stream_tid[i], NULL);
		IMP_Encoder_StopRecvPic(i);
		//11.1、false表示只销毁编码channel，不销毁编码group,省去了UnBind group的流程
		sample_res_encoder_exit(0, i, false);
	}

	resolution_size_t reenc_size[] = {resolution_size[RES_720P], resolution_size[RES_VGA]};

#ifdef CHANGE_FS_RESOLUTION
	// 12、Disable掉framesource channel0，因需要切换其分辨率到720p；
	sample_res_framesource_streamoff(0);
	// 12.1、false表示不销毁framesource 0的channel，这样可以省去变更Bind关系
	sample_res_framesource_exit(0, false);

	// 13、重新初始化framesource 0的channel为大小720p
	// 13.1、重新获取framesource channel 0的channel属性
	IMPFSChnAttr refschn0_attr;
	memset(&refschn0_attr, 0, sizeof(refschn0_attr));
	ret = sample_res_framesource_param_default(&refschn0_attr, sensor_type, reenc_size[0],
			i_fps_num[0], i_fps_den[0], vbnum[0]);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_framesource_param_default(%d) failed\n", i);
		return -1;
	}
	// 13.2、重新初始化framesource channel 0，false表示不重新创建framesource 0的channel，因之前未释放
	ret = sample_res_framesource_init(0, &refschn0_attr, false);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_framesource_init(0) failed\n");
		return -1;
	}
#endif

	// 14、重新初始化两路编码channel的通道
	for (i = 0; i < STREAM_CNT; i++) {
		// 14.1、获取编码channel的属性
		ret = sample_res_encoder_param_default(&encchn_attr[i], encchn_type[i], ENC_RC_MODE_H264CBR,
				reenc_size[i], i_fps_num[i], i_fps_den[i], outBitRate[i]);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "reuse sample_res_encoder_param_default(%d) failed\n", i);
			return -1;
		}
		// 14.2、初始化编码channel，对第二路encoder channel，true表示需要创建第二路encoder group，因之前未创建。
		ret = sample_res_encoder_init(i, i, &encchn_attr[i], i == 1 ? true : false);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "reuse sample_res_encoder_init(%d) failed\n", i);
			return -1;
		}
	}

	// 15、绑定第二路encoder group到第二路framesource
	if (STREAM_CNT > 1) {
		ret = IMP_System_Bind(&framesource_channel1_output0, &encoder_grp1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_channel1_output0, &encoder_grp1) failed\n", i);
			return -1;
		}
	}

#ifdef CHANGE_FS_RESOLUTION
	// 16、 重新开启第一路framesource，因之前关闭后重新初始化其属性到720p
	ret = sample_res_framesource_streamon(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_framesource_streamon(1) failed\n");
		return -1;
	}
#endif

	// 17、获取第一路720p和第二路VGA码流5s
	for (i = 0; i < STREAM_CNT; i++) {
		ret = pthread_create(&stream_tid[i], NULL, sample_res_do_get_stream_thread_func, &stream_param[i]);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "pthread_create stream_tid[%d] failed\n", i);
			return -1;
		}
	}

	sleep(5);

	// 18、停掉两路获取码流的线程
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		pthread_cancel(stream_tid[i]);
	}

	// 19、等待两路获取码流的线程完全停止
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		pthread_join(stream_tid[i], NULL);
		IMP_Encoder_StopRecvPic(i);
	}

	// 20、DISABLE掉两路framesource
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		sample_res_framesource_streamoff(i);
	}

	// 21、解除所有的绑定关系
	IMP_System_UnBind(&framesource_channel0_output0, &encoder_grp0);
	if (STREAM_CNT > 1) {
		IMP_System_UnBind(&framesource_channel1_output0, &encoder_grp1);
	}

	// 22、销毁之前建立的encoder和framesource channel和group
	for (i = STREAM_CNT - 1; i >= 0; i--) {
		sample_res_encoder_exit(i, i, true);
		sample_res_framesource_exit(i, true);
	}

	// 23、去初始化系统
	sample_res_system_exit(&sensor_info);

	return 0;
}
