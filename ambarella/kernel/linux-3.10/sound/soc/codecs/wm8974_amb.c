/*
 * wm8940_amb.c  --  WM8940 ALSA Soc Audio driver
 *
 * Copyright 2011 Ambarella Ltd.
 *
 * Author: Wu Dongge <dgwu@ambarella.com>
 *
 * Based on wm8974.c
 * Copyright 2006-2009 Wolfson Microelectronics PLC.
 *
 * Author: Liam Girdwood <linux@wolfsonmicro.com>
 *
 * History:
 *	2012/10/25 - [Wu Dongge] Created file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "wm8974_amb.h"

static const u16 wm8974_reg[WM8974_CACHEREGNUM] = {
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0050, 0x0000, 0x0140, 0x0000,
	0x0000, 0x0000, 0x0000, 0x00ff,
	0x0000, 0x0000, 0x0100, 0x00ff,
	0x0000, 0x0000, 0x012c, 0x002c,
	0x002c, 0x002c, 0x002c, 0x0000,
	0x0032, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0038, 0x000b, 0x0032, 0x0000,
	0x0008, 0x000c, 0x0093, 0x00e9,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0003, 0x0010, 0x0000, 0x0000,
	0x0000, 0x0002, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0039, 0x0000,
	0x0000,
};

#define WM8974_POWER1_BIASEN  0x08
#define WM8974_POWER1_BUFIOEN 0x04

struct wm8974_priv {
	unsigned int sysclk;
	enum snd_soc_control_type control_type;
	void *control_data;
};

#define wm8974_reset(c)	snd_soc_write(c, WM8974_RESET, 0)

static const char *wm8974_companding[] = {"Off", "NC", "u-law", "A-law" };
static const char *wm8974_deemp[] = {"None", "32kHz", "44.1kHz", "48kHz" };
static const char *wm8974_eqmode[] = {"Capture", "Playback" };
static const char *wm8974_bw[] = {"Narrow", "Wide" };
static const char *wm8974_eq1[] = {"80Hz", "105Hz", "135Hz", "175Hz" };
static const char *wm8974_eq2[] = {"230Hz", "300Hz", "385Hz", "500Hz" };
static const char *wm8974_eq3[] = {"650Hz", "850Hz", "1.1kHz", "1.4kHz" };
static const char *wm8974_eq4[] = {"1.8kHz", "2.4kHz", "3.2kHz", "4.1kHz" };
static const char *wm8974_eq5[] = {"5.3kHz", "6.9kHz", "9kHz", "11.7kHz" };
static const char *wm8974_alc[] = {"ALC", "Limiter" };

static const struct soc_enum wm8974_enum[] = {
	SOC_ENUM_SINGLE(WM8974_COMP, 1, 4, wm8974_companding), /* adc */
	SOC_ENUM_SINGLE(WM8974_COMP, 3, 4, wm8974_companding), /* dac */
	SOC_ENUM_SINGLE(WM8974_DAC,  4, 4, wm8974_deemp),
	SOC_ENUM_SINGLE(WM8974_EQ1,  8, 2, wm8974_eqmode),

	SOC_ENUM_SINGLE(WM8974_EQ1,  5, 4, wm8974_eq1),
	SOC_ENUM_SINGLE(WM8974_EQ2,  8, 2, wm8974_bw),
	SOC_ENUM_SINGLE(WM8974_EQ2,  5, 4, wm8974_eq2),
	SOC_ENUM_SINGLE(WM8974_EQ3,  8, 2, wm8974_bw),

	SOC_ENUM_SINGLE(WM8974_EQ3,  5, 4, wm8974_eq3),
	SOC_ENUM_SINGLE(WM8974_EQ4,  8, 2, wm8974_bw),
	SOC_ENUM_SINGLE(WM8974_EQ4,  5, 4, wm8974_eq4),
	SOC_ENUM_SINGLE(WM8974_EQ5,  8, 2, wm8974_bw),

	SOC_ENUM_SINGLE(WM8974_EQ5,  5, 4, wm8974_eq5),
	SOC_ENUM_SINGLE(WM8974_ALC3,  8, 2, wm8974_alc),
};

