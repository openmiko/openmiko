/*
 * IMP ISP header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_ISP_H__
#define __IMP_ISP_H__

#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * ISP模块头文件
 */

/**
 * @defgroup IMP_ISP
 * @ingroup imp
 * @brief 图像信号处理单元。主要包含图像效果设置、模式切换以及Sensor的注册添加删除等操作
 *
 * ISP模块与数据流无关，不需要进行Bind，仅作用于效果参数设置及Sensor控制。
 *
 * ISP模块的使能步骤如下：
 * @code
 * int ret = 0;
 * ret = IMP_ISP_Open(); // step.1 创建ISP模块
 * if(ret < 0){
 *     printf("Failed to ISPInit\n");
 *     return -1;
 * }
 * IMPSensorInfo sensor;
 * sensor.name = "xxx";
 * sensor.cbus_type = SENSOR_CONTROL_INTERFACE_I2C; // OR SENSOR_CONTROL_INTERFACE_SPI
 * sensor.i2c = {
 * 	.type = "xxx", // I2C设备名字，必须和sensor驱动中struct i2c_device_id中的name一致。
 *	.addr = xx,	//I2C地址
 *	.i2c_adapter_id = xx, //sensor所在的I2C控制器ID
 * }
 * OR
 * sensor.spi = {
 *	.modalias = "xx", //SPI设备名字，必须和sensor驱动中struct spi_device_id中的name一致。
 *	.bus_num = xx, //SPI总线地址
 * }
 * ret = IMP_ISP_AddSensor(&sensor); //step.2 添加一个sensor，在此操作之前sensor驱动已经添加到内核。
 * if (ret < 0) {
 *     printf("Failed to Register sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_EnableSensor(void); //step.3 使能sensor，现在sensor开始输出图像。
 * if (ret < 0) {
 *     printf("Failed to EnableSensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_EnableTuning(); //step.4 使能ISP tuning, 然后才能调用ISP调试接口。
 * if (ret < 0) {
 *     printf("Failed to EnableTuning\n");
 *     return -1;
 * }
 *
 * 调试接口请参考ISP调试接口文档。 //step.5 效果调试。
 *
 * @endcode
 * ISP模块的卸载步骤如下：
 * @code
 * int ret = 0;
 * IMPSensorInfo sensor;
 * sensor.name = "xxx";
 * ret = IMP_ISP_DisableTuning(); //step.1 关闭ISP tuning
 * if (ret < 0) {
 *     printf("Failed to disable tuning\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_DisableSensor(); //step.2 关闭sensor，现在sensor停止输出图像；在此操作前FrameSource必须全部关闭。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_DelSensor(&sensor); //step.3 删除sensor，在此操作前sensor必须关闭。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_Close(); //step.4 清理ISP模块，在此操作前所有sensor都必须被删除。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 * @endcode
 * 更多使用方法请参考Samples
 * @{
 */

/**
* 摄像头控制总线类型枚举
*/
typedef enum {
	TX_SENSOR_CONTROL_INTERFACE_I2C = 1,	/**< I2C控制总线 */
	TX_SENSOR_CONTROL_INTERFACE_SPI,	/**< SPI控制总线 */
} IMPSensorControlBusType;

/**
* 摄像头控制总线类型是I2C时，需要配置的参数结构体
*/
typedef struct {
	char type[20];		/**< I2C设备名字，必须与摄像头驱动中struct i2c_device_id中name变量一致 */
	int addr;		/**< I2C地址 */
	int i2c_adapter_id;	/**< I2C控制器 */
} IMPI2CInfo;
/**
* 摄像头控制总线类型是SPI时，需要配置的参数结构体
*/
typedef struct {
	char modalias[32];	/**< SPI设备名字，必须与摄像头驱动中struct spi_device_id中name变量一致 */
	int bus_num;		/**< SPI总线地址 */
} IMPSPIInfo;

/**
* 摄像头注册信息结构体
*/
typedef struct {
	char name[32];					/**< 摄像头名字 */
	IMPSensorControlBusType cbus_type;	/**< 摄像头控制总线类型 */
	union {
		IMPI2CInfo i2c;				/**< I2C总线信息 */
		IMPSPIInfo spi;				/**< SPI总线信息 */
	};
	unsigned short rst_gpio;		/**< 摄像头reset接口链接的GPIO，注意：现在没有启用该参数 */
	unsigned short pwdn_gpio;		/**< 摄像头power down接口链接的GPIO，注意：现在没有启用该参数 */
	unsigned short power_gpio;		/**< 摄像头power 接口链接的GPIO，注意：现在没有启用该参数 */
} IMPSensorInfo;

/**
 * @fn int IMP_ISP_Open(void)
 *
 * 打开ISP模块
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 创建ISP模块，准备向ISP添加sensor，并开启ISP效果调试功能。
 *
 * @attention 这个函数必须在添加sensor之前被调用。
 */
int IMP_ISP_Open(void);

