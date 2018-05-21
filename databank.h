// databank.h
// makes independent stream of each XYunit from multiplexed stream.

#ifndef DATABANK_H_INCLUDED
#define DATABANK_H_INCLUDED

#include <iostream>
#include <list>
#include "datarecord.h"

namespace MUONDAQ
{

class DataBank
{
public:
	static const int INVALID_DAQBOXNO = -1;
	static const unsigned int NUM_DAQBOXES = 4;

public:
	DataBank ();
	const DataRecord getrecord (unsigned int unitno,
		std::istream& is, std::ostream& os,
		bool outflag = true);
	unsigned int numdaqboxes () const { return m_numdaqboxes; }
	char daqboxchar (unsigned int idx) const
		{ if (idx < NUM_DAQBOXES) return m_daqboxchar[idx]; return (char)(0); }
	int daqboxno (char boxchar) const
		{ return m_daqboxno[(unsigned int)(boxchar) & 255]; }

public:
	std::list<DataRecord> m_datalist[NUM_DAQBOXES];

private:
	unsigned int m_numdaqboxes;
	int m_daqboxno[256];
	char m_daqboxchar[NUM_DAQBOXES];
};

}	// namespace MUONDAQ

#endif	// DATABANK_H_INCLUDED
