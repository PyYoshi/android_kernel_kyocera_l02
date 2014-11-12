/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 *
 * drivers/video/msm/mipi_renesas_video.h
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
#ifndef MIPI_RENESAS_VIDEO_H
#define MIPI_RENESAS_VIDEO_H

int mipi_renesas_video_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

int mipi_renesas_video_lcd_on_exec(struct msm_fb_data_type *mfd);
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
int mipi_renesas_video_lcd_early_off_exec(struct msm_fb_data_type *mfd);
#endif /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
int mipi_renesas_video_lcd_off_exec(struct msm_fb_data_type *mfd, int dstb);
void mipi_renesas_video_refresh_exec( struct msm_fb_data_type *mfd );
int mipi_renesas_video_panel_power( int on );
struct msm_fb_data_type *mipi_renesas_video_get_mfd(void);
uint32 mipi_renesas_video_cmds_rx(struct dsi_buf *rp, struct dsi_cmd_desc *cmds, int cnt);

void mipi_renesas_video_gamma_Ap_set( uint8_t *gamma_table );
void mipi_renesas_video_gamma_Am_set( uint8_t *gamma_table );
void mipi_renesas_video_gamma_Bp_set( uint8_t *gamma_table );
void mipi_renesas_video_gamma_Bm_set( uint8_t *gamma_table );
void mipi_renesas_video_gamma_Cp_set( uint8_t *gamma_table );
void mipi_renesas_video_gamma_Cm_set( uint8_t *gamma_table );

#ifndef CONFIG_DISP_EXT_UTIL
#include <linux/msm_mdp.h>

#define disp_ext_util_get_disp_state() LOCAL_POWER_OFF
#define disp_ext_util_set_disp_state(state)   (void)0;
#endif /* CONFIG_DISP_EXT_UTIL */

#endif  /* MIPI_RENESAS_VIDEO_H */
