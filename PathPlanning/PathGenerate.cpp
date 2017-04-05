#include "PathGenerate.h"
#include <assert.h>
#define LOG_CLOTHOID
bool PathGenerate::path_generate_grid(PosPoint startPt, PosPoint endPt, ckLcmType::VeloGrid_t& veloGrids, std::vector<RoadPoint>& rdPt) {

	//VeloGrid_t veloGrids=DataCenter::GetInstance().GetLidarData();

	// 
	int * grid_map_start = new int[MAP_HEIGHT];
	int * grid_map_end = new int[MAP_HEIGHT];
	int x_start = 0;
	int y_start = 0;
	int x_end = 0;
	int y_end = 0;
	// TRANSFORM TO GRID COORDINATE
	CoordTransform::LocaltoGrid(startPt, x_start, y_start);
	CoordTransform::LocaltoGrid(endPt, x_end, y_end);

	// create clothoid curve
	Clothoid path_clothoid(x_start, y_start, startPt.angle, x_end, y_end, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);

	//
	
	for (int i = 0; i < num_pt; i++)
	{
		// may be a bug can be saver
		/*for (int j = -4; j <= 4;j++)
		{
			if (((int)rdPt[i].x + j) >= 0 && ((int)rdPt[i].x + j) <= MAP_WIDTH -1)
				grid_map[MAP_WIDTH*(int)rdPt[i].y + (int)rdPt[i].x + j] = 1;
		}*/
		if (((int)rdPt[i].x + CAR_WIDTH) >= 0 && ((int)rdPt[i].x + CAR_WIDTH) <= MAP_WIDTH - 1)
		{
			grid_map_end[(int)rdPt[i].y] =
				(int)rdPt[i].x + CAR_WIDTH > grid_map_end[(int)rdPt[i].y] 
				?
				(int)rdPt[i].x + CAR_WIDTH : grid_map_end[(int)rdPt[i].y];
			
		}
		else {
			grid_map_end[(int)rdPt[i].y] = MAP_WIDTH - 1;
		}
		if (((int)rdPt[i].x - CAR_WIDTH) >= 0 && ((int)rdPt[i].x - CAR_WIDTH) <= MAP_WIDTH - 1)
		{
			grid_map_start[(int)rdPt[i].y] = 
				(int)rdPt[i].x - CAR_WIDTH < grid_map_start[(int)rdPt[i].y]
				?
				(int)rdPt[i].x - CAR_WIDTH : grid_map_start[(int)rdPt[i].y];
		}
		else {
			grid_map_start[(int)rdPt[i].y] = 0;
		}
		
	}

	//
#ifdef TEST
	for (int i = 0; i < num_pt; i++) {

		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if (((int)rdPt[i].x + j) >= 0 && ((int)rdPt[i].x + j) <= MAP_WIDTH - 1)
			{
				index = [MAP_WIDTH*(int)rdPt[i].y + (int)rdPt[i].x + j];
				if (veloGrids.velo_grid[index]) {
					return false
				}
			}

		}
	}
#endif // !TEST

	//int obstacle_size;
	for (size_t i = 0; i < veloGrids.height; i++)
	{
		for (size_t j = 0; j < veloGrids.width; j++)
		{
			int index = i*veloGrids.width + j;
			if (veloGrids.velo_grid[i])
			{
				
				bool flag = j >= grid_map_start[j] && grid_map_end[j] <= j;
				if (flag)
				{
					return false;
				}
			}
		}
	}
	delete[] grid_map_start;
	delete[] grid_map_end;

	return true;
	

}

bool PathGenerate::path_generate_local(PosPoint startPt, PosPoint endPt) {
	//// create clothoid curve
	//Clothoid path_clothoid(startPt.x, startPt.y, startPt.angle, endPt.x, endPt.y, endPt.angle);

	//// get roadPoints
	//int num_pt = 100;
	//RoadPoint *rdPt = new RoadPoint[num_pt];
	//RoadPoint *gridPt = new RoadPoint[num_pt];
	//path_clothoid.PointsOnClothoid(rdPt, num_pt);
	//// change to grid
	//for (auto i = 0; i < num_pt; i++)
	//{
	//	int x = 0;
	//	int y = 0;
	//	PosPoint pt;
	//	pt.x = rdPt[i].x;
	//	pt.y = rdPt[i].y;
	//	pt.angle = rdPt[i].angle;
	//	CoordTransform::LocaltoGrid(pt, x, y);
	//	gridPt[i].x = x;
	//	gridPt[i].y = y;
	//	gridPt[i].angle = rdPt[i].angle;
	//	gridPt[i].changeangle = rdPt[i].changeangle;
	//}
	//// define whether intersected or not

	//// step one get rightest point
	//int index = getRightestPoints(rdPt, num_pt);
	//// step two consider the car width
	//rdPt[index].x += 0.9;   

	//// TODO get the a,b,c
	//laserCurbs::pointXYZI coeff = DataCenter::GetInstance().GetRoadEdgeCoefficient(RIGHT);
	//double a, b, c;
	//a = coeff.x;
	//b = coeff.y;
	//c = coeff.z;
	//double distance = abs(a*rdPt[index].x + b*rdPt[index].y + c) / sqrtf(a*a + b*b);
	//if (distance<0.3)
	//{
	//	return false;
	//}
	//else
	//{
		return true;
	//}


}

