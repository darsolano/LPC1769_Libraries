/*
 * ubx.c
 *
 *  Created on: 27/2/2015
 *      Author: dsolano
 */

#include <ubx.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <ctype.h>


GPS_UBX_HEAD_pt ubxpack;

void gps_ubx_checksum(void)
{
	int i;
	short CK_A = 0, CK_B = 0;
	int n = ubxpack->size;
	for (i = 0; i < n; i++)
	{
		CK_A = CK_A + (int)*ubxpack->payload;
		CK_B = CK_B + CK_A;
		ubxpack->chksum = ((CK_A << 8) & 0xFF00) | (CK_B & 0x00FF);
	}
}

void gps_ubx_on_rx_new_byte(char * byte)
{

}
