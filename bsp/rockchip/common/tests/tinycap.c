/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    tinycap.c
  * @author  sugar zhang
  * @version V0.1
  * @date    6-8-2019
  * @brief   tinycap for rksoc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_AUDIO

#include "rk_audio.h"
#include "drv_heap.h"
#include "blsx.h"

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
#include "pcm_plugin_provider.h"

static uint16_t vol_l = 100, vol_r = 100;
#endif

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

static record_type_e record_state = RECORD_MAX;

struct wav_header
{
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

struct capture_config
{
    FILE *file;
    struct rt_device *card;
    uint32_t rate;
    uint32_t channels;
    uint32_t bits;
    uint32_t period_size;
    uint32_t period_count;
    uint32_t total_frames;
};

#if 0
static uint32_t capture_sample(FILE *file, struct rt_device *card,
                               uint32_t channels, uint32_t rate,
                               uint32_t bits, uint32_t period_size,
                               uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size;
    uint32_t frames_read = 0, count = 0;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return 0;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed£¬size is %d!\n", size);
        return 0;
    }

    param.channels = channels;
    param.sampleRate = rate;
    param.sampleBits = bits;

    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

    softvol.vol_l = vol_l;
    softvol.vol_r = vol_r;
    rt_kprintf("Set softvol: %d, %d\n", vol_l, vol_r);
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_GET_SOFTVOL, &softvol2);
    RT_ASSERT(ret == RT_EOK);
    rt_kprintf("Get softvol2: %d, %d\n", softvol2.vol_l, softvol2.vol_r);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);

    size = abuf.period_size * channels * (bits >> 3);
    buffer = rt_malloc(size);
    if (!buffer)
    {
        rt_kprintf("Unable to allocate %ld bytes\n", size);
        rt_device_close(card);
        return 0;
    }

    rt_kprintf("Capturing sample: %lu ch, %lu hz, %lu bits\n", channels, rate, bits);

    count = total_frames / period_size;
    while (count && rt_device_read(card, 0, buffer, abuf.period_size))
    {
        if (fwrite(buffer, 1, size, file) != size)
        {
            rt_kprintf("Error capturing sample\n");
            break;
        }
        frames_read += abuf.period_size;
        count--;
    }

    free(buffer);
    rt_free_uncache(abuf.buf);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_device_close(card);

    return frames_read;
}
#else
static uint32_t capture_sample(FILE *file, struct rt_device *card,
                               uint32_t channels, uint32_t rate,
                               uint32_t bits, uint32_t period_size,
                               uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size;
    uint32_t frames_read = 0;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return 0;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed£¬size is %d!\n", size);
        return 0;
    }

    param.channels = channels;
    param.sampleRate = rate;
    param.sampleBits = bits;

    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

    softvol.vol_l = vol_l;
    softvol.vol_r = vol_r;
    rt_kprintf("Set softvol: %d, %d\n", vol_l, vol_r);
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_GET_SOFTVOL, &softvol2);
    RT_ASSERT(ret == RT_EOK);
    rt_kprintf("Get softvol2: %d, %d\n", softvol2.vol_l, softvol2.vol_r);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);

    size = abuf.period_size * channels * (bits >> 3);
    buffer = rt_malloc(size);
    if (!buffer)
    {
        rt_kprintf("Unable to allocate %ld bytes\n", size);
        rt_device_close(card);
        return 0;
    }

    rt_kprintf("Capturing sample: %lu ch, %lu hz, %lu bits\n", channels, rate, bits);

    while (1)
    {		
    
		if(record_state == RECORD_STOP)//ÖÕÖ¹
		{
			record_state = RECORD_STOP;
			rt_kprintf("capture stop\n");
			break;
		}
		
		rt_device_read(card, 0, buffer, abuf.period_size);

		if(record_state != RECORD_PAUSE)
		{
			if (fwrite(buffer, 1, size, file) != size)
	        {
	            rt_kprintf("Error capturing sample\n");
	            break;
	        }
		}
        		
        frames_read += abuf.period_size;
		
    }

    free(buffer);
    rt_free_uncache(abuf.buf);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_device_close(card);

    return frames_read;
}
#endif

void capture_state_set(record_type_e data)
{
	record_state = data;
}

void capture_state_set1()
{
	record_state = RECORD_STOP;
}


static void do_tinycap(void *arg)
{
    struct wav_header header;
    struct capture_config *config = arg;
    FILE *file = config->file;
    uint32_t frames;

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = config->channels;
    header.sample_rate = config->rate;

    header.bits_per_sample = config->bits;
    header.byte_rate = (header.bits_per_sample / 8) * config->channels * config->rate;
    header.block_align = config->channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    /* leave enough room for header */
    fseek(file, sizeof(struct wav_header), SEEK_SET);
	record_state = RECORD_START;
    frames = capture_sample(file, config->card, config->channels,
                            config->rate, config->bits,
                            config->period_size, config->period_count, config->total_frames);

    printf("Captured %ld frames\n", frames);

    /* write header now all information is known */
    header.data_sz = frames * header.block_align;
    header.riff_sz = header.data_sz + sizeof(header) - 8;
	record_state = RECORD_STOP;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct wav_header), 1, file);

    fclose(file);
    rt_free(config);
}