void PathGenerate::path_generate() {

	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 400;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;
	// step three generate the path
	int delta_Grid_start = -5;
	int delta_Grid_end = 20;
	PosPoint startPt, endPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = 90 / 180.0 * PI;
	bool send_succeed = false;
	for (int i = delta_Grid_start; i < delta_Grid_end; i++)
	{
		endPt.y = target_Y;
		endPt.x = target_X - i;
		endPt.angle = target_Angle;
		//TODO set angle
		std::vector<RoadPoint> rdpt;
		if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdpt)) {
			std::cout << "congratulations a successful root" << std::endl;
			// TODO: step four send the data
#ifdef LOG_CLOTHOID

			FILE *fp = fopen("clothoid.txt", "a+");
#endif // LOG_CLOTHOID
			ckLcmType::DecisionDraw_t draw;
			draw.num = rdpt.size();
			draw.Path_x.reserve(draw.num);
			draw.Path_y.reserve(draw.num);
			for (RoadPoint pt : rdpt) {
				double x, y;
				CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
				draw.Path_x.push_back(x);
				draw.Path_y.push_back(y);
				fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
			}
#ifdef LOG_CLOTHOID
			fclose(fp);
#endif // 
			m_sendPath.SendDraw(draw);
			track.SetPath(rdpt);
			send_succeed = true;
			break;

		}
	}
	if (!send_succeed)
	{
		std::cout << "no path" << std::endl;
		track.SetPath(std::vector<RoadPoint>());
		//TODO: step five send message about how to stop
		CarInfo info;
		info.speed = 0;
		info.state = E_STOP;
		info.steerAngle = 0;
		info.gear = D;
		CarControl::GetInstance().SendCommand(info);
	}

}

double PathGenerate::getTargetDirection() {
	return 0.0;
}

int PathGenerate::getRightestPoints(RoadPoint *rdPt, int numPt) {

	int maxVal_x = rdPt[0].x;
	int index = 0;
	for (int i = 0; i < numPt; i++)
	{
		if (rdPt[i].x > maxVal_x)
		{
			maxVal_x = rdPt[i].x;
			index = i;
		}
	}
	return index;
}

