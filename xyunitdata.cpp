// xyunitdata.cpp

#include "xyunitdata.h"
#include <iomanip>

namespace BBTX036MULTI
{
const unsigned char
XYUnitData::m_header[] =
{
	0xff, 0xff, 0x55, 0x55,
};

const int
XYUnitData::m_numhits[] = 
{
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
};

const int
XYUnitData::m_sumhits[] =
{
	  0,  7,  6, 13,  5, 12, 11, 18,  4, 11, 10, 17,  9, 16, 15, 22,
	  3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
	  2,  9,  8, 15,  7, 14, 13, 20,  6, 13, 12, 19, 11, 18, 17, 24,
	  5, 12, 11, 18, 10, 17, 16, 23,  9, 16, 15, 22, 14, 21, 20, 27,
	  1,  8,  7, 14,  6, 13, 12, 19,  5, 12, 11, 18, 10, 17, 16, 23,
	  4, 11, 10, 17,  9, 16, 15, 22,  8, 15, 14, 21, 13, 20, 19, 26,
	  3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
	  6, 13, 12, 19, 11, 18, 17, 24, 10, 17, 16, 23, 15, 22, 21, 28,
	  0,  7,  6, 13,  5, 12, 11, 18,  4, 11, 10, 17,  9, 16, 15, 22,
	  3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
	  2,  9,  8, 15,  7, 14, 13, 20,  6, 13, 12, 19, 11, 18, 17, 24,
	  5, 12, 11, 18, 10, 17, 16, 23,  9, 16, 15, 22, 14, 21, 20, 27,
	  1,  8,  7, 14,  6, 13, 12, 19,  5, 12, 11, 18, 10, 17, 16, 23,
	  4, 11, 10, 17,  9, 16, 15, 22,  8, 15, 14, 21, 13, 20, 19, 26,
	  3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
	  6, 13, 12, 19, 11, 18, 17, 24, 10, 17, 16, 23, 15, 22, 21, 28,
};

const int
XYUnitData::m_numclst[] =
{
	 0, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 1,
	 1, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 2, 3, 3, 3, 3, 4, 3, 3, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 2, 3, 3, 3, 3, 4, 3, 3, 2, 3, 3, 3, 2, 3, 2, 2,
	 2, 3, 3, 3, 3, 4, 3, 3, 3, 4, 4, 4, 3, 4, 3, 3,
	 2, 3, 3, 3, 3, 4, 3, 3, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 2, 3, 3, 3, 3, 4, 3, 3, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2,
	 1, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1,
};

const int
XYUnitData::m_bitpos[] =
{
	128, 64, 32, 16, 8, 4, 2, 1,
};

const unsigned int
XYUnitData::m_smask[] =
{
	0x00ff, 0x007f, 0x003f, 0x001f, 0x000f, 0x0007, 0x0003, 0x0001,
};

const unsigned int
XYUnitData::m_emask[] =
{
	0x0080, 0x00c0, 0x00e0, 0x00f0, 0x00f8, 0x00fc, 0x00fe, 0x00ff,
};

XYUnitClock::XYUnitClock()
{
	for (int i = 0; i < BYTESIZE; i++)
		this->m_clockbuf[i] = 0;
}

XYUnitClock::XYUnitClock(const unsigned char* clockbuf)
{
	if (clockbuf == 0)
	{
		for (int i = 0; i < BYTESIZE; i++)
			this->m_clockbuf[i] = 0;
	}
	else
	{
		for (int i = 0; i < BYTESIZE; i++)
			this->m_clockbuf[i] = *clockbuf++;
		normalize ();
	}
}

XYUnitClock::XYUnitClock (unsigned int nanosec)
{
	for (int i = 0; i < BYTESIZE; i++)
		this->m_clockbuf[i] = 0;

	unsigned int u1 = nanosec / 1000;
	unsigned int u2 = nanosec % 1000;
	this->m_clockbuf[BYTESIZE - 1] = (unsigned char)(u2 & 0x00ff);
	this->m_clockbuf[BYTESIZE - 2] = (unsigned char)((u2 >> 8) & 0x0003);
	if (u1 == 0)
		return;
	u1 = (u1 << 2);
	this->m_clockbuf[BYTESIZE - 2] |= (unsigned char)(u1 & 0x00fc);
	for (int i = (BYTESIZE - 3); i >= 0; i--)
	{
		if (u1 == 0)
			break;
		u1 = (u1 >> 8);
		this->m_clockbuf[i] = (unsigned char)(u1 & 0x00ff);
	}
}

XYUnitClock::~XYUnitClock()
{
}

void
XYUnitClock::normalize ()
{
	// check the nano-sec count.
	// If it exceeds 1000, set the value to 999.
	// Micro-sec part does not change.

	// nano-sec part
	unsigned int u = 
		(((unsigned int)(this->m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x0003)) << 8)
		+ ((unsigned int)(this->m_clockbuf[BYTESIZE - 1]) & (unsigned int)(0x00ff));
	if (u < 1000)
		return;	// nothing to do.

	// set the nano-sec count = 999
	u = 999;

	this->m_clockbuf[BYTESIZE - 1] = (unsigned char)(u & 0x00ff);
	this->m_clockbuf[BYTESIZE - 2] = (this->m_clockbuf[BYTESIZE - 2] & (unsigned char)(0xfc))
		| (unsigned char)((u >> 8) & (unsigned int)(0x003));
}

const XYUnitClock
XYUnitClock::operator + (const XYUnitClock& xyclock2) const
{
	XYUnitClock xyclock1;
	// nano-sec counter (last 10 bits)
	unsigned int u1 = 
		(((unsigned int)(this->m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x0003)) << 8)
		+ ((unsigned int)(this->m_clockbuf[BYTESIZE - 1]) & (unsigned int)(0x00ff));
	unsigned int u2 =
		(((unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x0003)) << 8)
		+ ((unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 1]) & (unsigned int)(0x00ff));
	unsigned int u = u1 + u2;
	unsigned int uov = 0;
	if (u >= 1000)
	{
		uov = 4;	// micro-sec counter starts from bit3
		u = u - 1000;
	}
	xyclock1.m_clockbuf[BYTESIZE - 1] = (unsigned char)(u & 0x00ff);
	xyclock1.m_clockbuf[BYTESIZE - 2] = (unsigned char)((u >> 8) & 0x0003);
	
