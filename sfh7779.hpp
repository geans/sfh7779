#ifndef SFH7779_DRIVER
#define SFH7779_DRIVER


#define SFH7779_I2C_ADDR   	    0x39
#define SYSTEM_CONTROL_REG      0x40

#define MODE_CONTROL_REG        0x41
#define   PS_MODE_NORMAL        (0x00<<4)   // default
#define   PS_MODE_TWO_PULSE     (0x10<<4)
#define   MRR_ALS0PS0           (0x00<<0)   // default
#define   MRR_ALS0PS10          (0x01<<0)
#define   MRR_ALS0PS40          (0x02<<0)
#define   MRR_ALS0PS100         (0x03<<0)
#define   MRR_ALS0PS400         (0x04<<0)
#define   MRR_ALS100PS0         (0x05<<0)
#define   MRR_ALS100PS100       (0x06<<0)
#define   MRR_ALS100PS400       (0x07<<0)
#define   MRR_ALS401PS0         (0x08<<0)
#define   MRR_ALS401PS100       (0x09<<0)
#define   MRR_ALS400PS0         (0x0A<<0)
#define   MRR_ALS400PS400       (0x0B<<0)
#define   MRR_ALS50PS50         (0x0C<<0)

#define ALS_PS_CONTROL_REG      0x42
#define   PS_OUT_PROXIMITY      (0x00<<6)   // default
#define   PS_OUT_INFRARED_DC    (0x01<<6)
#define   ALS_GAIN_ALS1IR1      (0x00<<2)   // default
#define   ALS_GAIN_ALS2IR1      (0x04<<2)
#define   ALS_GAIN_ALS2IR2      (0x05<<2)
#define   ALS_GAIN_ALS64IR64    (0x0A<<2)
#define   ALS_GAIN_ALS128IR64   (0x0E<<2)
#define   ALS_GAIN_ALS128IR128  (0x0F<<2)
#define   LED_CURRENT_25MA      (0x00<<0)
#define   LED_CURRENT_50MA      (0x01<<0)
#define   LED_CURRENT_100M      (0x02<<0)
#define   LED_CURRENT_200MA     (0x03<<0)   // default

#define PERSISTANCE_REG         0x43
#define   INTR_ON_DATA_AVAIL    (0x00<<0)
#define   INTR_AFTER_1_VAL      (0x01<<0)   // default
#define   INTR_AFTER_2_VALS     (0x02<<0)
#define   INTR_AFTER_3_VALS     (0x03<<0)
#define   INTR_AFTER_4_VALS     (0x04<<0)
#define   INTR_AFTER_5_VALS     (0x05<<0)
#define   INTR_AFTER_6_VALS     (0x06<<0)
#define   INTR_AFTER_7_VALS     (0x07<<0)
#define   INTR_AFTER_8_VALS     (0x08<<0)
#define   INTR_AFTER_9_VALS     (0x09<<0)
#define   INTR_AFTER_10_VALS    (0x0A<<0)
#define   INTR_AFTER_11_VALS    (0x0B<<0)
#define   INTR_AFTER_12_VALS    (0x0C<<0)
#define   INTR_AFTER_13_VALS    (0x0D<<0)
#define   INTR_AFTER_14_VALS    (0x0E<<0)
#define   INTR_AFTER_15_VALS    (0x0F<<0)

#define PS_DATA_LSB_REG         0x44
#define PS_DATA_MSB_REG         0x45
#define ALS_VIS_DATA_LSB_REG    0x46
#define ALS_VIS_DATA_MSB_REG    0x47
#define ALS_IR_DATA_LSB_REG     0x48
#define ALS_IR_DATA_MSB_REG     0x49

