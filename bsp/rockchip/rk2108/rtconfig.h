#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define RKMCU_RK2108
#define PM_HAS_CUSTOM_CONFIG

/* RT-Thread Kernel */

#define RT_USING_CORE_RTTHREAD
/* RT_USING_CORE_FREERTOS is not set */
#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
/* RT_THREAD_PRIORITY_8 is not set */
#define RT_THREAD_PRIORITY_32
/* RT_THREAD_PRIORITY_256 is not set */
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 512
/* RT_USING_TIMER_SOFT is not set */
#define RT_DEBUG
/* RT_DEBUG_COLOR is not set */
/* RT_DEBUG_INIT_CONFIG is not set */
#define RT_DEBUG_USING_IO
/* RT_DEBUG_USING_GPIO is not set */
/* RT_DEBUG_THREAD_CONFIG is not set */
/* RT_DEBUG_SCHEDULER_CONFIG is not set */
/* RT_DEBUG_IPC_CONFIG is not set */
/* RT_DEBUG_TIMER_CONFIG is not set */
/* RT_DEBUG_IRQ_CONFIG is not set */
/* RT_DEBUG_MEM_CONFIG is not set */
/* RT_DEBUG_SLAB_CONFIG is not set */
/* RT_DEBUG_MEMHEAP_CONFIG is not set */
/* RT_DEBUG_MODULE_CONFIG is not set */

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
/* RT_USING_SIGNALS is not set */

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
/* RT_USING_NOHEAP is not set */
#define RT_USING_SMALL_MEM
/* RT_USING_SLAB is not set */
/* RT_USING_MEMHEAP_AS_HEAP is not set */
/* RT_USING_MEMTRACE is not set */
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
/* RT_USING_DEVICE_OPS is not set */
/* RT_USING_INTERRUPT_INFO is not set */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x30103
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M4
/* ARCH_CPU_STACK_GROWS_UPWARD is not set */

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
/* RT_USING_USER_MAIN is not set */

/* C++ features */

/* RT_USING_CPLUSPLUS is not set */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
/* FINSH_ECHO_DISABLE_DEFAULT is not set */
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
/* FINSH_USING_AUTH is not set */
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 3
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 16
#define RT_USING_DFS_MNTTABLE
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
/* RT_DFS_ELM_USE_LFN_0 is not set */
/* RT_DFS_ELM_USE_LFN_1 is not set */
/* RT_DFS_ELM_USE_LFN_2 is not set */
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
/* RT_DFS_ELM_LFN_UNICODE_1 is not set */
/* RT_DFS_ELM_LFN_UNICODE_2 is not set */
/* RT_DFS_ELM_LFN_UNICODE_3 is not set */
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
/* RT_DFS_ELM_USE_ERASE is not set */
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS
/* RT_USING_DFS_ROMFS is not set */
/* RT_USING_DFS_RAMFS is not set */
/* RT_USING_DFS_UFFS is not set */
/* RT_USING_DFS_JFFS2 is not set */
/* RT_USING_DFS_NFS is not set */
/* RT_USING_LITTLEFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 4096
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
/* RT_USING_CPUTIME is not set */
#define RT_USING_I2C
/* RT_USING_I2C_BITOPS is not set */
#define RT_USING_PIN
/* RT_USING_ADC is not set */
#define RT_USING_PWM
#define RT_USING_MTD_NOR
/* RT_USING_MTD_NAND is not set */
/* RT_USING_MTD is not set */
#define RT_USING_PM
/* RT_USING_PM_FG is not set */
/* RT_USING_PM_CHARGER is not set */
/* RT_USING_RTC is not set */
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 512
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 1536
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 1
#define RT_SDCARD_MOUNT_POINT "/mnt/sdcard"
/* RT_SDIO_DEBUG is not set */
#define RT_USING_SPI
/* RT_USING_QSPI is not set */
/* RT_USING_SPI_MSD is not set */
/* RT_USING_SFUD is not set */
/* RT_USING_W25QXX is not set */
/* RT_USING_GD is not set */
/* RT_USING_ENC28J60 is not set */
/* RT_USING_SPI_WIFI is not set */
#define RT_USING_WDT
#define RT_USING_AUDIO
/* RT_USING_SENSOR is not set */
/* RT_USING_TOUCH is not set */

