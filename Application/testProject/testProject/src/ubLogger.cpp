#include "ubLogger.h"


ubLogger::ubLogger()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 7); //Start with white text
}

void ubLogger::logInit()
{

}
