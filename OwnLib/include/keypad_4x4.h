/* 
 * File:   keypad_4x4.h
 * Author: dsolano
 *
 * Created on June 28, 2014, 11:43 AM
 */

#ifndef KEYPAD_4X4_H
#define	KEYPAD_4X4_H



#ifdef	__cplusplus
extern "C" {
#endif

#define KEYPAD_COL      4
#define KEYPAD_ROWS     4

void     kp_Init(void);
uint8_t  kp_GetKeyPressed(void);


#ifdef	__cplusplus
}
#endif

#endif	/* KEYPAD_4X4_H */

