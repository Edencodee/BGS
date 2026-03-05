
/*******************************************************************************
 * Copyright (c) 2020, STMicroelectronics - All Rights Reserved

 This file is part of VL53LX Core and is dual licensed,
 either 'STMicroelectronics
 Proprietary license'
 or 'BSD 3-clause "New" or "Revised" License' , at your option.

********************************************************************************

 'STMicroelectronics Proprietary license'

********************************************************************************

 License terms: STMicroelectronics Proprietary in accordance with licensing
 terms at www.st.com/sla0081

 STMicroelectronics confidential
 Reproduction and Communication of this document is strictly prohibited unless
 specifically authorized in writing by STMicroelectronics.


********************************************************************************

 Alternatively, VL53LX Core may be distributed under the terms of
 'BSD 3-clause "New" or "Revised" License', in which case the following
 provisions apply instead of the ones
 mentioned above :

********************************************************************************

 License terms: BSD 3-clause "New" or "Revised" License.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


********************************************************************************

*/




#ifndef _VL53LX_API_CORE_H_
#define _VL53LX_API_CORE_H_

#include "vl53lx_platform.h"

#ifdef __cplusplus
extern "C" {
#endif




VL53LX_Error VL53LX_get_version(
	VL53LX_DEV            Dev,
	VL53LX_ll_version_t  *pversion);





VL53LX_Error VL53LX_data_init(
	VL53LX_DEV         Dev,
	uint8_t            read_p2p_data);




VL53LX_Error VL53LX_read_p2p_data(
	VL53LX_DEV      Dev);




VL53LX_Error VL53LX_set_part_to_part_data(
	VL53LX_DEV                            Dev,
	VL53LX_calibration_data_t            *pcal_data);




VL53LX_Error VL53LX_get_part_to_part_data(
	VL53LX_DEV                            Dev,
	VL53LX_calibration_data_t            *pcal_data);




VL53LX_Error VL53LX_get_tuning_debug_data(
	VL53LX_DEV                            Dev,
	VL53LX_tuning_parameters_t            *ptun_data);




VL53LX_Error VL53LX_set_inter_measurement_period_ms(
	VL53LX_DEV          Dev,
	uint32_t            inter_measurement_period_ms);




VL53LX_Error VL53LX_get_inter_measurement_period_ms(
	VL53LX_DEV          Dev,
	uint32_t           *pinter_measurement_period_ms);




VL53LX_Error VL53LX_set_timeouts_us(
	VL53LX_DEV          Dev,
	uint32_t            phasecal_config_timeout_us,
	uint32_t            mm_config_timeout_us,
	uint32_t            range_config_timeout_us);




VL53LX_Error VL53LX_get_timeouts_us(
	VL53LX_DEV          Dev,
	uint32_t           *pphasecal_config_timeout_us,
	uint32_t           *pmm_config_timeout_us,
	uint32_t           *prange_config_timeout_us);




VL53LX_Error VL53LX_set_user_zone(
	VL53LX_DEV          Dev,
	VL53LX_user_zone_t *puser_zone);




VL53LX_Error VL53LX_get_user_zone(
	VL53LX_DEV          Dev,
	VL53LX_user_zone_t *puser_zone);




VL53LX_Error VL53LX_get_mode_mitigation_roi(
	VL53LX_DEV          Dev,
	VL53LX_user_zone_t *pmm_roi);




VL53LX_Error VL53LX_set_preset_mode(
	VL53LX_DEV                   Dev,
	VL53LX_DevicePresetModes     device_preset_mode,
	uint16_t                     dss_config__target_total_rate_mcps,
	uint32_t                     phasecal_config_timeout_us,
	uint32_t                     mm_config_timeout_us,
	uint32_t                     range_config_timeout_us,
	uint32_t                     inter_measurement_period_ms);




VL53LX_Error VL53LX_get_preset_mode_timing_cfg(
	VL53LX_DEV                   Dev,
	VL53LX_DevicePresetModes     device_preset_mode,
	uint16_t                    *pdss_config__target_total_rate_mcps,
	uint32_t                    *pphasecal_config_timeout_us,
	uint32_t                    *pmm_config_timeout_us,
	uint32_t                    *prange_config_timeout_us);



VL53LX_Error VL53LX_enable_xtalk_compensation(
	VL53LX_DEV                 Dev);



VL53LX_Error VL53LX_disable_xtalk_compensation(
	VL53LX_DEV                 Dev);




void VL53LX_get_xtalk_compensation_enable(
	VL53LX_DEV    Dev,
	uint8_t       *pcrosstalk_compensation_enable);



VL53LX_Error VL53LX_init_and_start_range(
	VL53LX_DEV                      Dev,
	uint8_t                         measurement_mode,
	VL53LX_DeviceConfigLevel        device_config_level);




VL53LX_Error VL53LX_stop_range(
	VL53LX_DEV  Dev);




VL53LX_Error VL53LX_get_measurement_results(
	VL53LX_DEV                  Dev,
	VL53LX_DeviceResultsLevel   device_result_level);




VL53LX_Error VL53LX_get_device_results(
	VL53LX_DEV                 Dev,
	VL53LX_DeviceResultsLevel  device_result_level,
	VL53LX_range_results_t    *prange_results);




VL53LX_Error VL53LX_clear_interrupt_and_enable_next_range(
	VL53LX_DEV       Dev,
	uint8_t          measurement_mode);




VL53LX_Error VL53LX_get_histogram_bin_data(
	VL53LX_DEV                   Dev,
	VL53LX_histogram_bin_data_t *phist_data);




void VL53LX_copy_sys_and_core_results_to_range_results(
	int32_t                           gain_factor,
	VL53LX_system_results_t          *psys,
	VL53LX_core_results_t            *pcore,
	VL53LX_range_results_t           *presults);



VL53LX_Error VL53LX_set_zone_dss_config(
	  VL53LX_DEV                      Dev,
	  VL53LX_zone_private_dyn_cfg_t  *pzone_dyn_cfg);




VL53LX_Error VL53LX_set_dmax_mode(
	VL53LX_DEV              Dev,
	VL53LX_DeviceDmaxMode   dmax_mode);



VL53LX_Error VL53LX_get_dmax_mode(
	VL53LX_DEV               Dev,
	VL53LX_DeviceDmaxMode   *pdmax_mode);




VL53LX_Error VL53LX_get_dmax_calibration_data(
	VL53LX_DEV                      Dev,
	VL53LX_DeviceDmaxMode           dmax_mode,
	VL53LX_dmax_calibration_data_t *pdmax_cal);




VL53LX_Error VL53LX_set_offset_correction_mode(
	VL53LX_DEV                     Dev,
	VL53LX_OffsetCalibrationMode   offset_cor_mode);




VL53LX_Error VL53LX_get_offset_correction_mode(
	VL53LX_DEV                    Dev,
	VL53LX_OffsetCorrectionMode  *poffset_cor_mode);




VL53LX_Error VL53LX_get_tuning_parm(
	VL53LX_DEV                     Dev,
	VL53LX_TuningParms             tuning_parm_key,
	int32_t                       *ptuning_parm_value);



VL53LX_Error VL53LX_set_tuning_parm(
	VL53LX_DEV                     Dev,
	VL53LX_TuningParms             tuning_parm_key,
	int32_t                        tuning_parm_value);



VL53LX_Error VL53LX_dynamic_xtalk_correction_enable(
	VL53LX_DEV                     Dev
	);



VL53LX_Error VL53LX_dynamic_xtalk_correction_disable(
	VL53LX_DEV                     Dev
	);




VL53LX_Error VL53LX_dynamic_xtalk_correction_apply_enable(
	VL53LX_DEV                          Dev
	);



VL53LX_Error VL53LX_dynamic_xtalk_correction_apply_disable(
	VL53LX_DEV                          Dev
	);



VL53LX_Error VL53LX_dynamic_xtalk_correction_single_apply_enable(
	VL53LX_DEV                          Dev
	);



VL53LX_Error VL53LX_dynamic_xtalk_correction_single_apply_disable(
	VL53LX_DEV                          Dev
	);



VL53LX_Error VL53LX_get_current_xtalk_settings(
	VL53LX_DEV                          Dev,
	VL53LX_xtalk_calibration_results_t *pxtalk
	);



VL53LX_Error VL53LX_set_current_xtalk_settings(
	VL53LX_DEV                          Dev,
	VL53LX_xtalk_calibration_results_t *pxtalk
	);

VL53LX_Error VL53LX_load_patch(VL53LX_DEV Dev);

VL53LX_Error VL53LX_unload_patch(VL53LX_DEV Dev);

#ifdef __cplusplus
}
#endif

#endif