/* Using WiFi */

#define RT_USING_WIFI
#define RT_WLAN_DEVICE_STA_NAME "wlan0"
#define RT_WLAN_DEVICE_AP_NAME "wlan1"
#define RT_WLAN_DEFAULT_PROT "lwip"
#define RT_WLAN_SCAN_WAIT_MS 10000
#define RT_WLAN_CONNECT_WAIT_MS 10000
#define RT_WLAN_SSID_MAX_LENGTH 32
#define RT_WLAN_PASSWORD_MAX_LENGTH 32
#define RT_WLAN_SCAN_SORT
#define RT_WLAN_CFG_INFO_MAX 3
#define RT_WLAN_WORKQUEUE_THREAD_NAME "wlan_job"
#define RT_WLAN_WORKQUEUE_THREAD_SIZE 2048
#define RT_WLAN_WORKQUEUE_THREAD_PRIO 22
#define RT_WLAN_DEV_EVENT_NUM 2
#define RT_WLAN_PROT_LWIP_PBUF_FORCE
/* RT_WLAN_DEBUG is not set */

/* Using USB */

/* RT_USING_USB_HOST is not set */
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID 0x2207
#define USB_PRODUCT_ID 0x0007
#define RT_USB_DEVICE_COMPOSITE
/* RT_USB_DEVICE_CDC is not set */
#define RT_USB_DEVICE_NONE
#define RT_USB_DEVICE_MSTORAGE
/* RT_USB_DEVICE_HID is not set */
/* RT_USB_DEVICE_RNDIS is not set */
/* RT_USB_DEVICE_ECM is not set */
/* RT_USB_DEVICE_WINUSB is not set */
/* RT_USB_DEVICE_UAC1 is not set */
#define RT_USB_MSTORAGE_DISK_NAME "sd0"
#define RT_USB_MSTORAGE_BUFLEN 4096

/* POSIX layer and C standard library */

#define RT_USING_LIBC
/* RT_USING_PTHREADS is not set */
#define RT_USING_POSIX
/* RT_USING_POSIX_MMAP is not set */
#define RT_USING_POSIX_TERMIOS
/* RT_USING_POSIX_AIO is not set */
/* RT_USING_MODULE is not set */

/* Network */

/* Socket abstraction layer */

/* RT_USING_SAL is not set */

/* light weight TCP/IP stack */

#define RT_USING_LWIP
#define RT_USING_LWIP141
/* RT_USING_LWIP202 is not set */
/* RT_USING_LWIP210 is not set */
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
/* RT_LWIP_SNMP is not set */
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.1.30"
#define RT_LWIP_GWADDR "192.168.1.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
/* RT_LWIP_PPP is not set */
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 10
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 1024
#define LWIP_NO_RX_THREAD
#define LWIP_NO_TX_THREAD
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
/* RT_LWIP_REASSEMBLY_FRAG is not set */
#define LWIP_NETIF_STATUS_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
/* RT_LWIP_NETIF_LOOPBACK is not set */
#define LWIP_NETIF_LOOPBACK 0
/* RT_LWIP_STATS is not set */
/* RT_LWIP_DEBUG is not set */

/* Modbus master and slave stack */

/* RT_USING_MODBUS is not set */

/* AT commands */

/* RT_USING_AT is not set */
/* LWIP_USING_DHCPD is not set */

/* System */

/* RT_USING_LITTLEVGL2RTT is not set */

/* VBUS(Virtual Software BUS) */

/* RT_USING_VBUS is not set */

/* Utilities */

