/*
 * datetime.h
 *
 *  Created on: 16/12/2014
 *      Author: dsolano
 */

#ifndef DATETIME_H_
#define DATETIME_H_

/*
lpcRTC.h - Header file for the lpcRTC Real-Time Clock

Version: 1.0.1
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>


typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
    uint32_t unixtime;
}RTCDateTime;


bool lpcRTC_initialize(void);
void lpcRTC_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void lpcRTC_setDateTime_word(uint32_t t);
void lpcRTC_setDateTime_array(const char* date, const char* time);
void lpcRTC_getDateTime(RTCDateTime*);
uint8_t lpcRTC_isReady(void);
char* lpcRTC_dateFormat(const char* dateFormat, RTCDateTime* dt);




#endif /* DATETIME_H_ */
