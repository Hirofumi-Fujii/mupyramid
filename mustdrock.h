// mustdrock.h
//
// Standard Rock poperties for muons
//
#ifndef MUSTDROCK_H_INCLUDED
#define MUSTDROCK_H_INCLUDED

namespace mylibrary
{

class MuStdRock
{
public:
	MuStdRock ();
	virtual ~MuStdRock ();
	bool ReadLBLTEXTFile (const char* fname);
	double CSDARange (double p) const;
	double pmin () const { return (m_p [0] / 1000.0); }
	double pmax () const { if (m_numdat > 0) return (m_p [m_numdat - 1] / 1000.0); return (m_p [0] / 1000.0); }
	int numdat () const { return m_numdat; }

protected:
	static const int MAX_TAB_SIZE = 512;
	int m_numdat;
	double m_p [MAX_TAB_SIZE];
	double m_CSDARange [MAX_TAB_SIZE];
};

}	// namespace mylibrary
#endif	// MUSTDROCK_H_INCLUDED
