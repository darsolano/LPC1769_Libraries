/*
 * circular.c
 *
 *  Created on: 5/3/2015
 *      Author: dsolano
 */

#include <circular.h>
#include <string.h>

void circ_buf_init(circ_buf_t * circ_buf, u8_t * buffer, size_t buffer_size)
{
	// Initialise the circular buffer structure to be empty
	circ_buf->start = buffer;
	circ_buf->end = buffer + (buffer_size - 1);
	circ_buf->write = buffer;
	circ_buf->read = buffer;
}

bool_t circ_buf_empty(circ_buf_t * circ_buf)
{
	return (circ_buf->read == circ_buf->write);
}

bool_t circ_buf_full(circ_buf_t * circ_buf)
{
	// Calculate next position of in pointer
	u8_t * next = circ_buf->write;
	if (next == circ_buf->end)
	{
		// Wrap pointer to start of buffer
		next = circ_buf->start;
	}
	else
	{
		// Increment pointer
		next++;
	}

	return (next == circ_buf->read);
}

bool_t circ_buf_wr_u8(circ_buf_t * circ_buf, const u8_t data)
{
	// Calculate next position of in pointer
	u8_t * next = circ_buf->write;
	if (next == circ_buf->end)
	{
		// Wrap pointer to start of buffer
		next = circ_buf->start;
	}
	else
	{
		// Increment pointer
		next++;
	}

	// Make sure buffer is not full
	if (next == circ_buf->read)
	{
		// Buffer is full
		return FALSE;
	}

	// Add data to buffer
	*circ_buf->write = data;

	// Advance pointer
	circ_buf->write = next;

	return TRUE;
}

u16_t circ_buf_wr_data(circ_buf_t * circ_buf, const u8_t * data,
		u16_t bytes_to_write)
{
	u8_t * next;
	u16_t bytes_written = 0;

	while (bytes_to_write)
	{
		// Calculate next position of in pointer
		next = circ_buf->write;
		if (next == circ_buf->end)
		{
			// Wrap pointer to start of buffer
			next = circ_buf->start;
		}
		else
		{
			// Increment pointer
			next++;
		}
		// Make sure buffer is not full
		if (next == circ_buf->read)
		{
			// Buffer is full
			break;
		}

		// Add data to buffer
		*circ_buf->write = *data++;

		// Advance pointer
		circ_buf->write = next;

		// Next byte
		bytes_written++;
		bytes_to_write--;
	}

	return bytes_written;
}

bool_t circ_buf_rd_u8(circ_buf_t * circ_buf, u8_t * data)
{
	u8_t * next;

	// See if there is data in the buffer
	if (circ_buf->write == circ_buf->read)
	{
		// Buffer is empty
		return FALSE;
	}

	// Fetch data
	*data = *circ_buf->read;

	// Calculate next position of out pointer
	next = circ_buf->read;
	if (next == circ_buf->end)
	{
		// Wrap pointer to start of buffer
		next = circ_buf->start;
	}
	else
	{
		// Increment pointer
		next++;
	}

	// Advance pointer
	circ_buf->read = next;

	return TRUE;
}

u16_t circ_buf_rd_data(circ_buf_t * circ_buf, u8_t * data, u16_t bytes_to_read)
{
	u8_t * next;
	u16_t bytes_read = 0;

	while (bytes_to_read)
	{
		// See if there is data in the buffer
		if (circ_buf->write == circ_buf->read)
		{
			// Buffer is empty
			break;
		}
		// Fetch data
		*data++ = *circ_buf->read;

		// Calculate next position
		next = circ_buf->read;
		if (next == circ_buf->end)
		{
			// Wrap pointer to start of buffer
			next = circ_buf->start;
		}
		else
		{
			// Increment pointer
			next++;
		}

		// Advance pointer
		circ_buf->read = next;

		// Next byte
		bytes_read++;
		bytes_to_read--;
	}

	return bytes_read;
}

/*
 * Read from_char and ends with the to_char
 * return data holds the full string and return a count o chars
 */
u16_t circ_buf_rd_from_to_char(circ_buf_t* cb , u8_t* ret_data ,  const u8_t to_char)
{
	u8_t * next;
	u16_t bytes_read = 0;

	do
	{
		// See if there is data in the buffer
		if (circ_buf_empty(cb))
		{
			// Buffer is empty
			break;
		}
		// Fetch data
		*ret_data++ = *cb->read;

		// Calculate next position
		next = cb->read;
		if (next == cb->end)
		{
			// Wrap pointer to start of buffer
			next = cb->start;
		}
		else
		{
			// Increment pointer
			next++;
		}

		// Advance pointer
		cb->read = next;

		// Next byte
		bytes_read++;
	}while (*cb->read != to_char);

	return bytes_read;

}
