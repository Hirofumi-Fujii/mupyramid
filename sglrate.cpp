// sglrate.cpp
// 

#include <iostream>
#include <sstream>
#include "sglrate.h"
#include "xyunitdata.h"

namespace MUONDAQ
{

SingleRate::SingleRate ()
{
	Clear ();
}

SingleRate::~SingleRate ()
{
}

void
SingleRate::Clear ()
{
	// Initialize DAQbox table
	m_numdaqboxes = 0;
	for (int i = 0; i < 256; i++)
		m_daqboxid [i] = MAX_DAQBOXES;
	for (int i = 0; i < MAX_DAQBOXES; i++)
		m_daqboxname [i] = (char)(0);

	// Clear the hit histograms
	m_totalfiles = 0;
	m_totaldatrec = 0;
	m_duration = 0;
	for (int i = 0; i < MAX_DAQBOXES; i++)
	{
		m_unithits [i] = 0.0;
		m_tufirst [i] = 0.0;
		m_tulast [i] = 0.0;
		for (int x = 0; x < NUM_XCHANNELS; x++)
			m_xhits [i][x] = 0.0;
		for (int y = 0; y < NUM_YCHANNELS; y++)
			m_yhits [i][y] = 0.0;
		for (int t = 0; t < MAX_TMHIST; t++)
			m_tmhist [i][t] = 0.0;
		for (int t = 0; t < MAX_THHIST; t++)
			m_thhist [i][t] = 0.0;
		for (int t = 0; t < MAX_TUHIST; t++)
			m_tuhist [i][t] = 0.0;
		for (int t = 0; t < MAX_TNHIST; t++)
			m_tnhist [i][t] = 0.0;
		for (int j = 0; j < MAX_YCLUST; j++)
			for (int k = 0; k < MAX_XCLUST; k++)
				m_xyclust [i][j][k] = 0.0;
	}
}

bool
SingleRate::Cumulate (std::istream& is)
{
	std::string sline;
	while (getline (is, sline))
	{
		if (sline.size () > 4)
		{
			if (sline.substr (0, 3) == "DAT")
			{
				m_totaldatrec += 1.0;
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
						for (int x = 0; x < NUM_XCHANNELS; x++)
						{
							if (xydata.Xhit (x))
								m_xhits [unitno][x] += 1.0;
						}
						for (int y = 0; y < NUM_YCHANNELS; y++)
						{
							if (xydata.Yhit (y))
								m_yhits [unitno][y] += 1.0;
						}
						int ncx = xydata.numXclusters ();
						if (ncx < 0)
							ncx = 0;
						if (ncx >= MAX_XCLUST)
							ncx = MAX_XCLUST - 1;
						int ncy = xydata.numYclusters ();
						if (ncy < 0)
							ncy = 0;
						if (ncy >= MAX_YCLUST)
							ncy = MAX_YCLUST - 1;
						m_xyclust [unitno][ncy][ncx] += 1.0;
						if (m_unithits [unitno] > 0.0)
						{
							BBTX036MULTI::XYUnitClock clkdiff
								= xydata.clockcount () - m_prev_clock [unitno];
							double tu = clkdiff.microsec ();
							double th = tu / 100.0;
							double tm = tu / 1000.0;
							if (tu > 200.0)
								tu = 200.0;
							double tn = clkdiff.nanosec ();
							double tt = tu * 1000.0 + tn;
							int nm = (int)(tm + 0.5);
							if (nm < 0)
								nm = 0;
							if (nm >= MAX_TMHIST)
								nm = MAX_TMHIST - 1;
							m_tmhist [unitno][nm] += 1.0;
							int nh = (int)(th + 0.5);
							if (nh < 0)
								nh = 0;
							if (nh >= MAX_THHIST)
								nh = MAX_THHIST - 1;
							m_thhist [unitno][nh] += 1.0;
							int nu = (int)(tu + 0.5);
							if (nu < 0)
								nu = 0;
							if (nu >= MAX_TUHIST)
								nu = MAX_TUHIST - 1;
							m_tuhist [unitno][nu] += 1.0;
							int nn = (int)(tt + 0.5);
							if (nn < 0)
								nn = 0;
							if (nn >= MAX_TNHIST)
								nn = MAX_TNHIST - 1;
							m_tnhist [unitno][nn] += 1.0;
						}
						else
							m_tufirst [unitno] = xydata.clockcount ().microsec ();
						m_tulast [unitno] = xydata.clockcount ().microsec ();
						m_prev_clock [unitno] = xydata.clockcount ();
						m_unithits [unitno] += 1.0;
					}
					else
						std::cerr << "XYdata error" << sline.substr(4) << std::endl;
				}
			}
			else if (sline.substr (0, 3) == "TWI")
			{
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
		}
	}
	return true;
}

