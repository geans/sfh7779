/*
 * sfh7779.cpp
 *
 *  Created on: 20/06/2021
 *      Author: Gean
 */

#include "sfh7779.hpp"

#define check(ret) if (ret != 0) return ret

/**
* Activate the sensor (begin measurement sampling).  Data samples are
* taken 10 times per second.
*
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::enable() {
	int ret;
	ret = write_reg(MODE_CONTROL_REG,     // Start ALS and PS sampling
			PS_MODE_NORMAL | MRR_ALS100PS100);
	check(ret);
	ret = write_reg(ALS_PS_CONTROL_REG,   // set ALS_VIS=ALS_IR GAIN = 64 current 25ma
			PS_OUT_PROXIMITY | ALS_GAIN_ALS64IR64 | LED_CURRENT_25MA);
	check(ret);
	ret = write_reg(PERSISTANCE_REG,      // set interrupt flag upon [any] data available
			INTR_ON_DATA_AVAIL);
	return ret;
}

/**
* Deactivate the sensor (stop measurement sampling and put the sensor in
* standby/low-power mode)
*
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::disable() {
	return write_reg(MODE_CONTROL_REG,       // Stop ALS and PS sampling
			PS_MODE_NORMAL | MRR_ALS0PS0);
}

/**
* Get raw proximity value.
*
* @param prox Receive the raw proximity value. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::proximity_raw(unsigned short &prox) {
	return read_short(PS_DATA_LSB_REG, prox);
}

/**
* Get raw visible ambient light value.
*
* @param vis Receive the raw visible ambient light value. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_vis_raw(unsigned short &vis) {
	return read_short(ALS_VIS_DATA_LSB_REG, vis);
}

/**
* Get raw infra-red value.
*
* @param ir Receive the raw infra-red value. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_ir_raw(unsigned short &ir) {
	return read_short(ALS_IR_DATA_LSB_REG, ir);
}


/* ALS CONSTANTS */
const double R1 = 0.109;
const double R2 = 0.429;
const double R3 = 1.45;
const double A1 = 1.534;
const double A2 = 1.339;
const double A3 = 0.701;
const double B1 = 3.759;
const double B2 = 1.972;
const double B3 = 0.483;

