/*
 * System utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_BASE_H__
#define __SU_BASE_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * Sysutils 基础功能头文件
 */

/**
 * @defgroup sysutils System Utils
 */

/**
 * @defgroup Sysutils_Base
 * @ingroup sysutils
 * @brief 系统基础功能. 
 * @{
 */

/**
 * 设备ID逻辑编码
 */
#define DEVICE_ID_MAGIC     "53ef"

/**
 * 设备ID逻辑编码长度
 */
#define DEVICE_ID_MAGIC_LEN 4

/**
 * 设备ID长度
 */
#define DEVICE_ID_LEN       32

/**
 * 设备型号\设备ID\固件版本信息的最大长度
 */
#define MAX_INFO_LEN        64

/**
 * 设备型号.
 */
typedef struct {
	char chr[MAX_INFO_LEN];		/**< 设备型号字符串 */
} SUModelNum;

/**
 * 设备软件版本.
 */
typedef struct {
	char chr[MAX_INFO_LEN];		/**< 设备软件版本字符串 */
} SUVersion;

/**
 * 设备ID.设备ID为唯一值，不同的CPU芯片间的值有差异
 */
typedef union {
	char chr[MAX_INFO_LEN];		/**< 设备ID字符串 */
	uint8_t hex[MAX_INFO_LEN];	/**< 设备ID二进制 */
} SUDevID;

/**
 * 系统时间结构体.
 */
typedef struct {
	int sec;	/**< 秒数，范围：0~59 */
	int min;	/**< 分钟数，范围：0~59 */
	int hour;	/**< 小时数，范围：0~23 */
	int mday;	/**< 一个月中的第几天，范围：1~31 */
	int mon;	/**< 月份，范围：1~12 */
	int year;	/**< 年份，范围：>1900 */
} SUTime;

/**
 * @fn int SU_Base_GetModelNumber(SUModelNum *modelNum)
 *
 * 获取设备型号.
 *
 * @param[out] modelNum 设备型号结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Base_GetModelNumber(SUModelNum *modelNum);

/**
 * @fn int SU_Base_GetVersion(SUVersion *version)
 *
 * 获取设备版本.
 *
 * @param[out] version 设备版本结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Base_GetVersion(SUVersion *version);

/**
 * @fn int SU_Base_GetDevID(SUDevID *devID)
 *
 * 获取设备ID.
 *
 * @param[out] devID 设备ID结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 每颗CPU芯片的设备ID是唯一的.
 *
 * @attention 无.
 */
int SU_Base_GetDevID(SUDevID *devID);

/**
 * @fn int SU_Base_GetTime(SUTime *time)
 *
 * 获得系统时间.
 *
 * @param[in] time 系统时间结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Base_GetTime(SUTime *time);

/**
 * @fn int SU_Base_SetTime(SUTime *time)
 *
 * 设置系统时间.
 *
 * @param[out] time 系统时间结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 系统时间参数需在合理范围，否则函数调用失败.
 */
int SU_Base_SetTime(SUTime *time);

/**
 * @fn int SU_Base_SUTime2Raw(SUTime *suTime, uint32_t *rawTime)
 *
 * 将SUTime类型的时间转换为以秒为单位的Raw时间.
 *
 * @param[in] suTime 系统时间结构体指针.
 * @param[out] rawTime Raw时间(从1970-01-01 00:00:00开始算起).
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 此函数可以用在设置相对秒数的Alarm.
 *
 * @attention 无.
 */
int SU_Base_SUTime2Raw(SUTime *suTime, uint32_t *rawTime);

/**
 * @fn int SU_Base_Raw2SUTime(uint32_t *rawTime, SUTime *suTime)
 *
 * 将以秒为单位的Raw时间转换为SUTime类型的时间.
 *
 * @param[in] rawTime Raw时间(从1970-01-01 00:00:00开始算起).
 * @param[out] suTime 系统时间结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 此函数可以用在设置相对秒数的Alarm.
 *
 * @attention 无.
 */
int SU_Base_Raw2SUTime(uint32_t *rawTime, SUTime *suTime);

/**
 * @fn int SU_Base_SetAlarm(SUTime *time)
 *
 * 设定闹钟时间.
 *
 * @param[in] time 系统时间结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 暂支持24小时内的闹钟设定.
 *
 * @attention 系统时间参数需在合理范围，否则函数调用失败.
 */
int SU_Base_SetAlarm(SUTime *time);

/**
 * @fn int SU_Base_GetAlarm(SUTime *time)
 *
 * 获得闹钟定时时间.
 *
 * @param[out] time 系统时间结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Base_GetAlarm(SUTime *time);

/**
 * @fn int SU_Base_EnableAlarm()
 *
 * 使能闹钟.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该函数之前，请调用SU_Base_GetAlarm（SUTime *time）设定闹钟时间.
 *
 * @attention 如果闹钟时间在当前系统时间之前返回失败.
 */
int SU_Base_EnableAlarm(void);

/**
 * @fn int SU_Base_DisableAlarm()
 *
 * 关闭闹钟.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Base_DisableAlarm(void);

/**
 * @fn int SU_Base_PollingAlarm(uint32_t timeoutMsec)
 *
 * 等待闹钟.
 *
 * @param[in] 超时时间，单位：毫秒.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该函数后，程序会进入阻塞状态，一直到闹钟响应退出或超时退出.
 *
 * @attention 无.
 */
int SU_Base_PollingAlarm(uint32_t timeoutMsec);

/**
 * @fn int SU_Base_Shutdown(void)
 *
 * 设备关机.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该函数后设备会立即关机并关闭主电源.
 *
 * @attention 在调用此函数之前请确保已保存所有文件.
 */
int SU_Base_Shutdown(void);

/**
 * @fn int SU_Base_Reboot(void)
 *
 * 设备重启.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该函数后设备会立即重启.
 *
 * @attention 在调用此函数之前请确保已保存所有文件.
 */
int SU_Base_Reboot(void);

/**
 * @fn int SU_Base_Suspend(void)
 *
 * 设备休眠.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 调用该函数后设备会立即进入休眠,函数正常退出后说明系统已经唤醒.
 *
 * @attention 无.
 */
int SU_Base_Suspend();

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SU_BASE_H__ */
