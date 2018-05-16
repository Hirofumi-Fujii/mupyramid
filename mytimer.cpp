// mytimer.cpp

#include "mytimer.h"

namespace mylibrary
{

MyTimer::MyTimer()
{
	m_stop_time = m_start_time = time(0);
}

MyTimer::~MyTimer()
{
}

double
MyTimer::difftime() const
{
	return (std::difftime(m_stop_time, m_start_time));
}

std::ostream&
MyTimer::csvout(std::ostream& os) const
{
	double dt = std::difftime(m_stop_time, m_start_time);

	char tstrs[64];
	char tstre[64];
	char* ptstr = asctime(localtime(&m_start_time));
	for (int i = 0; i < 64; i++)
	{
		tstrs[i] = 0;
		if ((*ptstr == '\n') || (*ptstr == 0))
			break;
		tstrs[i] = *ptstr++;
	}
	ptstr = asctime(localtime(&m_stop_time));
	for (int i = 0; i < 64; i++)
	{
		tstre[i] = 0;
		if ((*ptstr == '\n') || (*ptstr == 0))
			break;
		tstre[i] = *ptstr++;
	}

	os << "\"localtime\",\"start\",\"" << tstrs
		<< "\",\"stop\",\"" << tstre
		<< "\",\"difftime(sec)\"," << dt;

	return os;
}

}	// namespace mylibrary
