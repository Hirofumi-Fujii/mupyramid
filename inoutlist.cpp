// inoutlist.cpp

#include "inoutlist.h"

namespace mylibrary
{

InOut::~InOut()
{
}

InOutList::~InOutList()
{
}

const InOutList&
InOutList::merge (const InOut& inout)
{
	double rnewin = inout.rin();
	double rnewout = inout.rout();

	std::list<InOut>::iterator p = m_list.begin();

	while( p != m_list.end() )
	{
		double rin = p->rin();
		double rout = p->rout();
		if (rnewout < rin)
		{
			m_list.insert(p, InOut(rnewin, rnewout));
			return *this;
		}
		else if (rnewout <= rout)
		{
			if (rnewin < rin)
				*p = InOut(rnewin, rout);
			return *this;
		}

		if (rnewin <= rout)
		{
			if (rnewin > rin)
				rnewin = rin;
			p = m_list.erase( p );
		}
		else
			++p;
	}
	m_list.push_back( InOut(rnewin, rnewout) );
	return *this;
}

const InOutList&
InOutList::merge (const InOutList& list)
{
	std::list<InOut>::const_iterator p = list.m_list.begin();
	while ( p != list.m_list.end() )
		merge( *p++ );
	return *this;
}

const InOutList&
InOutList::remove (const InOut& inout)
{
	double rnewin = inout.rin();
	double rnewout = inout.rout();

	std::list<InOut>::iterator p = m_list.begin();

	while ( p != m_list.end() )
	{
		double rin = p->rin();
		double rout = p->rout();
		if (rnewout < rin)
		{
			return *this;
		}
		else if (rnewout < rout)
		{
			if (rnewin > rin)
			{ 
				p = m_list.insert(p, InOut(rin, rnewin));
				++p;
			}
			*p = InOut(rnewout, rout);
			return *this;
		}

		// Comes here if rnewout >= rout
		if (rnewin <= rin)
		{
			p = m_list.erase( p );
		}
		else
		{
			if (rnewin <= rout)
				*p = InOut(rin, rnewin);
			++p;
		}
	}
	return *this;
}

const InOutList&
InOutList::remove (const InOutList& list)
{
	std::list<InOut>::const_iterator p = list.m_list.begin();
	while ( p != list.m_list.end() )
		remove( *p++ );
	return *this;
}

double
InOutList::totallength() const
{
	double totlen = 0.0;
	std::list<InOut>::const_iterator p = m_list.begin();
	while ( p != m_list.end() )
	{
		totlen += p->length();
		++p;
	}
	return totlen;
}
	
}	// namespace mylibrary
