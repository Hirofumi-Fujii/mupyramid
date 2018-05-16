// mergedata.cpp
// g++ -Wall mergedata.cpp mergeopt.cpp xyunitdata.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o mergedata
//
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "mergeopt.h"
#include "gzfstream.h"
#include "xyunitdata.h"

static const int MAX_DAQBOXES = 4;
	double m_unithits [MAX_DAQBOXES];
	double m_events [MAX_DAQBOXES];
	BBTX036MULTI::XYUnitClock m_strt_clock[MAX_DAQBOXES];
	BBTX036MULTI::XYUnitClock m_tail_clock[MAX_DAQBOXES];
	BBTX036MULTI::XYUnitData m_merged_data[MAX_DAQBOXES];
	int m_numdaqboxes;
	int m_daqboxid[256];
	char m_daqboxname[MAX_DAQBOXES];

int
main (int argc, char* argv[])
{
	MUONDAQ::Mergeopt opt;
	opt.m_outfilename = "mergedata.dat.gz";
	if (!opt.set (argc, argv))
	{
		opt.usage (std::cout, argv[0]);
		return (-1);
	}

	// open the input file
	mylibrary::igzfstream ifs (opt.m_infilename.c_str());

	if (!ifs )
	{
		std::cerr << "ERROR input file ("
			<< opt.m_infilename
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	mylibrary::ogzfstream ofs (opt.m_outfilename.c_str());
	if (!ofs)
	{
		std::cerr << "ERROR output file ("
			<< opt.m_outfilename
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	ofs << "INFO ";
	for (int i = 0; i < argc; i++)
		ofs << ' ' << argv[i];
	ofs << std::endl;

	/// Initialization ///
	BBTX036MULTI::XYUnitClock mergew(opt.m_mergewidth);
	bool fixedwidth = opt.m_fixedwidth;

	m_numdaqboxes = 0;
	for (int i = 0; i < MAX_DAQBOXES; i++)
	{
		m_unithits [i] = 0.0;
		m_events [i] = 0.0;
	}
	for (int i = 0; i < 256; i++)
		m_daqboxid [i] = MAX_DAQBOXES;

	/// MAIN LOOP ///
	std::string sline;
	bool runefound = false;
	std::string runerec;
	unsigned long unummerged = 0;
	while (getline (ifs, sline))
	{
		if (sline.size () > 4)
		{
			if (sline.substr (0, 3) == "DAT")
			{
				char c = sline [3];
				int unitno = m_daqboxid[(int)(c) & 255];
				if ((unitno >= 0) && (unitno < MAX_DAQBOXES))
				{
					std::stringstream ss (sline.substr(4));
					unsigned long trignum;
					ss >> trignum;
					BBTX036MULTI::XYUnitData xydata;
					ss >> std::hex >> xydata;
					if (xydata.ready())
					{
						if (m_unithits [unitno] > 0.0)
						{
							if (xydata.clockcount () > m_tail_clock [unitno])
							{
								// Enough separated, i.e., new event
								// Dump the current merged data
								++unummerged;
								unsigned int oldprec = ofs.precision ();
								ofs.precision (20);
								ofs << "MRG" << m_daqboxname [unitno]
									<< std::dec << ' ' << unummerged
									<< ' ' << m_unithits [unitno]
									<< ' ' << m_strt_clock [unitno].microsec ()
									<< ' ' << m_strt_clock [unitno].nanosec ()
									<< ' ' << m_tail_clock [unitno].microsec ()
									<< ' ' << m_tail_clock [unitno].nanosec ()
									<< std::endl;
								ofs << "DAT" << m_daqboxname [unitno]
									<< std::dec << ' ' << unummerged
									<< ' ' << std::hex << std::setw(2)
									<< std::setfill('0')
									<< m_merged_data [unitno]
									<< std::endl;
								ofs.precision (oldprec);
								//
								// clear the buffer
								m_events [unitno] += 1.0;
								m_unithits [unitno] = 0.0;
							}
						}
						if (m_unithits [unitno] < 1.0)
						{
							// 1st data in the event
							m_merged_data [unitno] = xydata;
							m_strt_clock [unitno] = xydata.clockcount ();
							m_tail_clock [unitno] = xydata.clockcount () + mergew;
						}
						else
						{
							m_merged_data [unitno] = m_merged_data [unitno] | xydata;
							if (!fixedwidth)
								m_tail_clock [unitno] = xydata.clockcount () + mergew;
						}
						m_unithits [unitno] += 1.0;
					}
					else
						std::cerr << "XYdata error" << sline.substr(4) << std::endl;
				}
			}
			else if (sline.substr (0, 3) == "TWI")
			{
				ofs << sline << std::endl;
				char c = sline [3];
				int uid = (int)(c) & 255;
				if (m_daqboxid[uid] >= MAX_DAQBOXES)
				{
					if (m_numdaqboxes >= MAX_DAQBOXES)
					{
						std::cerr << "Number of DAQboxes exceeds MAX_DAQBOXES("
							<< MAX_DAQBOXES << ")."
							<< std::endl;
						return false;
					}
					m_daqboxid[uid] = m_numdaqboxes;
					m_daqboxname[m_numdaqboxes] = c;
					++m_numdaqboxes;
				}
			}
			else if (sline.substr (0, 4) == "RUNE")
			{
				runefound = true;
				runerec = sline;
			}
			else
				ofs << sline << std::endl;
		}
	}
	// Flush out the pending data
	//
	for (int i = 0; i < m_numdaqboxes; i++)
	{
		if (m_unithits [i] > 0.0)
		{
			++unummerged;
			unsigned int oldprec = ofs.precision ();
			ofs.precision (20);
			ofs << "MRG" << m_daqboxname [i]
				<< std::dec << ' ' << unummerged
				<< ' ' << m_unithits [i]
				<< ' ' << m_strt_clock [i].microsec ()
				<< ' ' << m_strt_clock [i].nanosec ()
				<< ' ' << m_tail_clock [i].microsec ()
				<< ' ' << m_tail_clock [i].nanosec ()
				<< std::endl;
			ofs << "DAT" << m_daqboxname [i]
				<< std::dec << ' ' << unummerged
				<< ' ' << std::hex << std::setw(2)
				<< std::setfill('0')
				<< m_merged_data [i]
				<< std::endl;
			ofs.precision (oldprec);
		}
	}

	if (runefound)
		ofs << runerec << std::endl;
	return 0;
}
