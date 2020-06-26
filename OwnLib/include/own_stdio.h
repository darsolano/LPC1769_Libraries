/*
 * own_stdio.h
 *
 *  Created on: Apr 6, 2016
 *      Author: dsolano
 */

#ifndef INCLUDE_OWN_STDIO_H_
#define INCLUDE_OWN_STDIO_H_

// POinter to function for output a char to a display stream IO
typedef void (*STDIO_t)(char data);


int own_printf(const char *format, ...);
int own_sprintf(char *out, const char *format, ...);
void own_stdio_init(STDIO_t outstream);
#endif /* INCLUDE_OWN_STDIO_H_ */
