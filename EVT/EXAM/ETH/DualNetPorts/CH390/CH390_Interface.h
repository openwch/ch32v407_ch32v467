/********************************** (C) COPYRIGHT *******************************
* File Name          : CH390.h
* Author             : WCH
* Version            : V1.0
* Date               : 2024/08/20
* Description        : CH390 interface header file
*******************************************************************************/

#ifndef __CH390_INTERFACE_H
#define __CH390_INTERFACE_H

#include <stdint.h>
#include "ch390.h"

void ch390_gpio_init(void);
void ch390_interrupt_init(void);
void ch390_spi_init(void);

uint16_t ch390_get_int_pin(void);
void ch390_delay_us(uint32_t time);
void ch390_hardware_reset(void);

/**
 * @name ch390_read_reg
 * @brief Read register
 * @param reg - Target register address
 * @return Register value
 */
uint8_t ch390_read_reg(uint8_t reg);

/**
 * @name ch390_write_reg
 * @brief Write register
 * @param reg - Target register address
 * @param value - Value to be written
 */
void ch390_write_reg(uint8_t reg, uint8_t value);

/**
 * @name ch390_read_mem
 * @brief Read data from RX SRAM
 * @param data - Data buffer
 * @param length - Length to read
 */
void ch390_read_mem(uint8_t *data, int length);

/**
 * @name ch390_write_mem
 * @brief Write data to TX SRAM
 * @param data - Data buffer
 * @param length - Length to write
 */
void ch390_write_mem(uint8_t *data, int length);

#endif /* __CH390_INTERFACE_H */
