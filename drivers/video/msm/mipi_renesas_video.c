/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 *
 * drivers/video/msm/mipi_renesas_video.c
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
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_renesas_video.h"
#include "mipi_renesas_video_tbl.h"
#include "mdp.h"
#include "mdp4.h"
#include "mipi_dsi.h"
#ifdef CONFIG_DISP_EXT_BLC
#include <linux/leds-lm3533.h>
#endif /* CONFIG_DISP_EXT_BLC */
#include "disp_ext.h"
static struct mipi_dsi_platform_data *mipi_renesas_video_pdata;

#define TM_GET_PID(id) (((id) & 0xff00)>>8)

static struct dsi_buf renesas_video_tx_buf;
static struct dsi_buf renesas_video_rx_buf;

static char renesas_video_rx_flag ;
static u32  renesas_video_r_data ;

static int mipi_renesas_video_lcd_init(void);

extern struct platform_device msm_mipi_dsi1_device;
static struct regulator *reg_l8, *reg_l2, *reg_lsv5;
static int dsi_boot_on = 0;
static struct msm_fb_data_type *mipi_renesas_video_mfd;
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
static int lcd_early_off_complete = 0;
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */

void mipi_renesas_video_cmds_tx(struct dsi_buf *tp, struct dsi_cmd_desc *cmds, int cnt)
{
	struct dcs_cmd_req cmdreq;

	cmdreq.cmds = cmds;
	cmdreq.cmds_cnt = cnt;
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);
}

static void mipi_renesas_video_rx_data( u32 data )
{
	renesas_video_r_data = data & 0x000000FF ;
	renesas_video_rx_flag = 2 ;
}
uint32 mipi_renesas_video_cmds_rx(struct dsi_buf *rp, struct dsi_cmd_desc *cmds, int cnt)
{
	struct dcs_cmd_req cmdreq;

	cmdreq.cmds = cmds;
	cmdreq.cmds_cnt = cnt;
	cmdreq.flags = (CMD_REQ_COMMIT | CMD_REQ_RX) ;
	cmdreq.rlen = cnt ;
	cmdreq.cb = mipi_renesas_video_rx_data ;
	renesas_video_rx_flag = 1 ;
	mipi_dsi_cmdlist_put(&cmdreq);
    return renesas_video_r_data ;
}

void mipi_renesas_video_refresh_exec( struct msm_fb_data_type *mfd )
{
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
   DISP_LOCAL_LOG_EMERG("DISP mipi_renesas_video_refresh_sequence S\n");

    /* Reset Sequence */
    DISP_LOCAL_LOG_EMERG(" Reset Sequence\n");
    gpio_set_value( 48, 0 );
    msleep(  5 );
    gpio_set_value( 48, 1 );
    msleep( 10 );

    /* On Sequence */
    DISP_LOCAL_LOG_EMERG(" On Sequence\n");
    gpio_set_value( 138, 1 );
    msleep( 20 );
	mipi_dsi_sw_reset();
   	mipi_dsi_op_mode_config(DSI_VIDEO_MODE);
   	msleep(1);
    mipi_set_tx_power_mode(0);
    DISP_LOCAL_LOG_EMERG("DISP %s display on sequence\n",__func__);
    mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
                      renesas_video_display_on_th_cmds,
                      ARRAY_SIZE(renesas_video_display_on_th_cmds) );
    mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
                      renesas_video_display_on_bh_cmds,
                      ARRAY_SIZE(renesas_video_display_on_bh_cmds) );
    DISP_LOCAL_LOG_EMERG("DISP mipi_renesas_720p_refresh_sequence E\n");
#else  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
	gpio_set_value(48, 0);
	msleep(5);
	gpio_set_value(48, 1);
	msleep(10);
	mipi_set_tx_power_mode(0);
	mipi_dsi_clk_cfg(1);
	mipi_dsi_cmds_tx(&renesas_cm_tx_buf, renesas_cm_refresh_cmds,
		ARRAY_SIZE(renesas_cm_refresh_cmds));
	mipi_dsi_clk_cfg(0);
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
}

