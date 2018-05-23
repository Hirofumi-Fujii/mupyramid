// coinmake.cpp

#include "coinmake.h"
#include <iomanip>
#include <list>
#include <ctime>
#include "coinopt.h"
#include "gzfstream.h"
#include "datarecord.h"
#include "databank.h"

namespace MUONDAQ
{

Coinmake::Coinmake (Coinopt& opt)
{
	m_coinwidth = opt.m_coinwidth;
	m_mergewidth = opt.m_mergewidth;
	m_singledump = opt.m_singledump;
	m_mergedump = opt.m_mergedump;
	m_longdump = opt.m_longdump;
	for (int i = 0; i < 256; i++)
		m_toffset[i] = opt.m_toffset[i];
	// 
}

Coinmake::~Coinmake ()
{
}

int
Coinmake::make (std::istream& is, std::ostream& os)
{
	time_t ts;
	time_t te;
	char tmstr[26];		// buffer for asctime() function; this function always
				// returns 26 characters includeing '\n' and  0.

	time (&ts);
	char* ptm = asctime(localtime(&ts));
	for (int i = 0; i < 24; i++)
		tmstr[i] = *ptm++;
	tmstr[24] = 0;

	// write information
	os << "INFO "
		<< "Coinmake version " << Coinmake::VERSION << std::endl;
	os << "INFO "
		<< "Coinmake compiled " << __DATE__ << ' ' << __TIME__ << std::endl;
	os << "INFO "
		<< "Coinmake make() start " << tmstr << std::endl;
	os << "INFO "
		<< "Coinmake.m_mergewidth " << m_mergewidth
		<< std::endl;
	os << "INFO "
		<< "Coinmake.m_coinwidth " << m_coinwidth
		<< std::endl;
	os << "INFO "
		<< "Coinmake.m_mergedump " << m_mergedump
		<< std::endl;
	os << "INFO "
		<< "Coinmake.m_longdump " << m_longdump
		<< std::endl;

	BBTX036MULTI::XYUnitClock coinwidth (m_coinwidth);
	BBTX036MULTI::XYUnitClock mergewidth (m_mergewidth);

	MUONDAQ::DataBank databank;
	std::list<MUONDAQ::DataRecord> mergelist[MUONDAQ::DataBank::NUM_DAQBOXES];
	MUONDAQ::DataRecord recnow[MUONDAQ::DataBank::NUM_DAQBOXES];
	BBTX036MULTI::XYUnitData xymerged[MUONDAQ::DataBank::NUM_DAQBOXES];

	// Making the merged data for each DAQBox
	int nready = 0;
	for (unsigned int u = 0; u < MUONDAQ::DataBank::NUM_DAQBOXES; u++)
	{
		recnow[u] = databank.getrecord (u, is, os);
		mergelist[u].clear ();
		xymerged[u] = recnow[u].xyunitdata();
		if (recnow[u].ready())
		{
			++nready;
			BBTX036MULTI::XYUnitClock tms = recnow[u].clockcount();
			BBTX036MULTI::XYUnitClock tme = tms + mergewidth;
			mergelist[u].push_back (recnow[u]);
			recnow[u] = databank.getrecord (u, is, os);
			while (recnow[u].ready() && (recnow[u].clockcount() <= tme))
			{
				mergelist[u].push_back (recnow[u]);
				xymerged[u] = xymerged[u] | recnow[u].xyunitdata ();
				tms = recnow[u].clockcount();
				tme = tms + mergewidth;
				recnow[u] = databank.getrecord (u, is, os);
			}
		}
	}

	// Set the m_clockoffset [] and 
	// Write the additional INFO
	os << "INFO "
		<< "databank.numdaqboxes() = " << databank.numdaqboxes ()
		<< std::endl;
	for (unsigned int u = 0; u < databank.numdaqboxes (); u++)
	{
		char cid = databank.daqboxchar (u);
		unsigned int uoffs = m_toffset [(unsigned int)(cid) & 255];
		m_clockoffset [u] = BBTX036MULTI::XYUnitClock (uoffs);
		os << "INFO "
			<< "m_clockoffset[" << u << "] = m_toffset[" << cid << "] = " << uoffs
			<< std::endl;
	}

	// Dump the DAQBox information
	os << "DAQB " << databank.numdaqboxes ();
	for (unsigned int u = 0; u < databank.numdaqboxes (); u++)
		os << ' ' << databank.daqboxchar (u);
	os << std::endl;

	// unit pairs
	unsigned int upairs = databank.numdaqboxes () / 2;
	os << "DAQP " << upairs;
	for (unsigned int u = 0; u < upairs; u++)
		os << ' ' << databank.daqboxchar (u * 2) << ' ' << databank.daqboxchar (u * 2 + 1);
	os << std::endl;

	for (unsigned int u = 0; u < databank.numdaqboxes (); u++)
	{
		os << "DCH" << databank.daqboxchar (u)
			<< " 0 59 0 59 0 59 60 119" << std::endl;
	}

	//
	// Making the coincidence record.
	//
	while (1)
	{
		// Find the earliest unit.
		int earliest = -1;
		nready = 0;
		for (unsigned int u = 0; u < MUONDAQ::DataBank::NUM_DAQBOXES; u++)
		{
			if (xymerged[u].ready())
			{
				++nready;
				if (earliest < 0)
					earliest = (int)(u);
				else if ((xymerged[earliest].clockcount() + m_clockoffset[earliest])
						 > (xymerged[u].clockcount() + m_clockoffset[u]))
					earliest = (int)(u);
			}
		}
		if (nready < 2)
			break;
		// Check the coincidence
		std::list<MUONDAQ::DataRecord> coinlist;		
		int ncoin[MUONDAQ::DataBank::NUM_DAQBOXES] = { 0 };
		BBTX036MULTI::XYUnitData xycoin[MUONDAQ::DataBank::NUM_DAQBOXES];
		BBTX036MULTI::XYUnitClock xytms = xymerged[earliest].clockcount() + m_clockoffset[earliest];
		BBTX036MULTI::XYUnitClock xytme = xytms + coinwidth;
		for (unsigned int u = 0; u < MUONDAQ::DataBank::NUM_DAQBOXES; u++)
		{
			if (xymerged[u].ready() && ((xymerged[u].clockcount() + m_clockoffset[u]) <= xytme))
			{
				// inside the coincidence window
				ncoin[u] += 1;
				// Move the data to the coincidence buffer
				xycoin[u] = xymerged[u];
				if (!mergelist[u].empty())
				{
					std::list<MUONDAQ::DataRecord>::iterator it = mergelist[u].begin();
					while (it != mergelist[u].end())
					{
						coinlist.push_back(*it);
						++it;
					}
				}

				// Make the new merged data.
				// First data is in recnow[].
				mergelist[u].clear ();
				xymerged[u] = recnow[u].xyunitdata();
				if (recnow[u].ready())
				{
					++nready;
					BBTX036MULTI::XYUnitClock tms = recnow[u].clockcount() + m_clockoffset[u];
					BBTX036MULTI::XYUnitClock tme = tms + mergewidth;
					mergelist[u].push_back (recnow[u]);
					recnow[u] = databank.getrecord (u, is, os);
					while (recnow[u].ready() &&
						((recnow[u].clockcount() + m_clockoffset[u]) <= tme))
					{
						mergelist[u].push_back (recnow[u]);
						xymerged[u] = xymerged[u] | recnow[u].xyunitdata ();
						tms = recnow[u].clockcount() + m_clockoffset[u];
						tme = tms + mergewidth;
						recnow[u] = databank.getrecord (u, is, os);
					}
				}
			}
		}
		int ncoinunits = 0;
		int ncoinpairs = 0;
		for (unsigned int u = 0; u < MUONDAQ::DataBank::NUM_DAQBOXES; u++)
			if (ncoin[u] > 0)
				ncoinunits += 1;
		for (unsigned int u = 0; u < upairs; u++)
			if ((ncoin[u * 2] > 0) || (ncoin[u * 2 + 1] > 0))
				ncoinpairs += 1;

		if ((ncoinpairs >= 2) || (m_singledump && (ncoinpairs > 0)))
		{
			os << "COIN" << std::dec;
			for (unsigned int u = 0; u < 3; u++)
			{
				int npcoin = 0;
				if (u < upairs)
				{
					if (ncoin[u * 2] > 0)
						npcoin += 1;
					if (ncoin[u * 2 + 1] > 0)
						npcoin += 2;
				}
				os << ' ' << npcoin;
			}
			
			for (unsigned int u = 0; u < 3; u++)
			{
				int nxh = 0;
				int nxc = 0;
				int xav = 0;
				int nyh = 0;
				int nyc = 0;
				int yav = 0;
				double msec = 0.0;
				double nsec = 0.0;
				if (u < upairs)
				{
					unsigned int ux = u * 2;
					unsigned int uy = u * 2 + 1;
					nxh = xycoin[ux].numXhits() + xycoin[ux].numYhits();
					nxc = xycoin[ux].numXclusters() + xycoin[ux].numYclusters();
					if ((xycoin[ux].numXhits() > 0) && (xycoin[ux].numYhits() > 0))
					{
						if (xycoin[ux].Xhit(59) && xycoin[ux].Yhit(0))
							nxc -= 1;
					}
					if (nxh > 0)
					{
						int sum = (xycoin[ux].numYhits() * 60)
							+ xycoin[ux].sumXhits()
							+ xycoin[ux].sumYhits();
						double av = (double)(sum * 10) / (double)(nxh);
						xav = (int)(av + 0.5);
					}
					nyh = xycoin[uy].numXhits() + xycoin[uy].numYhits();
					nyc = xycoin[uy].numXclusters() + xycoin[uy].numYclusters();
					if ((xycoin[uy].numXhits() > 0) && (xycoin[uy].numYhits() > 0))
					{
						if (xycoin[uy].Xhit(59) && xycoin[uy].Yhit(0))
							nyc -= 1;
					}
					if (nyh > 0)
					{
						int sum = (xycoin[uy].numYhits() * 60)
							+ xycoin[uy].sumXhits()
							+ xycoin[uy].sumYhits();
						double av = (double)(sum * 10) / (double)(nyh);
						yav = (int)(av + 0.5);
					}

					unsigned int ut = ux;
					if ((nxh > 0) && (nyh > 0))
					{
						if ((xycoin[uy].clockcount() + m_clockoffset[uy])
							> (xycoin[ux].clockcount() + m_clockoffset[ux]))
							ut = uy;
					}
					else if (nyh > 0)
						ut = uy;
					BBTX036MULTI::XYUnitClock coinclock = xycoin[ut].clockcount() + m_clockoffset[ut];
					msec = coinclock.microsec();
					nsec = coinclock.nanosec();
				}
				os
					<< std::dec
					<< ' ' << nxh
					<< ' ' << nxc
					<< ' ' << xav
					<< ' ' << nyh
					<< ' ' << nyc
					<< ' ' << yav
					<< ' ' << std::fixed << std::setprecision(0) << msec
					<< ' ' << std::fixed << std::setprecision(0) << nsec;
			}
			os << std::endl;
			// write the merged records
			if (m_mergedump)
			{
				os << "MRGS" << std::endl;
				for (unsigned int u = 0; u < MUONDAQ::DataBank::NUM_DAQBOXES; u++)
				{
					if (ncoin[u] > 0)
					{
						os << "DAT" << databank.daqboxchar(u) << ' '
							<< std::dec << ncoin[u] << ' '
							<< std::hex << std::setw(2) << std::setfill('0')
							<< xycoin[u]
							<< std::endl;
					}
				}
				os << "MRGE" << std::endl;
			}
			// write the original records
			if (m_longdump && (!coinlist.empty()))
			{
				os << "RAWS" << std::endl;
				std::list<MUONDAQ::DataRecord>::iterator it = coinlist.begin();
				while (it != coinlist.end())
				{
					os << it->sourcestr() << std::endl;
					++it;
				}
				os << "RAWE" << std::endl;
			}
		}				
	}
	time (&te);
	ptm = asctime (localtime(&te));
	for (int i = 0; i < 24; i++)
		tmstr[i] = *ptm++;
	tmstr[24] = 0;
	os << "INFO "
		<< "Coinmake make() end   " << tmstr << std::endl;

	return 0;
}

}	// namespace MUONDAQ
