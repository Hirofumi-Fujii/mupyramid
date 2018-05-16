// setupehall.cpp
//
// corrected 2015-10-22 thickness of unit 10 and 11
// corrected 2015-10-21

#include "setupehall.h"

namespace MUONEHALL
{

XYUnitGeometry
SetupEhall::xyunitgeom[SetupEhall::NUM_UNITS] =
{
	// Note that the X and Y position is the center position of the sensitive area.
	// Z is muon direction
	// Y is down to up
	// X is chosen to make (X,Y,Z) is right-handed

	//
	// Unit 7, 8, 9
	//
	XYUnitGeometry(
		XYUnitGeometry::Position (0.0, 0.0, 0.0),
		XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_POS),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ORDER_YX ),
	XYUnitGeometry(
		XYUnitGeometry::Position (4.5, 2.0, 1050.0),
		XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_POS),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ORDER_YX ),
	XYUnitGeometry(
		XYUnitGeometry::Position (-1.0, -500.0, (1050.0 + 2731.0 + 3188.0 + 5743.0)),
		XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ChannelProp (0, 99, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ORDER_XY ),
	//
	// Unit 10, 11
	//
	XYUnitGeometry(
		XYUnitGeometry::Position (-46.0, -100.0, (1050.0 + 2731.0)),
		XYUnitGeometry::Size (1600.0, 1600.0, 10.0),
		XYUnitGeometry::ChannelProp (0, 39, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ChannelProp (0, 39, XYUnitGeometry::DIR_POS),
		XYUnitGeometry::ORDER_YX ),
	XYUnitGeometry(
		XYUnitGeometry::Position (-54.0, -298.5, (1050.0 + 2731.0 + 3188.0)),
		XYUnitGeometry::Size (1600.0, 1600.0, 10.0),
		XYUnitGeometry::ChannelProp (60, 99, XYUnitGeometry::DIR_NEG),
		XYUnitGeometry::ChannelProp (60, 99, XYUnitGeometry::DIR_POS),
		XYUnitGeometry::ORDER_YX ),
};

}	// namespace MUONDAQ