	// micro-sec counter
	u1 = (unsigned int)(this->m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x00fc);
	u2 = (unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x00fc);
	u = u1 + u2 + uov;
	xyclock1.m_clockbuf[BYTESIZE - 2] |= (unsigned char)(u & 0x00fc);
	for (int i = (BYTESIZE - 3); i >= 0; i--)
	{
		uov = (u >> 8) & (unsigned int)(0x00ff);
		u1 = (unsigned int)(this->m_clockbuf[i]) & (unsigned int)(0x00ff);
		u2 = (unsigned int)(xyclock2.m_clockbuf[i]) & (unsigned int)(0x00ff);
		u = u1 + u2 + uov;
		xyclock1.m_clockbuf[i] = (unsigned char)(u & 0x00ff);
	}
	return xyclock1;
}

const XYUnitClock
XYUnitClock::operator - (const XYUnitClock& xyclock2) const
{
	XYUnitClock xyclock1;
	// nano-sec counter (last 10 bits)
	unsigned int u1 = 
		(((unsigned int)(this->m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x0003)) << 8)
		+ ((unsigned int)(this->m_clockbuf[BYTESIZE - 1]) & (unsigned int)(0x00ff));
	if (u1 > 999)
		u1 = 999;
	unsigned int u2 =
		(((unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x0003)) << 8)
		+ ((unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 1]) & (unsigned int)(0x00ff));
	if (u2 > 999)
		u2 = 999;
	unsigned uuf = 0;
	if (u2 > u1)
	{
		u1 += 1000;
		uuf = 4;	// micro-sec counter starts from bit3
	}
	unsigned int u = u1 - u2;
	xyclock1.m_clockbuf[BYTESIZE - 1] = (unsigned char)(u & 0x00ff);
	xyclock1.m_clockbuf[BYTESIZE - 2] = (unsigned char)((u >> 8) & 0x0003);
	
	// micro-sec counter
	u1 = (unsigned int)(this->m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x00fc);
	u2 = ((unsigned int)(xyclock2.m_clockbuf[BYTESIZE - 2]) & (unsigned int)(0x00fc)) + uuf;
	uuf = 0;
	if (u2 > u1)
	{
		u1 += 256;
		uuf = 1;
	}
	u = u1 - u2;
	xyclock1.m_clockbuf[BYTESIZE - 2] |= (unsigned char)(u & 0x00fc);
	for (int i = (BYTESIZE - 3); i >= 0; i--)
	{
		u1 = (unsigned int)(this->m_clockbuf[i]) & (unsigned int)(0x00ff);
		u2 = ((unsigned int)(xyclock2.m_clockbuf[i]) & (unsigned int)(0x00ff)) + uuf;
		uuf = 0;
		if (u2 > u1)
		{
			u1 += 256;
			uuf = 1;
		}
		u = u1 - u2;
		xyclock1.m_clockbuf[i] = (unsigned char)(u & 0x00ff);
	}
	return xyclock1;
}

