// xyunitgeom.cpp

#include "xyunitgeom.h"

namespace MUONEHALL
{


XYUnitGeometry::XYUnitGeometry (const Position& pos, const Size& sz,
			const ChannelProp& xchprop, const ChannelProp& ychprop, int order ) :
		m_pos(pos), m_size(sz), m_xchprop(xchprop), m_ychprop(ychprop), m_order(order)
{
	init ();
}

void
XYUnitGeometry::init ()
{

	// m_xzpos is the z-position of the X-plane
	// m_yzpos is the z-position of the Y-plane
	// These parameters depend on the m_order.

	if (m_order == ORDER_XY)
	{
		m_xzpos = m_pos.z () - (m_size.z() / 2.0);
		m_yzpos = m_pos.z () + (m_size.z() / 2.0);
	}
	else
	{
		m_xzpos = m_pos.z () + (m_size.z() / 2.0);
		m_yzpos = m_pos.z () - (m_size.z() / 2.0);
	}

	// Channel parameters
	m_numxch = m_xchprop.maxch () - m_xchprop.minch () + 1;
	m_xchsize = m_size.x() / (double(m_numxch));
	m_xcntrch = double(m_xchprop.maxch() + m_xchprop.minch()) / 2.0;

	m_numych = m_ychprop.maxch () - m_ychprop.minch () + 1;
	m_ychsize = m_size.y() / (double(m_numych));
	m_ycntrch = double(m_xchprop.maxch() + m_xchprop.minch()) / 2.0;
}

double
XYUnitGeometry::ch2pos (double ch, double cntrch, double chsize, int dir) const
{
	double dch = (ch - cntrch) * chsize;
	if (dir == DIR_NEG)
		dch = -dch;
	return dch;
}

double
XYUnitGeometry::xchpos (double ch) const
{
	double dpos = ch2pos (ch, m_xcntrch, m_xchsize, m_xchprop.dir());
	return (dpos + m_pos.x());
}

double
XYUnitGeometry::ychpos (double ch) const
{
	double dpos = ch2pos (ch, m_ycntrch, m_ychsize, m_ychprop.dir());
	return (dpos + m_pos.y());
}

}	// namespace MUONEHALL
