#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_COMMON_TEST_SUSPEND
#include "hal_base.h"

struct TIMER_REG *pTimer = TIMER0;
static uint32_t sleep_count = 0 ;
static rt_event_t wakeup_event;
static char rt_thread_stack[2048] = {0};

static int flag = 1;
static int a = 0;

void timer0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    pTimer->INTSTATUS = 0x01;
    rt_kprintf("suspend count: %d\n", sleep_count++);

    rt_interrupt_leave();
}

void testfun(void *parameter)
{
	while (1)
    {
        rt_kprintf("hello world\n");
		rt_thread_mdelay(500);
    }
}


static void timer0_wakeup_init(int argc, char **argv)
{
    int time_sec = 0;
    int sec_to_count = 0;
    if (argc >= 2)
    {
        time_sec = atoi(argv[1]);
    }

    sec_to_count = time_sec * PLL_INPUT_OSC_RATE;

    HAL_NVIC_SetIRQHandler(TIMER0_IRQn, (NVIC_IRQHandler) & timer0_IRQHandler);
    HAL_NVIC_EnableIRQ(TIMER0_IRQn);

    rt_kprintf("timer0 auto wake up:\n");
    pTimer->INTSTATUS = 0x01;
    pTimer->CONTROLREG = 0x00;
    pTimer->LOAD_COUNT[0] = sec_to_count;
    pTimer->LOAD_COUNT[1] = 0x0000;
    pTimer->CONTROLREG = 0x05;

	/*static struct rt_thread test;
	
    rt_thread_init(&test,
                   "test",
                   testfun,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX - 1,
                   100);
	//timer_app_init();
	rt_thread_startup(&test);*/

    return;
}

/* Pisces EVB use the GPIO1_B1 to wakeup system */
#define GPIO_BANKx      GPIO_BANK0//(GPIO_BANK1)
#define PINNUM_INT  (26)
#define GPIOx  GPIO0//(GPIO1)
#define GPIO_PIN_INT  GPIO_PIN_D2//(GPIO_PIN_B1)

#if 0
static void gpio_isr(void *args)
{
    /* enter interrupt */
    rt_interrupt_enter();

    if (HAL_GPIO_GetPinLevel(GPIOx, GPIO_PIN_INT) == GPIO_HIGH)
    {
        rt_kprintf("%s high 0\n", args);
        //rt_pm_request(1);
    }
    else
    {
        rt_kprintf("%s low 1\n", args);
        //rt_pm_release(1);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}
#else

static void gpio_isr(void *args)
{
    rt_interrupt_enter();
	
	if(rt_current_pm_state() != 1)
	{
		rt_pm_request(1);
	}
    rt_interrupt_leave();
}

#endif

int gpio_app()
{
	rt_kprintf("gpio_app!\n");
	/*static int flag = 1;
	rt_kprintf("key press!\n");
	if(flag == 1)
	{
		flag = 0;
		rt_pm_release(1);
	}
	else 
	{
		flag = 1;
		rt_pm_request(1);
	}*/

	return 0;
}

void gpio_test_interrupt(void *parameter)
{
    rt_err_t ret;
    struct GPIO_REG *gpio = GPIO0;

    rt_kprintf("gpio_test_interruptgpio_test_interruptgpio_test_interrupt\n");

    HAL_PINCTRL_SetIOMUX(GPIO_BANKx, GPIO_PIN_INT, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIOx, GPIO_PIN_INT, GPIO_IN);
    ret = rt_pin_attach_irq(BANK_PIN(GPIO_BANKx, PINNUM_INT), PIN_IRQ_MODE_FALLING, gpio_isr, "gpio isr");
    RT_ASSERT(ret == RT_EOK);
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANKx, PINNUM_INT),  PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);

    gpio->INT_EN_L = 0x02000200;

	wakeup_event = rt_event_create("wakup", RT_IPC_FLAG_FIFO);
    RT_ASSERT(wakeup_event != RT_NULL);

	while (1)
    {
        /* 等待唤醒事件 */
        if (rt_event_recv(wakeup_event,
                          1,
                          RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_FOREVER, RT_NULL) == RT_EOK)
        {
        	
            gpio_app();
        }
    }
}

static void _timeout_entry(void *parameter)
{
    rt_kprintf("current tick: %ld\n", rt_tick_get());
    /*if(flag == 1)
    {
    	if(HAL_GPIO_GetPinLevel(GPIOx, GPIO_PIN_INT) == GPIO_LOW)
    	{
    		a++;
			if(a >= 2)
			{
				a = 0;
				flag = 0;
				rt_kprintf("key press123!\n");
				rt_event_send(wakeup_event, 1);
			}
			
    	}
    }*/
}

static int timer_app_init(void)
{
	static rt_timer_t timer1;
	
    timer1 = rt_timer_create("timer_app",
                             _timeout_entry,
                             RT_NULL,
                             1000,
                             RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    if (timer1 != RT_NULL)
    {
        rt_timer_start(timer1);

        return 0;
    }
    else
    {
        return -1;
    }
}


void gpio_wake(int argc, char **argv)
{
    //gpio_test_interrupt();
	
	static struct rt_thread gpio_wake;
	
    /* initialize thread */
    rt_thread_init(&gpio_wake,
                   "gpio_wake",
                   gpio_test_interrupt,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX - 1,
                   100);
	//timer_app_init();
	rt_thread_startup(&gpio_wake);
}

static int b = 1;
void led_test(int argc, char **argv)
{
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A6, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A6, GPIO_OUT);
	
	rt_kprintf("key data is %d\n", HAL_KeyCtrl_GetValue(KEY_CTRL));
	if(b == 1)
	{
		b = 0;
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, 1);
	}
	else
	{
		b = 1;
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, 0);
	}
}


#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(timer0_wakeup_init, timer_wakeup, timer wakeup cmd);
MSH_CMD_EXPORT(gpio_wake, gpio test cmd);
MSH_CMD_EXPORT(led_test, led_test);

#endif

#endif