/**
 * @fn int IMP_ISP_Close(void)
 *
 * 关闭ISP模块
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark ISP模块，ISP模块不再工作。
 *
 * @attention 在使用这个函数之前，必须保证所有FrameSource和效果调试功能已经关闭，所有sensor都已被卸载.
 */
int IMP_ISP_Close(void);

/**
 * @fn int IMP_ISP_AddSensor(IMPSensorInfo *pinfo)
 *
 * 添加一个sensor，用于向ISP模块提供数据源
 *
 * @param[in] pinfo 需要添加sensor的信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 添加一个摄像头，用于提供图像。
 *
 * @attention 在使用这个函数之前，必须保证摄像头驱动已经注册进内核.
 */
int IMP_ISP_AddSensor(IMPSensorInfo *pinfo);

/**
 * @fn int IMP_ISP_DelSensor(IMPSensorInfo *pinfo)
 *
 * 删除一个sensor
 *
 * @param[in] pinfo 需要删除sensor的信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 删除一个摄像头。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经停止工作，即调用了IMP_ISP_DisableSensor函数.
 */
int IMP_ISP_DelSensor(IMPSensorInfo *pinfo);

/**
 * @fn int IMP_ISP_EnableSensor(void)
 *
 * 使能一个sensor
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 使能一个摄像头，使之开始传输图像, 这样FrameSource才能输出图像，同时ISP才能进行效果调试。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被添加进ISP模块.
 */
int IMP_ISP_EnableSensor(void);

/**
 * @fn int IMP_ISP_DisableSensor(void)
 *
 * 不使能一个sensor
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 不使能一个摄像头，使之停止传输图像, 这样FrameSource无法输出图像，同时ISP也不能进行效果调试。
 *
 * @attention 在使用这个函数之前，必须保证所有FrameSource都已停止输出图像，同时效果调试也在不使能态.
 */
int IMP_ISP_DisableSensor(void);

/**
 * @fn int IMP_ISP_SetSensorRegister(uint32_t reg, uint32_t value)
 *
 * 设置sensor一个寄存器的值
 *
 * @param[in] reg 寄存器地址
 *
 * @param[in] value 寄存器值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以直接设置一个sensor寄存器的值。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被使能.
 */
int IMP_ISP_SetSensorRegister(uint32_t reg, uint32_t value);

/**
 * @fn int IMP_ISP_GetSensorRegister(uint32_t reg, uint32_t *value)
 *
 * 获取sensor一个寄存器的值
 *
 * @param[in] reg 寄存器地址
 *
 * @param[in] value 寄存器值的指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以直接获取一个sensor寄存器的值。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被使能.
 */
int IMP_ISP_GetSensorRegister(uint32_t reg, uint32_t *value);

