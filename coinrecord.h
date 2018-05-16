// coinrecord.h
//
#ifndef COINRECORD_H_INCLUDED
#define COINRECORD_H_INCLUDED

#include <iostream>

namespace MUONDAQ
{
class CoinXYUnit
{
public:
	CoinXYUnit ();
	virtual ~CoinXYUnit ();
	std::istream& Read (std::istream& is);

public:
	int m_dataready;
	int m_numxhits;
	int m_numxclust;
	int m_xav;
	int m_numyhits;
	int m_numyclust;
	int m_yav;
	double m_usec;
	double m_nsec;
};

class CoinRecord
{
public:
	static const int NUM_DAQBOXES = 3;
	static const int MAX_XPOS = 1190;
	static const int MAX_YPOS = 1190;
public:
	CoinRecord ();
	virtual ~CoinRecord ();
	void Clear ();
	std::istream& Read (std::istream& is);
	const CoinXYUnit& XYUnit (int unitno) const;
	int numdat (int unitno) const;
	bool xy1cluster (int unitno, int maxhits=0) const;
	bool xyclusters (int unitno, int maxhits=0) const;
	int xpos (int unitno, int dir=1) const;
	int ypos (int unitno, int dir=1) const;
	int rxpos (int unitno) const { return xpos (unitno, -1); }
	int rypos (int unitno) const { return ypos (unitno, -1); }
	double microsec (int unitno) const;
	double nanosec (int unitno) const;

protected:
	int m_numdat[NUM_DAQBOXES];
	CoinXYUnit m_xy[NUM_DAQBOXES];
};

std::ostream&
operator << (std::ostream& os, const CoinRecord& coinrec);

std::istream&
operator >> (std::istream& is, CoinRecord& coinrec);

}	// namespace MUONDAQ
#endif	// COINRECORD_H_INCLUDED
