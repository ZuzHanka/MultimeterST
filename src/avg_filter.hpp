#include <cstdint>
#include <cstdio>

class AvgFilter
{
public:
	AvgFilter();

	~AvgFilter() {}

	uint16_t update(uint16_t sample)
	{
		m_sum = m_sum - m_buffer[m_idx_samples] + sample;
		m_buffer[m_idx_samples] = sample;
		m_idx_samples = ((m_idx_samples + 1) >= m_no_samples) ? (0) : (m_idx_samples + 1);
		m_average = (uint16_t) (m_sum / m_no_samples);
		return m_average;
	}

	uint16_t get()
	{
		return m_average;
	}

	bool set_no_samples(size_t no);

private:
	static const size_t BUFFER_SIZE = 100;

	size_t m_no_samples;

	size_t m_idx_samples;

	uint32_t m_sum;

	uint16_t m_buffer[BUFFER_SIZE];

	volatile uint16_t m_average;
};
