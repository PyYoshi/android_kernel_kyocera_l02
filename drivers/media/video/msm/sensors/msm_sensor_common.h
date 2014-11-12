/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2013 KYOCERA Corporation
 */

#ifndef MSM_SENSOR_COMMON_H
#define MSM_SENSOR_COMMON_H

#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <mach/camera.h>
#include <media/msm_camera.h>
#include <media/v4l2-subdev.h>
#include "msm_camera_i2c.h"
#include "msm_camera_eeprom.h"
#define Q8  0x00000100
#define Q10 0x00000400

#define MSM_SENSOR_MCLK_8HZ 8000000
#define MSM_SENSOR_MCLK_16HZ 16000000
#define MSM_SENSOR_MCLK_24HZ 24000000

#define DEFINE_MSM_MUTEX(mutexname) \
	static struct mutex mutexname = __MUTEX_INITIALIZER(mutexname)

struct gpio_tlmm_cfg {
	uint32_t gpio;
	uint32_t dir;
	uint32_t pull;
	uint32_t drvstr;
};

enum msm_sensor_reg_update {
	/* Sensor egisters that need to be updated during initialization */
	MSM_SENSOR_REG_INIT,
	/* Sensor egisters that needs periodic I2C writes */
	MSM_SENSOR_UPDATE_PERIODIC,
	/* All the sensor Registers will be updated */
	MSM_SENSOR_UPDATE_ALL,
	/* Not valid update */
	MSM_SENSOR_UPDATE_INVALID
};

enum msm_sensor_cam_mode_t {
	MSM_SENSOR_MODE_2D_RIGHT,
	MSM_SENSOR_MODE_2D_LEFT,
	MSM_SENSOR_MODE_3D,
	MSM_SENSOR_MODE_INVALID
};

enum msm_camera_power_config_t {
	REQUEST_GPIO,
	ENABLE_GPIO,
	REQUEST_VREG,
	ENABLE_VREG,
	CONFIG_CLK,
	CONFIG_EXT_POWER_CTRL,
	CONFIG_I2C_MUX,
};

struct msm_camera_power_seq_t {
	enum msm_camera_power_config_t power_config;
	uint32_t delay;
};

struct msm_sensor_id_info_t {
	uint16_t sensor_id_reg_addr;
	uint16_t sensor_id;
};

struct msm_sensor_reg_t {
	enum msm_camera_i2c_data_type default_data_type;
	struct msm_camera_i2c_reg_conf *start_stream_conf;
	uint8_t start_stream_conf_size;
	struct msm_camera_i2c_reg_conf *stop_stream_conf;
	uint8_t stop_stream_conf_size;
	struct msm_camera_i2c_reg_conf *group_hold_on_conf;
	uint8_t group_hold_on_conf_size;
	struct msm_camera_i2c_reg_conf *group_hold_off_conf;
	uint8_t group_hold_off_conf_size;
	struct msm_camera_i2c_conf_array *init_settings;
	uint8_t init_size;
	struct msm_camera_i2c_conf_array *mode_settings;
	struct msm_camera_i2c_conf_array *no_effect_settings;
	struct msm_sensor_output_info_t *output_settings;
	uint8_t num_conf;
	struct msm_camera_i2c_reg_conf *start_autofocus_conf;
	uint8_t start_autofocus_conf_size;
	uint32_t focus_mode;
	uint8_t af_start;
};

enum msm_sensor_device_type_t {
	MSM_SENSOR_I2C_DEVICE,
	MSM_SENSOR_PLATFORM_DEVICE,
};

struct v4l2_subdev_info {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	uint16_t fmt;
	uint16_t order;
};

struct msm_sensor_ctrl_t;

struct msm_sensor_v4l2_ctrl_info_t {
	uint32_t ctrl_id;
	int16_t min;
	int16_t max;
	int16_t step;
	struct msm_camera_i2c_enum_conf_array *enum_cfg_settings;
	int (*s_v4l2_ctrl) (struct msm_sensor_ctrl_t *,
		struct msm_sensor_v4l2_ctrl_info_t *, int);
};

