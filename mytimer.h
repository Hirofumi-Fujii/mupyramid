// mytimer.h
#ifndef MYTIMER_H_INCLUDED
#define MYTIMER_H_INCLUDED

#include <iostream>
#include <ctime>

namespace mylibrary
{

class MyTimer
{
public:
	MyTimer();
	virtual ~MyTimer();
	time_t start() { return (m_stop_time = m_start_time = time(0)); }
	time_t stop() { return (m_stop_time = time(0)); }
	time_t start_time() const { return m_start_time; }
	time_t stop_time() const { return m_stop_time; }
	double difftime() const;
	std::ostream& csvout(std::ostream& os) const;

protected:
	time_t m_start_time;
	time_t m_stop_time;
};
	
}	// namespace mylibrary

#endif	// MYTIMER_H_INCLUDED