#define RT_USING_CMBACKTRACE
/* PKG_CMBACKTRACE_PLATFORM_M0_M0PLUS is not set */
/* PKG_CMBACKTRACE_PLATFORM_M3 is not set */
#define PKG_CMBACKTRACE_PLATFORM_M4
/* PKG_CMBACKTRACE_PLATFORM_M7 is not set */
/* PKG_CMBACKTRACE_PLATFORM_NOT_SELECTED is not set */
#define PKG_CMBACKTRACE_DUMP_STACK
#define PKG_CMBACKTRACE_PRINT_ENGLISH
/* PKG_CMBACKTRACE_PRINT_CHINESE is not set */
/* PKG_CMBACKTRACE_FAULT_DUMP_TO_FLASH is not set */
/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */
/* RT_USING_RZM is not set */
#define RT_USING_ULOG
/* ULOG_OUTPUT_LVL_A is not set */
/* ULOG_OUTPUT_LVL_E is not set */
/* ULOG_OUTPUT_LVL_W is not set */
/* ULOG_OUTPUT_LVL_I is not set */
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
/* ULOG_USING_ISR_LOG is not set */
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128
/* ULOG_USING_ASYNC_OUTPUT is not set */

/* log format */

/* ULOG_OUTPUT_FLOAT is not set */
#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
/* ULOG_TIME_USING_TIMESTAMP is not set */
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
/* ULOG_OUTPUT_THREAD_NAME is not set */
#define ULOG_BACKEND_USING_CONSOLE
/* ULOG_USING_FILTER is not set */
/* ULOG_USING_SYSLOG is not set */
/* RT_USING_UTEST is not set */

/* ARM CMSIS */

/* RT_USING_CMSIS_OS is not set */
/* RT_USING_RTT_CMSIS is not set */

/* RT-Thread Benchmarks */

/* RT_USING_BENCHMARK is not set */
/* RT_USING_NET_HTTP is not set */
/* RT_USING_OTA is not set */

/* RT-Thread third party package */

/* PKG_CMSIS_RTOS2 is not set */
/* RT_USING_WLAN_WICED is not set */
#define RT_USING_NETUTILS
#define RT_NETUTILS_PING
/* RT_NETUTILS_TFTP is not set */
/* RT_NETUTILS_IPERF is not set */
/* RT_NETUTILS_NETIO is not set */
/* RT_NETUTILS_NTP is not set */
/* RT_NETUTILS_TELNET is not set */
/* RT_NETUTILS_TCPDUMP is not set */

/* Bluetooth */

/* BT_ENABLED is not set */

/* examples bluetooth */

/* Bluetooth examlpes */

/* Example 'BT API TEST' Config */

/* BT_API_TEST is not set */

/* Example 'BT DISCOVERY' Config */

/* BT_DISCOVERY_EXAMPLES is not set */

/* Example 'A2DP SINK' Config */

/* A2DP_SINK_EXAMPLE is not set */

/* Example 'A2DP SOURCE' Config  */

/* A2DP_SOURCE_EXAMPLE is not set */

/* Example 'HFP CLIENT' Config */

/* HFP_CLIENT_EXAMPLE is not set */
#define RT_USING_CYPRESS
#define RT_CYPRESS_WIFI
#define RT_CYPRESS_WIFI_SDIO
/* RT_CYPRESS_WIFI_SPI is not set */
#define RT_CYPRESS_BT
/* BT_DEV_MODE is not set */
/* BT_BUILD_VERBOSE is not set */
#define BT_AUDIO_DEVICE "audpwmp"
#define BT_MIC_DEVICE "pdmc"
#define BT_UART_NAME "/dev/uart2"
/* BT_FACTORY_TEST is not set */
#define CY_BT_DEMO_APP
/* DEMO_BLE_SENSOR is not set */
/* DEMO_BLE_CLIENT is not set */
/* DEMO_BLE_CLIENT_SERVER is not set */
#define DEMO_HF_A2DP
/* DEMO_SPP_BLE is not set */
/* DEMO_AVK is not set */
/* RT_USING_AUDIO_SERVER is not set */
/* RT_USING_PKGS is not set */

