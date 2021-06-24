/*
 * sfh7779.cpp
 *
 *  Created on: 2021
 *      Author: gean
 */

#include "sfh7779.hpp"


#define return_if_error(ret) if (ret != 0) return ret

/**
* Activate the sensor (begin measurement sampling).  Data samples are
* taken 10 times per second.
*
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::enable() {
    int ret;
    sfh7779_mode_control_reg_t md_reg;

    ret = read_reg(SFH7779_MODE_CONTROL_REG, md_reg.byte);
    return_if_error(ret);

    md_reg.bit.als_ps_time = ALS100_100_PS100;

    ret = write_reg(SFH7779_MODE_CONTROL_REG, md_reg.byte);

    return ret;
}

/**
* Deactivate the sensor (stop measurement sampling and put the sensor in
* standby/low-power mode)
*
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::disable() {
    int ret;
    sfh7779_mode_control_reg_t md_reg;

    ret = read_reg(SFH7779_MODE_CONTROL_REG, md_reg.byte);
    return_if_error(ret);

    md_reg.bit.als_ps_time = ALS0_PS0;

    ret = write_reg(SFH7779_MODE_CONTROL_REG, md_reg.byte);

    return ret;
}

/**
* Get raw proximity value.
*
* @param prox Receive the raw proximity value. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::proximity_raw(uint16_t &prox) {
    return read_short(SFH7779_PS_DATA_LSB_REG, prox);
}

/**
* Get raw visible ambient light value.
*
* @param vis Receive the raw visible ambient light value. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_vis_raw(uint16_t &vis) {
    return read_short(SFH7779_ALS_VIS_DATA_LSB_REG, vis);
}

/**
* Get raw infra-red value.
*
* @param ir Receive the raw infra-red value. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_ir_raw(uint16_t &ir) {
    return read_short(SFH7779_ALS_IR_DATA_LSB_REG, ir);
}

inline float switch_gain(uint8_t gain_raw) {
    switch(gain_raw) {
        case 0b00:
            return 1.0;
            break;
        case 0b01:
            return 2.0;
            break;
        case 0b10:
            return 64.0;
            break;
        case 0b11:
            return 128.0;
            break;
        default:
            return 0.0;
    }
}

/**
* Get visible ambient light value.
*
* @param light Receive the visible ambient light value. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ambient_light(double &light) {
    uint16_t als_vis;
    uint16_t als_ir;
    double als_quotient;
    double lux;
    sfh7779_als_ps_control_reg_t gain_reg;
    double gain_ir;
    double gain_vis;
    int ret;

    ret = als_vis_raw(als_vis);
    return_if_error(ret);
    
    ret = als_ir_raw(als_ir);
    return_if_error(ret);
    
    ret = read_reg(SFH7779_ALS_PS_CONTROL_REG, gain_reg.byte);
    return_if_error(ret);

    gain_ir = switch_gain(gain_reg.bit.gain_ir);
    gain_vis = switch_gain(gain_reg.bit.gain_vis);

    if (gain_vis == 0 || gain_ir == 0) {
        return -1;
    }

    als_quotient = (double)als_ir / (double)als_vis;

    // Lux value in front of sensor, no cover glass
    if(als_quotient < R1) {
        lux = A1 * als_vis / gain_vis - B1 * als_ir / gain_ir;
    } else if(als_quotient < R2) {
        lux = A2 * als_vis / gain_vis - B2 * als_ir / gain_ir;
    } else if(als_quotient <(0.95 * R3)) {
        lux = A3 * als_vis / gain_vis - B3 * als_ir / gain_ir;
    } else if(als_quotient <(1.5 * R3)) {
        lux = 2 * A3 * als_vis / gain_vis - 1.18 * B3 * als_ir / gain_ir;
    } else if(als_quotient <(2.5 * R3)) {
        lux = 4 * A3 * als_vis / gain_vis - 1.33 * B3 * als_ir / gain_ir;
    } else {
        lux = 8 * A3 * als_vis / gain_vis;
    }

    return ret;
}

/**
* Enable ambient light (ALS) interruption.
*
* @param threshold_high Threshold high value to activate interrupt.
* @param threshold_low Threshold low value to activate interrupt.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_enable(uint16_t threshold_high, uint16_t threshold_low){
    sfh7779_interrupt_control_reg_t interrupt_control;
    uint8_t lsb_Up  = threshold_high >> 8;
    uint8_t msb_Up  = threshold_high;
    uint8_t lsb_Low = threshold_low >> 8;
    uint8_t msb_Low = threshold_low;
    int ret;

    ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    interrupt_control.bit.int_trigger_als = INT_PIN_ALS_ACTIVE;

    ret = write_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    ret = write_reg(SFH7779_ALS_VIS_TH_LSB_REG, lsb_Up);
    return_if_error(ret);

    ret = write_reg(SFH7779_ALS_VIS_TH_MSB_REG, msb_Up);
    return_if_error(ret);

    ret = write_reg(SFH7779_ALS_VIS_TL_LSB_REG, lsb_Low);
    return_if_error(ret);

    ret = write_reg(SFH7779_ALS_VIS_TL_MSB_REG, msb_Low);
    return ret;
}

/**
* Disable ambient light (ALS) interruption.
*
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_disable() {
    sfh7779_interrupt_control_reg_t interrupt_control;
    int ret;

    ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    interrupt_control.bit.int_trigger_als = INT_PIN_ALS_INACTIVE;

    ret = write_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);

    return ret;
}

/**
* Gets the status of the ALS interrupt.
*
* @param status Receive the status of the interrupt by ambient light value. True -> active, False -> inactive. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_status(bool &status){
    sfh7779_interrupt_control_reg_t interrupt_control;

    int ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    status = interrupt_control.bit.als_int_status;

    return ret;
}

/**
* Enable proximity (PS) interruption.
*
* @param threshold_high Threshold high value to activate interrupt.
* @param threshold_low Threshold low value to activate interrupt.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_enable(uint16_t threshold_high, uint16_t threshold_low){
    sfh7779_interrupt_control_reg_t interrupt_control;
    uint8_t lsb_Up  = threshold_high >> 8;
    uint8_t msb_Up  = threshold_high;
    uint8_t lsb_Low = threshold_low >> 8;
    uint8_t msb_Low = threshold_low;
    int ret;

    ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    interrupt_control.bit.int_trigger_ps = INT_PIN_PS_ACTIVE;

    ret = write_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    ret = write_reg(SFH7779_PS_TH_LSB_REG, lsb_Up);
    return_if_error(ret);

    ret = write_reg(SFH7779_PS_TH_MSB_REG, msb_Up);
    return_if_error(ret);

    ret = write_reg(SFH7779_PS_TL_LSB_REG, lsb_Low);
    return_if_error(ret);

    ret = write_reg(SFH7779_PS_TL_MSB_REG, msb_Low);
    return ret;
}

/**
* Disable proximity (PS) interruption.
*
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_edisable(){
    sfh7779_interrupt_control_reg_t interrupt_control;
    int ret;

    ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    return_if_error(ret);

    interrupt_control.bit.int_trigger_ps = INT_PIN_PS_INACTIVE;

    ret = write_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);

    return ret;
}

/**
* Gets the status of the PS interrupt.
*
* @param status Receive the status of the interrupt by proximity value. True -> active, False -> inactive. Variable passing by reference.
* @return Operation status.
* @retval 0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_status(bool &status){
    sfh7779_interrupt_control_reg_t interrupt_control;

    int ret = read_reg(SFH7779_INTERRUPT_CONTROL_REG, interrupt_control.byte);
    status = interrupt_control.bit.ps_int_status;

    return ret;
}

/**
*  @brief Function for writing the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to be written.
*
*  @return Operation status.
*  @retval 0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::write_reg(uint8_t reg, uint8_t val){
    return write(nullptr, reg, &val, 1);
}

/**
*  @brief Function for reading the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to store the read data.
*
*  @return Operation status.
*  @retval 0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::read_reg(uint8_t reg, uint8_t &val){
    return read(nullptr, reg, &val, 1);
}

/**
*  @brief Function for reading the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to store the read data.
*
*  @return Operation status.
*  @retval 0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::read_short(uint8_t reg, uint16_t &val){
    uint8_t lsb;
    uint8_t msb;
    int32_t ret;

    ret = read_reg(reg, lsb);
    return_if_error(ret);

    ret = read_reg(reg+1, msb);
    return_if_error(ret);

    val = (msb << 8) + lsb;

    return ret;
}
