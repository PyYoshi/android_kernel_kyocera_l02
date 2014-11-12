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
 * (C) 2012 KYOCERA Corporation
 * (C) 2013 KYOCERA Corporation
 */

#include <linux/mfd/pm8xxx/batterydata-lib.h>

#ifdef QUALCOMM_ORIGINAL_FEATURE
static struct single_row_lut fcc_temp = {
	.x		= {-20, 0, 25, 40, 65},
	.y		= {1492, 1492, 1493, 1483, 1502},
	.cols	= 5
};
#else
static struct single_row_lut fcc_temp = {
	.x	= {-20, -10, 0, 10, 20, 35, 45, 60},
	.y	= {1707, 2218, 2321, 2335, 2650, 2663, 2424, 2393},
	.cols	= 8,
};
#endif

static struct single_row_lut palladium_1500_fcc_sf = {
	.x	= {0, 100, 200, 300, 400, 500},
	.y	= {100, 97, 91, 87, 84, 79},
	.cols	= 6,
};

static struct sf_lut palladium_1500_pc_sf = {
	.rows		= 10,
	.cols		= 6,
	/* row_entries are chargecycles */
	.row_entries	= {0, 100, 200, 400, 600, 800},
	.percent	= {100, 90, 80, 70, 60, 50, 40, 30, 20, 10},
	.sf		= {
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73},
			{100, 92, 89, 84, 78, 73}
	},
};

#ifdef QUALCOMM_ORIGINAL_FEATURE
static struct pc_temp_ocv_lut pc_temp_ocv = {
	.rows		= 29,
	.cols		= 5,
	.temp		= {-20, 0, 25, 40, 65},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40,
					35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5,
					4, 3, 2, 1, 0},
	.ocv		= {
				{4173, 4167, 4163, 4156, 4154},
				{4104, 4107, 4108, 4102, 4104},
				{4057, 4072, 4069, 4061, 4060},
				{3973, 4009, 4019, 4016, 4020},
				{3932, 3959, 3981, 3982, 3983},
				{3899, 3928, 3954, 3950, 3950},
				{3868, 3895, 3925, 3921, 3920},
				{3837, 3866, 3898, 3894, 3892},
				{3812, 3841, 3853, 3856, 3862},
				{3794, 3818, 3825, 3823, 3822},
				{3780, 3799, 3804, 3804, 3803},
				{3768, 3787, 3790, 3788, 3788},
				{3757, 3779, 3778, 3775, 3776},
				{3747, 3772, 3771, 3766, 3765},
				{3736, 3763, 3766, 3760, 3746},
				{3725, 3749, 3756, 3747, 3729},
				{3714, 3718, 3734, 3724, 3706},
				{3701, 3703, 3696, 3689, 3668},
				{3675, 3695, 3682, 3675, 3662},
				{3670, 3691, 3680, 3673, 3661},
				{3661, 3686, 3679, 3672, 3656},
				{3649, 3680, 3676, 3669, 3641},
				{3633, 3669, 3667, 3655, 3606},
				{3610, 3647, 3640, 3620, 3560},
				{3580, 3607, 3596, 3572, 3501},
				{3533, 3548, 3537, 3512, 3425},
				{3457, 3468, 3459, 3429, 3324},
				{3328, 3348, 3340, 3297, 3172},
				{3000, 3000, 3000, 3000, 3000}
	}
};
#else
static struct pc_temp_ocv_lut pc_temp_ocv = {
	.rows		= 29,
	.cols		= 8,
	.temp		= {-20, -10, 0, 10, 20, 35, 45, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55,
				50, 45, 40, 35, 30, 25, 20, 15, 10, 9,
				8, 7, 6, 5, 4, 3, 2, 1, 0
	},
	.ocv		= {
			{4105, 4145, 4167, 4176, 4297, 4306, 4211, 4206},
			{3864, 4033, 4091, 4112, 4232, 4244, 4156, 4153},
			{3796, 3975, 4045, 4066, 4178, 4190, 4108, 4107},
			{3745, 3917, 4003, 4034, 4126, 4137, 4064, 4062},
			{3704, 3870, 3940, 3967, 4076, 4086, 4024, 4020},
			{3669, 3828, 3895, 3927, 4038, 4043, 3980, 3981},
			{3638, 3793, 3860, 3895, 3968, 3984, 3946, 3945},
			{3611, 3763, 3831, 3856, 3932, 3952, 3902, 3901},
			{3586, 3740, 3805, 3828, 3887, 3905, 3859, 3861},
			{3560, 3720, 3784, 3807, 3849, 3859, 3833, 3835},
			{3536, 3705, 3766, 3789, 3823, 3831, 3812, 3814},
			{3512, 3690, 3751, 3774, 3801, 3808, 3795, 3795},
			{3489, 3673, 3741, 3761, 3782, 3790, 3780, 3780},
			{3466, 3652, 3732, 3752, 3767, 3774, 3766, 3765},
			{3443, 3627, 3722, 3744, 3756, 3759, 3745, 3738},
			{3420, 3595, 3705, 3732, 3746, 3742, 3729, 3719},
			{3397, 3557, 3678, 3710, 3728, 3723, 3710, 3700},
			{3372, 3512, 3639, 3671, 3694, 3691, 3673, 3663},
			{3347, 3465, 3611, 3659, 3670, 3669, 3663, 3655},
			{3343, 3455, 3605, 3656, 3669, 3668, 3662, 3654},
			{3338, 3444, 3598, 3653, 3667, 3667, 3661, 3653},
			{3333, 3433, 3590, 3649, 3665, 3665, 3659, 3651},
			{3329, 3421, 3579, 3643, 3662, 3662, 3656, 3647},
			{3324, 3408, 3565, 3633, 3656, 3657, 3645, 3634},
			{3319, 3393, 3544, 3610, 3639, 3635, 3614, 3599},
			{3314, 3376, 3513, 3568, 3593, 3589, 3568, 3557},
			{3310, 3356, 3467, 3512, 3533, 3527, 3505, 3496},
			{3305, 3332, 3397, 3427, 3447, 3443, 3431, 3426},
			{3300, 3300, 3300, 3300, 3300, 3300, 3300, 3300}
	},
};
#endif