bool
XYUnitClock::operator == (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] != xyclock2.m_clockbuf[i])
			return false;
	}
	return true;
}

bool
XYUnitClock::operator != (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] != xyclock2.m_clockbuf[i])
			return true;
	}
	return false;
}

bool
XYUnitClock::operator < (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] > xyclock2.m_clockbuf[i])
			return false;
		else if (this->m_clockbuf[i] < xyclock2.m_clockbuf[i])
			return true;
	}
	return false;
}

bool
XYUnitClock::operator <= (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] > xyclock2.m_clockbuf[i])
			return false;
		else if (this->m_clockbuf[i] < xyclock2.m_clockbuf[i])
			return true;
	}
	return true;
}

bool
XYUnitClock::operator > (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] < xyclock2.m_clockbuf[i])
			return false;
		else if (this->m_clockbuf[i] > xyclock2.m_clockbuf[i])
			return true;
	}
	return false;
}

bool
XYUnitClock::operator >= (const XYUnitClock& xyclock2) const
{
	for (int i = 0; i < XYUnitClock::BYTESIZE; i++)
	{
		if (this->m_clockbuf[i] < xyclock2.m_clockbuf[i])
			return false;
		else if (this->m_clockbuf[i] > xyclock2.m_clockbuf[i])
			return true;
	}
	return true;
}

double
XYUnitClock::microsec () const
{
	// NOTE:
	// The digits of the IEEE 754 binary64 floating point format
	// is 52(+1 hidden) bits. On the other hand, the micro-sec
	// counter of the BBT-X036 is 54 bits so that most significant
	// 2 bits will be lost. However, 2^52 microsec is more than
	// 140 years, this limitation may not cause significant
	// problem.

	double t = 0.0;
	// first 48 bits (= 6 bytes)
	for (int i = 0; i < 6; i++)
		t = t * 256.0 + double(m_clockbuf[i] & 255);
	// the rest 6 bits
	t = t * 64.0 + double((m_clockbuf[6] >> 2) & 63);
	return t;
}

double
XYUnitClock::nanosec () const
{
	// 10-bit counter
	double t = double(m_clockbuf[6] & 3) * 256.0
		+ double(m_clockbuf[7] & 255);
	return t;
}


/////// XYUnitData ///////

XYUnitData::XYUnitData() : m_numdat(0)
{
	for (int i = 0; i < DATASIZE; i++)
		m_datbuf[i] = 0;
}

XYUnitData::~XYUnitData()
{
}

