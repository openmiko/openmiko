/*
 * SU ADC header file.
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_ADC_H__
#define __SU_ADC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * ADC模块头文件
 */

/**
 * @defgroup Sysutils_ADC
 * @ingroup sysutils
 * @brief 模数转化模块
 *
 * 使用方法请参考Samples
 * @{
 */

/**
 * @fn int SU_ADC_Init(void);
 *
 * 初始化ADC模块
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 使用ADC之前，一定要调用这个函数。
 *
 * @attention 无。
 */
int SU_ADC_Init(void);

/**
 * @fn int SU_ADC_Exit(void);
 *
 * 去初始化ADC模块
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 不使用ADC之后，一定要调用这个函数。
 *
 * @attention 无。
 */
int SU_ADC_Exit(void);

/**
 * @fn int SU_ADC_EnableChn(uint32_t chn_num);
 *
 * 启动通道
 *
 * @param[in] chn_num 启动第几路通道
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 无.
 */
int SU_ADC_EnableChn(uint32_t chn_num);

/**
 * @fn int SU_ADC_DisableChn(uint32_t chn_num);
 *
 * 关闭通道
 *
 * @param[in] chn_num 停止第几路通道
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 无
 */
int SU_ADC_DisableChn(uint32_t chn_num);

/**
 * @fn int SU_ADC_GetChnValue(uint32_t chn_num, int *value);
 *
 * 得到第chn_num通道的ADC值
 *
 * @param[in] chn_num 第几路通道
 *
 * @param[out] value 得到的ADC值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 无
 */
int SU_ADC_GetChnValue(uint32_t chn_num, int *value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/**
 * @}
 */

#endif /* __SU_ADC_H__ */
