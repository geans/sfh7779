/*
 * sfh7779.hpp
 *
 *  Created on: 24/06/2021
 *      Author: gean
 */

#ifndef SFH7779_HPP_
#define SFH7779_HPP_


#define SFH7779_I2C_ID                  0x39

/* Registers */
#define SFH7779_SYSTEM_CONTROL_REG      0x40
#define SFH7779_MODE_CONTROL_REG        0X41
#define SFH7779_ALS_PS_CONTROL_REG      0X42
#define SFH7779_PERSISTANCE_REG         0X43
#define SFH7779_PS_DATA_LSB_REG         0X44
#define SFH7779_ALS_VIS_DATA_LSB_REG    0X46
#define SFH7779_ALS_IR_DATA_LSB_REG     0X48
#define SFH7779_INTERRUPT_CONTROL_REG   0X4A
#define SFH7779_PS_TH_LSB_REG           0X4B
#define SFH7779_PS_TH_MSB_REG           0X4C
#define SFH7779_PS_TL_LSB_REG           0X4D
#define SFH7779_PS_TL_MSB_REG           0X4E
#define SFH7779_ALS_VIS_TH_LSB_REG      0X4F
#define SFH7779_ALS_VIS_TH_MSB_REG      0X50
#define SFH7779_ALS_VIS_TL_LSB_REG      0X51
#define SFH7779_ALS_VIS_TL_MSB_REG      0X52
//#define SFH7779_PS_DATA_MSB_REG         0X45
//#define SFH7779_ALS_VIS_DATA_MSB_REG    0X47
//#define SFH7779_ALS_IR_DATA_MSB_REG     0X49

typedef union {
    struct {
        uint8_t als_ps_time         : 4;
        uint8_t ps_operating_mode   : 4;
        uint8_t reserved            : 3;
    } bit;
    uint8_t byte;
} sfh7779_mode_control_reg_t;

typedef enum {
    ALS0_PS0            = 0,
    ALS0_PS10,
    ALS0_PS40,
    ALS0_PS100,
    ALS0_PS400,
    ALS100_100_PS0,
    ALS100_100_PS100,
    ALS100_100_PS400,
    ALS400_100_PS0,
    ALS400_100_PS100,
    ALS400_400_PS0,
    ALS400_400_PS400,
    ALS50_50_PS50,
    PS_MODE_NORMAL      = 0,
    PS_MODE_TWICE
} sfh7779_mode_control_reg_enum;

typedef enum {
    GAIN_X1             = 0,
    GAIN_X2,
    GAIN_X64,
    GAIN_X128,
    LED_CURRENT_25MA    = 0,
    LED_CURRENT_50MA,
    LED_CURRENT_100M,
    LED_CURRENT_200MA,
    PS_OUT_PROXIMITY    = 0,
    PS_OUT_INFRARED_DC
} sfh7779_als_ps_control_reg_enum;

typedef union {
    struct {
        uint8_t led_current : 2;
        uint8_t gain_ir     : 2;
        uint8_t gain_vis    : 2;
        uint8_t ps_output   : 1;
        uint8_t reserved    : 1;
    } bit;
    uint8_t byte;
} sfh7779_als_ps_control_reg_t;

typedef enum {
    INTR_ON_DATA_AVAIL,
    INTR_AFTER_1_VAL,
    INTR_AFTER_2_VALS,
    INTR_AFTER_3_VALS,
    INTR_AFTER_4_VALS,
    INTR_AFTER_5_VALS,
    INTR_AFTER_6_VALS,
    INTR_AFTER_7_VALS,
    INTR_AFTER_8_VALS,
    INTR_AFTER_9_VALS,
    INTR_AFTER_10_VALS,
    INTR_AFTER_11_VALS,
    INTR_AFTER_12_VALS,
    INTR_AFTER_13_VALS,
    INTR_AFTER_14_VALS,
    INTR_AFTER_15_VALS,
} sfh7779_persistence_reg_enum;

typedef enum {
    INT_PIN_PS_INACTIVE       = 0x00, // bit 0, default
    INT_PIN_PS_ACTIVE         = 0x01,
    INT_PIN_ALS_INACTIVE      = 0x00, // bit 1, default
    INT_PIN_ALS_ACTIVE        = 0x01,
    INT_LATCHED               = 0x00, // bit 2, default
    INT_UNLATCHED             = 0x01,
    INT_ASSERT_LOW_ONLY       = 0x00, // bit 3, default
    INT_ASSERT_LOW_THEN_HIGH  = 0x01,
    INT_MODE_PS_HIGH          = 0x00, // bit 4-5, default
    INT_MODE_PS_HIGH_LOW_HYS  = 0x01,
    INT_MODE_PS_HIGH_LOW_OD   = 0x02,
    ALS_INT_INACTIVE          = 0x00, // bit 6, default
    ALS_INT_ACTIVE            = 0x01,
    PS_INT_INACTIVE           = 0x00, // bit 7, default
    PS_INT_ACTIVE             = 0x01,
} sfh7779_interrupt_control_enum;

typedef union {
    struct {
        uint8_t int_trigger_ps  : 1;
        uint8_t int_trigger_als : 1;
        uint8_t int_latch       : 1;
        uint8_t int_assert      : 1;
        uint8_t ps_int_mode     : 2;
        uint8_t als_int_status  : 1;
        uint8_t ps_int_status   : 1;
    } bit;
    uint8_t byte;
} sfh7779_interrupt_control_reg_t;

typedef enum {
    SWITCH_ERROR                    = -4,
    READ_ERROR                      = -3,
    WRITE_ERROR                     = -2,
    INVALID_FILEDESCRIPTOR_ERROR    = -1,
    SUCCESS                         = 0,
} error_num;

// ALS CONSTANTS
static const double R1 = 0.109;
static const double R2 = 0.429;
static const double R3 = 1.45;
static const double A1 = 1.534;
static const double A2 = 1.339;
static const double A3 = 0.701;
static const double B1 = 3.759;
static const double B2 = 1.972;
static const double B3 = 0.483;

#include <stdint.h>

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
    int proximity_raw(uint16_t &prox);
    int als_vis_raw(uint16_t &vis);
    int als_ir_raw(uint16_t &ir);
    int ambient_light(double &light);
    int als_interrupt_enable(uint16_t threshold_high, uint16_t threshold_low);
    int als_interrupt_disable();
    int als_interrupt_status(bool &status);
    int ps_interrupt_enable(uint16_t threshold_high, uint16_t threshold_low);
    int ps_interrupt_edisable();
    int ps_interrupt_status(bool &status);

private:
    write_ptr write;
    read_ptr read;
    int write_reg(uint8_t reg, uint8_t val);
    int read_reg(uint8_t reg, uint8_t &val);
    int read_short(uint8_t reg, uint16_t &val);
};

#endif /* SFH7779_HPP_ */