const XYUnitData
XYUnitData::operator | (const XYUnitData& xyunitdata2) const
{
	XYUnitData xyunitdata1;

	if (!this->ready())
	{
		if (xyunitdata2.ready())
		{
			for (int i = 0; i < DATASIZE; i++)
				xyunitdata1.m_datbuf[i] = xyunitdata2.m_datbuf[i];
			xyunitdata1.m_numdat = xyunitdata2.m_numdat;
		}
		return xyunitdata1;
	}
	else if (!xyunitdata2.ready())
	{
		for (int i = 0; i < DATASIZE; i++)
			xyunitdata1.m_datbuf[i] = this->m_datbuf[i];
		xyunitdata1.m_numdat = this->m_numdat;
		return xyunitdata1;
	}

	for (int i = 0; i < HEADERSIZE; i++)
		xyunitdata1.m_datbuf[i] = xyunitdata2.m_datbuf[i];

	for (int i = HEADERSIZE; i < TSTARTBYTE; i++)
		xyunitdata1.m_datbuf[i] =
			(this->m_datbuf[i] | xyunitdata2.m_datbuf[i]);

	bool earier = true;
	for (int i = 0; i < NUMTBYTES; i++)
	{
		if (xyunitdata2.m_datbuf[i + TSTARTBYTE] <
			this->m_datbuf[i + TSTARTBYTE])
		{
			earier = false;
			break;
		}
		else if (xyunitdata2.m_datbuf[i + TSTARTBYTE] >
			this->m_datbuf[i + TSTARTBYTE])
		{
			earier = true;
			break;
		}
	}

	if (earier)
	{
		for (int i = 0; i < NUMTBYTES; i++)
			xyunitdata1.m_datbuf[i + TSTARTBYTE] =
				xyunitdata2.m_datbuf[i + TSTARTBYTE];
	}
	else
	{
		for (int i = 0; i < NUMTBYTES; i++)
			xyunitdata1.m_datbuf[i + TSTARTBYTE] =
				this->m_datbuf[i + TSTARTBYTE];
	}
	xyunitdata1.m_numdat = xyunitdata2.m_numdat;
	return xyunitdata1;
}

int
XYUnitData::numhits (int ns, int startch, int endch) const
{
	// ns is the start byte index (count from 0) in m_datbuf[]
	// startch is the start channel number
	// endch is the end channel number
	// NO RANGE CHECK (this is private function), check the access range before calling.
	// Access range is [ns,ns+12].
	// 100 bits = 12 octects + 4 bits

	int nhit = 0;
	int isbyte = startch / 8;
	int isbit = startch % 8;
	int iebyte = endch / 8;
	int iebit = endch % 8;
	for (int i = isbyte; i <= iebyte; i++)
	{
		unsigned int u = m_datbuf[ns + i] & 255;
		if (i == isbyte)
			u &= m_smask[isbit];
		if (i == iebyte)
			u &= m_emask[iebit];
		nhit += m_numhits[u];
	}
	return nhit;
}

int
XYUnitData::numclst (int ns, int startch, int endch) const
{
	// ns is the start byte index (count from 0) in m_datbuf[]
	// startch is the start channel number
	// endch is the end channel number
	// NO RANGE CHECK (this is private function), check the access range before calling.
	// Access range is [ns,ns+12].
	// 100 bits = 12 octects + 4 bits

	int nclst = 0;
	int isbyte = startch / 8;
	int isbit = startch % 8;
	int iebyte = endch / 8;
	int iebit = endch % 8;
	int prevb = 0;
	for (int i = isbyte; i <= iebyte; i++)
	{
		unsigned int u = m_datbuf[ns + i] & 255;
		if (i == isbyte)
			u &= m_smask[isbit];
		if (i == iebyte)
			u &= m_emask[iebit];
		nclst += m_numclst[u];
		if (prevb && (u & 128))
			--nclst;
		prevb = u & 1;
	}
	return nclst;
}

int
XYUnitData::sumhits (int ns, int startch, int endch) const
{
	// ns is the start byte index (count from 0) in m_datbuf[]
	// startch is the start channel number
	// endch is the end channel number
	// NO RANGE CHECK (this is private function), check the access range before calling.
	// Access range is [ns,ns+12].

	int nsum = 0;
	int isbyte = startch / 8;
	int isbit = startch % 8;
	int iebyte = endch / 8;
	int iebit = endch % 8;
	for (int i = isbyte; i <= iebyte; i++)
	{
		unsigned int u = m_datbuf[ns + i] & 255;
		if (i == isbyte)
			u &= m_smask[isbit];
		if (i == iebyte)
			u &= m_emask[iebit];
		nsum += (m_numhits[u] * i * 8 + m_sumhits[u]);
	}
	return nsum;
}

