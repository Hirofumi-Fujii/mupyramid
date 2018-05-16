// coinrecord.cpp

#include "coinrecord.h"

namespace MUONDAQ
{

/////// CoinXYUnit ///////

CoinXYUnit::CoinXYUnit ()
{
	m_dataready = 0;
	m_numxclust = 0;
	m_numyclust = 0;
	m_numxhits = 0;
	m_numyhits = 0;
	m_xav = -1;
	m_yav = -1;
	m_usec = -1.0;
	m_nsec = -1.0;
}

CoinXYUnit::~CoinXYUnit ()
{
}

std::istream&
CoinXYUnit::Read (std::istream& is)
{
	m_dataready = 0;
	int n;

	// X plane
	if (!(is >> n))
		return is;
	m_numxhits = n;
	if (!(is >> n))
		return is;
	m_numxclust = n;
	if (!(is >> n))
		return is;
	m_xav = n;

	// Y plane
	if (!(is >> n))
		return is;
	m_numyhits = n;
	if (!(is >> n))
		return is;
	m_numyclust = n;
	if (!(is >> n))
		return is;
	m_yav = n;

	double d;
	// Clock
	if (!(is >> d))
		return is;
	m_usec = d;
	if (!(is >> d))
		return is;
	m_nsec = d;

	// All clean
//@@@	if ((m_numxhits > 0) && (m_numyhits > 0))
	// In the case of Pyramid system, one of
	// m_numxhits or m_numyhits may be zero.
	if ((m_numxhits > 0) || (m_numyhits > 0))
		m_dataready = 1;
	return is;
}

/////// CoinRecord ///////

CoinRecord::CoinRecord ()
{
	Clear ();
}

CoinRecord::~CoinRecord ()
{
}

void
CoinRecord::Clear ()
{
	for (int i = 0; i < NUM_DAQBOXES; i++)
		m_numdat[i] = 0;
}

std::istream&
CoinRecord::Read (std::istream& is)
{
	// Read after "COIN"
	Clear ();

	int ndat[NUM_DAQBOXES];
	for (int i = 0; i < NUM_DAQBOXES; i++)
		ndat[i] = 0;

	for (int i = 0; i < NUM_DAQBOXES; i++)
	{
		int n;
		if (!(is >> n))
			return is;
		ndat[i] = n;
	}
	for (int i = 0; i < NUM_DAQBOXES; i++)
	{
		if (!m_xy[i].Read (is))
			return is;
	}
	for (int i = 0; i < NUM_DAQBOXES; i++)
	{
		if (m_xy[i].m_dataready)
			m_numdat[i] = ndat[i];
	}
	return is;
}

const CoinXYUnit&
CoinRecord::XYUnit (int unitno) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		unitno = 0;
	return m_xy[unitno];
}

int
CoinRecord::numdat (int unitno) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return 0;
	return m_numdat[unitno];
}

bool
CoinRecord::xy1cluster (int unitno, int maxhits) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return false;
	if (m_numdat[unitno] <= 0)
		return false;
	if ((m_xy[unitno].m_numxclust != 1) ||
		(m_xy[unitno].m_numyclust != 1))
		return false;
	if (maxhits <= 0)
		return true;
	if ((m_xy[unitno].m_numxhits > maxhits) ||
		(m_xy[unitno].m_numyhits > maxhits))
		return false;
	return true;
}

bool
CoinRecord::xyclusters (int unitno, int maxhits) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return false;
	if (m_numdat[unitno] <= 0)
		return false;
	if ((m_xy[unitno].m_numxclust < 1) ||
		(m_xy[unitno].m_numyclust < 1))
		return false;
	if (maxhits <= 0)
		return true;
	if ((m_xy[unitno].m_numxhits > maxhits) ||
		(m_xy[unitno].m_numyhits > maxhits))
		return false;
	return true;
}

int
CoinRecord::xpos (int unitno, int dir) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return (-1);
	if (m_numdat[unitno] <= 0)
		return (-1);
	if (m_xy[unitno].m_numxhits <= 0)
		return (-1);
	if (dir < 0)
		return (MAX_XPOS - m_xy[unitno].m_xav);
	return m_xy[unitno].m_xav;
}

int
CoinRecord::ypos (int unitno, int dir) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return (-1);
	if (m_numdat[unitno] <= 0)
		return (-1);
	if (m_xy[unitno].m_numyhits <= 0)
		return (-1);
	if (dir < 0)
		return (MAX_YPOS - m_xy[unitno].m_yav);
	return m_xy[unitno].m_yav;
}

double
CoinRecord::microsec (int unitno) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return (-1.0);
	if (m_numdat[unitno] <= 0)
		return (-1.0);
	return m_xy[unitno].m_usec;
}

double
CoinRecord::nanosec (int unitno) const
{
	if ((unitno < 0) || (unitno >= NUM_DAQBOXES))
		return (-1.0);
	if (m_numdat[unitno] <= 0)
		return (-1.0);
	return m_xy[unitno].m_nsec;
}

// iostream 
std::ostream&
operator << (std::ostream& os, const CoinRecord& coinrec)
{
	// not implemented yet.
	return os;
}

std::istream&
operator >> (std::istream& is, CoinRecord& coinrec)
{
	return coinrec.Read (is);
}

}	// namespace MUONDAQ