/**
* Get visible ambient light value.
*
* @param light Receive the visible ambient light value. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ambient_light(double &light) {
    unsigned short als_vis;
    unsigned short als_ir;
    double als_quotient;
    double lux;
    unsigned char gain_reg;
    double gain;
    int ret;

	ret = als_vis_raw(als_vis);
	check(ret);
	ret = als_ir_raw(als_ir);
	check(ret);
	ret = read_reg(ALS_PS_CONTROL_REG, gain_reg);
	check(ret);

	gain_reg &= 0x30; // get bits 4 and 5
	switch(gain_reg) {
	case 0x00:
		gain = 1.0;
		break;
	case 0x10:
		gain = 2.0;
		break;
	case 0x20:
		gain = 64.0;
		break;
	case 0x30:
		gain = 128.0;
		break;
	default:
		return false;
	}

	als_quotient = (double)als_ir / (double)als_vis;

	// Lux Calculation based on ALS Gain = 64 and ALS_Int_Time = 100 ms
	// Lux value in front of sensor, no cover glass

	if(als_quotient < R1) {
		lux = A1 * als_vis / gain - B1 * als_ir / gain;
	} else if(als_quotient < R2) {
		lux = A2 * als_vis / gain - B2 * als_ir / gain;
	} else if(als_quotient <(0.95 * R3)) {
		lux = A3 * als_vis / gain - B3 * als_ir / gain;
	} else if(als_quotient <(1.5 * R3)) {
		lux = 2 * A3 * als_vis / gain - 1.18 * B3 * als_ir / gain;
	} else if(als_quotient <(2.5 * R3)) {
		lux = 4 * A3 * als_vis / gain - 1.33 * B3 * als_ir / gain;
	} else {
		lux = 8 * A3 * als_vis / gain;
	}

	return ret;
}

/**
* Enable ambient light (ALS) interruption.
*
* @param threshold_high Threshold high value to activate interrupt.
* @param threshold_low Threshold low value to activate interrupt.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low){
    unsigned char interrupt_control;
    unsigned char lsb_Up  = threshold_high >> 8;
    unsigned char msb_Up  = threshold_high;
    unsigned char lsb_Low = threshold_low >> 8;
    unsigned char msb_Low = threshold_low;
    int ret;

    ret = read_reg(INTERRUPT_CONTROL_REG, interrupt_control);
    check(ret);
    ret = write_reg(INTERRUPT_CONTROL_REG, interrupt_control | INT_PIN_ALS_ONLY);
    check(ret);
    ret = write_reg(ALS_VIS_TH_LSB_REG, lsb_Up);
    check(ret);
    ret = write_reg(ALS_VIS_TH_MSB_REG, msb_Up);
    check(ret);
    ret = write_reg(ALS_VIS_TL_LSB_REG, lsb_Low);
    check(ret);
    ret = write_reg(ALS_VIS_TL_MSB_REG, msb_Low);

    return ret;
}

/**
* Disable ambient light (ALS) interruption.
*
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_disable() {
	unsigned char interruptControl;
	int ret;

	ret = read_reg(INTERRUPT_CONTROL_REG, interruptControl);
	check(ret);
	ret = write_reg(INTERRUPT_CONTROL_REG, interruptControl & NOT_INT_PIN_ALS_ONLY);

    return ret;
}

/**
* Gets the status of the ALS interrupt.
*
* @param status Receive the status of the interrupt by ambient light value. True -> active, False -> inactive. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::als_interrupt_status(bool &status){
	unsigned char interruptControl;
	unsigned char mask = 0x40;

	int ret = read_reg(INTERRUPT_CONTROL_REG, interruptControl);
	status = (interruptControl & mask) == mask; // get the two MSB

	return ret;
}

/**
* Enable proximity (PS) interruption.
*
* @param threshold_high Threshold high value to activate interrupt.
* @param threshold_low Threshold low value to activate interrupt.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low){
    unsigned char interrupt_control;
    unsigned char lsb_Up  = threshold_high >> 8;
    unsigned char msb_Up  = threshold_high;
    unsigned char lsb_Low = threshold_low >> 8;
    unsigned char msb_Low = threshold_low;
    int ret;

    ret = read_reg(INTERRUPT_CONTROL_REG, interrupt_control);
    check(ret);
    ret = write_reg(INTERRUPT_CONTROL_REG, interrupt_control | INT_PIN_PS_ONLY);
    check(ret);
    ret = write_reg(PS_TH_LSB_REG, lsb_Up);
    check(ret);
    ret = write_reg(PS_TH_MSB_REG, msb_Up);
    check(ret);
    ret = write_reg(PS_TL_LSB_REG, lsb_Low);
    check(ret);
    ret = write_reg(PS_TL_MSB_REG, msb_Low);

    return ret;
}

/**
* Disable proximity (PS) interruption.
*
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_edisable(){
	unsigned char interrupt_control;
	int ret;

	ret = read_reg(INTERRUPT_CONTROL_REG, interrupt_control);
	check(ret);
	ret = write_reg(INTERRUPT_CONTROL_REG, interrupt_control & NOT_INT_PIN_PS_ONLY);

    return ret;
}

/**
* Gets the status of the PS interrupt.
*
* @param status Receive the status of the interrupt by proximity value. True -> active, False -> inactive. Variable passing by reference.
* @return Operation status.
* @retval >0 Success.
* @retval !=0 Failure Info.
*/
int SFH7779::ps_interrupt_status(bool &status){
	unsigned char interrupt_control;
	unsigned char mask = 0x80;

	int ret = read_reg(INTERRUPT_CONTROL_REG, interrupt_control);
	status = (interrupt_control & mask) == mask; // get the two MSB

	return ret;
}

/**
*  @brief Function for writing the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to be written.
*
*  @return Operation status.
*  @retval >0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::write_reg(unsigned char reg, unsigned char val){
	return write(nullptr, reg, &val, 1);
}

/**
*  @brief Function for reading the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to store the read data.
*
*  @return Operation status.
*  @retval >0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::read_reg(unsigned char reg, unsigned char &val){
	return read(nullptr, reg, &val, 1);
}

/**
*  @brief Function for reading the sensor's registers through I2C device.
*
*  @param reg Register address.
*  @param val Data byte whose value is to store the read data.
*
*  @return Operation status.
*  @retval >0 Success.
*  @retval !=0 Failure Info.
*/
int SFH7779::read_short(unsigned char reg, unsigned short &val){
	uint8_t lsb;
	uint8_t msb;
	int32_t ret;

	ret = read_reg(reg, lsb);
	if (ret != 0) return ret;
	ret = read_reg(reg+1, msb);
	if (ret != 0) return ret;

	val = (msb << 8) + lsb;

	return ret;
}