#define INTERRUPT_CONTROL_REG   0x4A
#define   PS_INT_ACTIVE             (0x01<<7)
#define   ALS_INT_ACTIVE            (0x01<<6)
#define   INT_MODE_PS_HIGH          (0x00<<4)   // default
#define   INT_MODE_PS_HIGHLOW_HYS   (0x01<<4)
#define   INT_MODE_PS_HIGHLOW_OD    (0x02<<4)
#define   INT_ASSERT_LOW_ONLY       (0x00<<3)   // default
#define   INT_ASSERT_LOW_THEN_HIGH  (0x01<<3)
#define   INT_LATCHED               (0x00<<2)   // default
#define   INT_UNLATCHED             (0x01<<2)
#define   INT_PIN_INACTIVE          (0x00<<0)   // default
#define   INT_PIN_PS_ONLY           (0x01<<0)   // default
#define   INT_PIN_ALS_ONLY          (0x02<<0)   // default
#define   INT_PIN_PS_AND_ALS        (0x03<<0)   // default
#define   NOT_INT_PIN_PS_ONLY	0xFE
#define   NOT_INT_PIN_ALS_ONLY	0xFD

#define PS_TH_LSB_REG           0X4B
#define PS_TH_MSB_REG           0X4C
#define PS_TL_LSB_REG           0X4D
#define PS_TL_MSB_REG           0X4E
#define ALS_VIS_TH_LSB_REG      0X4F
#define ALS_VIS_TH_MSB_REG      0X50
#define ALS_VIS_TL_LSB_REG      0X51
#define ALS_VIS_TL_MSB_REG      0X52

#include <cstdint>

/** @addtogroup  Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
typedef int32_t (*write_ptr)(void *, uint8_t, const uint8_t *, uint16_t);
typedef int32_t (*read_ptr)(void *, uint8_t, uint8_t *, uint16_t);


class SFH7779 {

public:

    /**
    * Constructor
    */
    SFH7779(write_ptr wr, read_ptr rd) {
    	write = wr;
    	read = rd;
    }

    /**
    * Activate the sensor (begin measurement sampling).  Data samples are
    * taken 10 times per second.
    *
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int enable();
        
    /**
    * Deactivate the sensor (stop measurement sampling and put the sensor in
    * standby/low-power mode)
    *
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int disable();

    /**
    * Get raw proximity value.
    *
    * @param prox Receive the raw proximity value. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int proximity_raw(unsigned short &prox);

    /**
    * Get raw visible ambient light value.
    *
    * @param vis Receive the raw visible ambient light value. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int als_vis_raw(unsigned short &vis);

    /**
    * Get raw infra-red value.
    *
    * @param ir Receive the raw infra-red value. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int als_ir_raw(unsigned short &ir);

    /**
    * Get visible ambient light value.
    *
    * @param light Receive the visible ambient light value. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int ambient_light(double &light);

    /**
    * Enable ambient light (ALS) interruption.
    *
    * @param threshold_high Threshold high value to activate interrupt.
    * @param threshold_low Threshold low value to activate interrupt.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int als_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low);

    /**
    * Disable ambient light (ALS) interruption.
    *
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int als_interrupt_disable();

    /**
    * Gets the status of the ALS interrupt.
    *
    * @param status Receive the status of the interrupt by ambient light value. True -> active, False -> inactive. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int als_interrupt_status(bool &status);

    /**
    * Enable proximity (PS) interruption.
    *
    * @param threshold_high Threshold high value to activate interrupt.
    * @param threshold_low Threshold low value to activate interrupt.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int ps_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low);

    /**
    * Disable proximity (PS) interruption.
    *
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int ps_interrupt_edisable();

    /**
    * Gets the status of the PS interrupt.
    *
    * @param status Receive the status of the interrupt by proximity value. True -> active, False -> inactive. Variable passing by reference.
    * @return Operation status.
    * @retval >0 Success.
    * @retval !=0 Failure Info.
    */
    int ps_interrupt_status(bool &status);

private:

    write_ptr write;
    read_ptr read;

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
    int write_reg(unsigned char reg, unsigned char val){
    	return write(nullptr, reg, &val, 1);
    }

    /**
    *  @brief Function for writing the sensor's registers through I2C device.
    *
    *  @param reg Register address.
    *  @param val Data byte whose value is to store the read data.
    *
    *  @return Operation status.
    *  @retval >0 Success.
    *  @retval !=0 Failure Info.
    */
    int read_reg(unsigned char reg, unsigned char &val){
    	return read(nullptr, reg, &val, 1);
    }

    int read_short(char reg, unsigned short &val){
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

};


#endif // SFH7779_DRIVER