static const char *wm8974_auxmode_text[] = { "Buffer", "Mixer" };

static const struct soc_enum wm8974_auxmode =
	SOC_ENUM_SINGLE(WM8974_INPUT,  3, 2, wm8974_auxmode_text);

static const DECLARE_TLV_DB_SCALE(digital_tlv, -12750, 50, 1);
static const DECLARE_TLV_DB_SCALE(eq_tlv, -1200, 100, 0);
static const DECLARE_TLV_DB_SCALE(inpga_tlv, -1200, 75, 0);
static const DECLARE_TLV_DB_SCALE(spk_tlv, -5700, 100, 0);

static const struct snd_kcontrol_new wm8974_snd_controls[] = {

SOC_SINGLE("Digital Loopback Switch", WM8974_COMP, 0, 1, 0),

SOC_ENUM("DAC Companding", wm8974_enum[1]),
SOC_ENUM("ADC Companding", wm8974_enum[0]),

SOC_ENUM("Playback De-emphasis", wm8974_enum[2]),
SOC_SINGLE("DAC Inversion Switch", WM8974_DAC, 0, 1, 0),

SOC_SINGLE_TLV("PCM Volume", WM8974_DACVOL, 0, 255, 0, digital_tlv),

SOC_SINGLE("High Pass Filter Switch", WM8974_ADC, 8, 1, 0),
SOC_SINGLE("High Pass Cut Off", WM8974_ADC, 4, 7, 0),
SOC_SINGLE("ADC Inversion Switch", WM8974_ADC, 0, 1, 0),

SOC_SINGLE_TLV("Capture Volume", WM8974_ADCVOL,  0, 255, 0, digital_tlv),

SOC_ENUM("Equaliser Function", wm8974_enum[3]),
SOC_ENUM("EQ1 Cut Off", wm8974_enum[4]),
SOC_SINGLE_TLV("EQ1 Volume", WM8974_EQ1,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ2 Bandwith", wm8974_enum[5]),
SOC_ENUM("EQ2 Cut Off", wm8974_enum[6]),
SOC_SINGLE_TLV("EQ2 Volume", WM8974_EQ2,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ3 Bandwith", wm8974_enum[7]),
SOC_ENUM("EQ3 Cut Off", wm8974_enum[8]),
SOC_SINGLE_TLV("EQ3 Volume", WM8974_EQ3,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ4 Bandwith", wm8974_enum[9]),
SOC_ENUM("EQ4 Cut Off", wm8974_enum[10]),
SOC_SINGLE_TLV("EQ4 Volume", WM8974_EQ4,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ5 Bandwith", wm8974_enum[11]),
SOC_ENUM("EQ5 Cut Off", wm8974_enum[12]),
SOC_SINGLE_TLV("EQ5 Volume", WM8974_EQ5,  0, 24, 1, eq_tlv),

SOC_SINGLE("DAC Playback Limiter Switch", WM8974_DACLIM1,  8, 1, 0),
SOC_SINGLE("DAC Playback Limiter Decay", WM8974_DACLIM1,  4, 15, 0),
SOC_SINGLE("DAC Playback Limiter Attack", WM8974_DACLIM1,  0, 15, 0),

SOC_SINGLE("DAC Playback Limiter Threshold", WM8974_DACLIM2,  4, 7, 0),
SOC_SINGLE("DAC Playback Limiter Boost", WM8974_DACLIM2,  0, 15, 0),

SOC_SINGLE("ALC Enable Switch", WM8974_ALC1,  8, 1, 0),
SOC_SINGLE("ALC Capture Max Gain", WM8974_ALC1,  3, 7, 0),
SOC_SINGLE("ALC Capture Min Gain", WM8974_ALC1,  0, 7, 0),

SOC_SINGLE("ALC Capture ZC Switch", WM8974_ALC2,  8, 1, 0),
SOC_SINGLE("ALC Capture Hold", WM8974_ALC2,  4, 15, 0),
SOC_SINGLE("ALC Capture Target", WM8974_ALC2,  0, 15, 0),

SOC_ENUM("ALC Capture Mode", wm8974_enum[13]),
SOC_SINGLE("ALC Capture Decay", WM8974_ALC3,  4, 15, 0),
SOC_SINGLE("ALC Capture Attack", WM8974_ALC3,  0, 15, 0),

SOC_SINGLE("ALC Capture Noise Gate Switch", WM8974_NGATE,  3, 1, 0),
SOC_SINGLE("ALC Capture Noise Gate Threshold", WM8974_NGATE,  0, 7, 0),

SOC_SINGLE("Capture PGA ZC Switch", WM8974_INPPGA,  7, 1, 0),
SOC_SINGLE_TLV("Capture PGA Volume", WM8974_INPPGA,  0, 63, 0, inpga_tlv),

SOC_SINGLE("Speaker Playback ZC Switch", WM8974_SPKVOL,  7, 1, 0),
SOC_SINGLE("Speaker Playback Switch", WM8974_SPKVOL,  6, 1, 1),
SOC_SINGLE_TLV("Speaker Playback Volume", WM8974_SPKVOL,  0, 63, 0, spk_tlv),

SOC_ENUM("Aux Mode", wm8974_auxmode),

SOC_SINGLE("Capture Boost(+20dB)", WM8974_ADCBOOST,  8, 1, 0),
SOC_SINGLE("Mono Playback Switch", WM8974_MONOMIX, 6, 1, 1),

/* DAC / ADC oversampling */
SOC_SINGLE("DAC 128x Oversampling Switch", WM8974_DAC, 3, 1, 0),
SOC_SINGLE("ADC 128x Oversampling Switch", WM8974_ADC, 3, 1, 0),
};

/* Speaker Output Mixer */
static const struct snd_kcontrol_new wm8974_speaker_mixer_controls[] = {
SOC_DAPM_SINGLE("Line Bypass Switch", WM8974_SPKMIX, 1, 1, 0),
SOC_DAPM_SINGLE("Aux Playback Switch", WM8974_SPKMIX, 5, 1, 0),
SOC_DAPM_SINGLE("PCM Playback Switch", WM8974_SPKMIX, 0, 1, 0),
};

/* Mono Output Mixer */
static const struct snd_kcontrol_new wm8974_mono_mixer_controls[] = {
SOC_DAPM_SINGLE("Line Bypass Switch", WM8974_MONOMIX, 1, 1, 0),
SOC_DAPM_SINGLE("Aux Playback Switch", WM8974_MONOMIX, 2, 1, 0),
SOC_DAPM_SINGLE("PCM Playback Switch", WM8974_MONOMIX, 0, 1, 0),
};

static DECLARE_TLV_DB_SCALE(wm8974_boost_vol_tlv, -1500, 300, 1);
/* Boost mixer */
static const struct snd_kcontrol_new wm8974_boost_mixer[] = {
	SOC_DAPM_SINGLE("Mic PGA Switch", WM8974_INPPGA, 6, 1, 1),
	SOC_DAPM_SINGLE_TLV("Aux Volume", WM8974_ADCBOOST,
			    0, 7, 0, wm8974_boost_vol_tlv),
	SOC_DAPM_SINGLE_TLV("Mic Volume", WM8974_ADCBOOST,
			    4, 7, 0, wm8974_boost_vol_tlv),
};

/* Input PGA */
static const struct snd_kcontrol_new wm8974_inpga[] = {
SOC_DAPM_SINGLE("Aux Switch", WM8974_INPUT, 2, 1, 0),
SOC_DAPM_SINGLE("MicN Switch", WM8974_INPUT, 1, 1, 0),
SOC_DAPM_SINGLE("MicP Switch", WM8974_INPUT, 0, 1, 0),
};

static const struct snd_soc_dapm_widget wm8974_dapm_widgets[] = {
SND_SOC_DAPM_MIXER("Speaker Mixer", WM8974_POWER3, 2, 0,
	&wm8974_speaker_mixer_controls[0],
	ARRAY_SIZE(wm8974_speaker_mixer_controls)),
SND_SOC_DAPM_MIXER("Mono Mixer", WM8974_POWER3, 3, 0,
	&wm8974_mono_mixer_controls[0],
	ARRAY_SIZE(wm8974_mono_mixer_controls)),
SND_SOC_DAPM_DAC("DAC", "HiFi Playback", WM8974_POWER3, 0, 0),

SND_SOC_DAPM_PGA("SpkN Out", WM8974_POWER3, 5, 0, NULL, 0),
SND_SOC_DAPM_PGA("SpkP Out", WM8974_POWER3, 6, 0, NULL, 0),
SND_SOC_DAPM_PGA("Mono Out", WM8974_POWER3, 7, 0, NULL, 0),
SND_SOC_DAPM_OUTPUT("MONOOUT"),
SND_SOC_DAPM_OUTPUT("SPKOUTP"),
SND_SOC_DAPM_OUTPUT("SPKOUTN"),

SND_SOC_DAPM_PGA("Aux Input", WM8974_POWER1, 6, 0, NULL, 0),
SND_SOC_DAPM_ADC("ADC", "HiFi Capture", WM8974_POWER2, 0, 0),
SND_SOC_DAPM_MIXER("Input PGA", WM8974_POWER2, 2, 0, &wm8974_inpga[0],
		   ARRAY_SIZE(wm8974_inpga)),
SND_SOC_DAPM_MIXER("Boost Mixer", WM8974_POWER2, 4, 0,
		   &wm8974_boost_mixer[0], ARRAY_SIZE(wm8974_boost_mixer)),

SND_SOC_DAPM_MICBIAS("Mic Bias", WM8974_POWER1, 4, 0),

SND_SOC_DAPM_INPUT("MICN"),
SND_SOC_DAPM_INPUT("MICP"),
SND_SOC_DAPM_INPUT("AUX"),
};

static const struct snd_soc_dapm_route wm8974_dapm_routes[] = {
	/* Mono output mixer */
	{"Mono Mixer", "PCM Playback Switch", "DAC"},
	{"Mono Mixer", "Aux Playback Switch", "Aux Input"},
	{"Mono Mixer", "Line Bypass Switch", "Boost Mixer"},

	/* Speaker output mixer */
	{"Speaker Mixer", "PCM Playback Switch", "DAC"},
	{"Speaker Mixer", "Aux Playback Switch", "Aux Input"},
	{"Speaker Mixer", "Line Bypass Switch", "Boost Mixer"},

	/* Outputs */
	{"Mono Out", NULL, "Mono Mixer"},
	{"MONOOUT", NULL, "Mono Out"},
	{"SpkN Out", NULL, "Speaker Mixer"},
	{"SpkP Out", NULL, "Speaker Mixer"},
	{"SPKOUTN", NULL, "SpkN Out"},
	{"SPKOUTP", NULL, "SpkP Out"},

	/* Boost Mixer */
	{"ADC", NULL, "Boost Mixer"},

	{"Boost Mixer", "Aux Volume", "Aux Input"},
	{"Boost Mixer", "Mic PGA Switch", "Input PGA"},
	{"Boost Mixer", "Mic Volume", "MICP"},

	/* Input PGA */
	{"Input PGA", "Aux Switch", "AUX"},
	{"Input PGA", "MicN Switch", "MICN"},
	{"Input PGA", "MicP Switch", "MICP"},

	/* Inputs */
	//{"Aux Input", NULL, "AUX"},
};

struct pll_ {
	unsigned int pre_div:1;
	unsigned int n:4;
	unsigned int k;
};

/* The size in bits of the pll divide multiplied by 10
 * to allow rounding later */
#define FIXED_PLL_SIZE ((1 << 24) * 10)

static void pll_factors(struct pll_ *pll_div,
			unsigned int target, unsigned int source)
{
	unsigned long long Kpart;
	unsigned int K, Ndiv, Nmod;

	/* There is a fixed divide by 4 in the output path */
	target *= 4;

	Ndiv = target / source;
	if (Ndiv < 6) {
		source /= 2;
		pll_div->pre_div = 1;
		Ndiv = target / source;
	} else
		pll_div->pre_div = 0;

	if ((Ndiv < 6) || (Ndiv > 12))
		printk(KERN_WARNING
			"WM8974 N value %u outwith recommended range!\n",
			Ndiv);

	pll_div->n = Ndiv;
	Nmod = target % source;
	Kpart = FIXED_PLL_SIZE * (long long)Nmod;

	do_div(Kpart, source);

	K = Kpart & 0xFFFFFFFF;

	/* Check if we need to round */
	if ((K % 10) >= 5)
		K += 5;

	/* Move down to proper range now rounding is done */
	K /= 10;

	pll_div->k = K;
}

static int wm8974_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id,
		int source, unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct pll_ pll_div;
	u16 reg;

	if (freq_in == 0 || freq_out == 0) {
		/* Clock CODEC directly from MCLK */
		reg = snd_soc_read(codec, WM8974_CLOCK);
		snd_soc_write(codec, WM8974_CLOCK, reg & 0x0ff);

		/* Turn off PLL */
		reg = snd_soc_read(codec, WM8974_POWER1);
		snd_soc_write(codec, WM8974_POWER1, reg & 0x1df);
		return 0;
	}

	pll_factors(&pll_div, freq_out, freq_in);

	snd_soc_write(codec, WM8974_PLLN, (pll_div.pre_div << 4) | pll_div.n);
	snd_soc_write(codec, WM8974_PLLK1, pll_div.k >> 18);
	snd_soc_write(codec, WM8974_PLLK2, (pll_div.k >> 9) & 0x1ff);
	snd_soc_write(codec, WM8974_PLLK3, pll_div.k & 0x1ff);
	reg = snd_soc_read(codec, WM8974_POWER1);
	snd_soc_write(codec, WM8974_POWER1, reg | 0x020);

	/* Run CODEC from PLL instead of MCLK */
	reg = snd_soc_read(codec, WM8974_CLOCK);
	snd_soc_write(codec, WM8974_CLOCK, reg | 0x100);

	return 0;
}

static int wm8974_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				 int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8974_priv *wm8974 = snd_soc_codec_get_drvdata(codec);
	u16 reg;

	switch(clk_id) {
	case WM8974_MCLKIN:
		/* Clock CODEC directly from MCLK */
		reg = snd_soc_read(codec, WM8974_CLOCK) & 0x0fe;
		snd_soc_write(codec, WM8974_CLOCK, reg);

		/* Turn off PLL */
		reg = snd_soc_read(codec, WM8974_POWER1);
		snd_soc_write(codec, WM8974_POWER1, reg & 0x1df);
		break;
	case WM8974_PLLOUT:
		/* Clock CODEC is generated by PLL */
		reg = snd_soc_read(codec, WM8974_CLOCK) & 0x0fe;
		reg |= 0x100;
		snd_soc_write(codec, WM8974_CLOCK, reg);

		/* Turn on PLL */
		reg = snd_soc_read(codec, WM8974_POWER1);
		snd_soc_write(codec, WM8974_POWER1, reg | 0x020);

//		wm8974_set_dai_pll(codec_dai, clk_id, 0, freq, request_out);
		break;
	case WM8974_MCLKIN_BCLKOUT:
		reg = snd_soc_read(codec, WM8974_CLOCK) & 0x0fe;
		reg |= 0x001;
		snd_soc_write(codec, WM8974_CLOCK, reg);

		/* Turn on PLL */
		reg = snd_soc_read(codec, WM8974_POWER1);
		snd_soc_write(codec, WM8974_POWER1, reg | 0x020);

//		wm8974_set_dai_pll(codec_dai, clk_id, 0, freq, request_out);
		break;

	}


	wm8974->sysclk = freq;
	return 0;
}

/*
 * Configure WM8974 clock dividers.
 */
static int wm8974_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 reg;

	switch (div_id) {
	case WM8974_OPCLKDIV:
		reg = snd_soc_read(codec, WM8974_GPIO) & 0x1cf;
		snd_soc_write(codec, WM8974_GPIO, reg | div);
		break;
	case WM8974_MCLKDIV:
		reg = snd_soc_read(codec, WM8974_CLOCK) & 0x11f;
		snd_soc_write(codec, WM8974_CLOCK, reg | div);
		break;
	case WM8974_BCLKDIV:
		reg = snd_soc_read(codec, WM8974_CLOCK) & 0x1e3;
		snd_soc_write(codec, WM8974_CLOCK, reg | div);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int wm8974_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 iface = 0;
	u16 clk = snd_soc_read(codec, WM8974_CLOCK) & 0x1fe;

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		clk |= 0x0001;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= 0x0010;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= 0x0008;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= 0x00018;
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface |= 0x0180;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= 0x0100;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface |= 0x0080;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_write(codec, WM8974_IFACE, iface);
	snd_soc_write(codec, WM8974_CLOCK, clk);
	return 0;
}

static int wm8974_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	u16 iface = snd_soc_read(codec, WM8974_IFACE) & 0x19f;
	u16 adn = snd_soc_read(codec, WM8974_ADD) & 0x1f1;

	/* bit size */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		iface |= 0x0020;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		iface |= 0x0040;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		iface |= 0x0060;
		break;
	}

	/* filter coefficient */
	switch (params_rate(params)) {
	case 8000:
		adn |= (0x5 << 1);
		break;
	case 11025:
	case 12000:
		adn |= (0x4 << 1);
		break;
	case 16000:
		adn |= (0x3 << 1);
		break;
	case 24000:
	case 22050:
		adn |= (0x2 << 1);
		break;
	case 32000:
		adn |= (0x1 << 1);
		break;
	case 44100:
	case 48000:
		break;
	}

	snd_soc_write(codec, WM8974_IFACE, iface);
	snd_soc_write(codec, WM8974_ADD, adn);
	return 0;
}

