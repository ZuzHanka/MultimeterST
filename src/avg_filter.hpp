#ifndef AVG_FILTER_HPP_
#define AVG_FILTER_HPP_

#include <cstdint>
#include <cstdio>

class AvgFilter
{
public:
	AvgFilter()
	:
		m_no_samples(100),
		m_idx(0),
		m_sum(0),
		m_average(0),
		m_is_new_average(false)
	{
	}

	~AvgFilter() {}

	uint16_t update(uint16_t sample)
	{
		m_sum += sample;
		m_idx++;
		if (m_idx >= m_no_samples)
		{
			m_sum += m_idx >> 1;  // division by 2 to correct rounded value
			m_average = m_sum / m_idx;
			m_idx = 0;
			m_sum = 0;
			m_is_new_average = true;
		}

		return m_average;
	}

	uint16_t get()
	{
		m_is_new_average = false;
		return m_average;
	}

	bool is_new_average()
	{
		return m_is_new_average;
	}

	void set_no_samples(uint16_t no)
	{
		m_no_samples = no;
	}


private:
	uint16_t m_no_samples;

	uint16_t m_idx;

	uint32_t m_sum;

	volatile uint16_t m_average;

	volatile bool m_is_new_average;
};

#endif  // AVG_FILTER_HPP_
