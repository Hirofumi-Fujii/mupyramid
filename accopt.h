// accopt.h

#ifndef ACCOPT_H_INCLUDED
#define ACCOPT_H_INCLUDED

#include <iostream>
#include <string>

namespace MUPYRAMID
{

class Accopt
{
public:
	Accopt ();
	virtual ~Accopt ();
	void usage (std::ostream& os, const char* pname) const;
	bool set (int argc, char* argv[]);

public:
	enum { XYXY, XYYX, YXXY, YXYX, };

public:
	std::string m_outfilename;
	int m_order;
	double m_xygap;
	double m_unitdist;
};

}	// namespace MUPYRAMID

#endif	// ACCOPT_H_INCLUDED