typedef enum {
	IMPISP_TUNING_OPS_MODE_DISABLE,			/**< 不使能该模块功能 */
	IMPISP_TUNING_OPS_MODE_ENABLE,			/**< 使能该模块功能 */
	IMPISP_TUNING_OPS_MODE_BUTT,			/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPTuningOpsMode;

typedef enum {
	IMPISP_TUNING_OPS_TYPE_AUTO,			/**< 该模块的操作为自动模式 */
	IMPISP_TUNING_OPS_TYPE_MANUAL,			/**< 该模块的操作为手动模式 */
	IMPISP_TUNING_OPS_TYPE_BUTT,			/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPTuningOpsType;

/**
 * @fn int IMP_ISP_EnableTuning(void)
 *
 * 使能ISP效果调试功能
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor被执行且返回成功.
 */
int IMP_ISP_EnableTuning(void);

/**
 * @fn int IMP_ISP_DisableTuning(void)
 *
 * 不使能ISP效果调试功能
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证在不使能sensor之前，先不使能ISP效果调试（即调用此函数）.
 */
int IMP_ISP_DisableTuning(void);

/**
 * @fn int IMP_ISP_Tuning_SetSensorFPS(uint32_t fps_num, uint32_t fps_den)
 *
 * 设置摄像头输出帧率
 *
 * @param[in] fps_num 设定帧率的分子参数
 * @param[in] fps_den 设定帧率的分母参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetSensorFPS(uint32_t fps_num, uint32_t fps_den);

/**
 * @fn int IMP_ISP_Tuning_GetSensorFPS(uint32_t *fps_num, uint32_t *fps_den)
 *
 * 获取摄像头输出帧率
 *
 * @param[in] fps_num 获取帧率分子参数的指针
 * @param[in] fps_den 获取帧率分母参数的指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 * @attention 在使能帧通道开始传输数据之前必须先调用此函数获取摄像头默认帧率。
 */
int IMP_ISP_Tuning_GetSensorFPS(uint32_t *fps_num, uint32_t *fps_den);

/**
 * ISP抗闪频属性参数结构体。
 */
typedef enum {
	IMPISP_ANTIFLICKER_DISABLE,	/**< 不使能ISP抗闪频功能 */
	IMPISP_ANTIFLICKER_50HZ,	/**< 使能ISP抗闪频功能, 并设置频率为50HZ */
	IMPISP_ANTIFLICKER_60HZ,	/**< 使能ISP抗闪频功能，并设置频率为60HZ */
	IMPISP_ANTIFLICKER_BUTT,	/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPAntiflickerAttr;

/**
 * @fn int IMP_ISP_Tuning_SetAntiFlickerAttr(IMPISPAntiflickerAttr attr)
 *
 * 设置ISP抗闪频属性
 *
 * @param[in] attr 设置参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetAntiFlickerAttr(IMPISPAntiflickerAttr attr);

/**
 * @fn int IMP_ISP_Tuning_GetAntiFlickerAttr(IMPISPAntiflickerAttr *pattr)
 *
 * 获得ISP抗闪频属性
 *
 * @param[in] pattr 获取参数值指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetAntiFlickerAttr(IMPISPAntiflickerAttr *pattr);

/**
 * ISP 动态范围压缩模块操作模式。
 */
typedef enum {
	IMPISP_DRC_MANUAL,				/**< ISP 动态范围压缩模块手动模式 */
	IMPISP_DRC_UNLIMIT,				/**< ISP 动态范围压缩模块最高压缩模式 */
	IMPISP_DRC_HIGH,				/**< ISP 动态范围压缩模块高压缩模式 */
	IMPISP_DRC_MEDIUM,				/**< ISP 动态范围压缩模块中压缩模式 */
	IMPISP_DRC_LOW,					/**< ISP 动态范围压缩模块低压缩模式 */
	IMPISP_DRC_DISABLE,				/**< ISP 动态范围压缩模块不使能 */
} IMPISPDrcMode;

/**
 * ISP 动态范围压缩模块属性参数。
 */
typedef struct {
	IMPISPDrcMode mode;				/**< ISP 动态范围压缩模块操作模式选择 */
	unsigned char drc_strength;			/**< 手动模式下设置的目标值，取值范围为[0, 0xff] */
	unsigned char slop_max;				/**< 强度控制参数，取值范围为[0, oxff] */
	unsigned char slop_min;				/**< 强度控制参数，取值范围为[0, oxff] */
	unsigned short black_level;			/**< DRC增强的最小像素值，取值范围为[0, oxfff] */
	unsigned short white_level;			/**< DRC增强的最大像素值，取值范围为[0, oxfff] */
} IMPISPDrcAttr;

/**
 * @fn int IMP_ISP_Tuning_SetRawDRC(IMPISPDrcAttr *pinfo)
 *
 * 设置ISP RAW图动态范围压缩模块参数
 *
 * @param[in] pinfo 动态范围参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 设置RGB RAW图动态范围压缩的操作类型，强度，需要增强的最大最小像素值。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetRawDRC(IMPISPDrcAttr *pinfo);

/**
 * @fn int IMP_ISP_Tuning_GetRawDRC(IMPISPDrcAttr *pinfo)
 *
 * 获取ISP RAW图动态范围压缩模块参数
 *
 * @param[in] pinfo 动态范围参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 设置RGB RAW图动态范围压缩的操作类型，强度，需要增强的最大最小像素值。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetRawDRC(IMPISPDrcAttr *pinfo);

/**
 * ISP 空间降噪模块属性参数。
 */
typedef struct {
	IMPISPTuningOpsMode enable;			/**< 使能空间降噪功能 */
	IMPISPTuningOpsType type;				/**< 空间降噪功能操作类型，自动或手动 */
	unsigned char sinter_strength;				/**< 空间降噪强度，手动模式有效，取值范围为[0, 0xff] */
} IMPISPSinterDenoiseAttr;

/**
 * ISP 时域降噪模块操作模式。
 */
typedef enum {
	IMPISP_TEMPER_DISABLE,				/**< ISP 时域降噪模块不使能 */
	IMPISP_TEMPER_AUTO,					/**< ISP 时域降噪模块自动模式 */
	IMPISP_TEMPER_MANUAL,				/**< ISP 时域降噪模块手动模式 */
} IMPISPTemperMode;

/**
 * ISP 时域降噪模块属性参数。
 */
typedef struct imp_isp_temper_denoise_attr {
	IMPISPTemperMode type;					/**< 时域降噪功能操作类型，不使能，自动或手动 */
	unsigned char temper_strength;				/**< 时域降噪强度，手动模式有效，取值范围为[0, 0xff] */
} IMPISPTemperDenoiseAttr;


/**
 * @fn int IMP_ISP_Tuning_SetSinterDnsAttr(IMPISPSinterDenoiseAttr *pinfo)
 *
 * 设置ISP 空间降噪模块参数
 *
 * @param[in] pinfo 时域降噪参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 设置空间降噪模块校正参数。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSinterDnsAttr(IMPISPSinterDenoiseAttr *pinfo);

/**
 * @fn int IMP_ISP_Tuning_GetSinterDnsAttr(IMPISPSinterDenoiseAttr *pinfo)
 *
 * 获取ISP 空间降噪模块参数
 *
 * @param[in] pinfo 空间降噪参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 获取空间降噪模块校正参数。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSinterDnsAttr(IMPISPSinterDenoiseAttr *pinfo);
#if 0
/**
* @fn int IMP_ISP_Tuning_SetTemperDnsCtl(IMPISPTemperDenoiseAttr *pinfo);
*
* 设置ISP 时域降噪模块模式
*
* @param[in] pinfo 时域降噪参数指针
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @remark 设置时域降噪模块模式参数，默认自动模式。
*
* @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
*/
int IMP_ISP_Tuning_SetTemperDnsCtl(IMPISPTemperDenoiseAttr *pinfo);
#endif
/**
 * @fn int IMP_ISP_Tuning_SetTemperDnsAttr(IMPISPTemperDenoiseAttr *pinfo)
 *
 * 设置ISP 时域降噪模块参数
 *
 * @param[in] pinfo 时域降噪参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 设置时域降噪模块校正参数，默认开启该模块；由于该模块需要额外内存，如果不需要请关闭该模块，一旦关闭内存就被释放，不能再被开启。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetTemperDnsAttr(IMPISPTemperDenoiseAttr *pinfo);

/**
 * @fn int IMP_ISP_Tuning_GetTemperDnsAttr(IMPISPTemperDenoiseAttr *pinfo)
 *
 * 获取ISP 时域降噪模块参数
 *
 * @param[in] pinfo 时域降噪模块参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 获取时域降噪模块校正参数。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetTemperDnsAttr(IMPISPTemperDenoiseAttr *pinfo);

/**
 * @fn int IMP_ISP_Tuning_SetWDRAttr(IMPISPTuningOpsMode mode)
 *
 * 设置ISP 宽动态模块是否使能
 *
 * @param[in] mode 宽动态模块参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 设置宽动态模块校正参数，默认开启该模块；由于该模块需要额外内存，如果不需要请关闭该模块，一旦关闭内存就被释放，不能再被开启。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetWDRAttr(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetWDRAttr(IMPISPTuningOpsMode *pmode)
 *
 * 获取ISP 宽动态模块当前状态
 *
 * @param[in] pmode 宽动态模块参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 获取宽动态模块校正参数。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetWDRAttr(IMPISPTuningOpsMode *pmode);

/**
 * ISP 综合扩展功能，场景模式选择。
 */
typedef enum {
	IMPISP_SCENE_MODE_AUTO = 0,					/**< 自动模式 */
	IMPISP_SCENE_MODE_BEACH_SNOW = 2,				/**< 海滩雪景模式 */
	IMPISP_SCENE_MODE_CANDLE_LIGHT = 3,				/**< 蜡烛灯光模式 */
	IMPISP_SCENE_MODE_DAWN_DUSK = 4,				/**< 傍晚模式 */
	IMPISP_SCENE_MODE_FALL_COLORS = 5,				/**< 秋天场景模式 */
	IMPISP_SCENE_MODE_FIREWORKS = 6,				/**< 焰火场景模式 */
	IMPISP_SCENE_MODE_LANDSCAPE = 7,				/**< 风景模式 */
	IMPISP_SCENE_MODE_NIGHT = 8,					/**< 夜晚模式 */
	IMPISP_SCENE_MODE_PARTY_INDOOR = 9,				/**< 室内晚会模式 */
	IMPISP_SCENE_MODE_SPORTS = 11,					/**< 运动模式 */
	IMPISP_SCENE_MODE_SUNSET = 12,					/**< 日落模式 */
	IMPISP_SCENE_MODE_TEXT = 13,					/**< 文本模式 */
	IMPISP_SCENE_MODE_NIGHT_PORTRAIT = 14,				/**< 夜间肖像模式 */
} IMPISPSceneMode;

/**
 * @fn int IMP_ISP_Tuning_SetSceneMode(IMPISPSceneMode mode)
 *
 * 设置ISP 综合效果场景模式
 *
 * @param[in] mode 效果场景模式参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSceneMode(IMPISPSceneMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetSceneMode(IMPISPSceneMode *pmode)
 *
 * 获取ISP 综合效果场景模式
 *
 * @param[in] pmode 效果场景模式参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSceneMode(IMPISPSceneMode *pmode);

/**
 * ISP 综合扩展功能，色彩模式选择。
 */
typedef enum {
	IMPISP_COLORFX_MODE_AUTO = 0,				/**< 自动模式 */
	IMPISP_COLORFX_MODE_BW = 1,				/**< 黑白模式 */
	IMPISP_COLORFX_MODE_SEPIA = 2,				/**< 棕褐色模式 */
	IMPISP_COLORFX_MODE_NEGATIVE = 3,			/**< 反向色模式 */
	IMPISP_COLORFX_MODE_VIVID = 9,				/**< 鲜艳模式 */
} IMPISPColorfxMode;

/**
 * @fn int IMP_ISP_Tuning_SetColorfxMode(IMPISPColorfxMode mode)
 *
 * 设置ISP 综合效果色彩模式
 *
 * @param[in] mode 色彩模式参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetColorfxMode(IMPISPColorfxMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetColorfxMode(IMPISPColorfxMode *pmode)
 *
 * 获取ISP 综合效果色彩模式
 *
 * @param[in] pmode 色彩模式参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetColorfxMode(IMPISPColorfxMode *pmode);

/**
 * @fn int IMP_ISP_Tuning_SetBrightness(unsigned char bright)
 *
 * 设置ISP 综合效果图片亮度
 *
 * @param[in] bright 图片亮度参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加亮度，小于128降低亮度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetBrightness(unsigned char bright);

/**
 * @fn int IMP_ISP_Tuning_GetBrightness(unsigned char *pbright)
 *
 * 获取ISP 综合效果图片亮度
 *
 * @param[in] bright 图片亮度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加亮度，小于128降低亮度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetBrightness(unsigned char *pbright);

/**
 * @fn int IMP_ISP_Tuning_SetContrast(unsigned char contrast)
 *
 * 设置ISP 综合效果图片对比度
 *
 * @param[in] contrast 图片对比度参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加对比度，小于128降低对比度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetContrast(unsigned char contrast);

/**
 * @fn int IMP_ISP_Tuning_GetContrast(unsigned char *pcontrast)
 *
 * 获取ISP 综合效果图片对比度
 *
 * @param[in] contrast 图片对比度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加对比度，小于128降低对比度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetContrast(unsigned char *pcontrast);

 /**
 * @fn int IMP_ISP_Tuning_SetSharpness(unsigned char sharpness)
 *
 * 设置ISP 综合效果图片锐度
 *
 * @param[in] sharpness 图片锐度参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加锐度，小于128降低锐度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSharpness(unsigned char sharpness);

/**
 * @fn int IMP_ISP_Tuning_GetSharpness(unsigned char *psharpness)
 *
 * 获取ISP 综合效果图片锐度
 *
 * @param[in] sharpness 图片锐度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加锐度，小于128降低锐度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSharpness(unsigned char *psharpness);

/**
 * @fn int IMP_ISP_Tuning_SetSaturation(unsigned char sat)
 *
 * 设置ISP 综合效果图片饱和度
 *
 * @param[in] sat 图片饱和度参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加饱和度，小于128降低饱和度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSaturation(unsigned char sat);

/**
 * @fn int IMP_ISP_Tuning_GetSaturation(unsigned char *psat)
 *
 * 获取ISP 综合效果图片饱和度
 *
 * @param[in] sat 图片饱和度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加饱和度，小于128降低饱和度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSaturation(unsigned char *psat);
#if 0
/**
 * @fn int IMP_ISP_Tuning_SetISPProcess(IMPISPTuningOpsMode mode)
 *
 * 旁路ISP模块处理，直接输出sensor图像
 *
 * @param[in] mode 是否旁路ISP模块处理参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 如果使能该功能，sensor输出的图像将不经过ISP模块的任何处理，直接输出。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetISPProcess(IMPISPTuningOpsMode mode);
#endif
/**
 * @fn int IMP_ISP_Tuning_SetFWFreeze(IMPISPTuningOpsMode mode)
 *
 * 冻结ISP固件操作，使能表示冻结ISP固件，这是ISP的自动算法将停止；不使能表示ISP固件能正常工作。
 *
 * @param[in] mode 是否冻结ISP固件参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetFWFreeze(IMPISPTuningOpsMode mode);


/**
 * @fn int IMP_ISP_Tuning_SaveAllParam(char *path)
 *
 * 保存ISP模块所有调试参数
 *
 * @param[in] path 路径参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 保存调试的所有ISP参数。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SaveAllParam(char *path);

/**
 * @fn int IMP_ISP_Tuning_SetISPBypass(IMPISPTuningOpsMode enable)
 *
 * ISP模块是否bypass
 *
 * @param[in] enable 是否bypass输出模式
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 在使用这个函数之前，必须保证ISP模块是关闭的.
 */
int IMP_ISP_Tuning_SetISPBypass(IMPISPTuningOpsMode enable);

/**
 * @fn int IMP_ISP_Tuning_GetTotalGain(uint32_t *gain)
 *
 * 获取ISP输出图像的整体增益值
 *
 * @param[in] gain 获取增益值参数的指针,其数据存放格式为[24.8]，高24bit为整数，低8bit为小数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetTotalGain(uint32_t *gain);

/**
 * 设置ISP图像镜面效果功能是否使能
 *
 * @fn int IMP_ISP_Tuning_SetISPHflip(IMPISPTuningOpsMode mode)
 *
 * @param[in] mode 是否使能镜面效果
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPHflip(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPHflip(IMPISPTuningOpsMode *pmode)
 *
 * 获取ISP图像镜面效果功能的操作状态
 *
 * @param[in] pmode 操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPHflip(IMPISPTuningOpsMode *pmode);

/**
 * @fn int IMP_ISP_Tuning_SetISPVflip(IMPISPTuningOpsMode mode)
 *
 * 设置ISP图像上下反转效果功能是否使能
 *
 * @param[in] mode 是否使能图像上下反转
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPVflip(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPVflip(IMPISPTuningOpsMode *pmode)
 *
 * 获取ISP图像上下反转效果功能的操作状态
 *
 * @param[in] pmode 操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPVflip(IMPISPTuningOpsMode *pmode);

/**
 * ISP 工作模式配置，正常模式或夜视模式。
 */
typedef enum {
	IMPISP_RUNNING_MODE_DAY = 0,				/**< 正常模式 */
	IMPISP_RUNNING_MODE_NIGHT = 1,				/**< 夜视模式 */
	IMPISP_RUNNING_MODE_BUTT,					/**< 最大值 */
} IMPISPRunningMode;

/**
 * @fn int IMP_ISP_Tuning_SetISPRunningMode(IMPISPRunningMode mode)
 *
 * 设置ISP工作模式，正常模式或夜视模式；默认为正常模式。
 *
 * @param[in] mode运行模式参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * 示例：
 * @code
 * IMPISPRunningMode mode;
 *
 *	if( it is during a night now){
		mode = IMPISP_RUNNING_MODE_NIGHT
	}else{
		mode = IMPISP_RUNNING_MODE_DAY;
	}
	ret = IMP_ISP_Tuning_SetISPRunningMode(mode);
	if(ret){
		IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetISPRunningMode error !\n");
		return -1;
	}
 *
 * @endcode
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPRunningMode(IMPISPRunningMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPRunningMode(IMPISPRunningMode *pmode)
 *
 * 获取ISP工作模式，正常模式或夜视模式。
 *
 * @param[in] pmode操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPRunningMode(IMPISPRunningMode *pmode);

/**
 * ISP 自动曝光策略配置，正常模式或曝光优先模式。
 **/
typedef enum {
	IMPISP_AE_STRATEGY_SPLIT_BALANCED = 0,              /**< 正常模式 */
	IMPISP_AE_STRATEGY_SPLIT_INTEGRATION_PRIORITY = 1,      /**< 曝光优先模式 */
	IMPISP_AE_STRATEGY_BUTT,                    /**< 最大值 */
} IMPISPAeStrategy;

/**
 * @fn int IMP_ISP_Tuning_SetAeStrategy(IMPISPAeStrategy strategy)
 *
 * 设置ISP工作模式，自动曝光策略配置，正常模式或曝光优先模式；默认为正常模式。
 *
 * @param[in] strategy 自动曝光策略参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 * 示例：
 * @code
 * IMPISPAeStrategy strategy;
 *
 *  if( it is a normal scence){
		mode = IMPISP_AE_STRATEGY_SPLIT_BALANCED
	 }else{
		mode = IMPISP_AE_STRATEGY_SPLIT_INTEGRATION_PRIORITY;
     }
	 ret = IMP_ISP_Tuning_SetAeStrategy(strategy);
	if(ret){
	IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetAeStrategy error !\n");
	return -1;
	}
*
* @endcode
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetAeStrategy(IMPISPAeStrategy strategy);

/**
* @fn int IMP_ISP_Tuning_GetAeStrategy(IMPISPAeStrategy *pstrategy)
*
* 获取ISP自动曝光策略配置，正常模式或曝光优先模式。
* @param[in] pstrategy 自动曝光策略参数指针
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetAeStrategy(IMPISPAeStrategy *pstrategy);

/**
 * gamma
 */
typedef struct {
	uint16_t gamma[129];		/**< gamma参数数组，有129个点 */
} IMPISPGamma;

/**
* @fn int IMP_ISP_Tuning_SetGamma(IMPISPGamma *gamma)
*
* 设置GAMMA参数.
* @param[in] gamma gamma参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetGamma(IMPISPGamma *gamma);

/**
* @fn int IMP_ISP_Tuning_GetGamma(IMPISPGamma *gamma)
*
* 获取GAMMA参数.
* @param[out] gamma gamma参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetGamma(IMPISPGamma *gamma);

/**
* @fn int IMP_ISP_Tuning_SetAeComp(int comp)
*
* 设置AE补偿。AE补偿参数可以调整图像AE target，建议设置范围为[90-150].
* @param[in] comp AE补偿参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetAeComp(int comp);

/**
* @fn int IMP_ISP_Tuning_GetAeComp(int *comp)
*
* 获取AE补偿。
* @param[out] comp AE补偿参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetAeComp(int *comp);

/**
 * 曝光模式
 */
enum isp_core_expr_mode {
	ISP_CORE_EXPR_MODE_AUTO,			/**< 自动模式 */
	ISP_CORE_EXPR_MODE_MANUAL,			/**< 手动模式 */
};

/**
 * 曝光单位
 */
enum isp_core_expr_unit {
	ISP_CORE_EXPR_UNIT_LINE,			/**< 行 */
	ISP_CORE_EXPR_UNIT_US,				/**< 毫秒 */
};

/**
 * 曝光参数
 */
typedef union isp_core_expr_attr{
	struct {
		enum isp_core_expr_mode mode;		/**< 设置的曝光模式 */
		enum isp_core_expr_unit unit;		/**< 设置的曝光单位 */
		uint16_t time;
	} s_attr;
	struct {
		enum isp_core_expr_mode mode;			/**< 获取的曝光模式 */
		uint16_t integration_time;		/**< 获取的曝光时间，单位为行 */
		uint16_t integration_time_min;	/**< 获取的曝光最小时间，单位为行 */
		uint16_t integration_time_max;	/**< 获取的曝光最大时间，单位为行 */
		uint16_t one_line_expr_in_us;		/**< 获取的一行曝光时间对应的微妙数 */
	} g_attr;
}IMPISPExpr;


/**
 * @fn int IMP_ISP_Tuning_SetExpr(IMPISPExpr *expr)
 *
 * 设置AE参数。
 *
 * @param[in] expr AE参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetExpr(IMPISPExpr *expr);

/**
 * @fn int IMP_ISP_Tuning_GetExpr(IMPISPExpr *expr)
 *
 * 获取AE参数。
 *
 * @param[out] expr AE参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetExpr(IMPISPExpr *expr);

/**
 * 曝光统计区域选择
 */
typedef union isp_core_ae_roi_select{
	struct {
		unsigned endy :8;                   /**< 结束点y坐标 (0 ~ 255)*/
		unsigned endx :8;                   /**< 结束点x坐标 (0 ~ 255)*/
		unsigned starty :8;                 /**< 起始点y坐标 (0 ~ 255)*/
		unsigned startx :8;                 /**< 起始点x坐标 (0 ~ 255)*/
	};
	uint32_t value;
} IMPISPAERoi;

/**
 * @fn int IMP_ISP_Tuning_AE_GetROI(IMPISPAERoi *isp_ae_roi)
 *
 * 获取AE统计区域。
 *
 * @param[out] isp_ae_roi AE矩形统计区域参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_AE_GetROI(IMPISPAERoi *isp_ae_roi);

/**
 * @fn int IMP_ISP_Tuning_AE_SetROI(IMPISPAERoi *isp_ae_roi)
 *
 * 设置AE统计区域。
 *
 * @param[in] isp_ae_roi AE矩形统计区域参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_AE_SetROI(IMPISPAERoi *isp_ae_roi);

/**
 * 白平衡模式
 */
enum isp_core_wb_mode {
	ISP_CORE_WB_MODE_AUTO = 0,			/**< 自动模式 */
	ISP_CORE_WB_MODE_MANUAL,			/**< 手动模式 */
	ISP_CORE_WB_MODE_DAY_LIGHT,			/**< 晴天 */
	ISP_CORE_WB_MODE_CLOUDY,			/**< 阴天 */
	ISP_CORE_WB_MODE_INCANDESCENT,		/**< 白炽灯 */
	ISP_CORE_WB_MODE_FLOURESCENT,		/**< 荧光灯 */
	ISP_CORE_WB_MODE_TWILIGHT,			/**< 黄昏 */
	ISP_CORE_WB_MODE_SHADE,				/**< 阴影 */
	ISP_CORE_WB_MODE_WARM_FLOURESCENT,	/**< 暖色荧光灯 */
};

/**
 * 白平衡参数
 */
typedef struct isp_core_wb_attr{
	enum isp_core_wb_mode mode;		/**< 白平衡模式，分为自动与手动模式 */
	uint16_t rgain;			/**< 红色增益，手动模式时有效 */
	uint16_t bgain;			/**< 蓝色增益，手动模式时有效 */
}IMPISPWB;

/**
 * @fn int IMP_ISP_Tuning_SetWB(IMPISPWB *wb)
 *
 * 设置白平衡功能设置。可以设置自动与手动模式，手动模式主要通过设置rgain、bgain实现。
 *
 * @param[in] wb 设置的白平衡参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetWB(IMPISPWB *wb);

/**
 * @fn int IMP_ISP_Tuning_GetWB(IMPISPWB *wb)
 *
 * 获取白平衡功能设置。
 *
 * @param[out] wb 获取的白平衡参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetWB(IMPISPWB *wb);

/**
 * @fn IMP_ISP_Tuning_GetWB_Statis(IMPISPWB *wb)
 *
 * 获取白平衡统计值。
 *
 * @param[out] wb 获取的白平衡统计值。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetWB_Statis(IMPISPWB *wb);

/**
 * @fn int IMP_ISP_Tuning_Awb_GetCwfShift(IMPISPWB* isp_wb_attr)
 *
 * 获取ISP中AWB的CWF光源的位置
 *
 * @param[out] isp_wb_attr CWF光源位置坐标
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_Awb_GetCwfShift(IMPISPWB *isp_wb_attr);

/**
 * @fn int IMP_ISP_Tuning_Awb_SetCwfShift(IMPISPWB *isp_wb_attr)
 *
 * 设置ISP中AWB的CWF光源的位置
 *
 * @param[in] isp_wb_attr CWF光源位置坐标
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 * 示例：
 * @code
 * IMPISPWB isp_wb_attr;
 *
 *isp_wb_attr.rgain = x;
 *isp_wb_attr.bgain = y;
 *IMP_ISP_Tuning_Awb_SetCwfShift(isp_wb_attr);
 if(ret){
 IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_Awb_SetCwfShift error !\n");
 return -1;
 }
 *
 * @endcode
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_Awb_SetCwfShift(IMPISPWB *isp_wb_attr);

/**
 * @fn int IMP_ISP_Tuning_SetMaxAgain(uint32_t gain)
 *
 * 设置sensor可以设置最大Again。
 *
 * @param[in] gain sensor可以设置的最大again.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetMaxAgain(uint32_t gain);

/**
 * @fn int IMP_ISP_Tuning_GetMaxAgain(uint32_t *gain)
 *
 * 获取sensor可以设置最大Again。
 *
 * @param[out] gain sensor可以设置的最大again.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetMaxAgain(uint32_t *gain);

/**
 * @fn int IMP_ISP_Tuning_SetMaxDgain(uint32_t gain)
 *
 * 设置ISP可以设置的最大Dgain。
 *
 * @param[in] ISP Dgain 可以设置的最大dgain.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetMaxDgain(uint32_t gain);

/**
 * @fn int IMP_ISP_Tuning_GetMaxDgain(uint32_t *gain)
 *
 * 获取ISP设置的最大Dgain。
 *
 * @param[out] ISP Dgain 可以得到设置的最大的dgain.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetMaxDgain(uint32_t *gain);

/**
 * @fn int IMP_ISP_Tuning_SetVideoDrop(void (*cb)(void))
 *
 * 设置视频丢失功能。当出现sensor与主板的连接线路出现问题时，设置的回调函数会被执行。
 *
 * @param[in] cb 回调函数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetVideoDrop(void (*cb)(void));

/**
 * @fn int IMP_ISP_Tuning_SetHiLightDepress(uint32_t strength)
 *
 * 设置强光抑制强度。
 *
 * @param[in] strength 强光抑制强度参数.取值范围为［0-10], 0表示关闭功能。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetHiLightDepress(uint32_t strength);

/**
 * @fn int IMP_ISP_Tuning_SetTemperStrength(uint32_t ratio)
 *
 * 设置3D降噪强度。
 *
 * @param[in] ratio 强度调节比例.如果设置为90则表示设置为默认值的90%.取值范围为［50-150].
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetTemperStrength(uint32_t ratio);

/**
 * @fn int IMP_ISP_Tuning_SetSinterStrength(uint32_t ratio)
 *
 * 设置2D降噪强度。
 *
 * @param[in] ratio 强度调节比例.如果设置为90则表示设置为默认值的90%.取值范围为［50-150].
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetSinterStrength(uint32_t ratio);

/**
 * @fn int IMP_ISP_Tuning_SetDPStrength(uint32_t ratio)
 *
 * 设置DPC强度。
 *
 * @param[in] ratio 强度调节比例.如果设置为90则表示设置为默认值的90%.取值范围为［50-150].
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetDPStrength(uint32_t ratio);

/**
 * ISP ANTIFOG 工作模式。
 */
typedef enum {
	ANTIFOG_DISABLE,			/**< 关闭 */
	ANTIFOG_STRONG,				/**< 强 */
	ANTIFOG_MEDIUM,				/**< 中 */
	ANTIFOG_WEAK,				/**< 弱 */
}IMPISPAntiFogAttr;

/**
* @fn int IMP_ISP_Tuning_SetAntiFogAttr(IMPISPAntiFogAttr attr)
*
* 设置ANTIFOG属性。
* @param[in] attr ANTIFOG属性参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetAntiFogAttr(IMPISPAntiFogAttr attr);

/**
 * ISP EV 参数。
 */
typedef struct {
	uint32_t ev;			/**< 曝光值 */
	uint32_t expr_us;		/**< 曝光时间 */
	uint32_t ev_log2;		/**<log格式曝光时间 */
	uint32_t again;			/**< 模拟增益 */
	uint32_t dgain;			/**< 数字增益 */
	uint32_t gain_log2;		/**< log格式增益 */
}IMPISPEVAttr;

/**
* @fn int IMP_ISP_Tuning_GetEVAttr(IMPISPEVAttr *attr)
*
* 获取EV属性。
* @param[out] attr EV属性参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetEVAttr(IMPISPEVAttr *attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/**
 * @}
 */

#endif /* __IMP_ISP_H__ */
