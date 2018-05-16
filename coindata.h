// coindata.h

#ifndef COINDATA_H_INCLUDED
#define COINDATA_H_INCLUDED

namespace MUONDAQ
{
class CoinUnitData
{
public:
	CoinUnitData ();
	virtual ~CoinUnitData ();

public:
	int m_numdata;
	int m_numXclusters;
	int m_numYclusters;
	int m_numXhits;
	int m_numYhits;
	int m_xaverage;
	int m_yaverage;
	double m_usec;
	double m_nsec;
};

}	// namespace MUONDAQ
#endif	// COINDATA_H_INCLUDED