static struct rbatt_lut oem_rbatt_initial = {
	.rows	= 22,
	.cols	= 5,
	.temp	= {-20, 0, 25, 40, 65},
	.vbatt	= {4350, 4300, 4250, 4200, 4150, 4100, 4050, 4000,
		   3950, 3900, 3850, 3800, 3750, 3700, 3650, 3600,
		   3550, 3500, 3450, 3400, 3350, 3300
	},
	.rbatt	= {
				{608, 290,  97, 81, 75},
				{608, 290,  96, 81, 74},
				{608, 285,  96, 80, 75},
				{608, 275,  96, 81, 75},
				{608, 266,  95, 80, 75},
				{608, 258,  95, 80, 75},
				{608, 253,  95, 80, 75},
				{605, 248,  95, 81, 75},
				{602, 245,  95, 81, 75},
				{597, 243,  95, 80, 76},
				{588, 242,  95, 80, 75},
				{577, 241,  95, 80, 75},
				{568, 241,  96, 81, 76},
				{562, 245,  97, 81, 76},
				{559, 252,  98, 82, 77},
				{561, 259,  99, 83, 78},
				{565, 269, 100, 84, 79},
				{570, 286, 102, 85, 80},
				{573, 315, 105, 85, 80},
				{574, 342, 107, 86, 81},
				{574, 357, 109, 87, 81},
				{574, 365, 112, 88, 82}
	}
};

static struct soc_adjust_lut oem_soc_adjust = {
	.rows		= 6,
	.cols		= 8,
	.temp		= {-30, -20, -10, 0, 10, 25, 40, 60},
	.ibatt		= {1000, 800, 600, 400, 200, 100},
	.soc_adjust	= {
				{15, 10,  6,  0,  0,  0,  0,  0},
				{17, 11,  7,  0,  0,  0,  0,  0},
				{20, 12,  8,  0,  0,  0,  0,  0},
				{23, 13,  9,  1,  0,  0,  0,  0},
				{27, 14, 10,  1,  0,  0,  0,  0},
				{33, 15, 11,  2,  0,  0,  0,  0}
	}
};

static struct single_row_lut oem_cycle_adjust = {
	.x	= {130, 170, 220, 250, 280, 320},
	.y	= {0, 100, 200, 300, 400, 500},
	.cols	= 6,
};