static int wm8974_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	u16 mute_reg = snd_soc_read(codec, WM8974_DAC) & 0xffbf;

	if (mute)
		snd_soc_write(codec, WM8974_DAC, mute_reg | 0x40);
	else
		snd_soc_write(codec, WM8974_DAC, mute_reg);
	return 0;
}

static int wm8974_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	u16 val = 0;
	u16 pwr_reg = snd_soc_read(codec, WM8974_POWER1) & 0x170;
	int ret = 0;

	//printk("****wm8974_set_bias_level: level: %d\n", level);
	switch (level) {
	case SND_SOC_BIAS_ON:
		/* ensure bufioen and biasen and micben */
		pwr_reg |= (1 << 2) | (1 << 3) | (1 << 4);
		/* Enable thermal shutdown */
		val = snd_soc_read(codec, WM8974_OUTPUT);
		ret = snd_soc_write(codec, WM8974_OUTPUT, val | 0x2);
		if (ret)
			break;
		/* set vmid to 50k */
		ret = snd_soc_write(codec, WM8974_POWER1, pwr_reg | 0x1);
		break;
	case SND_SOC_BIAS_PREPARE:
		/* ensure bufioen and biasen */
		pwr_reg |= (1 << 2) | (1 << 3);
		ret = snd_soc_write(codec, WM8974_POWER1, pwr_reg | 0x1);
		break;
	case SND_SOC_BIAS_STANDBY:
		/* ensure bufioen and biasen */
		pwr_reg |= (1 << 2) | (1 << 3);
		/* set vmid to 300k for standby */
		ret = snd_soc_write(codec, WM8974_POWER1, pwr_reg | 0x2);
		break;
	case SND_SOC_BIAS_OFF:
		ret = snd_soc_write(codec, WM8974_POWER1, 0);
		ret = snd_soc_write(codec, WM8974_POWER2, 0);
		ret = snd_soc_write(codec, WM8974_POWER3, 0);
		break;
	}

          codec->dapm.bias_level = level;

	return ret;
}

