// mergeopt.h

#ifndef MERGEOPT_H_INCLUDED
#define MERGEOPT_H_INCLUDED

#include <iostream>
#include <string>

namespace MUONDAQ
{

class Mergeopt
{
public:
	Mergeopt ();
	virtual ~Mergeopt ();
	void usage (std::ostream& os, const char* pname) const;
	bool set (int argc, char* argv[]);

public:
	std::string m_infilename;
	std::string m_outfilename;
	unsigned int m_mergewidth;
	bool m_fixedwidth;
};

}	// namespace MUONDAQ

#endif	// MERGEOPT_H_INCLUDED