#ifdef QUALCOMM_ORIGINAL_FEATURE
static struct sf_lut rbatt_sf = {
	.rows		= 29,
	.cols		= 5,
	/* row_entries are temperature */
	.row_entries	= {-20, 0, 20, 40, 65},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40,
					35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5,
					4, 3, 2, 1, 0},
	.sf		= {
				{357, 187, 100, 91, 91},
				{400, 208, 105, 94, 94},
				{390, 204, 106, 95, 96},
				{391, 201, 108, 98, 98},
				{391, 202, 110, 98, 100},
				{390, 200, 110, 99, 102},
				{389, 200, 110, 99, 102},
				{393, 202, 101, 93, 100},
				{407, 205, 99, 89, 94},
				{428, 208, 100, 91, 96},
				{455, 212, 102, 92, 98},
				{495, 220, 104, 93, 101},
				{561, 232, 107, 95, 102},
				{634, 245, 112, 98, 98},
				{714, 258, 114, 98, 98},
				{791, 266, 114, 97, 100},
				{871, 289, 108, 95, 97},
				{973, 340, 124, 108, 105},
				{489, 241, 109, 96, 99},
				{511, 246, 110, 96, 99},
				{534, 252, 111, 95, 98},
				{579, 263, 112, 96, 96},
				{636, 276, 111, 95, 97},
				{730, 294, 109, 96, 99},
				{868, 328, 112, 98, 104},
				{1089, 374, 119, 101, 115},
				{1559, 457, 128, 105, 213},
				{12886, 1026, 637, 422, 3269},
				{170899, 127211, 98968, 88907, 77102},
	}
};
#else
static struct sf_lut rbatt_sf = {
	.rows		= 29,
	.cols		= 5,
	/* row_entries are temperature */
	.row_entries	= {-20, 0, 25, 40, 65},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40,
					35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5,
					4, 3, 2, 1, 0},
	.sf		= {
				{436, 233, 100, 84, 80},
				{436, 233, 100, 84, 80},
				{435, 229, 100, 85, 80},
				{432, 224,  98, 83, 80},
				{431, 223,  98, 84, 80},
				{428, 224,  98, 83, 80},
				{428, 225,  98, 84, 80},
				{427, 228,  98, 83, 80},
				{426, 231,  98, 83, 80},
				{426, 235,  98, 83, 80},
				{426, 240,  98, 83, 80},
				{426, 244,  99, 83, 79},
				{426, 249, 100, 83, 80},
				{427, 251, 102, 84, 80},
				{428, 254, 102, 83, 80},
				{428, 256, 103, 84, 80},
				{428, 257, 104, 83, 80},
				{429, 261, 106, 84, 81},
				{431, 262, 109, 84, 81},
				{430, 263, 109, 84, 80},
				{431, 264, 110, 83, 81},
				{431, 264, 111, 84, 81},
				{431, 264, 112, 83, 80},
				{432, 266, 113, 84, 81},
				{431, 267, 114, 85, 82},
				{431, 268, 116, 85, 82},
				{432, 270, 119, 86, 83},
				{433, 272, 123, 87, 84},
				{433, 272, 123, 87, 84},
	}
};
#endif

#ifdef QUALCOMM_ORIGINAL_FEATURE
struct bms_battery_data palladium_1500_data = {
	.fcc			= 1500,
	.fcc_temp_lut		= &fcc_temp,
	.pc_temp_ocv_lut	= &pc_temp_ocv,
	.rbatt_sf_lut		= &rbatt_sf,
	.default_rbatt_mohm	= 236,
	.rbatt_capacitive_mohm	= 50,
};
#else
struct bms_battery_data palladium_1500_data = {
	.fcc			= 2700,
	.fcc_temp_lut		= &fcc_temp,
	.pc_temp_ocv_lut	= &pc_temp_ocv,
	.rbatt_sf_lut		= &rbatt_sf,
	.default_rbatt_mohm	= 127,
	.fcc_sf_lut		= &palladium_1500_fcc_sf,
	.pc_sf_lut		= &palladium_1500_pc_sf,
	.delta_rbatt_mohm	= 60,
};
#endif

struct pm8921_bms_oem_battery_data pm8921_bms_oem_data = {
	.rbatt_initial_lut	= &oem_rbatt_initial,
	.soc_adjust_lut		= &oem_soc_adjust,
	.cycle_adjust_lut	= &oem_cycle_adjust,
};

