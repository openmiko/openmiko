/*
 * Misc utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_MISC_H__
#define __SU_MISC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * Sysutils 其他功能头文件
 */

/**
 * @defgroup Sysutils_Misc
 * @ingroup sysutils
 * @brief 其他功能.
 * @{
 */

/**
 * 按键事件.
 */
typedef enum {
	KEY_RELEASED,	/**< 按键抬起 */
	KEY_PRESSED,	/**< 按键按下 */
} SUKeyEvent;

/**
 * LED行为命令.
 */
typedef enum {
	LED_OFF,		/**< LED关闭 */
	LED_ON,			/**< LED打开 */
} SULedCmd;

/**
 * @fn int SU_Key_OpenEvent(void)
 *
 * 获得按键事件句柄.
 *
 * @param 无
 *
 * @retval >0 按键事件句柄.
 * @retval <=0 失败.
 *
 * @remarks 在成功获得一个按键事件句柄之后，即开始“记录”按键事件，直到关闭这个按键事件。
 * @remarks 若打开多个句柄，则每个按键事件会记录一份按键事件。
 * @remarks 例如，两个线程分别打开了一个按键事件，每个线程持有一个句柄，则这两个线程会读取到相同的事件序列。
 * 但是如果两个线程共享同一个句柄，则每个按键事件只能被读取到一次。
 *
 * @attention 无。
 */
int SU_Key_OpenEvent(void);

/**
 * @fn int SU_Key_CloseEvent(int evfd)
 *
 * 关闭按键事件.
 *
 * @param[in] evfd 按键事件句柄
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无
 *
 * @attention 无。
 */
int SU_Key_CloseEvent(int evfd);

/**
 * @fn int SU_Key_ReadEvent(int evfd, int *keyCode, SUKeyEvent *event)
 *
 * 读取按键事件.
 *
 * @param[in] evfd 按键事件句柄
 * @param[in] keyCode 按键码
 * @param[out] event 按键事件指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 该函数阻塞，直到有按键事件发生返回.
 * @remarks 按键码的定义在linux/input.h中，与GPIO的映射关系定义在kernel板级文件中。
 * @remarks 例如几个常用的按键：
 * @code
	#define KEY_HOME                102 //HOME键
	#define KEY_POWER               116 //开关机键，一般也可用来作为唤醒键
	#define KEY_WAKEUP              143 //唤醒键，除POWER键之外用来唤醒系统的按键
	#define KEY_F13                 183 //当PIR作为按键使用时被定义为F13键
 * @endcode
 *
 * @remarks 按键键码与GPIO号的定义，是否作为唤醒源，有效电平等信息均定义在内核板级文件中，如下所示：
 *
 * @code
	struct gpio_keys_button __attribute__((weak)) board_buttons[] = {
	#ifdef GPIO_HOME
		{
			.gpio           = GPIO_HOME,		//定义GPIO号
			.code           = KEY_HOME,			//定义按键码
			.desc           = "home key",
			.active_low     = ACTIVE_LOW_HOME,	//定义有效电平
	#ifdef WAKEUP_HOME
			.wakeup         = WAKEUP_HOME,		//定义是否可做为唤醒源，1为可唤醒suspend
	#endif
	#ifdef CAN_DISABLE_HOME
			.can_disable    = CAN_DISABLE_HOME,	//定义是否可以被Disable
	#endif
		},
	#endif
	#ifdef GPIO_POWER
		{
			.gpio           = GPIO_POWER,
			.code           = KEY_POWER,
			.desc           = "power key",
			.active_low     = ACTIVE_LOW_POWER,
	#ifdef WAKEUP_POWER
			.wakeup         = WAKEUP_POWER,
	#endif
	#ifdef CAN_DISABLE_POWER
			.can_disable    = CAN_DISABLE_POWER,
	#endif
		},
	#endif
	}
 * @endcode
 * @remarks 对于数字PIR，一种使用方式是把PIR作为一个按键定义，PIR触发相当于按键按下事件(@ref KEY_PRESSED)，
 * PIR恢复相当于按键抬起事件(@ref KEY_RELEASED)。若需要PIR唤醒功能，则把PIR对应的按键定义为唤醒源即可。
 * @remarks API详细使用方法请参考sample-keyevent.c.
 *
 * @attention 无。
 */
int SU_Key_ReadEvent(int evfd, int *keyCode, SUKeyEvent *event);

/**
 * @fn int SU_Key_DisableEvent(int keyCode)
 *
 * Disable按键事件.
 *
 * @param[in] keyCode 按键码
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 如果按键被配置为唤醒源，那么在系统suspend时，(无论该按键是否被Open)按下按键会使系统唤醒。
 * 在Disable按键事件后，系统会关闭按键事件的中断，该按键也就无法唤醒系统
 * @remarks 该API可用来Disable PIR"按键"唤醒系统。
 *
 * @attention 无。
 */
int SU_Key_DisableEvent(int keyCode);

/**
 * @fn int SU_Key_EnableEvent(int keyCode)
 *
 * Enable按键事件.
 *
 * @param[in] keyCode 按键码
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 作为Disable按键事件的反过程。详见@ref SU_Key_DisableEvent(int keyCode)
 *
 * @attention 无。
 */
int SU_Key_EnableEvent(int keyCode);

/**
 * @fn int SU_LED_Command(int ledNum, SULedCmd cmd)
 *
 * 发送LED命令.
 *
 * @param[in] ledNum LED号.
 * @param[in] cmd LED行为命令.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks LED号根据开发板的不同而不同。LED号定义在内核板级文件中，注册为Linux标
 * 准Fixed Regulator设备。在板级文件中需定义LED的GPIO号，有效电平，电源递归关系等
 * 信息。下面是定义了两个LED fixed regulator的例子：
 * @code
    FIXED_REGULATOR_DEF(  //定义fixed regulator
            led0,
            "LED0",         3300000,        GPIO_PA(14),
            HIGH_ENABLE,    UN_AT_BOOT,     0,
            "ldo7",         "vled0",        NULL);

    FIXED_REGULATOR_DEF(
            led1,
            "LED1",         3300000,        GPIO_PA(15),
            HIGH_ENABLE,    UN_AT_BOOT,     0,
            "ldo7",         "vled1",        NULL);

    static struct platform_device *fixed_regulator_devices[] __initdata = {
            &gsensor_regulator_device,
            &led0_regulator_device,
            &led1_regulator_device,
    };

    static int __init fix_regulator_init(void)  //在subsys_initcall_sync中注册regulator
    {
            int i;

            for (i = 0; i < ARRAY_SIZE(fixed_regulator_devices); i++)
                    fixed_regulator_devices[i]->id = i;

            return platform_add_devices(fixed_regulator_devices,
                                        ARRAY_SIZE(fixed_regulator_devices));
    }
    subsys_initcall_sync(fix_regulator_init);
 * @endcode
 * @remarks 此API的使用示例：
 * @code
   if (SU_LED_Command(0, LED_ON) < 0)  //使能LED0
       printf("LED0 turn on error\n");
   if (SU_LED_Command(1, LED_ON) < 0)  //使能LED1
       printf("LED0 turn on error\n");
   if (SU_LED_Command(0, LED_OFF) < 0)  //关闭LED0
       printf("LED1 turn off error\n");
   if (SU_LED_Command(1, LED_OFF) < 0)  //关闭LED1
       printf("LED1 turn off error\n");
 * @endcode
 * @attention 无。
 */
int SU_LED_Command(int ledNum, SULedCmd cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SU_MISC_H__ */