#define WM8974_RATES (SNDRV_PCM_RATE_8000_48000)

#define WM8974_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
	SNDRV_PCM_FMTBIT_S24_LE)

static const struct snd_soc_dai_ops wm8974_ops = {
	.hw_params = wm8974_pcm_hw_params,
	.set_sysclk = wm8974_set_dai_sysclk,
	.digital_mute = wm8974_mute,
	.set_fmt = wm8974_set_dai_fmt,
	.set_clkdiv = wm8974_set_dai_clkdiv,
	.set_pll = wm8974_set_dai_pll,
};

static struct snd_soc_dai_driver wm8974_dai = {
	.name = "wm8974-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,   /* Only 1 channel of data */
		.rates = WM8974_RATES,
		.formats = WM8974_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,   /* Only 1 channel of data */
		.rates = WM8974_RATES,
		.formats = WM8974_FORMATS,},
	.ops = &wm8974_ops,
	.symmetric_rates = 1,
};

static int wm8974_suspend(struct snd_soc_codec *codec)
{
	wm8974_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int wm8974_resume(struct snd_soc_codec *codec)
{
	snd_soc_cache_sync(codec);
	wm8974_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}

static int wm8974_probe(struct snd_soc_codec *codec)
{
	int ret = 0;
	printk(KERN_NOTICE "wm8974 audio codec is provided by amba Wu Dongge\n");

	ret = snd_soc_codec_set_cache_io(codec, 7, 9, SND_SOC_I2C);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

	ret = wm8974_reset(codec);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to issue reset\n");
		return ret;
	}

	wm8974_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	snd_soc_write(codec, WM8974_POWER1, 0x180);

	/************************ RECORD PATH SETTINGS *******************************/
	snd_soc_write(codec, WM8974_CLOCK, 0x0000);	/*set MCLK, MCLKDIV 1, BCLKDIV 1, BCLK and frame clock are inputs*/
	snd_soc_write(codec, WM8974_GPIO, 0x0000);
	snd_soc_write(codec, WM8974_ADD, 0x0000);
	snd_soc_write(codec, WM8974_INPUT, 0x0103);
	snd_soc_write(codec, WM8974_INPPGA, 0x003f);	/*set PGA gain max 35.25db*/
	snd_soc_write(codec, WM8974_ADCBOOST, 0x0100);	/*set PGB2BOOST, MICP2BOOSTVOL gain*/
	snd_soc_write(codec, WM8974_ADC, 0x0188);	/*Enable high pass filter, select audio mode*/
	snd_soc_write(codec, WM8974_ADCVOL, 0x00ff);	/*Set ADC vol, 0db*/

       snd_soc_write(codec, WM8974_ALC1, 0x00134);	/*Enable ALC, set max gain +29.25db and min gain +12db*/
	snd_soc_write(codec, WM8974_ALC2, 0x003c);      /* disable zero cross switch, hold =  11ms,  target= -10.5db  */
       snd_soc_write(codec, WM8974_ALC3, 0x0098);      /*  ALC mode, Decay = 9, attack = 8*/

	snd_soc_write(codec, WM8974_NGATE, 0x000c);	/*Enable nosie gate and set threshold -63db*/

	/************************ PLAYBACK PATH SETTINGS *******************************/
	snd_soc_write(codec, WM8974_DAC, 0x0008);		/*Disable soft mute, Disable DAC auto mute, no inversion*/
	snd_soc_write(codec, WM8974_DACVOL, 0x00ff);	/*Set DACVOL 0dB*/
	snd_soc_write(codec, WM8974_DACLIM1, 0x0000);	/*Disable DAC Limiter*/
	snd_soc_write(codec, WM8974_DACLIM2, 0x000c);	/*Set DAC boost 12dB*/
	snd_soc_write(codec, WM8974_PLLN, 0x0080);		/*PLL power OFF*/
	snd_soc_write(codec, WM8974_SPKMIX, 0x0001);	/*select output DAC to speaker mixer*/
	snd_soc_write(codec, WM8974_SPKVOL, 0x003f);	/*Eable speaker out and set speaker volume +6dB*/
	snd_soc_write(codec, WM8974_MONOMIX, 0x0000); /*Not select DAC to mono*/

	ret = snd_soc_add_codec_controls(codec, wm8974_snd_controls,
			     ARRAY_SIZE(wm8974_snd_controls));

	return ret;
}

