#pragma once
#include <fstream>
#include <iostream>
#include <time.h>
#include <Windows.h>

class ubLogger {
private:

public:
	time_t startTime;
	HANDLE hConsole;
	std::ofstream chaifile;
	std::ofstream viconfile;
	std::ofstream cubefile;

	ubLogger();
	void logInit();

};