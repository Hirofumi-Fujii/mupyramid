// pcutopt.h

#ifndef PCUTOPT_H_INCLUDED
#define PCUTOPT_H_INCLUDED

#include <iostream>
#include <string>

namespace MUPYRAMID
{

class Pcutopt
{
public:
	Pcutopt ();
	virtual ~Pcutopt ();
	void usage (std::ostream& os, const char* pname) const;
	bool set (int argc, char* argv[]);

public:
	std::string m_trxfilename;
	bool m_trxfilename_given;
	std::string m_cosfilename;
	bool m_cosfilename_given;
	std::string m_rngfilename;
	bool m_rngfilename_given;
	double m_pcut0;
	bool m_pcut0_given;
};

}	// namespace MUPYRAMID

#endif	// PCUTOPT_H_INCLUDED
