// databank.cpp

#include "databank.h"

namespace MUONDAQ
{
DataBank::DataBank ()
{
	m_numdaqboxes = 0;
	for (int i = 0; i < 256; i++)
		m_daqboxno[i] = NUM_DAQBOXES;
	for (unsigned int u = 0; u < NUM_DAQBOXES; u++)
		m_daqboxchar[u] = (char)(0);
}

const DataRecord
DataBank::getrecord (unsigned int unitno, std::istream& is, std::ostream& os,
	bool outflag)
{
	if (unitno >= NUM_DAQBOXES)
	{
		// returns empty record
		DataRecord rec;
		return rec;
	}
	std::string recstr;
	while (is && m_datalist[unitno].empty())
	{
		if (!getline (is, recstr))
			break;
		if (recstr.size() >= 4)
		{
			if (recstr.substr (0,3) == "DAT")
			{
				unsigned int uno = m_daqboxno[(unsigned int)(recstr[3]) & 255];
				if (uno < NUM_DAQBOXES)
				{
					DataRecord rec (recstr);
					m_datalist[uno].push_back (rec);
				}
			}
			else
			{
				if ((recstr.substr (0,3) == "VOL")
					|| (recstr.substr (0,3) == "TWI"))
				{
					unsigned int uc = (unsigned int)(recstr[3]) & 255;
					if ((m_numdaqboxes < NUM_DAQBOXES) &&
						(m_daqboxno[uc] >= NUM_DAQBOXES))
					{
						m_daqboxno[uc] = m_numdaqboxes;
						m_daqboxchar[m_numdaqboxes] = recstr[3];
						++m_numdaqboxes;
					}
				}
				if (outflag)
					os << recstr << std::endl;
			}
		}
	}
	if (m_datalist[unitno].empty())
	{
		// returns empty record
		DataRecord rec;
		return rec;
	}
	DataRecord rec = m_datalist[unitno].front();
	m_datalist[unitno].pop_front();
	return rec;
}

}	// namespace MUONDAQ
