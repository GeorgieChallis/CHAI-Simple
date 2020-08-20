#include "ubLogger.h"


ubLogger::ubLogger()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 7); //Start with white text
	save = false;
}

int ubLogger::LogInit()
{
	int numTrials = 0;
	time_t startTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::cout << "Session Start: " <<  ctime(&startTime) << std::endl;
	std::cout << "----------------------------------------" << std::endl << std::endl;
	std::cout << "Make sure all I/O devices are correctly connected now." << std::endl;

	if (save) {
		std::string filename = "";


		std::cout << "Type file name for logging (no spaces)" << std::endl;
		std::cin >> filename;
		std::cout << "Enter the number of trials required:" << std::endl;
		std::cin >> numTrials;

		std::string oculusFilename = filename + "_HMD.csv";
		std::string viconFilename = filename + "_markers.csv";
		std::string cubeFilename = filename + "_cube.csv";

		chaifile.open(oculusFilename, std::ios::app);
		viconfile.open(viconFilename, std::ios::app);
		cubefile.open(cubeFilename, std::ios::app);
	}

	return numTrials;
}

void ubLogger::Warn(std::string warning)
{
	SetConsoleTextAttribute(hConsole, 0x0e);
	std::cout << "[Warning: ";
	std::cout << warning;
	std::cout << "]" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
}

void ubLogger::Error(std::string error)
{
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "[Error: ";
	std::cout << error;
	std::cout << "]" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
}

void ubLogger::Info(std::string info)
{
	std::cout << "[Info: ";
	std::cout << info;
	std::cout << "]" << std::endl;

}