/* RT-Thread board config */

#define RT_BOARD_NAME "audio_demo_rk2108_v11"
#define RK_SRAM_END 0x20100000
/* M4_JTAG_ENABLE is not set */
/* DSP_JTAG_ENABLE is not set */

/* RT-Thread rockchip common drivers */

/* HAL_DBG_ON is not set */
/* RT_USING_SND_GLB_RST is not set */
/* RT_USING_RESET is not set */
/* RT_USING_CRU_DUMP is not set */
/* RT_USING_REGULATOR_DUMP is not set */
/* RT_USING_PMIC is not set */
#define RT_USING_CACHE
#define RT_USING_UNCACHE_HEAP
#define RT_UNCACHE_HEAP_ORDER 0x10
#define RT_USING_LARGE_HEAP
#define RT_LARGE_MALLOC_THRRESH 512
#define RT_LARGE_HEAP_SIZE 524288
/* RT_USING_DTCM_HEAP is not set */
/* RT_USING_PSRAM_HEAP is not set */

/* Enable Fault Dump Hook */

/* RT_USNING_FAULT_DUMP_HOOK is not set */
#define RT_USING_SNOR
#define RT_SNOR_SPEED 80000000
/* RT_SNOR_DUAL_IO is not set */
#define RT_USING_SNOR_FSPI_HOST
/* RT_USING_SNOR_SFC_HOST is not set */
/* RT_USING_SNOR_SPI_HOST is not set */

/* RT-Thread rockchip pm drivers */

/* RT_USING_PM_DVFS is not set */
/* RT_USING_PM_REQ_PWR is not set */
/* RT_USING_PM_REQ_CLK is not set */
/* RT_USING_DVFS_DBG is not set */
/* RT_USING_PM_RUNTIME is not set */
/* RT_USING_PM_TIMER is not set */

/* RT-Thread rockchip vicap driver */

/* RT_USING_VICAP is not set */
/* RT_USING_WDT_DUMP is not set */
/* RT_USING_QPIPSRAM is not set */

/* RT-Thread rockchip coredump driver */

/* RT_USING_COREDUMP is not set */
/* RT_USING_SPINAND is not set */

/* RT-Thread rockchip rk2108 drivers */

/* Enable Audio */

#define RT_USING_CODEC
#define RT_USING_ACDCDIG
#define RT_USING_DRIVER_AUDIO_PCM_PLUGIN
#define RT_USING_DRIVER_AUDIO_PCM_PLUGIN_HW
#define RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL

/* PCM Softvol Configuration */

#define SOFTVOL_MIN_DB "-50.0"
#define SOFTVOL_MAX_DB "49.0"
#define SOFTVOL_FORMAT "S16_LE"
#define SOFTVOL_CHANNELS 2
#define SOFTVOL_RESOLUTION 100

/* Audio Cards */

#define RT_USING_AUDIO_CARD
#define RT_USING_AUDIO_CARD_ACDCDIG
/* RT_USING_ACDCDIG_PDM0 is not set */
#define RT_USING_ACDCDIG_PDM0_SDI2
#define RT_USING_AUDIO_CARD_AUDIOPWM
/* RT_USING_AUDIO_CARD_I2S_MIC is not set */
#define RT_USING_AUDIO_CARD_PDM_MIC
/* RT_USING_AUDIO_CARD_PCM_BT is not set */
/* RT_USING_AUDIO_CARD_ES7243 is not set */
/* RT_USING_AUDIO_CARD_ES8156 is not set */
/* RT_USING_AUDIO_CARD_ES8311 is not set */
/* RT_USING_AUDIO_CARD_ES8388 is not set */
/* RT_USING_AUDIO_CARD_ADC_ES8311_ECHO is not set */
/* RT_USING_AUDIO_CARD_PDM_ES8388_ECHO is not set */

