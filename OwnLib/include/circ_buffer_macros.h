#ifndef __CIRC_BUFFER_MACROS_H__
#define __CIRC_BUFFER_MACROS_H__
/* =============================================================================

    Copyright (c) 2012 Pieter Conradie [http://piconomic.co.za]
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
    
    Title:          circ_buffer_macros.h : Tiny, macro based, circular buffer implementation
    Author(s):      Pieter Conradie
    Creation Date:  2012/06/10
    Revision Info:  $Id: circ_buffer_macros.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/** 
   @ingroup UTILS 
   @defgroup CIRC_BUFFER_MACROS circ_buffer_macros.h : Tiny, macro based, circular buffer implementation

   Tiny, macro based, circular buffer implementation.
    
   File(s):
   - utils/circ_buffer_macros.h
    
   Source:

   - http://idlehands.typepad.com/idle-hands/2009/11/let-freedom-circular.html
   - http://idlehands.typepad.com/idle-hands/2009/11/buffers-continued.html
   - http://idlehands.typepad.com/files/2009/11/buffers-continued/cq_lib.h

   License:

   Copyright (c) 2009, Brian Orr
   All rights reserved.

   Redistribution and use in source and binary forms, with or without 
   modification, are permitted provided that the following conditions are 
   met:

       Redistributions of source code must retain the above copyright notice, 
       this list of conditions and the following disclaimer.
       Redistributions in binary form must reproduce the above copyright 
       notice, this list of conditions and the following disclaimer in the 
       documentation and/or other materials provided with the distribution.
       Neither the name of the author nor the names of any contributors may 
       be used to endorse or promote products derived from this software 
       without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
   POSSIBILITY OF SUCH DAMAGE.

   @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef u8_t circ_buf_index_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
/**
   Declare a static circular buffer structure.

   @param circ_buf          Name of the circular buffer structure
   @param circ_buf_size     Data buffer size, which *MUST* be a multiple of 2
                            and equal to or less than MAX_OF_TYPE(circ_buf_index_t),
                            e.g. 2, 4, 8, 16, 32, ...
 */
#define CIRC_BUF_DECLARE(circ_buf, circ_buf_size)                   \
struct                                                              \
{                                                                   \
    volatile circ_buf_index_t read_index;                           \
    volatile circ_buf_index_t write_index;                          \
    u8_t                buffer[(circ_buf_size)];                    \
} circ_buf

/**
   Initialise the circular buffer structure to be empty. 
    
   @param circ_buf      Name of the circular buffer structure 
 */
#define CIRC_BUF_INIT(circ_buf)                                     \
   do                                                               \
   {                                                                \
      (circ_buf).read_index  = 0;                                   \
      (circ_buf).write_index = 0;                                   \
   }                                                                \
   while(0)

/**
    Test to see if the circular buffer is empty.
    
    @param circ_buf     Name of the circular buffer structure 
 */
#define CIRC_BUF_EMPTY(circ_buf)  \
   ((circ_buf).read_index == (circ_buf).write_index)


/**
    Test to see if the circular buffer is full.
    
    @param circ_buf     Name of the circular buffer structure 
 */
#define CIRC_BUF_FULL(circ_buf) \
   ((circ_buf).read_index == (((circ_buf).write_index + 1) & (SIZEOF_ARRAY((circ_buf).buffer)-1)))

/**
    Reads the next available byte from the circular buffer.
    
    The caller is responsible for making sure the circular buffer is not empty
    before the call. See CIRC_BUF_EMPTY()
    
    @param circ_buf     Name of the circular buffer structure
    @param data         The variable that the byte will be stored in
 */
#define CIRC_BUF_READ(circ_buf, data)                           \
   do                                                           \
   {                                                            \
      circ_buf_index_t index = (circ_buf).read_index;           \
      (data) = (circ_buf).buffer[index];                        \
      index = (index+1) & (SIZEOF_ARRAY((circ_buf).buffer)-1);  \
      (circ_buf).read_index = index;                            \
   } while(0)

/**
    Writes a byte into the circular buffer.
    
    The caller is responsible for making sure the circular buffer is not full before
    the call. See CIRC_BUF_FULL()
    
    @param circ_buf     Name of the circular buffer structure
    @param data         The byte that will be written to the circular buffer
 */
#define CIRC_BUF_WRITE(circ_buf, data)                          \
   do                                                           \
   {                                                            \
      circ_buf_index_t index = (circ_buf).write_index;          \
      (circ_buf).buffer[index] = (data);                        \
      index = (index+1) & (SIZEOF_ARRAY((circ_buf).buffer)-1);  \
      (circ_buf).write_index = index;                           \
   } while(0)

/// @}
#endif
