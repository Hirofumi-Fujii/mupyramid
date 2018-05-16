// setupehall.h

#ifndef SETUPEHALL_H_INCLUDED
#define SETUPEHALL_H_INCLUDED

#include "xyunitgeom.h"

namespace MUONEHALL
{
class SetupEhall
{
public:
	static const int NUM_UNITS = 5;
	static XYUnitGeometry xyunitgeom[NUM_UNITS];
};

}	// namespace MUONEHALL

#endif	// SETUPEHALL_H_INCLUDED
