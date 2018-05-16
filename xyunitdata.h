// xyunitdata.h
//
// header file for XYUnitData class of BBT-X036-SUM module.
// (c) 2014 Hirofumi Fujii (KEK)
//

#ifndef XYUNITDATA_H_INCLUDED
#define XYUNITDATA_H_INCLUDED

#include <iostream>

namespace BBTX036MULTI
{
class XYUnitClock
{
public:
	static const int BYTESIZE = 8;

public:
	XYUnitClock ();
	XYUnitClock (const unsigned char* clockbuf);
	XYUnitClock (unsigned int nanosec);
	virtual ~XYUnitClock ();

	// arithmetic operators
	const XYUnitClock operator + (const XYUnitClock& xyclock2) const;
	const XYUnitClock operator - (const XYUnitClock& xyclock2) const;

	// comparison operators
	bool operator == (const XYUnitClock& xyclock2) const;
	bool operator != (const XYUnitClock& xyclock2) const;
	bool operator < (const XYUnitClock& xyclock2) const;
	bool operator <= (const XYUnitClock& xyclock2) const;
	bool operator > (const XYUnitClock& xyclock2) const;
	bool operator >= (const XYUnitClock& xyclock2) const;

	// helper function
	double microsec () const;
	// returns micro-sec counts

	double nanosec () const;
	// returns nano-sec counts

protected:
	void normalize ();
	
protected:
	unsigned char m_clockbuf[BYTESIZE];
};


class XYUnitData
{
public:
	static const int HEADERSIZE = 4;	// header size in bytes
	static const int DATASIZE = 44;		// data size in bytes including the header
	static const int XSTARTBYTE =  4;	// X block start byte
	static const int NUMXBYTES = 16;	// number of bytes for X block
	static const int YSTARTBYTE = 20;	// Y block start byte
	static const int NUMYBYTES = 16;	// number of bytes for Y block
	static const int TSTARTBYTE = 36;	// Time (clock) block start byte
	static const int NUMTBYTES = 8;		// nunber of bytes for Time (clock) block
	static const int NUMXCHANNELS = 100;	// number of X channels
	static const int NUMYCHANNELS = 100;	// number of Y channels

// formatted i/o
friend std::ostream& operator << (std::ostream& os, const XYUnitData& xyunitdata);
friend std::istream& operator >> (std::istream& is, XYUnitData& xyunitdata);

private:
	static const unsigned char m_header[HEADERSIZE];
	static const int m_numhits[256];
	static const int m_sumhits[256];
	static const int m_numclst[256];
	static const int m_bitpos[8];
	static const unsigned int m_smask[8];
	static const unsigned int m_emask[8];

public:
	XYUnitData ();
	virtual ~XYUnitData ();

	// arithmetic operator
	const XYUnitData operator | (const XYUnitData & xyunitdata2) const;

	int numXhits (int startch = 0, int endch = (NUMXCHANNELS - 1)) const
	// returns number of hit channels in X group
		{ return numhits (XSTARTBYTE, startch, endch); }

	int numYhits (int startch = 0, int endch = (NUMYCHANNELS - 1)) const
	// returns number of hit channels in Y group
		{ return numhits (YSTARTBYTE, startch, endch); }

	int numXclusters (int startch = 0, int endch = (NUMXCHANNELS - 1)) const
	// returns number of clusters in X group
		{ return numclst (XSTARTBYTE, startch, endch); }

	int numYclusters (int startch = 0, int endch = (NUMYCHANNELS - 1)) const
	// returns number of clusters in Y group
		{ return numclst (YSTARTBYTE, startch, endch); }

	int sumXhits (int startch = 0, int endch = (NUMYCHANNELS - 1)) const
	// returns sum of hit positions in X group
		{ return sumhits (XSTARTBYTE, startch, endch); }

	int sumYhits (int startch = 0, int endch = (NUMYCHANNELS - 1)) const
	// returns sum of hit positions in Y group
		{ return sumhits (YSTARTBYTE, startch, endch); }

	bool Xhit (int nch) const;
	// hit test for channel number 'nch' in X group

	bool Yhit (int nch) const;
	// hit test for channel number 'nch' in Y group

	bool ready () const { return (m_numdat == DATASIZE); }
	// test for data ready

	const XYUnitClock clockcount () const;

	double microsec () const;
	// returns micro-sec counts

	double nanosec () const;
	// returns nano-sec counts

	bool Xbittest (int nbit) const;
	bool Ybittest (int nbit) const;
	bool Tbittest (int nbit) const;

private:
	int numhits (int ns, int startch, int endch) const;
	int numclst (int ns, int startch, int endch) const;
	int sumhits (int ns, int startch, int endch) const;
	bool hittest (int ns, int nch) const;
	bool checkdata ();

private:
	int m_numdat;
	unsigned char m_datbuf[DATASIZE];
};

std::ostream&
operator << (std::ostream& os, const XYUnitData& xyunitdata);

std::istream&
operator >> (std::istream& is, XYUnitData& xyunitdata);

}	// namespace BBTX036MULTI

#endif	// XYUNITDATA_H_INCLUDED
