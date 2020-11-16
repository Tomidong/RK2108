

#include <rtthread.h>
#include <rthw.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <dfs_posix.h>
#include <dfs_select.h>
//#include <termios.h>
#include "drv_gpio.h"
#include "drv_heap.h"
#include "hal_base.h"
#include "drv_gpio.h"  
#include "rtdef.h"
#include "drv_keyctrl.h"
#include "rk_audio.h"
#include "blsx.h"

#define LOG_TAG "blxs"
#define LOG_LVL LOG_LVL_DBG

#include <ulog.h>


#define RT_TIMER_THREAD_STACK_SIZE 1024*2

static rt_device_t wdg_dev;    /* 看门狗设备句柄 */
//static rt_mq_t msg_queue = {0};	

static int s_bt_fd = -1;	//蓝牙串口通信的描述符

static rt_uint8_t rt_thread_stack[RT_TIMER_THREAD_STACK_SIZE];
static rt_uint8_t rt_thread_stack2[RT_TIMER_THREAD_STACK_SIZE];

static struct rt_event event;

extern int tinyplay_audio();	//开始录音的函数(文件的路径暂时被写死了)
extern int tinycap_audio();		//开始播放的函数(文件的路径在那时被写死了)
extern void app_record_control(int num); 
extern void app_player_control(int num);
extern void capture_state_set(record_type_e data); //录音时的控制函数
extern void player_state_set(player_type_e data);	//播放时的控制函数
extern int vol_sub();		
extern int vol_add();


//功放(静音)控制函数，eg: falg为真，静音；反之不静音。
void app_mute_control(mute_e flag)
{
	int ret = 0;
	ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_B1);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, 1&(~ret));
	
	/*if(flag == MUTE_TYPE)
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, GPIO_LOW);
	else 
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, GPIO_HIGH);*/
}

//3个LED灯的控制
void app_led_control(led_state_e led_state)
{
	int ret = 0;
		
	switch(led_state)
	{
		case LED_RECORD:
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_A6);
			HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, 1&(~ret));
			
			//ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_D2);
			//HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D2, 1&(~ret));
			break;
			
		case LED_MUTE:
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_A7);
			HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A7, 1&(~ret));
			break;
			
		case LED_BT_CONNECT:
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_B0);
			HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B0, 1&(~ret));
			break;
			
		default:
			LOG_E("app_led_control error");
			break;
	}
	//rt_printf("ret is %d\n", 1&(~ret));
}


//发送消息
static void app_key_doing(char num)
{
	cmd_type data = 0;
	
	rt_printf("num is %d\n", num);

	//短按是1-4，长按是8-11
	if(num == 0x1)
	{		
		data = TYPE_POWER;			 
	}
	else if(num == 0x2)
	{
		data = TYPE_RECORD;
	}
	else  if(num == 0x3)
	{
		data = TYPE_MUTE;
	}
	else  if(num == 0x4)
	{
		data = TYPE_VOL_ADD;
	}
	else  if(num == 0x8)
	{
		data = 8;
	}
	else  if(num == 0x9)
	{
		data = 9;
	}
	else  if(num == 0xa)
	{
		data = 10;
	}
	else  if(num == 0xb)
	{
		data = 11;
	}
	else
	{
		data = YTPE_MAX;
	}
	
	rt_event_send(&event, data);	
}


//按键触发时的回调函数
static rt_err_t app_key_read_data_cb(rt_device_t dev, rt_size_t size)
{
    char ch;
    rt_size_t i;
    rt_err_t ret = RT_EOK;
	//rt_printf("size is %d\n", size);
		
    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            //rt_printf("dev : %s read 0x%x\n", dev->parent.name, ch);
			app_key_doing(ch);						
        }
        else
        {
            ret = rt_get_errno();
            LOG_E("dev : %s read error %d", dev->parent.name, ret);
        }
    }

    return ret;
}

rt_device_t keyctrl_dev = RT_NULL;

//打开按键的字符设备，用于检测按键的值
static int app_key_open_device(char * pcName)
{
	rt_err_t result;
    struct rt_keyctrl_info info;
	

	//通过名字查找list_device命令里面的设备
	keyctrl_dev = rt_device_find(pcName);
	
	RT_ASSERT(keyctrl_dev != RT_NULL);
	
	//获取设备信息
	result = rt_device_control(keyctrl_dev, RT_KEYCTRL_CTRL_GET_INFO, &info);
    if (result != RT_EOK)
    {
        LOG_E("device : %s cmd RT_KEYCTRL_CTRL_GET_INFO failed\n",
                   keyctrl_dev->parent.name);
        return result;
    }

	result = rt_device_init(keyctrl_dev);
    if (result != RT_EOK)
    {
        LOG_E("To initialize device:%s failed. The error code is %d\n",
                   keyctrl_dev->parent.name, result);
        return result;
    }

    result = rt_device_open(keyctrl_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (result != RT_EOK)
    {
        LOG_E("To open device:%s failed. The error code is %d\n",
                   keyctrl_dev->parent.name, result);
        return result;
    }

	//设置按键的回调函数
    rt_device_set_rx_indicate(keyctrl_dev, app_key_read_data_cb);

    return RT_EOK;
	
}

static void app_gpio_init()
{
	//红灯（默认是点亮的，设置低电平就是点亮，高电平就是熄灭）
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A6, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A6, GPIO_OUT);
	//HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, 1);
    //红灯
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A7, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A7, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A7, 1);
	//HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A7, GPIO_IN);
	
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D2, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D2, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D2, 1);
	
	//功放引脚
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_B1, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_B1, GPIO_OUT);

	//双色的红蓝灯
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_B0, PIN_CONFIG_MUX_FUNC0); //蓝灯
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_B0, GPIO_OUT);
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D2, PIN_CONFIG_MUX_FUNC0); //红灯
	HAL_GPIO_SetPinDirection(GPIO_BANK0, GPIO_PIN_D2, GPIO_OUT);
	
}

