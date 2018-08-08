// mustdrock.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "mustdrock.h"

namespace mylibrary
{

MuStdRock::MuStdRock ()
{
	m_numdat = 0;
}

MuStdRock::~MuStdRock ()
{
}

bool
MuStdRock::ReadLBLTEXTFile (const char* fname)
{
	m_numdat = 0;
	std::ifstream ifs (fname);
	if (!ifs)
		return false;

	std::string sline;
	while (getline (ifs, sline))
	{
		int nitems = 0;
		double val [12];
		if (sline.size () > 24)
		{
			std::stringstream ss (sline);
			for (int i = 0; i < 12; i++)
			{
				double dv;
				if (ss >> dv)
					val [nitems++] = dv;
			}
		}
		if (nitems >= 12)
		{
			if (m_numdat < MAX_TAB_SIZE)
			{
				m_p [m_numdat] = val[1];
				m_CSDARange [m_numdat] = val[8];
				++m_numdat;
			}
		}
		sline.clear ();
	}
	if (m_numdat < 2)
		return false;
	return true;
}

double
MuStdRock::CSDARange (double p) const
{
	double pmev = p * 1000.0;
	if ((m_numdat <= 0) || (pmev < m_p [0]))
		return 0.0;
	if (pmev > m_p [m_numdat - 1])
		return m_CSDARange [m_numdat - 1];

	int iu = 0;
	while (iu < m_numdat)
	{
		if (m_p [iu] > pmev)
			break;
		++iu;
	}
	if (iu <= 0)
		return 0.0;

	double r = (m_CSDARange [iu] - m_CSDARange [iu - 1])
			/ (m_p [iu] - m_p [iu - 1]) * (pmev - m_p [iu - 1])
			+ m_CSDARange [iu - 1];
	return r;
}

}	// namespace mylibrary
