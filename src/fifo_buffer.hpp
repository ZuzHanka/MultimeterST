/*
 * fifo_buffer.hpp
 *
 *  Created on: 7 Feb 2021
 *      Author: ZuzHanka
 */

#ifndef FIFO_BUFFER_HPP_
#define FIFO_BUFFER_HPP_

#include <cstdint>
#include <cstring>


class FIFObuffer
{
public:
	FIFObuffer()
	:
		m_start(0),
		m_end(0)
	{
	}

	~FIFObuffer() {}

	void push(const char * data, size_t data_len)
	{
		for (; data_len > 0; data_len--)
		{
			size_t tmp_end = (m_end + 1) % (BUFFER_SIZE);
			if (tmp_end == m_start) break;
			m_buffer[m_end] = *data;
			data++;
			m_end = tmp_end;
		}
	}

	size_t pop(char * data, size_t data_len)
	{
		size_t count = 0;
		for (; data_len > 0; data_len--)
		{
			size_t tmp_start = m_start;
			if (tmp_start == m_end) break;
			*data = m_buffer[tmp_start];
			data++;
			m_start = (tmp_start + 1) % (BUFFER_SIZE);
			count++;
		}
		return count;
	}

private:
	static const size_t BUFFER_SIZE = 1024;

	char m_buffer[BUFFER_SIZE];

	volatile size_t m_start;
	volatile size_t m_end;

};



#endif /* FIFO_BUFFER_HPP_ */