bool
XYUnitData::hittest (int ns, int nch) const
{
	int nb = (nch / 8) + ns;
	int nc = nch & 7;
	return (m_datbuf[nb] & m_bitpos[nc]);
}

bool
XYUnitData::Xhit (int nch) const
{
	if ((nch < 0) || (nch >= NUMXCHANNELS))
		return false;
	return hittest (XSTARTBYTE, nch);
}

bool
XYUnitData::Yhit (int nch) const
{
	if ((nch < 0) || (nch >= NUMYCHANNELS))
		return false;
	return hittest (YSTARTBYTE, nch);
}

bool
XYUnitData::checkdata()
{
	if (m_numdat != DATASIZE)
		return false;
	for (int i = 0; i < HEADERSIZE; i++)
	{
		if (m_datbuf[i] != m_header[i])
		{
			m_numdat = 0;
			return false;
		}
	}
	return true;
}

const XYUnitClock
XYUnitData::clockcount () const
{
	XYUnitClock xyclock (&m_datbuf[TSTARTBYTE]);
	return xyclock;
}

double
XYUnitData::microsec () const
{
	// NOTE:
	// The digits of the IEEE 754 binary64 floating point format
	// is 52(+1 hidden) bits. On the other hand, the micro-sec
	// counter of the BBT-X036 is 54 bits so that most significant
	// 2 bits will be lost. However, 2^52 microsec is more than
	// 140 years, this limitation may not cause significant
	// problem.

	double t = 0.0;
	// first 48 bits (= 6 bytes)
	for (int i = 0; i < 6; i++)
		t = t * 256.0 + double(m_datbuf[TSTARTBYTE + i] & 255);
	// the rest 6 bits
	t = t * 64.0 + double((m_datbuf[TSTARTBYTE + 6] >> 2) & 63);
	return t;
}

double
XYUnitData::nanosec () const
{
	// 10-bit counter
	double t = double(m_datbuf[TSTARTBYTE + 6] & 3) * 256.0
		+ double(m_datbuf[TSTARTBYTE + 7] & 255);
	return t;
}

bool
XYUnitData::Xbittest (int nbit) const
{
	if ((nbit < 0) || (nbit >= (NUMXBYTES * 8)))
		return false;
	return hittest (XSTARTBYTE, nbit);
}

bool
XYUnitData::Ybittest (int nbit) const
{
	if ((nbit < 0) || (nbit >= (NUMYBYTES * 8)))
		return false;
	return hittest (YSTARTBYTE, nbit);
}

bool
XYUnitData::Tbittest (int nbit) const
{
	if ((nbit < 0) || (nbit >= (NUMTBYTES * 8)))
		return false;
	return hittest (TSTARTBYTE, nbit);
}

std::istream&
operator >> (std::istream& is, XYUnitData& xyunitdata)
{
	xyunitdata.m_numdat = 0;
	unsigned int u = 0;
	while (is && (xyunitdata.m_numdat < XYUnitData::DATASIZE) && (is >> u))
		xyunitdata.m_datbuf[xyunitdata.m_numdat++] = (unsigned char)(u & 255);
	xyunitdata.checkdata();
	return is;
}

std::ostream&
operator << (std::ostream& os, const XYUnitData& xyunitdata)
{
	if (xyunitdata.m_numdat != XYUnitData::DATASIZE)
	{
		if (os)
			os << "(invalid data)";
		return os;
	}
	std::streamsize w = os.width();
	char c = os.fill();

	for (int i = 0; i < XYUnitData::DATASIZE; i++)
	{
		if (i)
			os << ' ';
		os << std::setw(w) << std::setfill(c) << (int(xyunitdata.m_datbuf[i]) & 255);
	}

	return os;
}

}	// namespace BBTX036MULTI
