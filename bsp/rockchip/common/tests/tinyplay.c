/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    tinyplay.c
  * @author  sugar zhang
  * @version V0.1
  * @date    6-8-2019
  * @brief   tinyplay for rksoc
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

static uint16_t vol_l = 20, vol_r = 20;
static struct rt_device *s_card = NULL;
#else

#endif

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header
{
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header
{
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt
{
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

struct playback_config
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

static player_type_e s_player_state = PLAYER_MAX;

#if 1
static void play_sample(FILE *file, struct rt_device *card, uint32_t channels,
                        uint32_t rate, uint32_t bits, uint32_t period_size,
                        uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;

    rt_err_t ret;
    char *buffer;
    uint32_t size;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed!\n");
        return;
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
        rt_kprintf("Unable to allocate %d bytes\n", size);
        rt_device_close(card);
        return;
    }

    rt_kprintf("Playing sample: %lu ch, %lu hz, %lu bit\n", channels, rate, bits);

    while (1)
    {
    	if(s_player_state == PLAYER_PAUSE)
    	{   		
    		rt_thread_mdelay(50);
    		continue;
    	}
		else if(s_player_state == PLAYER_STOP)
		{
			s_player_state = PLAYER_STOP;
			break;
		}
		
    	if(fread(buffer, 1, size, file) <= 0)
    	{
    		break;
    	}		
		
		if (1) //让人的声音清晰，不然人声非常小，背景音乐声音盖住了人声。
        {
            short *temp = (short *)buffer;
            float data_l;
            float data_r;
            float data_mix;
            for (int i = 0; i < abuf.period_size * channels; i += 2)
            {
                data_l = (float)(temp[i]);
                data_r = (float)(temp[i + 1]);
                if (data_l > 0.0 && data_r > 0.0)
                    data_mix = data_l + data_r - data_l * data_r / INT16_MAX;
                else if (data_l < 0.0 && data_r < 0.0)
                    data_mix = data_l + data_r - data_l * data_r / INT16_MIN;
                else
                    data_mix = data_l + data_r;
                temp[i] = (short)data_mix;
                temp[i + 1] = (short)(-data_mix);
            }
        }
			
        if (!rt_device_write(card, 0, buffer, abuf.period_size))
        {
            rt_kprintf("Error playing sample\n");
            break;
        }

    }

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif
    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_free(buffer);
    rt_free_uncache(abuf.buf);
    rt_device_close(card);
	s_card = NULL;
}
#else
static void play_sample(FILE *file, struct rt_device *card, uint32_t channels,
                        uint32_t rate, uint32_t bits, uint32_t period_size,
                        uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size, count;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed!\n");
        return;
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
        rt_kprintf("Unable to allocate %d bytes\n", size);
        rt_device_close(card);
        return;
    }

    rt_kprintf("Playing sample: %lu ch, %lu hz, %lu bit\n", channels, rate, bits);

    count = total_frames / period_size;
    while (count && (fread(buffer, 1, size, file) > 0))
    {
        if (!rt_device_write(card, 0, buffer, abuf.period_size))
        {
            rt_kprintf("Error playing sample\n");
            break;
        }
        count--;
    }

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif
    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_free(buffer);
    rt_free_uncache(abuf.buf);
    rt_device_close(card);
}

#endif

static void do_tinyplay(void *arg)
{
    struct playback_config *config = arg;
    FILE *file = config->file;

	s_player_state = PLAYER_START;
    play_sample(file, config->card, config->channels, config->rate,
                config->bits, config->period_size, config->period_count,
                config->total_frames);

	s_player_state = PLAYER_STOP;
    fclose(file);
    rt_free(config);

    rt_kprintf("Done\n");
}

static int tinyplay(int argc, char **argv)
{
    struct playback_config *config;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    rt_thread_t tid = RT_NULL;
    uint32_t timeout = UINT32_MAX;
    int more_chunks = 1;
    char card[RT_NAME_MAX] = {0};
    char *filename;
    FILE *file;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s file.wav [-D card] [-p period_size]"
                   " [-n n_periods] [-t seconds]\n", argv[0]);
        return 1;
    }

    filename = argv[1];
    file = fopen(filename, "rb");
    if (!file)
    {
        rt_kprintf("Unable to open file '%s'\n", filename);
        return 1;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
            (riff_wave_header.wave_id != ID_WAVE))
    {
        rt_kprintf("Error: '%s' is not a riff/wave file\n", filename);
        fclose(file);
        return 1;
    }

    do
    {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id)
        {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    }
    while (more_chunks);

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->file = file;
    config->rate = chunk_fmt.sample_rate;
    config->bits = chunk_fmt.bits_per_sample;
    config->channels = chunk_fmt.num_channels;
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
                timeout = atoi(*argv) * chunk_fmt.sample_rate;
        }
        if (strcmp(*argv, "-p") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_size = atoi(*argv);
        }
        if (strcmp(*argv, "-n") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_count = atoi(*argv);
        }
        if (strcmp(*argv, "-D") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                rt_strncpy(card, *argv, RT_NAME_MAX);
        }
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
        if (strcmp(*argv, "-L") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_l = atoi(*argv);
        }
        if (strcmp(*argv, "-R") == 0)
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

    config->total_frames = timeout;

    tid = rt_thread_create("tinyplay", do_tinyplay, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;

err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

//static char name[3][30] = {"/mnt/sdcard/test.wav", "/mnt/sdcard/test2.wav", "/mnt/sdcard/test3.wav"};
static char name[4][30] = {"/test.wav","/mnt/sdcard/123.wav", "/mnt/sdcard/123.wav", "/mnt/sdcard/123.wav"};

static int id = 0;

int tinyplay_audio()
{
	if(s_player_state == PLAYER_START)
	{
		rt_kprintf("playing now");
		s_player_state = PLAYER_STOP;
		return 0;
	}

    struct playback_config *config;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    rt_thread_t tid = RT_NULL;
    uint32_t timeout = UINT32_MAX;
    int more_chunks = 1;
    char card[RT_NAME_MAX] = {0};
    FILE *file;

    file = fopen(name[id], "rb");
    if (!file)
    {
        rt_kprintf("Unable to open file %s\n", name[id]);
        return 1;
    }
	/*id++;
	if(id > 2)
	{
		id = 0;
	}*/

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
            (riff_wave_header.wave_id != ID_WAVE))
    {
        rt_kprintf("Error: /sdcard/123.wav is not a riff/wave file\n");
        fclose(file);
        return 1;
    }

    do
    {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id)
        {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    }
    while (more_chunks);

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->file = file;
    config->rate = chunk_fmt.sample_rate;
    config->bits = chunk_fmt.bits_per_sample;
    config->channels = chunk_fmt.num_channels;
    config->period_size = 1024;
    config->period_count = 4;

    /* parse command line arguments */
    config->card = rt_device_find("audpwmp");
	
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", card);
        goto err;
    }
	s_card = config->card;

    config->total_frames = timeout;

    tid = rt_thread_create("tinyplay", do_tinyplay, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;

err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

void player_state_set(player_type_e data)
{
	s_player_state = data;
}

void player_vol_add()
{
	snd_softvol_t softvol = {0};
	
	if(vol_r < 100)
	{
		vol_r += 10;
		vol_l += 10;
		if(vol_r > 100)
			vol_r = 100;
		if(vol_l > 100)
			vol_l = 100;
	}
	softvol.vol_l = vol_l;
	softvol.vol_r = vol_r;
	if(s_card != NULL)
		rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
	
	rt_kprintf("player_vol_add\n");
}

void player_vol_sub()
{
	snd_softvol_t softvol = {0};
	if(vol_r > 0)
	{
		vol_r -= 10;
		vol_l -= 10;
		if(vol_r < 0)
			vol_r = 0;
		if(vol_l < 0)
			vol_l = 0;
	}
	softvol.vol_l = vol_l;
	softvol.vol_r = vol_r;

	if(s_card != NULL)
		rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
	
	rt_kprintf("player_vol_sub\n");
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(tinyplay, play wav file);
MSH_CMD_EXPORT(tinyplay_audio, play wav file);

#endif

#endif
