// inoutlist.h
#ifndef INOUTLIST_H_INCLUDED
#define INOUTLIST_H_INCLUDED

#include <cmath>
#include <cfloat>
#include <list>

namespace mylibrary
{
class InOut
{
public:
	InOut() : m_rin(DBL_MAX), m_rout(DBL_MAX) { return; }
	InOut(double rin, double rout) : m_rin(rin), m_rout(rout)
		{ if (rout >= rin) return; m_rin = rout; m_rout = rin; return; }
	virtual ~InOut();
	double length() const { return (m_rout - m_rin); }
	double rin() const { return m_rin; }
	double rout() const { return m_rout;}

protected:
	double m_rin;
	double m_rout;
};

class InOutList
{
public:
	InOutList() : m_numelem(0) { return; }
	virtual ~InOutList();
	const InOutList& merge(const InOut& inout);
	const InOutList& merge(const InOutList& list);
	const InOutList& remove(const InOut& inout);
	const InOutList& remove(const InOutList& list);
	double totallength() const;
	bool empty() const { return m_list.empty(); }
	int size() const { return m_list.size(); }

protected:
	int m_numelem;
	std::list<InOut> m_list;
};

}	// namespace mylibrary

#endif	// INOUTLIST_H_INCLUDED
