/* drivers/video/exynos/panels/s6e3fa0_lcd_ctrl.c
 *
 * Samsung SoC MIPI LCD CONTROL functions
 *
 * Copyright (c) 2014 Samsung Electronics
 *
 * Jiun Yu, <jiun.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include "s6e3fa0_gamma.h"
#include "s6e3fa0_param.h"
#include "lcd_ctrl.h"
#include "../decon_display/dsim_reg.h"

#define GAMMA_PARAM_SIZE 26

/* Porch values. It depends on command or video mode */
#define S6E3FA0_CMD_VBP		10
#define S6E3FA0_CMD_VFP		1
#define S6E3FA0_CMD_VSA		1
#define S6E3FA0_CMD_HBP		1
#define S6E3FA0_CMD_HFP		1
#define S6E3FA0_CMD_HSA		1

#define S6E3FA0_VIDEO_VBP	2
#define S6E3FA0_VIDEO_VFP	20
#define S6E3FA0_VIDEO_VSA	2
#define S6E3FA0_VIDEO_HBP	20
#define S6E3FA0_VIDEO_HFP	20
#define S6E3FA0_VIDEO_HSA	20

#define S6E3FA0_HORIZONTAL	1080
#define S6E3FA0_VERTICAL	1920

/*
 * 3FAH0 lcd init sequence
 *
 * Parameters
 *	- mic : if mic is enabled, MIC_ENABLE command must be sent
 *	- mode : LCD init sequence depends on command or video mode
 */
void lcd_init(struct decon_lcd *lcd)
{
	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_F0,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_F0)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_F0 command.\n");
	msleep(12);

	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_F1,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_F1)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_F1 command.\n");
	msleep(12);

	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_FC,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_FC)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_FC command.\n");
	msleep(12);

	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_ED,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_ED)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_ED command.\n");
	msleep(12);

	if (lcd->mode == COMMAND_MODE) {
		if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_FD,
					ARRAY_SIZE(SEQ_TEST_KEY_ON_FD)) < 0)
			dsim_err("fail to send SEQ_TEST_KEY_ON_FD command.\n");
		msleep(12);

		if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE_PARAM, SEQ_TEST_KEY_ON_F6[0],
					SEQ_TEST_KEY_ON_F6[1]) < 0)
			dsim_err("fail to send SEQ_TEST_KEY_ON_F6 command.\n");
		mdelay(12);
	} else {
		if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_E7,
					ARRAY_SIZE(SEQ_TEST_KEY_ON_E7)) < 0)
			dsim_err("fail to send SEQ_TEST_KEY_ON_E7 command.\n");
		msleep(120);
	}

	if (lcd->mic)
		dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE_PARAM, SEQ_MIC_ENABLE[0],
				SEQ_MIC_ENABLE[1]);

	if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE, SEQ_SLEEP_OUT[0], 0) < 0)
		dsim_err("fail to send SEQ_SLEEP_OUT command.\n");
	mdelay(20);

	if (lcd->mode == VIDEO_MODE) {
		if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE, SEQ_DISPLAY_ON[0], 0) < 0)
			dsim_err("fail to send SEQ_DISPLAY_ON command.\n");
		mdelay(120);

		if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE_PARAM, SEQ_TEST_KEY_ON_F2[0],
					SEQ_TEST_KEY_ON_F2[1]) < 0)
			dsim_err("fail to send SEQ_TEST_KEY_ON_F2 command.\n");
		mdelay(12);
	}

	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_EB,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_EB)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_EB command.\n");
	mdelay(12);

	if (dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_TEST_KEY_ON_C0,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_C0)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_C0 command.\n");
	mdelay(12);

	if (lcd->mode == COMMAND_MODE) {
		if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE, SEQ_TE_ON[0], 0) < 0)
			dsim_err("fail to send SEQ_TE_ON command.\n");
		mdelay(12);
	}
}

void lcd_enable(void)
{
	if (dsim_wr_data(MIPI_DSI_DCS_SHORT_WRITE, SEQ_DISPLAY_ON[0], 0) < 0)
		dsim_err("fail to send SEQ_DISPLAY_ON command.\n");
}

void lcd_disable(void)
{
	/* This function needs to implement */
}

/*
 * Set gamma values
 *
 * Parameter
 *	- backlightlevel : It is from 0 to 26.
 */
int lcd_gamma_ctrl(u32 backlightlevel)
{
	int ret;

	ret = dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)gamma22_table[backlightlevel],
			GAMMA_PARAM_SIZE);
	if (ret) {
		dsim_err("fail to write gamma value.\n");
		return ret;
	}

	return 0;
}

int lcd_gamma_update(void)
{
	int ret;

	ret = dsim_wr_data(MIPI_DSI_DCS_LONG_WRITE, (u32)SEQ_GAMMA_UPDATE,
			ARRAY_SIZE(SEQ_GAMMA_UPDATE));
	if (ret) {
		dsim_err("fail to update gamma value.\n");
		return ret;
	}

	return 0;
}
