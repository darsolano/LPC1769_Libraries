/*
 * circular.h
 *	Circular Buffer API
 *  Created on: 5/3/2015
 *      Author: dsolano
 */

#ifndef INCLUDE_CIRCULAR_H_
#define INCLUDE_CIRCULAR_H_

/* =============================================================================

 Copyright (c) 2008 Pieter Conradie [http://piconomic.co.za]
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 3. Credit must appear prominently in all internet publications (including
 advertisements) of products or projects that use this software by
 including the following text and URL:
 "Uses Piconomic FW Library <http://piconomic.co.za>"

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 Title:          circ_buffer.h : FIFO circular buffer
 Author(s):      Pieter Conradie
 Creation Date:  2008/08/06
 Revision Info:  $Id: circ_buffer.h 13 2015-02-22 06:33:44Z pieterconradie $

 ============================================================================= */

/**
 @ingroup UTILS
 @defgroup CIRC_BUFFER circ_buffer.h : FIFO circular buffer

 A data structure that uses a single, fixed-size buffer as if it were
 connected end-to-end (circular).

 File(s):
 - utils/circ_buffer.h
 - utils/circ_buffer.c

 A fixed-sized buffer is managed as a FIFO buffer with a "zero-copy" policy,
 i.e. data is not shifted (copied) when data is removed or added to the
 buffer. If more data is written to the buffer than there is space for, it is
 ignored/discarded; no buffer overflow vulnerability.

 In this implementation, the maximum amount of bytes that can be stored is
 one less than the size of the fixed-size buffer, e.g. if the buffer size is
 8 bytes, then a maximum of 7 bytes can be stored.

 Reference:
 - http://en.wikipedia.org/wiki/Circular_buffer

 Example:

 @code
 Buffer is empty:

 start       write     end
 |           |        |
 [ ][ ][ ][ ][ ][ ][ ][ ]
 |
 read

 One byte is written to the buffer ('1'):

 write
 |
 [ ][ ][ ][ ][1][ ][ ][ ]
 |
 read

 One byte is read ('1'); buffer is empty again:

 write
 |
 [ ][ ][ ][ ][ ][ ][ ][ ]
 |
 read

 5 bytes are written ('2','3','4','5','6'); buffer wraps:

 write
 |
 [5][6][ ][ ][ ][2][3][4]
 |
 read

 2 bytes are written ('7','8'); buffer is full:

 write
 |
 [5][6][7][8][ ][2][3][4]
 |
 read
 @endcode
 */
/// @{
/* _____STANDARD INCLUDES____________________________________________________ */
#include <lpc_types.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */


/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Ring buffer structure
typedef struct
{
	u8_t *start;    ///< Pointer to first byte of fixed-size buffer
	u8_t *end;      ///< Pointer to last byte of fixed-size buffer
	u8_t *write;    ///< Pointer that is one ahead of last byte written to the buffer
	u8_t *read;     ///< Pointer to the first byte to be read from the buffer
} circ_buf_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 Initialize the circular buffer.

 @param circ_buf         Pointer to the circular buffer object
 @param buffer           Fixed-size data buffer
 @param buffer_size      Fixed-size data buffer size
 */
void circ_buf_init(circ_buf_t * circ_buf, u8_t * buffer, size_t buffer_size);

/**
 See if the circular buffer is empty.

 @param circ_buf  Pointer to the circular buffer object

 @retval TRUE            buffer is empty
 @retval FALSE           buffer contains data
 */
bool_t circ_buf_empty(circ_buf_t * circ_buf);

/**
 See if the circular buffer is full.

 @param circ_buf  Pointer to the circular buffer object

 @retval TRUE            buffer is full
 @retval FALSE           buffer is NOT full
 */
bool_t circ_buf_full(circ_buf_t * circ_buf);

/**
 Write (store) a byte in the circular buffer.

 @param circ_buf         Pointer to the circular buffer object
 @param data             The byte to store in the circular buffer

 @retval TRUE            Byte has been stored in the circular buffer
 @retval FALSE           Buffer is full and byte was not stored
 */
bool_t circ_buf_wr_u8(circ_buf_t * circ_buf, const u8_t data);

/**
 Write (store) data in the circular buffer

 @param circ_buf         Pointer to the circular buffer object
 @param data             Pointer to array of data to be stored in the circular
 buffer
 @param bytes_to_write   Amount of data bytes to be written

 @return u16_t           The actual number of data bytes stored, which may
 be less than the number specified, because the
 buffer is full.
 */
u16_t circ_buf_wr_data(circ_buf_t * circ_buf, const u8_t * data, u16_t bytes_to_write);

/**
 Read (retrieve) a byte from the circular buffer.

 @param circ_buf         Pointer to the circular buffer object
 @param data             Pointer to location where byte must be stored

 @retval TRUE            Valid byte has been retrieved
 @retval FALSE           Buffer is empty
 */
bool_t circ_buf_rd_u8(circ_buf_t * circ_buf, u8_t * data);

/**
 Read (retrieve) data from the circular buffer.

 @param circ_buf          Pointer to the circular buffer object
 @param data              Pointer to location where data must be stored
 @param bytes_to_read     Number of bytes to retrieve

 @return u16_t            The actual number of bytes retrieved, which may be less
 than the number specified, because the buffer is empty.
 */
u16_t circ_buf_rd_data(circ_buf_t * circ_buf, u8_t * data, u16_t bytes_to_read);

/**
 Read (retrieve) data from the circular buffer.

 @param circ_buf          Pointer to the circular buffer object
 @param ret_data          Pointer to location where data must be stored
 @param to_char           Const Char from start

 @return u16_t            The actual number of bytes retrieved, which may be less
 than the number specified, because the buffer is empty.
 */
u16_t circ_buf_rd_from_to_char(circ_buf_t* cb , u8_t* ret_data , const u8_t to_char);
/* _____MACROS_______________________________________________________________ */

/// @}
#endif /* INCLUDE_CIRCULAR_H_ */
