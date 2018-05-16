// xyunitgeom.h
//
#ifndef XYUNITGEOM_H_INCLUDED
#define XYUNITGEOM_H_INCLUDED

namespace MUONEHALL
{
class XYUnitGeometry
{
public:
	enum { ORDER_XY = 0, ORDER_YX };
	enum { DIR_NEG = -1, DIR_POS = 1 };

public:
	class Position
	{
	public:
		Position (double xpos, double ypos, double zpos) :
			m_x(xpos), m_y(ypos), m_z(zpos) { return; }
		double x () const { return m_x; }
		double y () const { return m_y; }
		double z () const { return m_z; }

	protected:
		double m_x;
		double m_y;
		double m_z;
	};

	class Size
	{
	public:
		Size (double xsize, double ysize, double zsize) :
			m_x(xsize), m_y(ysize), m_z(zsize) { return; }
		double x () const { return m_x; }
		double y () const { return m_y; }
		double z () const { return m_z; }

	protected:
		double m_x;
		double m_y;
		double m_z;
	};

	class ChannelProp
	{
	public:
		ChannelProp (int minch, int maxch, int dir) :
			m_minch(minch), m_maxch(maxch), m_dir(dir) { return; }
		int minch () const { return m_minch; }
		int maxch () const { return m_maxch; }
		int dir () const { return m_dir; }

	protected:
		int m_minch;
		int m_maxch;
		int m_dir;
	};

	XYUnitGeometry (const Position& pos, const Size& sz,
		const ChannelProp& xchprop, const ChannelProp& ychprop, int order);

	const Position& position () const { return m_pos; }
	const Size& size () const { return m_size; }
	const ChannelProp& xchannelprop () const { return m_xchprop; }
	const ChannelProp& ychannelprop () const { return m_ychprop; }
	int order () const { return m_order; }

	double zx () const { return m_xzpos; }
	double zy () const { return m_yzpos; }
	double xchsize () const { return m_xchsize; }
	double ychsize () const { return m_ychsize; }
	double xchpos (double ch) const;
	double ychpos (double ch) const;

private:
	void init ();
	double ch2pos (double ch, double cntrch, double chsize, int dir) const;

protected:
	Position m_pos;
	Size m_size;
	ChannelProp m_xchprop;
	ChannelProp m_ychprop;
	int m_order;

	// The followings are depend on the above parameters
	int m_numxch;
	int m_numych;
	double m_xzpos;
	double m_yzpos;
	double m_xcntrch;
	double m_ycntrch;
	double m_xchsize;
	double m_ychsize;
};

}	// namespace MUONDAQ

#endif	// XYUNITGEOM_H_INCLUDD
