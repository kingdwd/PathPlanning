#include "ClothoidTrack.h"
#include "TrackFinder.h"
#include "LocalCarStatus.h"
#include "Topology.h"
#include "CarControl.h"
#include "Clothoid.h"



ClothoidTrack::ClothoidTrack()
{
}


ClothoidTrack::~ClothoidTrack()
{
}

void ClothoidTrack::Track()
{
	if (path.size() < 10) return;
	CarInfo info;
	RoadPoint curX = RoadPoint::toRoadPoint(LocalCarStatus::GetInstance().GetPosition());
	int curIndex = TrackFinder::FindPointIndex(path, curX);
	do {
		if (curIndex < 0) {
			info.gear = D;
			info.speed = 0;
			info.state = E_STOP;
			info.steerAngle = LocalCarStatus::GetInstance().GetSteerAngle();
			break;
		}
		inCurve = TrackFinder::InCurve(inCurve, path, curX, curIndex);
		RoadPoint refX;
		if (inCurve) {
			refX = path[TrackFinder::AnchorPoint(path, curX, curIndex, 10)];
			info.speed = refSpeedCurve;
		}
		else {
			refX = path[TrackFinder::AnchorPoint(path, curX, curIndex, 15)];
			info.speed = refSpeedStraight;
		}
		info.gear = D;
		info.state = START;
		Clothoid clothoid(curX.x, curX.y, curX.angle, refX.x, refX.y, refX.angle);
		std::vector<RoadPoint> rdpt;
		rdpt.reserve(100);
		clothoid.PointsOnClothoid(rdpt, 100);
		info.steerAngle = atan(-rdpt[0].k*LocalCarStatus::GetInstance().GetL())
			*LocalCarStatus::GetInstance().GetSteerRatio()
			* 180 / PI;
	} while (false);
	CarControl::GetInstance().SendCommand(info);
}