static int tinycap(int argc, char **argv)
{
    rt_thread_t tid = RT_NULL;
    struct capture_config *config;
    char card[RT_NAME_MAX] = {0};
    uint32_t timeout = 10; /* 10 seconds default */
    FILE *file = RT_NULL;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s file.wav [-D card] [-c channels] "
                   "[-r rate] [-b bits] [-p period_size] [-n n_periods] [-t seconds]\n", argv[0]);
        return 1;
    }

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    file = fopen(argv[1], "wb");
    if (!file)
    {
        rt_kprintf("Unable to create file '%s'\n", argv[1]);
        goto err;
    }

    config->file = file;
    config->rate = 16000;
    config->bits = 16;
    config->channels = 2;
    config->period_size = 1024;
    config->period_count = 4;

    /* parse command line arguments */
    argv += 2;
    argc -= 2;
    while (argc)
    {
        if (strcmp(*argv, "-t") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                timeout = atoi(*argv);
        }
        else if (strcmp(*argv, "-c") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->channels = atoi(*argv);
        }
        else if (strcmp(*argv, "-r") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->rate = atoi(*argv);
        }
        else if (strcmp(*argv, "-b") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->bits = atoi(*argv);
        }
        else if (strcmp(*argv, "-D") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                rt_strncpy(card, *argv, RT_NAME_MAX);
        }
        else if (strcmp(*argv, "-p") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_size = atoi(*argv);
        }
        else if (strcmp(*argv, "-n") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_count = atoi(*argv);
        }
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
        else if (strcmp(*argv, "-L") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_l = atoi(*argv);
        }
        else if (strcmp(*argv, "-R") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_r = atoi(*argv);
        }
#endif
        argv++;
        argc--;
    }

    config->card = rt_device_find(card);
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", card);
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("tinycap", do_tinycap, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

int tinycap_audio()
{
	if(record_state == RECORD_START)
	{
		rt_kprintf("recording now");
		record_state = RECORD_STOP;
		return 0;	
	}
	
    rt_thread_t tid = RT_NULL;
    struct capture_config *config;
    uint32_t timeout = 10; /* 10 seconds default */
    FILE *file = RT_NULL;

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    file = fopen("/mnt/sdcard/record.wav", "w+");
    if (!file)
    {
        rt_kprintf("Unable to create file '/mnt/sdcard/record.wav'\n");
        goto err;
    }

    config->file = file;
    config->rate = 16000;
    config->bits = 16;
    config->channels = 6;
    config->period_size = 1024;
    config->period_count = 4;

    config->card = rt_device_find("adcc");
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", "adcc");
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("tinycap", do_tinycap, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

//----------------------------------------------------
struct rt_device *card = NULL;

int test()
{
	struct AUDIO_GAIN_INFO info;
	struct AUDIO_DB_CONFIG db_config;
	int ret ;
	
	if(card == NULL)
	{
		card = rt_device_find("adcc");
		if (!card)
	    {
	        rt_kprintf("Can't find sound device: %s\n", "adcc");
	        return 1;
	    }
	}

	rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);

	ret = rt_device_control(card, RK_AUDIO_CTL_GET_GAIN_INFO, &info);
    if (ret != RT_EOK)
    {
        rt_kprintf("fail to get gain info\n");
        return -RT_ERROR;
    }
	rt_kprintf("maxdB %d, mindB %d, step %d\n", info.maxdB, info.mindB, info.step);

    ret = rt_device_control(card, RK_AUDIO_CTL_GET_GAIN, &db_config);
    if (ret != RT_EOK)
    {
        rt_kprintf("fail to get gain\n");
        return -RT_ERROR;
    }
	rt_kprintf("db %d, ch %d\n", db_config.dB, db_config.ch);
    //db_config.dB = info.mindB + volume * (info.maxdB - info.mindB) / 100;
    //db_config.dB = db_config.dB - db_config.dB % info.step;
    //rt_kprintf("db_config.dB = %d\n", db_config.dB);

	rt_device_close(card);
	return 0;
}

void gb_set()
{
	struct AUDIO_DB_CONFIG db_config;
	
	rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);
	rt_device_control(card, RK_AUDIO_CTL_GET_GAIN, &db_config);

	db_config.dB = 37500;
	rt_device_control(card, RK_AUDIO_CTL_SET_GAIN, &db_config);

	rt_device_close(card);
}


#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(tinycap, capture wav file);
MSH_CMD_EXPORT(tinycap_audio, capture wav file);
MSH_CMD_EXPORT(capture_state_set1, capture wav file);
MSH_CMD_EXPORT(test, test);
MSH_CMD_EXPORT(gb_set, gb_set);

#endif

#endif
