/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 *
 * drivers/video/msm/mipi_renesas_video_720p_pt.c
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
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_renesas_video.h"

static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
  /* regulator */
  {0x03, 0x0a, 0x04, 0x00, 0x20},
  /* timing */
  {0x79, 0x25, 0x12, 0x00, 0x36, 0x40, 0x16, 0x29, 0x20, 0x03, 0x04, 0xa0},
  /* phy ctrl */
  {0x5f, 0x00, 0x00, 0x10},
  /* strength */
  {0xff, 0x00, 0x06, 0x00},
  /* pll control */
  {0x01, 0xb5, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63, 0x41, 0x0f, 0x03,
   0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_video_renesas_720p_pt_init(void)
{
	int ret;

	if (msm_fb_detect_client("mipi_video_renesas_720p")) {
		return 0;
	}

	pinfo.xres = 720;
	pinfo.yres = 1280;
	pinfo.bpp = 24;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.wait_cycle = 0;
	pinfo.pdest = DISPLAY_1;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;

	pinfo.lcdc.h_back_porch = 16;
	pinfo.lcdc.h_front_porch = 187;
	pinfo.lcdc.h_pulse_width = 16;
	pinfo.lcdc.v_back_porch = 3;
	pinfo.lcdc.v_front_porch = 9;
	pinfo.lcdc.v_pulse_width = 4;
	pinfo.lcdc.border_clr = 0; /* blake */
	pinfo.lcdc.underflow_clr = 0xff; /* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.lcd.blt_ctrl = 1;

	pinfo.lcd.refx100 = 6000;
	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;

	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;
	pinfo.mipi.t_clk_post = 0x02;
	pinfo.mipi.t_clk_pre = 0x19;
	pinfo.mipi.stream = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.vc = 0;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.pulse_mode_hsa_he = FALSE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.esc_byte_ratio = 3;
	pinfo.mipi.tx_eot_append = TRUE;

	ret = mipi_renesas_video_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_720P_PT);
	if (ret) {
		pr_err("%s: failed to register device!\n", __func__);
	}

	return ret;
}

module_init(mipi_video_renesas_720p_pt_init);
