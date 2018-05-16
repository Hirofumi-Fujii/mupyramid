// sglrate.h
//
// Single rate for single run
//

#ifndef SGLRATE_H_INCLUDED
#define SGLRATE_H_INCLUDED

#include <iostream>
#include <string>
#include "xyunitdata.h"

namespace MUONDAQ
{
class SingleRate
{
public:
	static const int MAX_DAQBOXES = 4;
	static const int NUM_XCHANNELS = 100;
	static const int NUM_YCHANNELS = 100;
	static const int MAX_TMHIST = 200;
	static const int MAX_THHIST = 200;
	static const int MAX_TUHIST = 200;
	static const int MAX_TNHIST = 200;
	static const int MAX_XCLUST = 6;
	static const int MAX_YCLUST = 6;

public:
	SingleRate ();
	virtual ~SingleRate ();
	void Clear ();
	bool Cumulate (std::istream& is);
	void CSVwrite (std::ostream& os) const;

protected:
	double m_totalfiles;
	double m_totaldatrec;
	double m_duration;
	double m_unithits [MAX_DAQBOXES];
	double m_xhits [MAX_DAQBOXES][NUM_XCHANNELS];
	double m_yhits [MAX_DAQBOXES][NUM_YCHANNELS];
	BBTX036MULTI::XYUnitClock m_prev_clock[MAX_DAQBOXES];
	double m_tmhist [MAX_DAQBOXES][MAX_TMHIST];
	double m_thhist [MAX_DAQBOXES][MAX_THHIST];
	double m_tuhist [MAX_DAQBOXES][MAX_TUHIST];
	double m_tnhist [MAX_DAQBOXES][MAX_TNHIST];
	double m_xyclust [MAX_DAQBOXES][MAX_YCLUST][MAX_XCLUST];

private:
	int m_numdaqboxes;
	int m_daqboxid[256];
	char m_daqboxname[MAX_DAQBOXES];
};

}	// namespace MUONDAQ

#endif	// SGLRATE_H_INCLUDED
