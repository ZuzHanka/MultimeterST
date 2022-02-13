#include "avg_filter.hpp"

#ifdef SLAVE_TEMP_IFDEF
uint16_t AvgFilter::m_no_samples = 1;
#else
uint16_t AvgFilter::m_no_samples = 40;
#endif
