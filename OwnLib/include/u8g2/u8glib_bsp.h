/*
 * u8glib.h
 *
 *  Created on: Dec 17, 2018
 *      Author: dsolano
 */

#ifndef U8GLIB_BSP_H_
#define U8GLIB_BSP_H_

#include <chip.h>
#include <u8g2/u8g2.h>

#define DEFINE_PIN(name, port, pin) \
inline static void name##_DEASSERT() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
} \
inline static void name##_OFF() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
} \
inline static void name##_LOW() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
} \
inline static void name##_ASSERT() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_ON() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_HIGH() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_TOGGLE() { \
  LPC_GPIO[port].PIN ^= (1 << pin); \
} \
inline static void name##_INPUT() { \
  LPC_GPIO[port].DIR &= ~(1UL << pin); \
} \
inline static void name##_OUTPUT() { \
  LPC_GPIO[port].DIR |= (1UL << pin); \
} \
inline static bool name##_READ() { \
  return (( LPC_GPIO[port].PIN >> pin) & 1); \
}\
inline static void name##_DESELECTED() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_SELECTED() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
}

uint8_t u8x8_gpio_and_delay_i2c_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_hw_i2c_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_hw_spi_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_spi_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif /* U8GLIB_BSP_H_ */
