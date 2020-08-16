/*
 * Battery utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_BATTERY_H__
#define __SU_BATTERY_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * Sysutils 电池管理头文件
 */

/**
 * @defgroup Sysutils_Battery
 * @ingroup sysutils
 * @brief 电池管理
 * @{
 */

/**
 * 电池状态.
 */
typedef enum {
	Unknown = -1,		/**< 未知状态或者没有电池*/
	Charging,			/**< 充电中 */
	Discharging,		/**< 未充电 */
	Full,				/**< 充满电 */
} SUBatStatus;

/**
 * 电池事件.
 */
typedef enum {
	AC_ONLINE,			/**< AC Adapter插入 */
	AC_OFFLINE,			/**< AC Adapter拔出 */
	USB_ONLINE,			/**< USB插入 */
	USB_OFFLINE,		/**< USB拔出 */
} SUBatEvent;

/**
 * @fn int SU_Battery_GetStatus(SUBatStatus *status)
 *
 * 获取电池状态.
 *
 * @param[in] status 电池状态指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 该函数直接返回,无阻塞.
 *
 * @attention 无.
 */
int SU_Battery_GetStatus(SUBatStatus *status);

/**
 * @fn int SU_Battery_GetEvent(SUBatEvent *event)
 *
 * 获取电池事件.
 *
 * @param[out] event 电池事件指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 该函数阻塞,直到有事件发生返回.
 *
 * @attention 无.
 */
int SU_Battery_GetEvent(SUBatEvent *event);

/**
 * @fn int SU_Battery_GetCapacity(void)
 *
 * 获取电池电量百分比.
 *
 * @param 无.
 *
 * @retval >=0 电池电量百分比.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Battery_GetCapacity(void);

/**
 * @fn int SU_Battery_GetVoltageUV(void)
 *
 * 获取电池当前电压.
 *
 * @param 无.
 *
 * @retval >=0 电池电压,单位uV.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_Battery_GetVoltageUV(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SU_BATTERY_H__ */