/* Audio Interfaces */

#define RT_USING_PCM
#define RT_USING_AUDIOPWM
#define RT_USING_PDM
#define RT_USING_PDM0
/* RT_USING_VAD is not set */
#define RT_USING_CRU
#define RT_USING_DMA
#define RT_USING_DMA_PL330
#define RT_USING_PMU
/* RT_USING_DISPLAY is not set */
#define RT_USING_KEYCTRL
#define RT_USING_PWM0

/* Enable DSP */

#define RT_USING_DSP
/* RT_USING_DSPFW_LOADER is not set */
/* RT_USING_DSP_TRACE is not set */
#define RT_DSP_UART_PORT -1

/* Enable UART */

#define RT_USING_UART
#define RT_USING_UART0
/* RT_USING_UART1 is not set */
#define RT_USING_UART2

/* Enable I2C */

#define RT_USING_I2C0
/* RT_USING_I2C1 is not set */
#define RT_USING_I2C2

/* Enable SPI */

/* RT_USING_SPI2APB is not set */
/* RT_USING_SPI1 is not set */
/* RT_USING_SPI2 is not set */

/* Enable SDIO/SDMMC/eMMC */

#define RT_USING_SDIO0
#define RT_USING_SYSTICK

/* RT-Thread application */

#define RT_USING_APP_NULL
/* RT_USING_ANIMATION1_DEMO is not set */
/* RT_USING_ANIMATION2_DEMO is not set */
/* RT_USING_CLOCK_DEMO is not set */
/* RT_USING_CLOCK_DARK is not set */
/* RT_USING_CLOCK_WHISKEY is not set */
/* RT_USING_RECORDING_PEN_APP is not set */
/* RT_USING_IOT_DISPLAY is not set */
/* RT_USING_PCBA_TEST is not set */
/* RT_USING_RK_IOT_APP is not set */
/* ENABLE_DUAL_TRACKING is not set */

/* RT-Thread bsp test case */

/* RT_USING_PRIVATE_TEST is not set */

/* RT-Thread Common Test case */

/* RT_USING_TC is not set */
/* RT_USING_FILE_TEST is not set */
#define RT_USING_COMMON_TEST
/* RT_USING_AUTO_TEST is not set */
/* RT_USING_COMMON_TEST_SUSPEND is not set */
#define RT_USING_COMMON_TEST_AUDIO
/* RT_USING_COMMON_TEST_CODEC_ES8388 is not set */
/* RT_USING_COMMON_TEST_CPU_USAGE is not set */
/* RT_USING_COMMON_TEST_DCACHE is not set */
/* RT_USING_COMMON_TEST_ICACHE is not set */
/* RT_USING_COMMON_TEST_DSP is not set */
/* RT_USING_DSP_OPUS_CODEC_TEST is not set */
/* RT_USING_COMMON_TEST_GPIO is not set */
/* RT_USING_COMMON_TEST_I2C is not set */
/* RT_USING_COMMON_TEST_KEYCTRL is not set */
/* RT_USING_COMMON_TEST_MBOX is not set */
/* RT_USING_COMMON_MEMTESTER is not set */
/* RT_USING_COMMON_TEST_MPU is not set */
/* RT_USING_COMMON_TEST_PM is not set */
/* RT_USING_COMMON_TEST_SNOR is not set */
/* RT_USING_COMMON_TEST_SPI is not set */
/* RT_USING_COMMON_STRESS_TEST is not set */
/* RT_USING_COMMON_TEST_UART is not set */
/* RT_USING_COMMON_TEST_WDT is not set */
/* RT_USING_COMMON_TEST_PERFORMANCE is not set */
/* RT_USING_COMMON_TEST_FWANALYSIS is not set */
/* RT_USING_COMMON_TEST_TIMER is not set */
#define RT_USING_FWANALYSIS

#endif