bool PathGenerate::path_generate_grid_obstacle(PosPoint startPt, PosPoint endPt, VeloGrid_t& veloGrids, std::vector<RoadPoint>& rdPt)
{
	// 
	int x_start = startPt.x;
	int y_start = startPt.y;
	int x_end = endPt.x;
	int y_end = endPt.y;
	// TRANSFORM TO GRID COORDINATE
//	CoordTransform::LocaltoGrid(startPt, x_start, y_start);
//	CoordTransform::LocaltoGrid(endPt, x_end, y_end);

	// create clothoid curve
	Clothoid path_clothoid(x_start, y_start, startPt.angle, x_end, y_end, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	//rdPt.clear();
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);
	for (int i = 0; i < num_pt; i++)
	{
		// may be a bug can be saver
		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if ((int)(rdPt[i].x + 0.5 + j) >= 0 && ((int)(rdPt[i].x + 0.5 + j)) <= MAP_WIDTH - 1)
			{
				int index = MAP_WIDTH*(int)(rdPt[i].y + 0.5) + (int)(rdPt[i].x + 0.5) + j;
				if (veloGrids.velo_grid[index]) {
					return false;
				}
			}
		}
		

	}

	
	return true;
}
void PathGenerate::createClothoidTable(){


	// step one define the start point and its directions
	PosPoint start_pt, endPt;
	start_pt.x = 75;
	start_pt.y = 200;
	start_pt.angle = PI / 2;

	clothoidMap.clear();
	clothoidMap.resize((GRID_END - GRID_START) *(ANGLE_END - ANGLE_START));
	// step two define the end point and create some directions
	
	for (int i = GRID_START; i < GRID_END; i++)
	{

		endPt.y = target_Y;
		endPt.x = target_X - i;
		for (int angle = ANGLE_START; angle < ANGLE_END;angle++)
		{
			endPt.angle = angle / 180.0 *PI;
			std::vector<RoadPoint> rdPt;
			generateClothoidPoints(start_pt, endPt, rdPt);
			clothoidMap[(GRID_END - GRID_START)* (i - GRID_START) + (angle - ANGLE_START)] = rdPt;
		}
		
	}


}
void PathGenerate::generateClothoidPoints(PosPoint startPt, PosPoint endPt, std::vector<RoadPoint>& rdPt){
	
	// create clothoid curve
	Clothoid path_clothoid(startPt.x, startPt.y, startPt.angle, endPt.x, endPt.y, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	rdPt.clear();
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);
}
std::vector<RoadPoint> PathGenerate::getRdPtFromTable(int grid, int angle){

	int index = grid*(GRID_END - GRID_START) + angle - ANGLE_START;
	assert(index >= 0 && index <= clothoidMap.size());
	return clothoidMap[index];
}
void PathGenerate::path_generate_using_bug()
{
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 400;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;
	// step three generate the path
	int delta_Grid_start = -5;
	int delta_Grid_end = 20;
	PosPoint startPt, endPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = 90 / 180.0 * PI;
	std::vector<std::vector<RoadPoint>> success_root;
	for (int i = GRID_START; i < GRID_END; i++)
	{

		endPt.y = target_Y;
		endPt.x = target_X - i;
		for (int angle = ANGLE_START; angle < ANGLE_END; angle++)
		{
			
			std::vector<RoadPoint> rdPt;
			rdPt = getRdPtFromTable(i, angle);
			if (Topology::check_velogrid_rdPt_intersected(veloGrids, rdPt))
			{
				success_root.push_back(rdPt);
			}
			
		}
	}

	//store and compare the difference of the road
	if (pre_Root.size() == 0)
	{
		if (success_root.size() == 0)
		{
			return;
		}
		else
		{
			// random choose first one
			// or you can give a direction 
			// then i can compute the cost
			// then choose a better one
			// random choose may cause a problem
			pre_Root = success_root[0];
		}
		
	}
	else
	{
		if (success_root.size() == 0 )
		{
			// choice using the pre the go 
			// or stop
			std::cout << "no path" << std::endl;
			track.SetPath(std::vector<RoadPoint>());
			//TODO: step five send message about how to stop
			CarInfo info;
			info.speed = 0;
			info.state = E_STOP;
			info.steerAngle = 0;
			info.gear = D;
			CarControl::GetInstance().SendCommand(info);
		}
		else{
			int simi_max = -1000;
			std::vector<RoadPoint> path;
			for (auto root : success_root)
			{
				double simi = similarity(pre_Root, root);
				if (simi > simi_max){
					simi_max = simi;
					path = root;
				}
			}
			// send the data
			ckLcmType::DecisionDraw_t draw;
			draw.num = path.size();
			draw.Path_x.reserve(draw.num);
			draw.Path_y.reserve(draw.num);
			for (RoadPoint& pt : path) {
				double x, y;
				CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
				draw.Path_x.push_back(x);
				draw.Path_y.push_back(y);
				//fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
			}
			m_sendPath.SendDraw(draw);
			
		}
		
	}

}
bool PathGenerate::path_generate_turning(VeloGrid_t& veloGrids){
	// define search region
	// lane width 3 so 6 grid
	int RegionWidth = 0;
	int RegionHeight = 0;
	int RegionStartX = 0;
	int RegionStartY = 0;

	PosPoint endPt, startPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = PI / 2.;


	if (PreDirection == TURN_LEFT)
	{
		RegionStartX = 75 - 12;
		RegionStartY = 200 + 12;
		RegionWidth = 4;
		RegionHeight = 4;
		endPt.angle = PI;

	}
	else if (PreDirection == TUN_RIGHT)
	{
		RegionStartX = 75 + 6;
		RegionStartY = 200 + 12;
		RegionWidth = 4;
		RegionHeight = 4;
		endPt.angle = 0;
	}
	else
	{
		return false;
	}

	//
	


	for (int i = 0; i < RegionWidth;i++)
	{
		for (int j = 0; j < RegionHeight;j++)
		{
			endPt.x = RegionStartX + i;
			endPt.y = RegionStartY + j;
			std::vector<RoadPoint> rdpt;
			if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdpt)) {
				std::cout << "congratulations a successful root" << std::endl;

				// pass the data
			}

		}
	}
	
}
bool PathGenerate::path_generate_for_fun(){
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 400;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;

	//
	std::vector<PosPoint> obstacles;
	PosPoint startPt;
	PosPoint endPt;
	std::vector<PosPoint> nodePt;
	nodePt.push_back(startPt);

	while (true)
	{
		// recursive to avoid the obstacles
		endPt = obstacles[0];
		std::vector<RoadPoint> rdPt;
		if (path_generate_grid_obstacle(startPt, endPt, veloGrids,rdPt))
		{
			// you can consider only store the root point rather than the rdPt
			nodePt.push_back(endPt);

		}
		startPt = endPt;
		endPt.x = target_X;
		endPt.y = target_Y;
		endPt.angle = target_Angle;

		// some questions remain to be solved

	}
}