void
SingleRate::CSVwrite (std::ostream& os) const
{
	unsigned uoldprec = os.precision ();
	os.precision (20);

	os << "DAQboxes," << m_numdaqboxes << std::endl;
	os << "Name";
	for (int i = 0; i < m_numdaqboxes; i++)
		os << ',' << "BOX" << m_daqboxname[i];
	os << std::endl;

	os << "TotalHits";
	for (int i = 0; i < m_numdaqboxes; i++)
		os << ',' << m_unithits [i];
	os << std::endl;

	os << "FirstClock(micro-sec)";
	for (int i = 0; i < m_numdaqboxes; i++)
		os << ',' << m_tufirst [i];
	os << std::endl;

	os << "LastClock(micro-sec)";
	for (int i = 0; i < m_numdaqboxes; i++)
		os << ',' << m_tulast [i];
	os << std::endl;

	os << "X-channel";
	for (int j = 0; j < NUM_XCHANNELS; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "U" << m_daqboxname[i] << "X";
		for (int j = 0; j < NUM_XCHANNELS; j++)
		{
			os << ',' << m_xhits[i][j];
		}
		os << std::endl;
	}

	os << "Y-channel";
	for (int j = 0; j < NUM_YCHANNELS; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "U" << m_daqboxname[i] << "Y";
		for (int j = 0; j < NUM_YCHANNELS; j++)
		{
			os << ',' << m_yhits[i][j];
		}
		os << std::endl;
	}

	os << "T(milli)";
	for (int j = 0; j < MAX_TMHIST; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "T" << m_daqboxname[i] << "M";
		for (int j = 0; j < MAX_TMHIST; j++)
		{
			os << ',' << m_tmhist[i][j];
		}
		os << std::endl;
	}

	os << "T(100micro)";
	for (int j = 0; j < MAX_TMHIST; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "T" << m_daqboxname[i] << "H";
		for (int j = 0; j < MAX_THHIST; j++)
		{
			os << ',' << m_thhist[i][j];
		}
		os << std::endl;
	}

	os << "T(micro)";
	for (int j = 0; j < MAX_TUHIST; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "T" << m_daqboxname[i] << "U";
		for (int j = 0; j < MAX_TUHIST; j++)
		{
			os << ',' << m_tuhist[i][j];
		}
		os << std::endl;
	}

	os << "T(nano)";
	for (int j = 0; j < MAX_TNHIST; j++)
		os << ',' << j;
	os << std::endl;

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "T" << m_daqboxname[i] << "N";
		for (int j = 0; j < MAX_TNHIST; j++)
		{
			os << ',' << m_tnhist[i][j];
		}
		os << std::endl;
	}

	for (int i = 0; i < m_numdaqboxes; i++)
	{
		os << "Clusters" << m_daqboxname[i] << std::endl;
		os << "Y/X";
		for (int k = 0; k < MAX_XCLUST; k++)
			os << ',' << k;
		os << std::endl;
		for (int j = 0; j < MAX_YCLUST; j++)
		{
			os << j;
			for (int k = 0; k < MAX_XCLUST; k++)
				os << ',' << m_xyclust[i][j][k];
			os << std::endl;
		}
	}

	os.precision(uoldprec);

}

}	// namespace MUONDAQ
