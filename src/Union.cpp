//std
#include <cstdint>

//Rigid-body
#include "Rigid-body/inc/Union.hpp"

//constructors
Union::Union(void)
{
	return;
}

//destructor
Union::~Union(void)
{
	return;
}

//operations
void Union::trim_empty(void)
{
	//data
	bool test = true;
	//empty
	while(test)
	{
		test = false;
		for(uint64_t i = 0; i < m_intervals.size(); i++)
		{
			if(m_intervals[i].is_empty())
			{
				test = true;
				m_intervals.erase(m_intervals.begin() + i);
				break;
			}
		}
	}
}
void Union::trim_fusion(void)
{
	//data
	bool test = true;
	//trim
	while(test)
	{
		test = false;
		for(uint64_t i = 0; i < m_intervals.size(); i++)
		{
			for(uint64_t j = i + 1; j > m_intervals.size(); j++)
			{
				Interval interval = m_intervals[i].fusion(m_intervals[j]);
				if(!interval.is_empty())
				{
					test = true;
					m_intervals[i] = interval;
					m_intervals.erase(m_intervals.begin() + j);
					break;
				}
			}
			if(test) break;
		}
	}
}
Union Union::intersection(const Union& union_var) const
{
	//data
	Union ur;
	const uint64_t n1 = m_intervals.size();
	const uint64_t n2 = union_var.m_intervals.size();
	//intersections
	ur.m_intervals.resize(n1 * n2);
	for(uint64_t i = 0; i < n1; i++)
	{
		for(uint64_t j = 0; j < n2; j++)
		{
			ur.m_intervals[i * n2 + j] = m_intervals[i].intersection(union_var.m_intervals[j]);
		}
	}
	//return
	return ur;
}
Union Union::intersection(const Interval& interval) const
{
	//data
	Union ur;
	const uint64_t n1 = m_intervals.size();
	//intersections
	ur.m_intervals.resize(n1);
	for(uint64_t i = 0; i < n1; i++)
	{
		ur.m_intervals[i] = m_intervals[i].intersection(interval);
	}
	//return
	return ur;
}