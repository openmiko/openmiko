/*
 * Cipher utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_CIPHER_H__
#define __SU_CIPHER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * Sysutils 加解密管理头文件
 */

/**
 * @defgroup Sysutils_Cipher
 * @ingroup sysutils
 * @brief 加解密管理
 * @{
 */

/**
 * 选择加密算法.
 * @remarks 支持AES和DES两种加密算法.
 */
typedef enum IN_UNF_CIPHER_ALG_E
{
	IN_UNF_CIPHER_ALG_AES = 0x0,
	IN_UNF_CIPHER_ALG_DES = 0x1
} IN_UNF_CIPHER_ALG;

/**
 * 选择加密模式.
 * @remarks 支持CBC和ECB两种加密模式.
 */
typedef enum IN_UNF_CIPHER_WORK_MODE_E
{
	IN_UNF_CIPHER_WORK_MODE_ECB = 0x0,
	IN_UNF_CIPHER_WORK_MODE_CBC = 0x1,
	IN_UNF_CIPHER_WORK_MODE_OTHER = 0x2
} IN_UNF_CIPHER_WORK_MODE;

/**
 * 选择加密使用的密钥长度.
 * @remarks 受硬件的限制,现阶段只支持128bit长度的KEY.
 */
typedef enum IN_UNF_CIPHER_KEY_LENGTH_E
{
	IN_UNF_CIPHER_KEY_AES_128BIT = 0x0,
} IN_UNF_CIPHER_KEY_LENGTH;

/**
 * 选择加密算法一次处理的数据长度.
 * @remarks 受硬件的限制,现阶段只支持一次处理128bit长度的数据.
 */
typedef enum IN_UNF_CIPHER_BIT_WIDTH_E
{
	IN_UNF_CIPHER_BIT_WIDTH_128BIT = 0x0,
} IN_UNF_CIPHER_BIT_WIDTH;

/**
 * 选择加密处理控制结构体.
 */
typedef struct IN_UNF_CIPHER_CTRL_S
{
	unsigned int key[4];				/**< 加密时使用的密钥 */
	unsigned int IV[4];					/**< 加密时使用的IV向量 */
	unsigned int enDataLen;				/**< 需要处理的数据总长度*/
	IN_UNF_CIPHER_ALG enAlg;			/**< 处理数据使用的加密算法*/
	IN_UNF_CIPHER_BIT_WIDTH enBitWidth;	/**< 加密算法一次处理的数据长度*/
	IN_UNF_CIPHER_WORK_MODE enWorkMode;	/**< 处理数据使用的加密算法的模式*/
	IN_UNF_CIPHER_KEY_LENGTH enKeyLen;	/**< 加密算法使用的KEY的长度*/
} IN_UNF_CIPHER_CTRL;

/**
 * @fn int SU_CIPHER_Init(void)
 *
 * 加密模块打开接口.
 *
 * @param	无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_CIPHER_Init(void);

/**
 * @fn int SU_CIPHER_Exit(void)
 *
 * 加密模块关闭接口.
 *
 * @param	无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int SU_CIPHER_Exit(void);

/**
 * @fn int SU_CIPHER_CreateHandle(void)
 *
 * 获得加密模块句柄接口.
 *
 * @param	无.
 *
 * @retval  成功: 返回句柄.
 * @retval  失败: retval <  0.
 *
 * @remarks 无.
 *
 * @attention 该函数可以多次调用,每调用一次都会返回一个句柄.
 * 当调用的N次该函数之后,需要调用N次SU_CIPHER_DestroyHandle()才可以
 * 将所有的句柄销毁.
 *
 */
int SU_CIPHER_CreateHandle(void);

/**
 * @fn int SU_CIPHER_DestroyHandle(int fd)
 *
 * 销毁加密模块句柄.
 *
 * @param[in] fd 需要销毁的句柄
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 该函数可以多次调用,每调用一次都会销毁一个句柄.
 *
 */
int SU_CIPHER_DestroyHandle(int fd);

/**
 * @fn int SU_CIPHER_ConfigHandle(int hCipher, IN_UNF_CIPHER_CTRL* Ctrl)
 *
 * 对加密模块进行配置.
 *
 * @param[in] hCipher 需要进行配置的句柄.
 * @param[in] Ctrl 带有配置信息的结构体.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention  无.
 *
 */
int SU_CIPHER_ConfigHandle(int hCipher, IN_UNF_CIPHER_CTRL* Ctrl);

/**
 * @fn int SU_CIPHER_Encrypt(int hCipher, unsigned int * srcAddr, unsigned int * dstAddr, unsigned int dataLen)
 *
 * 开始进行数据的加密.
 *
 * @param[in] hCipher 需要进行操作的句柄.
 * @param[in] srcAddr 需要进行加密的数据源地址.
 * @param[in] dstAddr 加密完成之后的数据存放地址.
 * @param[in] dataLen 需要处理的数据长度.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention  加密的数据长度dataLen最大不要超过1Mbyte (1024*1024).
 *
 */
int SU_CIPHER_Encrypt(int hCipher, unsigned int * srcAddr, unsigned int * dstAddr, unsigned int dataLen);

/**
 * @fn int SU_CIPHER_Decrypt(int hCipher, unsigned int * srcAddr, unsigned int * dstAddr, unsigned int dataLen);
 *
 * 开始进行数据的解密.
 *
 * @param[in] hCipher 需要进行操作的句柄.
 * @param[in] srcAddr 需要进行解密的数据源地址.
 * @param[in] dstAddr 解密完成之后的数据存放地址.
 * @param[in] dataLen 需要处理的数据长度.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention  解密的数据长度dataLen最大不要超过1Mbyte (1024*1024).
 *
 */
int SU_CIPHER_Decrypt(int hCipher, unsigned int * srcAddr, unsigned int * dstAddr, unsigned int dataLen);

/**
 * 错误码.
 */
#define REINIT				-10	/**< 重复初始化 */
#define INIT_FAILED			-11	/**< 初始化失败*/
#define FAILED_GETHANDLE	-12 /**< 获取句柄失败*/
#define INVALID_PARA		-13	/**< 无效的参数*/
#define SET_PARA_FAILED		-14 /**< 设置参数失败*/
#define FAILURE				-15 /**< 操作失败*/
#define SET_DATALEN_ERR		-16	/**< 设置操作数据长度错误*/
#define EXIT_ERR			-17	/**< 模块退出失败*/
#define UNINIT				-18	/**< 模块未初始化*/
#define FAILED_DESHANDLE	-19 /**< 销毁句柄失败*/

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SU_CIPHER_H__ */
