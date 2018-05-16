// coindata.cpp

#include "coindata.h"

namespace MUONDAQ
{
CoinData::CoinData (std::string& strsrc)
{
	for (int i = 0; i < NUM_UNITS; i++)
	{
		m_numdata[i] = 0;
		m_numXclusts[i] = 0;
		m_numYclusts[i] = 0;
		m_numXhits[i] = 0;
		m_numYhits[i] = 0;
		m_xav[i] = -1;
		m_yav[i] = -1;
		m_usec[i] = 0.0;
		m_nsec[i] = 0.0;
	}
}

}	// namespace MUONDAQ