/* power down chip */
static int wm8974_remove(struct snd_soc_codec *codec)
{
	wm8974_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_wm8974 = {
	.probe = 	wm8974_probe,
	.remove = 	wm8974_remove,
	.suspend = 	wm8974_suspend,
	.resume =	wm8974_resume,
	.set_bias_level = wm8974_set_bias_level,
	.reg_cache_size = ARRAY_SIZE(wm8974_reg),
	.reg_word_size = sizeof(u16),
	.reg_cache_default = wm8974_reg,
	.reg_cache_step = 1,
	.dapm_widgets = wm8974_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(wm8974_dapm_widgets),
	.dapm_routes = wm8974_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(wm8974_dapm_routes),
};

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
static int wm8974_i2c_probe(struct i2c_client *i2c,
				      const struct i2c_device_id *id)
{
	struct wm8974_priv *wm8974;

	wm8974 = devm_kzalloc(&i2c->dev, sizeof(struct wm8974_priv), GFP_KERNEL);
	if (wm8974 == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, wm8974);

	return snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_wm8974, &wm8974_dai, 1);
}

static int wm8974_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static struct of_device_id wm8974_of_match[] = {
	{ .compatible = "ambarella,wm8974",},
	{},
};
MODULE_DEVICE_TABLE(of, wm8974_of_match);


static const struct i2c_device_id wm8974_i2c_id[] = {
	{ "wm8974", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8974_i2c_id);

static struct i2c_driver wm8974_i2c_driver = {
	.driver = {
		.name = "wm8974-codec",
		.owner = THIS_MODULE,
		.of_match_table = wm8974_of_match,
	},
	.probe =    wm8974_i2c_probe,
	.remove =   wm8974_i2c_remove,
	.id_table = wm8974_i2c_id,
};
#endif

static int __init wm8974_modinit(void)
{
	int ret = 0;

	printk(KERN_INFO "wm8974_amb.c :: __init wm8974_modinit(void)\n");

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	ret = i2c_add_driver(&wm8974_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register wm8974 I2C driver: %d\n",
		       ret);
	}
#endif
	return ret;
}
module_init(wm8974_modinit);

static void __exit wm8974_exit(void)
{
#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	i2c_del_driver(&wm8974_i2c_driver);
#endif
}
module_exit(wm8974_exit);

MODULE_DESCRIPTION("ASoC WM8974 driver");
MODULE_AUTHOR("Wu Dongge");
MODULE_LICENSE("GPL");
