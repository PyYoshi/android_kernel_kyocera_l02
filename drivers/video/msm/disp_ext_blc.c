/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 *
 * drivers/video/msm/disp_ext_blc.c
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
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include <linux/disp_ext_blc.h>
#include "mdp4.h"
#include "disp_ext.h"
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#include "mipi_renesas_video.h"
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/

static struct dsi_buf disp_ext_blc_tx_buf;
static struct dsi_buf disp_ext_blc_rx_buf;

static uint8  select_mode=0;
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
/* cabc select */
static uint8  select_mode_ctrl=0;
static char protect_on[2]  = {0xB0, 0x03};
static char protect_off[2] = {0xB0, 0x00};
static char cabc_change[2] = {0xBB, 0x0B};
static struct dsi_cmd_desc cabc_mode_select_cmds[] = {
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0,  sizeof(protect_off), protect_off},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0,  sizeof(cabc_change), cabc_change},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0,  sizeof(protect_on), protect_on}
};
#else
static uint8  select_mode_ctrl=0;
/* cabc select */
static char cmd1_select[2] = {0xFF, 0x00};
static char cabc_change[2] = {0x55, 0x00};
static struct dsi_cmd_desc cabc_mode_select_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,  sizeof(cmd1_select), cmd1_select},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,  sizeof(cabc_change), cabc_change},
};
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/

extern void mipi_renesas_video_cmds_tx(struct dsi_buf *tp, struct dsi_cmd_desc *cmds, int cnt);
uint8 disp_ext_blc_get_select_mode(void)
{
	return select_mode;
}

void disp_ext_blc_set_select_mode(uint8 sw)
{
	select_mode = sw;
}

int disp_ext_blc_mode_select( uint8_t mode )
{
	struct msm_fb_data_type *mfd;

    DISP_LOCAL_LOG_EMERG("DISP mipi_renesas_video_cabc_mode_select mode=%d S\n",mode);
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_mipitx_lock();
	disp_ext_util_disp_local_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	if( disp_ext_util_get_disp_state() != LOCAL_DISPLAY_ON ) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
		disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		pr_err("%s:panel off\n", __func__);
		return -1;
	}

	if( mode > 0x01 ) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
		disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		pr_err("%s:parameter err\n", __func__);
		return -1;
	}

	if( select_mode_ctrl == mode ) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
		disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		DISP_LOCAL_LOG_EMERG("%s:Request a double. state=%d,req=%d\n",__func__,select_mode_ctrl,mode);
		return 0;
	}

#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
	mfd = mipi_renesas_video_get_mfd( );
#else
	mfd = mipi_novatek_wxga_get_mfd();
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/
	if( mfd == NULL ) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
		disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		pr_err("%s:mfd == NULL\n", __func__);
		return -1;
	}

	select_mode_ctrl=mode;

#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
	msm_fb_pan_lock();
	msm_fb_ioctl_ppp_lock();
	msm_fb_ioctl_lut_lock();
	if( mode == 0 ) {
		cabc_change[1] = 0x0A;
	}
	else {
		cabc_change[1] = 0x0A + select_mode;
	}
#else
	msm_fb_pan_lock();
	msm_fb_ioctl_ppp_lock();
	msm_fb_ioctl_lut_lock();
	mdp4_dsi_cmd_dma_busy_wait(mfd);
	mdp4_dsi_blt_dmap_busy_wait(mfd);
	mipi_dsi_mdp_busy_wait(mfd);
	if( mode == 0 ) {
		cabc_change[1] = 0;
	}
	else {
		cabc_change[1] = select_mode;
	}
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/
	pr_debug("%s:cabc_change[%d,%d]\n", __func__,cabc_change[0],cabc_change[1]);
	DISP_LOCAL_LOG_EMERG("%s:cabc_change[%d,%d]\n", __func__,cabc_change[0],cabc_change[1]);
	mipi_set_tx_power_mode(0);
	mipi_renesas_video_cmds_tx( &disp_ext_blc_tx_buf,
					cabc_mode_select_cmds,
					ARRAY_SIZE(cabc_mode_select_cmds));
	udelay(1);

	msm_fb_ioctl_lut_unlock();
	msm_fb_ioctl_ppp_unlock();
	msm_fb_pan_unlock();
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_unlock();
	disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
	pr_debug("%s:cabc select\n", __func__);
    DISP_LOCAL_LOG_EMERG("DISP mipi_renesas_video_cabc_mode_select E\n");

	return 0;
}

void disp_ext_blc_init(void)
{
	mipi_dsi_buf_alloc(&disp_ext_blc_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&disp_ext_blc_rx_buf, DSI_BUF_SIZE);
}
