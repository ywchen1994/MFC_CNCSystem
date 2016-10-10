#pragma once

#define ROBOT_ORIGIN_OFFSET_X (20);
#define ROBOT_ORIGIN_OFFSET_Y (-430);

#define ROBOT_6_AXIS_SOCKET_PORT (25650)

struct DataPackage {
	char PREFIX[4] = { 'N','2','r','2' };
	float X;
	float Y;
	float Z;
	float theta;
	bool XY_ready;
	bool beProcessed = false;
	bool workDone = false;
};
