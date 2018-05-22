// coinopt.h

#ifndef COINOPT_H_INCLUDED
#define COINOPT_H_INCLUDED

#include <iostream>
#include <string>

namespace MUONDAQ
{

class Coinopt
{
public:
	Coinopt ();
	virtual ~Coinopt ();
	void usage (std::ostream& os, const char* pname) const;
	bool set (int argc, char* argv[]);

public:
	std::string m_infilename;
	std::string m_outfilename;
	unsigned int m_coinwidth;
	unsigned int m_mergewidth;
	bool m_singledump;
	bool m_mergedump;
	bool m_longdump;
	unsigned int m_toffset [256];
};

}	// namespace MUONDAQ

#endif	// COINOPT_H_INCLUDED
