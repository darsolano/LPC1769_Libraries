/*
 * i2cscanner.h
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#ifndef I2CSCANNER_H_
#define I2CSCANNER_H_

#include  <chip.h>


#define I2CDEV				LPC_I2C0
#define I2C_CLOCK_FREQ		400000

#define UART_DEV			LPC_UART3
#define UART_RATE			115200


void i2cscan(I2C_ID_T module);

#endif /* I2CSCANNER_H_ */
