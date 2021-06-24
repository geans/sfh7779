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

    int enable();    
    int disable();
    int proximity_raw(unsigned short &prox);
    int als_vis_raw(unsigned short &vis);
    int als_ir_raw(unsigned short &ir);
    int ambient_light(double &light);
    int als_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low);
    int als_interrupt_disable();
    int als_interrupt_status(bool &status);
    int ps_interrupt_enable(unsigned short threshold_high, unsigned short threshold_low);
    int ps_interrupt_edisable();
    int ps_interrupt_status(bool &status);

private:

    write_ptr write;
    read_ptr read;

    int write_reg(unsigned char reg, unsigned char val);
    int read_reg(unsigned char reg, unsigned char &val);
    int read_short(unsigned char reg, unsigned short &val);

};


#endif // SFH7779_DRIVER
