// coinmake.h
//
// 108
//   2018-05-22
//     clockoffset added.
// 107  
//   2015-12-15 for Pyramid system; 
//     special_coin() and write_special() are removed.
//
// 104
//   - correct when the speical DAQbox hit but other list is empty.
//

#ifndef COINMAKE_H_INCLUDED
#define COINMAKE_H_INCLUDED

#include <iostream>
#include "coinopt.h"
#include "databank.h"
// #include "xyunitdata.h"

namespace MUONDAQ
{
class Coinmake
{
public:
	static const int VERSION = 108;
	static const int L_CHANNEL_S = 0;
	static const int L_CHANNEL_E = 59;
	static const int H_CHANNEL_S = 0;
	static const int H_CHANNEL_E = 59;

public:
	Coinmake (Coinopt& opt);
	virtual ~Coinmake ();
	int make (std::istream& is, std::ostream& os);

protected:
	unsigned int m_coinwidth;
	unsigned int m_mergewidth;
	bool m_singledump;
	bool m_mergedump;
	bool m_longdump;
	unsigned int m_toffset[256];
	BBTX036MULTI::XYUnitClock m_clockoffset [DataBank::NUM_DAQBOXES];
};

}	// namespace MUONDAQ

#endif	// COINMAKE_H_INCLUDED
