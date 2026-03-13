//std
#include <algorithm>

//Rigid body
#include "Rigid-body/inc/Interval.hpp"

//constructors
Interval::Interval(void) : m_min(0), m_max(0)
{
	return;
}
Interval::Interval(double min, double max) : m_min(min), m_max(max)
{
	return;
}

//destructor
Interval::~Interval(void)
{
	return;
}

bool Interval::is_empty(void) const
{
	return m_min >= m_max || abs(m_min - m_max) < 1e-5 * std::max(abs(m_min), abs(m_max));
}
Interval Interval::fusion(const Interval& interval) const
{
	const double min = std::min(m_min, interval.m_min);
	const double max = std::max(m_max, interval.m_max);
	const bool t1 = m_max > interval.m_min && m_min < interval.m_max;
	const bool t2 = interval.m_max > m_min && interval.m_min < m_max;
	return t1 || t2 ? Interval(min, max) : Interval(0, 0);
}
Interval Interval::intersection(const Interval& interval) const
{
	const double min = std::max(m_min, interval.m_min);
	const double max = std::min(m_max, interval.m_max);
	return is_empty() || interval.is_empty() ? Interval(0, 0) : Interval(min, max);
}