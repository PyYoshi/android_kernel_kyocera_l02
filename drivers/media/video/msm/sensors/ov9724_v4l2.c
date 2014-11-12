/* Copyright (c) 2013, The Linux Foundation. All Rights Reserved.
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
 */
/*
 * This software is contributed or developed by KYOCERA Corporation.
 * (C) 2013 KYOCERA Corporation
 */

#include "msm_sensor.h"
#define SENSOR_NAME "ov9724"
#define PLATFORM_DRIVER_NAME "msm_camera_ov9724"
#define ov9724_obj ov9724_##obj

DEFINE_MUTEX(ov9724_mut);
static struct msm_sensor_ctrl_t ov9724_s_ctrl;

static uint32_t ov9724_g_coase_integration_time = 0;
static uint32_t ov9724_g_coase_integration_time_store = 0;

extern int32_t msm_sensor_enable_i2c_mux(struct msm_camera_i2c_conf *i2c_conf);
extern int32_t msm_sensor_disable_i2c_mux(struct msm_camera_i2c_conf *i2c_conf);

static struct msm_camera_i2c_reg_conf ov9724_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf ov9724_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf ov9724_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf ov9724_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf ov9724_prev_settings[] = {
//ZANTEI
// For the measure against an error, 
// write to the appropriate register.
	{0x0601, 0x00},
};

static struct msm_camera_i2c_reg_conf ov9724_recommend_settings[] = {
    {0x0103,0x01},
    {0x3210,0x43},
    {0x3606,0x75},
    {0x3705,0x67},
    {0x0340,0x02},
    {0x0341,0xf8},
    {0x0342,0x06},
    {0x0343,0x28},
    {0x0202,0x02},
    {0x0203,0xf0},
    {0x4800,0x64}, //[5] enable clock gate
    {0x4801,0x0f},
    {0x4801,0x8f},
    {0x4814,0x2b},
    {0x4307,0x3a},
    {0x5000,0x06},
    {0x5001,0x73},
    {0x0205,0x3f},
//    {0x0101,0x01},
    {0x0101,0x02},
    {0x0100,0x01},
    {0x0205,0x38},
};

static struct v4l2_subdev_info ov9724_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array ov9724_init_conf[] = {
	{&ov9724_recommend_settings[0],
		ARRAY_SIZE(ov9724_recommend_settings), 0,
		MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array ov9724_confs[] = {
	{&ov9724_prev_settings[0],
		ARRAY_SIZE(ov9724_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t ov9724_dimensions[] = {
	{
		.x_output = 0x500, /* 1280 */
		.y_output = 0x2D0, /* 720 */
		.line_length_pclk = 0x628, /* 1576 */
		.frame_length_lines = 0x2F8, /* 760 */
//		.vt_pixel_clk = 35932800,
		.vt_pixel_clk = 36000000,
//		.op_pixel_clk = 184500000,
//		.op_pixel_clk = 36000000,
		.op_pixel_clk = 360000000,
		.binning_factor = 1,
	},
};

static struct msm_sensor_output_reg_addr_t ov9724_reg_addr = {
	.x_output = 0x034c,
	.y_output = 0x034e,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_id_info_t ov9724_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x9724,
};

static struct msm_sensor_exp_gain_info_t ov9724_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0204,//?? ov9724 gain only have register R205 , ov9724 is R204/R205
	.vert_offset = 6,
};

static const struct i2c_device_id ov9724_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov9724_s_ctrl},
	{ }
};

static struct i2c_driver ov9724_i2c_driver = {
	.id_table = ov9724_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov9724_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


/* for debug */
#if 0
#undef CDBG
#define CDBG pr_err
#endif

static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

int32_t msm_sensor_ov9724_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	
	CDBG("%s: %d\n", __func__, __LINE__);

	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* data->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}

	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}

	rc = msm_camera_config_vreg(&s_ctrl->sensor_i2c_client->client->dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: regulator on failed\n", __func__);
		goto config_vreg_failed;
	}

	rc = msm_camera_enable_vreg(&s_ctrl->sensor_i2c_client->client->dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: enable regulator failed\n", __func__);
		goto enable_vreg_failed;
	}

	rc = msm_camera_config_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: config gpio failed\n", __func__);
		goto config_gpio_failed;
	}

	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0) {
		pr_err("%s: clk enable failed\n", __func__);
		goto enable_clk_failed;
	}

	usleep_range(1000, 2000);

	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(1);

	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_enable_i2c_mux(data->sensor_platform_info->i2c_conf);

	CDBG("%s: %d\n", __func__, __LINE__);
	return rc;

enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
config_gpio_failed:
	msm_camera_enable_vreg(&s_ctrl->sensor_i2c_client->client->dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 0);

enable_vreg_failed:
	msm_camera_config_vreg(&s_ctrl->sensor_i2c_client->client->dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
config_vreg_failed:
	msm_camera_request_gpio_table(data, 0);
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
	return rc;
}

int32_t msm_sensor_ov9724_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CDBG("%s\n", __func__);
	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_disable_i2c_mux(
			data->sensor_platform_info->i2c_conf);

	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(0);
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);
	msm_camera_config_gpio_table(data, 0);
	/* 1ms wait */
	usleep_range(1000, 1000);
	msm_camera_enable_vreg(&s_ctrl->sensor_i2c_client->client->dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
	msm_camera_config_vreg(&s_ctrl->sensor_i2c_client->client->dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
	gpio_set_value_cansleep(55, 0);
	msm_camera_request_gpio_table(data, 0);
	kfree(s_ctrl->reg_ptr);
	return 0;
}

static int32_t msm_sensor_ov9724_set_note_takepic(struct msm_sensor_ctrl_t *s_ctrl)
{
    ov9724_g_coase_integration_time_store = ov9724_g_coase_integration_time;
    return 0;
}

static int32_t msm_sensor_ov9724_write_exp_gain1(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line, int32_t luma_avg, uint16_t fgain)
{
	uint32_t fl_lines;
	uint8_t offset;
	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;
	fl_lines += (fl_lines & 0x01);

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	ov9724_g_coase_integration_time = line;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

static int32_t msm_sensor_ov9724_get_exp_time(struct msm_sensor_ctrl_t *s_ctrl, uint32_t *exposure_time)
{
	*exposure_time = ov9724_g_coase_integration_time_store;
	return 0;
}

static int32_t msm_sensor_ov9724_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
    /* Do nothing */
    return 0;
}

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&ov9724_i2c_driver);
}

static struct v4l2_subdev_core_ops ov9724_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov9724_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov9724_subdev_ops = {
	.core = &ov9724_subdev_core_ops,
	.video  = &ov9724_subdev_video_ops,
};

static struct msm_sensor_fn_t ov9724_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = msm_sensor_ov9724_write_exp_gain1,
	.sensor_write_snapshot_exp_gain = msm_sensor_ov9724_write_exp_gain1,
//	.sensor_write_exp_gain = msm_sensor_write_exp_gain1,
//	.sensor_write_snapshot_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_setting = msm_sensor_setting,
	.sensor_csi_setting = msm_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_ov9724_power_up,
	.sensor_power_down = msm_sensor_ov9724_power_down,
	.sensor_match_id =  msm_sensor_ov9724_match_id,
	.sensor_adjust_frame_lines = msm_sensor_adjust_frame_lines1,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_get_exposure_time = msm_sensor_ov9724_get_exp_time,
	.sensor_set_note_takepic = msm_sensor_ov9724_set_note_takepic,
};

static struct msm_sensor_reg_t ov9724_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov9724_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov9724_start_settings),
	.stop_stream_conf = ov9724_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov9724_stop_settings),
	.group_hold_on_conf = ov9724_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(ov9724_groupon_settings),
	.group_hold_off_conf = ov9724_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(ov9724_groupoff_settings),
	.init_settings = &ov9724_init_conf[0],
	.init_size = ARRAY_SIZE(ov9724_init_conf),
	.mode_settings = &ov9724_confs[0],
	.output_settings = &ov9724_dimensions[0],
	.num_conf = ARRAY_SIZE(ov9724_confs),
};

static struct msm_sensor_ctrl_t ov9724_s_ctrl = {
	.msm_sensor_reg = &ov9724_regs,
	.sensor_i2c_client = &ov9724_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.vreg_seq = NULL,
	.num_vreg_seq = 0,
	.sensor_output_reg_addr = &ov9724_reg_addr,
	.sensor_id_info = &ov9724_id_info,
	.sensor_exp_gain_info = &ov9724_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &ov9724_mut,
	.sensor_i2c_driver = &ov9724_i2c_driver,
	.sensor_v4l2_subdev_info = ov9724_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov9724_subdev_info),
	.sensor_v4l2_subdev_ops = &ov9724_subdev_ops,
	.func_tbl = &ov9724_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Omnivision WXGA Bayer sensor driver");
MODULE_LICENSE("GPL v2");

