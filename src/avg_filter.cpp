#include "avg_filter.hpp"

AvgFilter::AvgFilter()
:
	m_average(0)
{
	(void) set_no_samples(BUFFER_SIZE);
}

bool AvgFilter::set_no_samples(size_t no)
{
	if (no > BUFFER_SIZE)
	{
		return false;
	}

	for (size_t i = 0; i < no; i++)
	{
		m_buffer[i] = m_average;
	}
	m_sum = m_average * no;
	m_no_samples = no;
	m_idx_samples = 0;

	return true;
}