int mipi_renesas_video_panel_power( int on )
{
	int rc;
	local_disp_state_enum disp_state;

	disp_state = disp_ext_util_get_disp_state();

	DISP_LOCAL_LOG_EMERG("%s start:state=%d req=%d\n", __func__,disp_state, on);

	if (on == 1) {
		if(disp_state == LOCAL_DISPLAY_DEF){
		} else if(disp_state == LOCAL_DISPLAY_OFF){
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L8 - Normal mode\n",__func__);
			rc = regulator_set_optimum_mode(reg_l8, 40000);
			if (rc < 0) {
				pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L2 - Normal mode\n",__func__);
			rc = regulator_set_optimum_mode(reg_l2, 40000);
			if (rc < 0) {
				pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			}

			DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
			gpio_set_value(48, 0);
			msleep(20);
			DISP_LOCAL_LOG_EMERG("%s: high gpio48\n",__func__);
			gpio_set_value(48, 1);
			msleep(20);
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		} else if(disp_state == LOCAL_DISPLAY_POWER_OFF) {
			DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
			gpio_set_value(48, 0);

			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L8 - Normal mode\n",__func__);
			rc = regulator_set_optimum_mode(reg_l8, 40000);
			if (rc < 0) {
				pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L2 - Normal mode\n",__func__);
			rc = regulator_set_optimum_mode(reg_l2, 40000);
			if (rc < 0) {
				pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: enable reg_lsv5\n",__func__);
			rc = regulator_enable(reg_lsv5);
			if (rc) {
				pr_err("enable 8921_lvs5 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: enable reg_l8\n",__func__);
			rc = regulator_enable(reg_l8);
			if (rc) {
				pr_err("enable l8 failed, rc=%d\n", rc);
			}
			msleep(20);
			DISP_LOCAL_LOG_EMERG("%s: enable l2\n",__func__);
			rc = regulator_enable(reg_l2);
			if (rc) {
				pr_err("enable l2 failed, rc=%d\n", rc);
			}
			msleep(1);
			DISP_LOCAL_LOG_EMERG("%s: high gpio48\n",__func__);
			gpio_set_value(48, 1);
			msleep(20);
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		} else {
			DISP_LOCAL_LOG_EMERG("%s:end Double call of power supply ON\n",__func__);
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		}
	}
	else {
		if(disp_state == LOCAL_DISPLAY_DEF){
		} else if(disp_state == LOCAL_DISPLAY_ON){
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L8 - LPM\n",__func__);
			rc = regulator_set_optimum_mode(reg_l8, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L2 - LPM\n",__func__);
			rc = regulator_set_optimum_mode(reg_l2, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		} else if(disp_state == LOCAL_DISPLAY_OFF){
			DISP_LOCAL_LOG_EMERG("%s: disable l2\n",__func__);
			rc = regulator_disable(reg_l2);
			if (rc) {
				pr_err("enable l2 failed, rc=%d\n", rc);
			}
			msleep(5);
			DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
			gpio_set_value(48, 0);
			msleep(20);

			DISP_LOCAL_LOG_EMERG("%s: disable reg_l8\n",__func__);
			rc = regulator_disable(reg_l8);
			if (rc) {
				pr_err("disable reg_l8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: disable reg_lsv5\n",__func__);
			rc = regulator_disable(reg_lsv5);
			if (rc) {
				pr_err("disable 8921_lvs5 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		} else if(disp_state == LOCAL_DISPLAY_POWER_ON) {
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L8 - LPM\n",__func__);
			rc = regulator_set_optimum_mode(reg_l8, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode L8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L2 - LPM\n",__func__);
			rc = regulator_set_optimum_mode(reg_l2, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode L2 failed, rc=%d\n", rc);
			}

			DISP_LOCAL_LOG_EMERG("%s: disable l2\n",__func__);
			rc = regulator_disable(reg_l2);
			if (rc) {
				pr_err("enable l2 failed, rc=%d\n", rc);
			}
			msleep(5);
			DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
			gpio_set_value(48, 0);

			msleep(20);

			DISP_LOCAL_LOG_EMERG("%s: disable reg_l8\n",__func__);
			rc = regulator_disable(reg_l8);
			if (rc) {
				pr_err("disable reg_l8 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s: disable reg_lsv5\n",__func__);
			rc = regulator_disable(reg_lsv5);
			if (rc) {
				pr_err("disable 8921_lvs5 failed, rc=%d\n", rc);
			}
			DISP_LOCAL_LOG_EMERG("%s:end\n",__func__);
			return 0;
		} else {
			DISP_LOCAL_LOG_EMERG("%s:end Double call of power supply OFF\n",__func__);
			return 0;
		}
	}

	if (disp_state == LOCAL_DISPLAY_DEF) {

		reg_lsv5 = regulator_get(NULL,"8921_lvs5");
		if (IS_ERR(reg_lsv5)) {
			pr_err("could not get 8921_lvs5, rc = %ld\n",
				PTR_ERR(reg_lsv5));
			goto panel_power_err;
		}
		reg_l8 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc");
		if (IS_ERR(reg_l8)) {
			pr_err("could not get 8921_l8, rc = %ld\n",
				PTR_ERR(reg_l8));
			goto panel_power_err;
		}
		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8921_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			goto panel_power_err;
		}
		rc = regulator_set_voltage(reg_l8, 2800000, 2800000);
		if (rc) {
			pr_err("set_voltage l8 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = gpio_request(48, "disp_rst_n");
		if (rc) {
			pr_err("request gpio 48 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = gpio_request(138, "disp_dcdc_en");
		if (rc) {
			pr_err("request gpio 138 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = gpio_tlmm_config( GPIO_CFG( 138,0,GPIO_CFG_OUTPUT,
										GPIO_CFG_NO_PULL,GPIO_CFG_2MA),
								GPIO_CFG_ENABLE );
		if (rc) {
			pr_err("config gpio 138 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		if (gpio_get_value(48) == 1) {
			pr_info("%s already power on\n", __func__);
			dsi_boot_on = 1;
		}
		disp_ext_util_set_disp_state(LOCAL_DISPLAY_POWER_OFF);
	}

	if (on)
	{
		if (!dsi_boot_on) {
			DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
			gpio_set_value(48, 0);
		}

		DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L8 - Normal mode\n",__func__);
		rc = regulator_set_optimum_mode(reg_l8, 40000);
		if (rc < 0) {
			pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		DISP_LOCAL_LOG_EMERG("%s: set_optimum_mode:L2 - Normal mode\n",__func__);
		rc = regulator_set_optimum_mode(reg_l2, 40000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			goto panel_power_err;
		}

		DISP_LOCAL_LOG_EMERG("%s: enable reg_lsv5\n",__func__);
		rc = regulator_enable(reg_lsv5);
		if (rc) {
			pr_err("enable 8921_lvs5 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		DISP_LOCAL_LOG_EMERG("%s: enable reg_l8\n",__func__);
		rc = regulator_enable(reg_l8);
		if (rc) {
			pr_err("enable l8 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		msleep(20);
		DISP_LOCAL_LOG_EMERG("%s: enable l2\n",__func__);
		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		msleep(1);
		if (!dsi_boot_on) {
			DISP_LOCAL_LOG_EMERG("%s: high gpio48\n",__func__);
			gpio_set_value(48, 1);
		}

		msleep(20);
		disp_ext_util_set_disp_state(LOCAL_DISPLAY_POWER_ON);
	}
	else
	{
		disp_ext_util_set_disp_state(LOCAL_DISPLAY_POWER_OFF);
		DISP_LOCAL_LOG_EMERG("%s: low gpio48\n",__func__);
		gpio_set_value(48, 0);
		msleep(20);
		DISP_LOCAL_LOG_EMERG("%s: disable reg l8\n",__func__);
		rc = regulator_disable(reg_l8);
		if (rc) {
			pr_err("disable reg_l8 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		DISP_LOCAL_LOG_EMERG("%s: disable reg lsv5\n",__func__);
		rc = regulator_disable(reg_lsv5);
		if (rc) {
			pr_err("disable 8921_lvs5 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = regulator_set_optimum_mode(reg_l2, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
		rc = regulator_set_optimum_mode(reg_l8, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			goto panel_power_err;
		}
	}
	DISP_LOCAL_LOG_EMERG("%s:end\n", __func__);

	return 0;

panel_power_err:
	DISP_LOCAL_LOG_EMERG("%s:err end\n", __func__);
	return -ENODEV;
}

int mipi_renesas_video_lcd_on_exec(struct msm_fb_data_type *mfd)
{
	struct fb_info *fbi;
	local_disp_state_enum disp_state;
	int is_boot;
    DISP_LOCAL_LOG_EMERG("DISP %s S\n",__func__);

	pr_info("checkpoint: %s: start\n", __func__);
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	disp_state = disp_ext_util_get_disp_state();
	if (disp_state == LOCAL_DISPLAY_ON) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		DISP_LOCAL_LOG_EMERG("DISP %s disp_state(%d) E\n",__func__,disp_state);
		return 0;
	}

	if (TM_GET_PID(mfd->panel.id) == MIPI_DSI_PANEL_720P_PT) {
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
		if(disp_ext_board_get_panel_detect() == -1){
			pr_err("%s:panel not found\n", __func__);
#ifdef CONFIG_DISP_EXT_UTIL
			disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
			return 0;
		}
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
		if ( mipi_renesas_video_panel_power( 1 ) ) {
#ifdef CONFIG_DISP_EXT_UTIL
			disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
			return -ENODEV;
		}

		is_boot = (disp_ext_board_get_panel_detect() == 0) ? 1 : 0;

#ifdef CONFIG_DISP_EXT_BOARD
		if( disp_ext_board_detect_board(mfd) == -1 ) {
			mipi_renesas_video_panel_power( 0 );
			pr_err("%s:disp_ext_board_detect_board err:\n", __func__);
#ifdef CONFIG_DISP_EXT_UTIL
			disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
			return 0;
		}
#endif /* CONFIG_DISP_EXT_BOARD */

		fbi = mfd->fbi;
		memset(fbi->screen_base, 0x00, fbi->fix.smem_len);
		pr_debug("%s:Frame buffer clear addr=%#X size=%d\n", __func__,(uint)fbi->screen_base,fbi->fix.smem_len);

		if (!(is_boot && dsi_boot_on)) {
			DISP_LOCAL_LOG_EMERG("%s: high gpio138\n",__func__);
			gpio_set_value(138, 1);
			msleep(20);
			mipi_set_tx_power_mode(0);
			DISP_LOCAL_LOG_EMERG("DISP %s display on sequence\n",__func__);
			mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
							renesas_video_display_on_th_cmds,
							ARRAY_SIZE(renesas_video_display_on_th_cmds));
			mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
							renesas_video_display_on_bh_cmds,
							ARRAY_SIZE(renesas_video_display_on_bh_cmds));
		} else {
			mipi_set_tx_power_mode(0);
			DISP_LOCAL_LOG_EMERG("DISP %s skip initalize\n",__func__);
			mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
							renesas_video_display_on_bh_cmds,
							ARRAY_SIZE(renesas_video_display_on_bh_cmds));
		}
		
		disp_ext_util_set_disp_state(LOCAL_DISPLAY_ON);
#ifdef CONFIG_DISP_EXT_REFRESH
		disp_ext_reflesh_set_sw(0);
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#else
		disp_ext_refresh_te_monitor_timer_release();
		disp_ext_reflesh_before_te_run_count_init();
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/
#endif /* CONFIG_DISP_EXT_REFRESH */
	}
	else {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		pr_err("%s:err end\n", __func__);
		return -EINVAL;
	}

#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
	pr_info("checkpoint: %s: end\n", __func__);
    DISP_LOCAL_LOG_EMERG("DISP %s E\n",__func__);

	return 0;
}

static int mipi_renesas_video_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
  
    DISP_LOCAL_LOG_EMERG("DISP %s\n",__func__);

	mfd = platform_get_drvdata(pdev);
	pr_info("%s\n", __func__);
	mipi_renesas_video_mfd = mfd;

	if (!mfd) {
		return -ENODEV;
	}
	if (mfd->key != MFD_KEY) {
		return -EINVAL;
	}

	return mipi_renesas_video_lcd_on_exec(mfd);
}

int mipi_renesas_video_lcd_off_exec(struct msm_fb_data_type *mfd, int dstb)
{
    DISP_LOCAL_LOG_EMERG("DISP %s S\n",__func__);

	pr_info("checkpoint: %s: start\n", __func__);
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#else  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	if (disp_ext_util_get_disp_state() != LOCAL_DISPLAY_ON) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		DISP_LOCAL_LOG_EMERG("DISP %s disp_state(%d) E\n",__func__,disp_ext_util_get_disp_state());
		return 0;
	}

#ifdef CONFIG_DISP_EXT_REFRESH
	disp_ext_refresh_te_monitor_timer_release();
#endif /* CONFIG_DISP_EXT_REFRESH */

	mipi_set_tx_power_mode(0);
	mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
					renesas_cm_display_off_th_cmds,
					ARRAY_SIZE(renesas_cm_display_off_th_cmds));
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
	DISP_LOCAL_LOG_EMERG("%s: low gpio138\n",__func__);
	gpio_set_value(138, 0);
	msleep(20);

	if( dstb == 1 )
	{
		mipi_set_tx_power_mode(1);
		mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
						renesas_cm_display_off_bh_cmds,
						ARRAY_SIZE(renesas_cm_display_off_bh_cmds));

		mipi_renesas_video_panel_power(0);
	}

	disp_ext_util_set_disp_state(LOCAL_DISPLAY_OFF);
#ifdef CONFIG_DISP_EXT_REFRESH
	disp_ext_reflesh_set_sw(0);
#endif /* CONFIG_DISP_EXT_REFRESH */
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
	pr_info("checkpoint: %s: end\n", __func__);

    DISP_LOCAL_LOG_EMERG("DISP %s E\n",__func__);
	return 0;
}

static int mipi_renesas_video_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

    DISP_LOCAL_LOG_EMERG("DISP %s\n",__func__);

#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
	if (lcd_early_off_complete == 0) {
		pr_err("%s:lcd_early_off not complete.\n", __func__);
		return -EINVAL;
	}
	lcd_early_off_complete = 0;
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */

	mfd = platform_get_drvdata(pdev);
	pr_info("%s\n", __func__);
	if (!mfd) {
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
		return -ENODEV;
	}
	if (mfd->key != MFD_KEY) {
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
		return -EINVAL;
	}

	return mipi_renesas_video_lcd_off_exec(mfd, 1);
}

#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
int mipi_renesas_video_lcd_early_off_exec(struct msm_fb_data_type *mfd)
{
    DISP_LOCAL_LOG_EMERG("DISP %s S\n",__func__);

	pr_info("checkpoint: %s: start\n", __func__);
	if(disp_ext_board_get_panel_detect() == -1){
		pr_err("%s:panel not found\n", __func__);
		return 0;
	}

#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	if (disp_ext_util_get_disp_state() != LOCAL_DISPLAY_ON) {
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_disp_local_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		DISP_LOCAL_LOG_EMERG("DISP %s disp_state(%d) E\n",__func__,disp_ext_util_get_disp_state());
		return 0;
	}

	mipi_set_tx_power_mode(0);
	mipi_renesas_video_cmds_tx( &renesas_video_tx_buf,
					renesas_cm_display_off_th_cmds,
					ARRAY_SIZE(renesas_cm_display_off_th_cmds));

	pr_info("checkpoint: %s: end\n", __func__);
	lcd_early_off_complete = 1;

    DISP_LOCAL_LOG_EMERG("DISP %s E\n",__func__);
	return 0;
}

static int mipi_renesas_video_lcd_early_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

    DISP_LOCAL_LOG_EMERG("DISP %s\n",__func__);

	lcd_early_off_complete = 0;

	mfd = platform_get_drvdata(pdev);
	pr_info("%s\n", __func__);
	if (!mfd) {
		return -ENODEV;
	}
	if (mfd->key != MFD_KEY) {
		return -EINVAL;
	}

	return mipi_renesas_video_lcd_early_off_exec(mfd);
}
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */

static void mipi_renesas_video_shutdown(struct platform_device *pdev)
{
    DISP_LOCAL_LOG_EMERG("DISP %s S\n",__func__);

#ifdef CONFIG_DISP_EXT_BLC
	(void)light_led_cabc_set(LIGHT_MAIN_WLED_CABC_OFF);
#endif /* CONFIG_DISP_EXT_BLC */

#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_mipitx_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	if (disp_ext_util_get_disp_state() == LOCAL_DISPLAY_POWER_OFF) {
		DISP_LOCAL_LOG_EMERG("%s Display shutdown Now\n", __func__);
#ifdef CONFIG_DISP_EXT_UTIL
		disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
		return;
	}

	if( mipi_renesas_video_mfd != NULL ) {
		if( disp_ext_util_get_disp_state() == LOCAL_DISPLAY_ON){
			DISP_LOCAL_LOG_EMERG("%s Display deepstandby \n", __func__);
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
			mipi_renesas_video_lcd_early_off_exec(mipi_renesas_video_mfd);
#endif /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
			mipi_renesas_video_lcd_off_exec(mipi_renesas_video_mfd, 0);
		}
	}

#ifdef CONFIG_DISP_UTIL_DSI_CLK_OFF
	disp_ext_util_dsi_clk_off();
#endif /*CONFIG_DISP_UTIL_DSI_CLK_OFF*/

	DISP_LOCAL_LOG_EMERG("%s Display power off \n", __func__);
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_lock();
#endif /* CONFIG_DISP_EXT_UTIL */
	mipi_renesas_video_panel_power( 0 );
#ifdef CONFIG_DISP_EXT_UTIL
	disp_ext_util_disp_local_unlock();
	disp_ext_util_mipitx_unlock();
#endif /* CONFIG_DISP_EXT_UTIL */
	disp_ext_util_set_disp_state(LOCAL_DISPLAY_POWER_OFF);
    DISP_LOCAL_LOG_EMERG("DISP %s E\n",__func__);
}

static void mipi_renesas_video_set_backlight(struct msm_fb_data_type *mfd)
{
	pr_debug("No Support %s: back light level %d\n", __func__, mfd->bl_level);
}

static int __devinit mipi_renesas_video_lcd_probe(struct platform_device *pdev)
{
    DISP_LOCAL_LOG_EMERG("DISP %s \n",__func__);

	if (pdev->id == 0) {
		mipi_renesas_video_pdata = pdev->dev.platform_data;
		return 0;
	}

	if (mipi_renesas_video_pdata == NULL) {
		pr_err("%s.invalid platform data.\n", __func__);
		return -ENODEV;
	}

	mipi_renesas_video_panel_power(1);
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_renesas_video_lcd_probe,
	.shutdown = mipi_renesas_video_shutdown,
	.driver = {
		.name   = "mipi_renesas_video",
	},
};

static struct msm_fb_panel_data renesas_video_panel_data = {
	.on		= mipi_renesas_video_lcd_on,
	.off		= mipi_renesas_video_lcd_off,
#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
	.early_off	= mipi_renesas_video_lcd_early_off,
#endif  /* CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT */
	.set_backlight  = mipi_renesas_video_set_backlight,
#ifdef CONFIG_DISP_EXT_UTIL
#ifdef CONFIG_DISP_EXT_PROPERTY
	.set_nv		= disp_ext_util_set_kcjprop,
#endif /* CONFIG_DISP_EXT_PROPERTY */
#endif /* CONFIG_DISP_EXT_UTIL */
#ifdef CONFIG_DISP_EXT_REFRESH
	.refresh	= disp_ext_refresh_seq,
#endif /* CONFIG_DISP_EXT_REFRESH */
};

static int ch_used[3];
int mipi_renesas_video_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel]) {
		return -ENODEV;
	}

	ch_used[channel] = TRUE;

	ret = mipi_renesas_video_lcd_init();
	if (ret) {
		pr_err("mipi_renesas_video_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_renesas_video", (panel << 8)|channel);
	if (!pdev) {
		return -ENOMEM;
	}

	renesas_video_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &renesas_video_panel_data,
		sizeof(renesas_video_panel_data));
	if (ret) {
		pr_err("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_err("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

#ifdef CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT
#else
#ifdef CONFIG_DISP_EXT_REFRESH
	disp_ext_refresh_set_te_monitor_init();
#endif /* CONFIG_DISP_EXT_REFRESH */
#endif /*CONFIG_FB_MSM_MIPI_RENESAS_VIDEO_720P_PT*/
	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int mipi_renesas_video_lcd_init(void)
{
	mipi_dsi_buf_alloc(&renesas_video_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&renesas_video_rx_buf, DSI_BUF_SIZE);
#ifdef CONFIG_DISP_EXT_REFRESH
	disp_ext_reflesh_init();
#endif /* CONFIG_DISP_EXT_REFRESH */
#ifdef CONFIG_DISP_EXT_BLC
	disp_ext_blc_init();
#endif /* CONFIG_DISP_EXT_BLC */
	mipi_renesas_video_mfd = NULL;

	return platform_driver_register(&this_driver);
}

struct msm_fb_data_type *mipi_renesas_video_get_mfd(void)
{
	return mipi_renesas_video_mfd;
}

void mipi_renesas_video_gamma_Ap_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_a_positive[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
void mipi_renesas_video_gamma_Am_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_a_negative[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
void mipi_renesas_video_gamma_Bp_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_b_positive[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
void mipi_renesas_video_gamma_Bm_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_b_negative[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
void mipi_renesas_video_gamma_Cp_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_c_positive[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
void mipi_renesas_video_gamma_Cm_set( uint8_t *gamma_table )
{
	memcpy( &config_gamma_control_set_c_negative[1], gamma_table,  MSMFB_GAMMA_KCJPROP_DATA_NUM );
}