struct msm_sensor_fn_t {
	void (*sensor_start_stream) (struct msm_sensor_ctrl_t *);
	void (*sensor_stop_stream) (struct msm_sensor_ctrl_t *);
	void (*sensor_group_hold_on) (struct msm_sensor_ctrl_t *);
	void (*sensor_group_hold_off) (struct msm_sensor_ctrl_t *);

	int32_t (*sensor_set_fps) (struct msm_sensor_ctrl_t *,
			struct fps_cfg *);
	int32_t (*sensor_write_exp_gain) (struct msm_sensor_ctrl_t *,
			uint16_t, uint32_t, int32_t, uint16_t);
	int32_t (*sensor_write_snapshot_exp_gain) (struct msm_sensor_ctrl_t *,
			uint16_t, uint32_t, int32_t, uint16_t);
	int32_t (*sensor_setting) (struct msm_sensor_ctrl_t *,
			int update_type, int rt);
	int32_t (*sensor_csi_setting) (struct msm_sensor_ctrl_t *,
			int update_type, int rt);
	int32_t (*sensor_set_sensor_mode)
			(struct msm_sensor_ctrl_t *, int, int);
	int32_t (*sensor_mode_init) (struct msm_sensor_ctrl_t *,
		int, struct sensor_init_cfg *);
	int32_t (*sensor_get_output_info) (struct msm_sensor_ctrl_t *,
		struct sensor_output_info_t *);
	int (*sensor_config) (struct msm_sensor_ctrl_t *, void __user *);
	int (*sensor_power_down)
		(struct msm_sensor_ctrl_t *);
	int (*sensor_power_up) (struct msm_sensor_ctrl_t *);
	int32_t (*sensor_match_id)(struct msm_sensor_ctrl_t *s_ctrl);
	void (*sensor_adjust_frame_lines) (struct msm_sensor_ctrl_t *s_ctrl);
	int32_t (*sensor_get_csi_params)(struct msm_sensor_ctrl_t *,
		struct csi_lane_params_t *);
	int (*sensor_set_vision_mode)(struct msm_sensor_ctrl_t *s_ctrl,
			int32_t vision_mode_enable);
	int (*sensor_set_vision_ae_control)(
			struct msm_sensor_ctrl_t *s_ctrl, int ae_mode);
	int32_t (*sensor_read_eeprom)(struct msm_sensor_ctrl_t *);
	int32_t (*sensor_hdr_update)(struct msm_sensor_ctrl_t *,
		 struct sensor_hdr_update_parm_t *);
/*	int32_t (*sensor_set_auto_focus) (struct msm_sensor_ctrl_t *);*/
	int32_t (*sensor_set_auto_focus) (struct msm_sensor_ctrl_t *, 
		enum msm_v4l2_output_mode_enum_type);
	int32_t (*sensor_set_note_takepic) (struct msm_sensor_ctrl_t *);
	int32_t (*sensor_get_exposure_time) (struct msm_sensor_ctrl_t *, uint32_t *);
	int32_t (*sensor_get_lx_data) (struct msm_sensor_ctrl_t *);
	int32_t (*sensor_get_low_light_info) (struct msm_sensor_ctrl_t *, int32_t *);
	int32_t (*sensor_set_white_balance) (struct msm_sensor_ctrl_t *, int8_t);
	int32_t (*sensor_set_exposure_compensation) (struct msm_sensor_ctrl_t *, int8_t);
	int32_t (*sensor_set_bestshot) (struct msm_sensor_ctrl_t *, int8_t);
	void (*sensor_get_frame_skip_flg) (struct msm_sensor_ctrl_t *, uint8_t *);
	int32_t (*sensor_set_low_light) (struct msm_sensor_ctrl_t *);
	void (*sensor_init) (struct msm_sensor_ctrl_t *);
	void (*sensor_cancel_auto_focus) (struct msm_sensor_ctrl_t *);
	int32_t (*sensor_set_focus_mode) (struct msm_sensor_ctrl_t *, int32_t);
//	int32_t (*sensor_set_caf) (struct msm_sensor_ctrl_t *, uint8_t);
	int32_t (*sensor_set_caf) (struct msm_sensor_ctrl_t *, struct sensor_caf_info_t);
	void (*sensor_set_touch_focus) (struct msm_sensor_ctrl_t *, struct sensor_touch_info_t);
	int32_t (*sensor_set_aec_lock) (struct msm_sensor_ctrl_t *, uint8_t);
	int32_t (*sensor_set_awb_lock) (struct msm_sensor_ctrl_t *, uint8_t);
	int32_t (*sensor_get_lens_position) (struct msm_sensor_ctrl_t *, uint8_t *);
	int32_t (*sensor_capture_preset) (struct msm_sensor_ctrl_t *, uint16_t, uint16_t);
	int32_t (*sensor_capture_postset) (struct msm_sensor_ctrl_t *);
	int32_t (*sensor_get_is_flash) (struct msm_sensor_ctrl_t *, uint8_t *);
	int32_t (*sensor_auto_bracket) (struct msm_sensor_ctrl_t *, int);
	int32_t (*sensor_get_bracket_exposure_time) (struct msm_sensor_ctrl_t *, uint32_t *);
	int32_t (*sensor_set_flicker_detect) (struct msm_sensor_ctrl_t * , int);
	int32_t (*sensor_get_isp_settings) (struct msm_sensor_ctrl_t * , struct isp_settings_t *);
	int32_t (*sensor_set_flicker_lib) (struct msm_sensor_ctrl_t * , struct set_flicker_data_t *);
	int32_t (*sensor_get_flicker_lib) (struct msm_sensor_ctrl_t * , struct get_flicker_data_t *);
	void (*sensor_scheduler_destroy) (struct msm_sensor_ctrl_t *);
};