static void wakeup_isr(void *args)
{
    rt_interrupt_enter();
	
	if(rt_current_pm_state() != 1)
	{
		rt_pm_request(1);
	}
    rt_interrupt_leave();
}

void gpio_interrupt_init(void)
{
    rt_err_t ret;
    struct GPIO_REG *gpio = GPIO0;

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D2, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D2, GPIO_IN);
    ret = rt_pin_attach_irq(BANK_PIN(GPIO_BANK0, 26), PIN_IRQ_MODE_RISING_FALLING, wakeup_isr, "wakeup_isr");
    RT_ASSERT(ret == RT_EOK);
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANK0, 26),  PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);

    gpio->INT_EN_L = 0x02000200;
}


void wdg_keepalive(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}

//看门狗的初始化
void app_wdt_init()
{
	
	rt_uint32_t timeout = 1;       /* 溢出时间，单位：秒*/
	int type  = 0;
	
	/* 根据设备名称查找看门狗设备，获取设备句柄 */
	wdg_dev = rt_device_find("dw_wdt");
	/* 初始化设备 */
	rt_device_init(wdg_dev);

	/* 设置看门狗溢出时间 */
	rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, (void *)timeout);

	rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, &type);
	
	/* 设置空闲线程回调函数 */
	rt_thread_idle_sethook(wdg_keepalive);
	
}

extern void application_start();
extern int rk_snd_card_init(void);
extern int rt_hw_codec_es8311_init(void);

static void app_thread_entry(void *parameter)
{
	
	uint32_t ret = -1;

	//打开按键检测
	app_key_open_device("keyctrl");	

	//打开看门狗
	app_wdt_init();
	
	//if (access(RT_SDCARD_MOUNT_POINT, F_OK) < 0)
	//	mkdir(RT_SDCARD_MOUNT_POINT, 0);
	
    /* 初始化事件对象 */
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
	
	LOG_I("hello world!, %s\n", __func__);
	
	//application_start();
	
    while (1)
    {
    	//rt_thread_mdelay(1000);
		//rt_printf("hello world123!, %s\n", __func__);
		//rt_kprintf("current tick: %ld\n", rt_tick_get());
    	#if 1
    	//等待消息
		rt_event_recv(&event, TYPE_POWER|TYPE_RECORD|TYPE_MUTE|TYPE_VOL_ADD|TYPE_VOL_SUB,
						RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      	RT_WAITING_FOREVER, &ret);
		
		switch(ret)
		{
			case TYPE_POWER:
				LOG_D("power control");
				//tinyplay_audio();
				app_led_control(2);
				//tinycap_audio();
				//rt_pm_request(1);
				break;

			case TYPE_RECORD:
				LOG_D("record control");
				//tinycap_audio();
				//tinyplay_audio();
				app_led_control(1);
				//rt_pm_release(1);
				break;

			case TYPE_MUTE:
				LOG_D("mute control play");
				//tinyplay_audio();
				//app_mute_control(1);
				app_led_control(3);
				//player_vol_add();		
				break;

			case TYPE_VOL_ADD:
				LOG_D("add vol control");
				//player_vol_add();
				//player_vol_sub();
				break;
			
			case TYPE_VOL_SUB:
				LOG_D("sub vol control");
				//player_vol_sub();
				break;
			
			case 8:
				break;
				
			case 9:
				break;
				
			case 10:
				break;
				
			case 11:
				break;
				
			default:
				LOG_E("have not this select");
				break;
		}		
		
		#endif				
    }
}

static void app_thread_entry2(void *parameter)
{
	int i = 0, ret = 0;

	for(i = 0; i < 10; i++)
	{
		rt_thread_mdelay(1000);
		if((ret = rt_hw_codec_es8311_init()) == RT_EOK)
		{
			LOG_I("es8311 init suc, count is %d\n", i+1);
			break;
		}
		else if(ret == -1)
		{
			LOG_I("es8311 already init\n");
			break;
		}
			
	}
	
	rt_thread_mdelay(10);
	rk_snd_card_init();
	rt_thread_mdelay(10);

	//蓝牙初始化
	//application_start();
	//system("bt");

	LOG_I("application_start init suc!\n");
}


//所有的线程都可以在这里面创建
void app_thread_init(void)
{
	static struct rt_thread app;
	static struct rt_thread app2;
	
    /* initialize thread */
    rt_thread_init(&app,
                   "app",
                   app_thread_entry,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX - 1,
                   32);

	/* initialize thread */
    rt_thread_init(&app2,
                   "app2",
                   app_thread_entry2,
                   RT_NULL,
                   &rt_thread_stack2[0],
                   sizeof(rt_thread_stack2),
                   RT_THREAD_PRIORITY_MAX - 1,
                   32);
		
    /* startup */
    rt_thread_startup(&app);
	//rt_thread_startup(&app2);
	
	LOG_I("create task suc! \r\n");
	
}

void sd_mount()
{
    while (1)
    {
        rt_thread_mdelay(5000);
        if(rt_device_find("root") != RT_NULL)
        {
            if (dfs_mount("root", "/", "elm", 0, 0) == RT_EOK)
            {
                LOG_I("root mount to '/'");
                break;
            }
            else
            {
                LOG_W("root mount to '/' failed!");
            }
        }
    }
}


void blsx_entry()
{
	//i/o初始化		
	app_gpio_init();

	//gpio_interrupt_init();

	//rt_hw_gpio_init();
	
	//创建线程
	app_thread_init();
	//sd_mount();
}



