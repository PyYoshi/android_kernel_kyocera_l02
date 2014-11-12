/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 *
 * drivers/video/msm/mipi_renesas_video_tbl.h
 *
 * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
*/
#ifndef MIPI_RENESAS_VIDEO_TBL_H
#define MIPI_RENESAS_VIDEO_TBL_H

#define RENESAS_VIDEO_SEQ_DELAY 0
#define RENESAS_VIDEO_SLEEP_OUT_DELAY 130
#define RENESAS_VIDEO_SLEEP_IN_DELAY 90

static char config_mcap[2] = {0xB0, 0x00};
static char config_acr[2] = {0xB2, 0x00};
static char config_clk_if[2] = {0xB3, 0x0C};
static char config_pxl_format[2] = {0xB4, 0x02};

static char config_cabc_pfm_pwm_control[] = {
	0xB9,
	0x01, 0x00, 0x75, 0x00,
};
static char config_cabc_user_parameter[] = {
	0xBE,
	0xFF, 0x0F, 0x00, 0x3F, 0x04,
	0x40, 0x00, 0x5D,
};
static char config_cabc_blc_1[] = {
	0xB7,
	0x18, 0x00, 0x18, 0x18, 0x0C,
	0x0A, 0x4C, 0x0A, 0x6C, 0x0A, 
	0x0C, 0x0A, 0x00, 0x10,
};
static char config_cabc_blc_2[] = {
	0xB8,
	0xF8, 0xDA, 0x6D, 0xFB, 0xFF,
	0xFF, 0xCF, 0xDF, 0xE5, 0xFF,
	0xFF, 0xFF, 0xFF,
};
static char config_cabc_on[2] = {0xBB, 0x0B};

static char config_panel_drive_setting[] = {
	0xC0,
	0x40, 0x02, 0x7F, 0xC8, 0x08,
};

static char config_display_h_timing[] = {
	0xC1,
	0x00, 0xA8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x9D, 0x08, 0x27,
	0x09, 0x00, 0x00, 0x00, 0x00,
};

static char config_source_output[] = {
	0xC2,
	0x00, 0x00, 0x09, 0x00, 0x00,
};

static char config_gate_ic_control[] = {
	0xC3, 0x04,
};

static char config_ltps_if_control_01[] = {
	0xC4,
	0x4D, 0x83, 0x00,
};

static char config_source_output_mode[] = {
	0xC6,
	0x12, 0x00, 0x08, 0x71, 0x00,
	0x00, 0x00, 0x80, 0x00, 0x04,
};

static char config_ltps_if_control_02[] = {
	0xC7, 0x22,
};

static char config_gamma_control[] = {
	0xC8,
	0x4C, 0x0C, 0x0C, 0x0C,
};

static char config_gamma_control_set_a_positive[] = {
	0xC9,
	0x00, 0x05, 0x10, 0x2E, 0x37,
	0x34, 0x3E, 0x46, 0x3A, 0x34,
	0x35, 0x35, 0x3F,
};

static char config_gamma_control_set_a_negative[] = {
	0xCA,
	0x00, 0x0A, 0x2A, 0x2B, 0x25,
	0x19, 0x21, 0x2B, 0x28, 0x31,
	0x4F, 0x3A, 0x3F,
};

static char config_gamma_control_set_b_positive[] = {
	0xCB,
	0x00, 0x10, 0x26, 0x3E, 0x43,
	0x3E, 0x46, 0x4C, 0x3F, 0x38,
	0x3B, 0x28, 0x39,
};

static char config_gamma_control_set_b_negative[] = {
	0xCC,
	0x06, 0x17, 0x24, 0x27, 0x20,
	0x13, 0x19, 0x21, 0x1C, 0x21,
	0x39, 0x2F, 0x3F,
};

static char config_gamma_control_set_c_positive[] = {
	0xCD,
	0x00, 0x18, 0x36, 0x4F, 0x52,
	0x4B, 0x50, 0x53, 0x44, 0x3C,
	0x3E, 0x37, 0x3F,
};