struct msm_sensor_csi_info {
	uint8_t is_csic;
};

enum msm_sensor_state {
	MSM_SENSOR_POWER_UP,
	MSM_SENSOR_POWER_DOWN,
};

struct msm_sensor_eeprom_data {
	uint8_t *data;
	uint32_t length;
};

struct msm_sensor_ctrl_t {
	struct  msm_camera_sensor_info *sensordata;
	struct i2c_client *msm_sensor_client;
	struct i2c_driver *sensor_i2c_driver;
	struct platform_device *pdev;
	struct msm_camera_i2c_client *sensor_i2c_client;
	uint16_t sensor_i2c_addr;
	enum msm_camera_vreg_name_t *vreg_seq;
	int num_vreg_seq;
	struct msm_camera_power_seq_t *power_seq;
	int num_power_seq;
	enum msm_sensor_device_type_t sensor_device_type;

	struct msm_sensor_output_reg_addr_t *sensor_output_reg_addr;
	struct msm_sensor_id_info_t *sensor_id_info;
	struct msm_sensor_exp_gain_info_t *sensor_exp_gain_info;
	struct msm_sensor_reg_t *msm_sensor_reg;
	struct msm_sensor_v4l2_ctrl_info_t *msm_sensor_v4l2_ctrl_info;
	uint16_t num_v4l2_ctrl;
	uint8_t is_csic;
	int8_t vision_mode_flag;

	uint16_t curr_line_length_pclk;
	uint16_t curr_frame_length_lines;

	uint32_t fps_divider;
	enum msm_sensor_resolution_t curr_res;
	enum msm_sensor_cam_mode_t cam_mode;

	struct mutex *msm_sensor_mutex;

	struct v4l2_subdev sensor_v4l2_subdev;
	struct v4l2_subdev_info *sensor_v4l2_subdev_info;
	uint8_t sensor_v4l2_subdev_info_size;
	struct v4l2_subdev_ops *sensor_v4l2_subdev_ops;
	struct msm_sensor_fn_t *func_tbl;
	struct regulator **reg_ptr;
	struct clk *cam_clk[2];
	long clk_rate;
	enum msm_sensor_state sensor_state;
	/* Number of frames to delay after start / stop stream in Q10 format.
	   Initialize to -1 for this value to be ignored */
	int16_t wait_num_frames;
	/* minimum delay after stop / stop stream in ms */
	uint16_t min_delay;
	/* delay (in ms) after power up sequence */
	uint16_t power_seq_delay;
	struct msm_sensor_eeprom_data eeprom_data;
	struct delayed_work  work;
};

struct msm_sensor_ctrl_t *get_sctrl(struct v4l2_subdev *sd);

#endif
