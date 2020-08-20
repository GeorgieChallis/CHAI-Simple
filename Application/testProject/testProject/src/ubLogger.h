#pragma once
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <Windows.h>
#include <string>

class ubLogger {
private:

public:
	time_t startTime;
	HANDLE hConsole;
	std::ofstream chaifile;
	std::ofstream viconfile;
	std::ofstream cubefile;
	bool save;

	ubLogger();
	int LogInit();
	void Error(std::string warning);
	void Warn(std::string warning);
	void Info(std::string warning);
};