static char config_gamma_control_set_c_negative[] = {
	0xCE,
	0x00, 0x08, 0x21, 0x23, 0x1B,
	0x0C, 0x0F, 0x14, 0x0D, 0x10,
	0x29, 0x27, 0x3F,
};

static char config_power_setting_1[] = {
	0xD0,
	0x6A, 0x64, 0x01,
};

static char config_power_setting_2[] = {
	0xD1,
	0x77, 0xD4,
};

static char config_power_setting_for_internal[] = {
	0xD3, 0x33,
};

static char config_vplvl_vnlvl_setting[] = {
	0xD5,
	0x0C, 0x0C,
};

static char config_vcomdc_setting_01[] = {
	0xD8,
	0x34, 0x64, 0x23, 0x25, 0x62,
	0x32,
};

static char config_vcomdc_setting_02[] = {
	0xDE,
	0x10, 0x80, 0x11, 0x0B, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
};

static char config_panel_drive_setting_02[] = {
	0xFD,
	0x04, 0x55, 0x53, 0x00, 0x70,
	0xFF, 0x10, 0x73,
};

static char config_exit_sleep_mode[2] = {0x11, 0x00};

static char config_set_display_on[2] = {0x29, 0x00};

static char config_enter_sleep_mode[2] = {0x10, 0x00};

static char config_send_by_lp_mode[2] = {0xB1, 0x01};


static struct dsi_cmd_desc renesas_video_display_on_th_cmds[] = {
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_mcap),
			config_mcap},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_acr),
			config_acr},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_clk_if),
			config_clk_if},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_pxl_format),
			config_pxl_format},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_cabc_pfm_pwm_control),
			config_cabc_pfm_pwm_control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_cabc_user_parameter),
			config_cabc_user_parameter},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_cabc_blc_1),
			config_cabc_blc_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_cabc_blc_2),
			config_cabc_blc_2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_cabc_on),
			config_cabc_on},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_panel_drive_setting),
			config_panel_drive_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_display_h_timing),
			config_display_h_timing},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_source_output),
			config_source_output},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gate_ic_control),
			config_gate_ic_control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_ltps_if_control_01),
			config_ltps_if_control_01},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_source_output_mode),
			config_source_output_mode},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_ltps_if_control_02),
			config_ltps_if_control_02},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control),
			config_gamma_control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_a_positive),
			config_gamma_control_set_a_positive},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_a_negative),
			config_gamma_control_set_a_negative},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_b_positive),
			config_gamma_control_set_b_positive},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_b_negative),
			config_gamma_control_set_b_negative},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_c_positive),
			config_gamma_control_set_c_positive},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_gamma_control_set_c_negative),
			config_gamma_control_set_c_negative},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_power_setting_1),
			config_power_setting_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_power_setting_2),
			config_power_setting_2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_power_setting_for_internal),
			config_power_setting_for_internal},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_vplvl_vnlvl_setting),
			config_vplvl_vnlvl_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_vcomdc_setting_01),
			config_vcomdc_setting_01},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_vcomdc_setting_02),
			config_vcomdc_setting_02},
	{DTYPE_GEN_LWRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_panel_drive_setting_02),
			config_panel_drive_setting_02},
	{DTYPE_DCS_WRITE, 1, 0, 0, RENESAS_VIDEO_SLEEP_OUT_DELAY,
		sizeof(config_exit_sleep_mode),
			config_exit_sleep_mode},
};
static struct dsi_cmd_desc renesas_video_display_on_bh_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_set_display_on),
			config_set_display_on},
};

static struct dsi_cmd_desc renesas_cm_display_off_th_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, RENESAS_VIDEO_SLEEP_IN_DELAY,
		sizeof(config_enter_sleep_mode),
			config_enter_sleep_mode},
};

static struct dsi_cmd_desc renesas_cm_display_off_bh_cmds[] = {
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_mcap),
			config_mcap},
	{DTYPE_GEN_WRITE2, 1, 0, 0, RENESAS_VIDEO_SEQ_DELAY,
		sizeof(config_send_by_lp_mode),
			config_send_by_lp_mode},
};

#endif  /* MIPI_RENESAS_VIDEO_TBL_H */
