#include "testModule.h"
#include <iostream>
#include <iomanip>
#include "CoordTransform.h"
using namespace std;
void test_To_Guassian(double grad, double minute, double second,
	double grad1, double minute1, double second1){
	double lon = grad + minute / 60.0 + second / 3600.0;
	double lat = grad1 + minute1 / 60.0 + second1 / 3600;
	cout << "lon:" << lon << "lat:" << lat << endl;
	double x = 0, y = 0;
	LBtoxy test(lon, lat, 6);
	test.calculateAll();
	x = test.getx();
	y = test.gety() + 500000;
	//CoordSystem coord_system = WGS84;
	//CoordTransform::LongLat2XY(lon, lat, coord_system, x, y);
	//double real_y = gt_x, real_x = gt_y;
	cout << setiosflags(ios::fixed)<<"x:"<<setprecision(10) << x << "y:" << y << endl;
	//cout << setiosflags(ios::fixed) << "delta_x:" << setprecision(10) << (real_x - x) 
		//<< "delta_y:" << real_y - y << endl;
	//double reverse_lon = 0, reverse_lat = 0;
	//CoordTransform::XY2LongLat(x, y, coord_system, reverse_lon, reverse_lat);
	//cout << setiosflags(ios::fixed) << "x:" << setprecision(10) << reverse_lon << "y:" << reverse_lat - lat<< endl;
}
void test_To_Guassian(
	double lon, double lat
	){
	LBtoxy test(lon,lat,6);
	test.calculateAll();
	cout << setiosflags(ios::fixed) << setprecision(10)<< test.getx() << "/" << test.gety() + 500000 << endl;
}


void test_Clothoid()
{
	RoadPoint *rdPt = new RoadPoint[100];

	Clothoid clo(0.0, 0.0, 0.0, 50.0, 50.0, PI / 2.0);
	//clo.PointsOnClothoid(rdPt, 100);
}

void test_Array(int a[], int length)
{
	for (int i = 0; i < length; i++) {
		a[i] = i;
	}
}

int64_t timecal(std::function<void()> f) {
	std::chrono::high_resolution_clock::time_point startTime
		= std::chrono::high_resolution_clock::now();
	f();
	std::chrono::high_resolution_clock::time_point endTime
		= std::chrono::high_resolution_clock::now();
	std::chrono::microseconds time
		= std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	return time.count();
}

#define CAL_TIME(func) timecal([&]()->void{func})

void test_spline()
{
	std::vector<double> X{ 0,1,2,3 };
	std::vector<double> Y{ 0,0.5,2.0,1.5 };
	std::cout << CAL_TIME(
		auto s = Topology::CubicSpline(X, Y, -0.3, 3.3);
	std::cout << s << std::endl;) << std::endl;
	//std::cout<<timecal([&]() -> void {
	//	auto s = Topology::CubicSpline(X, Y, -0.3, 3.3);
	//	std::cout << s << std::endl;
	//})<<std::endl;
	//std::chrono::high_resolution_clock::time_point startTime
	//	= std::chrono::high_resolution_clock::now();
	//std::chrono::high_resolution_clock::time_point endTime
	//	= std::chrono::high_resolution_clock::now();
	//std::chrono::microseconds time
	//	= std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	//std::cout << time.count() << std::endl;
}

void test_axis_transform()
{
	RoadPoint pt;
	pt.x = 246340.95890675954;
	pt.y = 3382180.4580072556;
	pt.angle = 1.32;
	pt.changeangle = 1;
	pt.k = 2;
	RoadPoint org = pt;
	org.y += 4;
	CoordTransform::WorldToLocal(org, pt, &pt);
	CoordTransform::LocalToWorld(org, pt, &pt);
}
