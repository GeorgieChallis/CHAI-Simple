//-----------------------------------
#pragma region

//CHAI3D
#include "chai3d.h"
//OpenGL Wrapper
#include <GLFW/glfw3.h>
//OCULUS SDK
#include "COculus.h"
//For VICON Code
#include "DataStreamClient.h"
// FOR C++ Serial Port Class
#include "SerialPort.h"
#include "SerialPort.cpp"
#include "Defaults.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <string.h>
#include <time.h>
#include <cmath>

#ifdef WIN32
#include <conio.h>   // For _kbhit()
#include <cstdio>   // For getchar()
#include <windows.h> // For Sleep()
#endif // WIN32

#pragma endregion Includes

#pragma region
//-Namespaces-------------------------------------------------------------------
using namespace chai3d;
using namespace std;
using namespace ViconDataStreamSDK::CPP;
//------------------------------------------------------------------------------
#pragma  endregion Namespaces

#pragma region
static bool trialRunning = false;
static int numTrials = 0;
double edistance; //Euclidean distance between cubes

struct trial {
	int moveType;
};

vector<trial> trialList;

#pragma endregion Trial Data

#pragma region
//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------
// ------ LOGGING -----------
time_t startTime;
//Console
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
ofstream chaifile;
ofstream viconfile;
ofstream cubefile;
//convert to resource path
#define RESOURCE_PATH(p)	(char*)((resourceRoot+string(p)).c_str())

// a world that contains all objects of the virtual environment
cWorld* world;
// a camera to render the world in the window display
cCamera* camera;
// a light source to illuminate the objects in the world
cSpotLight *light;
// a few shape primitives that compose our scene
cMesh* my_cube;
cMesh* target_cube;

// a flag to indicate if the haptic simulation currently running
bool simulationRunning = false;
// a flag to indicate if the haptic simulation has terminated
bool simulationFinished = true;

// a handle to window display context
GLFWwindow* window = NULL;
// current width of window
int width = 0;
// current height of window
int height = 0;
// swap interval for the display context (vertical synchronization)
int swapInterval = 1;



cThread *cubeThread;

//Values for changing the cube's position/size
static double mycube_posX = 0.0;
static double mycube_posY = 0.0;
static double mycube_posZ = 0.0;
static double cube_size = 0.2;

static double targetcube_posX = 0.0;
static double targetcube_posY = 0.0;
static double targetcube_posZ = 0.0;

//-----------------------------------------------------------------

#pragma endregion Global variables - CHAI

#pragma region
//----------------------------------------
//	SETUP Serial
//----------------------------------------
SerialPort serialPort;
static bool serialOK = false;
static bool cubeTransparent = false;
static double quaternion[4];

#pragma endregion Global variables - Serial Port

#pragma region
//----------------------------------------
// SETUP RIFT
//----------------------------------------
cOVRRenderContext renderContext;
cOVRDevice oculusVR;
bool oculusInit = false;
#pragma endregion Global variables - Oculus

#pragma region
//---------
//  VICON
//---------
cThread *viconThread;
static bool viconConnected = false;
Output_GetMarkerGlobalTranslation _Output_GetMarkerGlobalTranslation;
Output_GetLabeledMarkerGlobalTranslation _Output_GetLabeledMarkerGlobalTranslation;
Output_GetUnlabeledMarkerGlobalTranslation _Output_GetUnlabeledMarkerGlobalTranslation;

ViconDataStreamSDK::CPP::Client MyClient;

std::string Adapt(const bool i_Value)
{
	return i_Value ? "True" : "False";
}

std::string Adapt(const TimecodeStandard::Enum i_Standard)
{
	switch (i_Standard)
	{
	default:
	case TimecodeStandard::None:
		return "0";
	case TimecodeStandard::PAL:
		return "1";
	case TimecodeStandard::NTSC:
		return "2";
	case TimecodeStandard::NTSCDrop:
		return "3";
	case TimecodeStandard::Film:
		return "4";
	case TimecodeStandard::NTSCFilm:
		return "5";
	case TimecodeStandard::ATSC:
		return "6";
	}
}

std::string Adapt(const Direction::Enum i_Direction)
{
	switch (i_Direction)
	{
	case Direction::Forward:
		return "Forward";
	case Direction::Backward:
		return "Backward";
	case Direction::Left:
		return "Left";
	case Direction::Right:
		return "Right";
	case Direction::Up:
		return "Up";
	case Direction::Down:
		return "Down";
	default:
		return "Unknown";
	}
}

std::string Adapt(const DeviceType::Enum i_DeviceType)
{
	switch (i_DeviceType)
	{
	case DeviceType::ForcePlate:
		return "ForcePlate";
	case DeviceType::Unknown:
	default:
		return "Unknown";
	}
}

std::string Adapt(const Unit::Enum i_Unit)
{
	switch (i_Unit)
	{
	case Unit::Meter:
		return "Meter";
	case Unit::Volt:
		return "Volt";
	case Unit::NewtonMeter:
		return "NewtonMeter";
	case Unit::Newton:
		return "Newton";
	case Unit::Kilogram:
		return "Kilogram";
	case Unit::Second:
		return "Second";
	case Unit::Ampere:
		return "Ampere";
	case Unit::Kelvin:
		return "Kelvin";
	case Unit::Mole:
		return "Mole";
	case Unit::Candela:
		return "Candela";
	case Unit::Radian:
		return "Radian";
	case Unit::Steradian:
		return "Steradian";
	case Unit::MeterSquared:
		return "MeterSquared";
	case Unit::MeterCubed:
		return "MeterCubed";
	case Unit::MeterPerSecond:
		return "MeterPerSecond";
	case Unit::MeterPerSecondSquared:
		return "MeterPerSecondSquared";
	case Unit::RadianPerSecond:
		return "RadianPerSecond";
	case Unit::RadianPerSecondSquared:
		return "RadianPerSecondSquared";
	case Unit::Hertz:
		return "Hertz";
	case Unit::Joule:
		return "Joule";
	case Unit::Watt:
		return "Watt";
	case Unit::Pascal:
		return "Pascal";
	case Unit::Lumen:
		return "Lumen";
	case Unit::Lux:
		return "Lux";
	case Unit::Coulomb:
		return "Coulomb";
	case Unit::Ohm:
		return "Ohm";
	case Unit::Farad:
		return "Farad";
	case Unit::Weber:
		return "Weber";
	case Unit::Tesla:
		return "Tesla";
	case Unit::Henry:
		return "Henry";
	case Unit::Siemens:
		return "Siemens";
	case Unit::Becquerel:
		return "Becquerel";
	case Unit::Gray:
		return "Gray";
	case Unit::Sievert:
		return "Sievert";
	case Unit::Katal:
		return "Katal";

	case Unit::Unknown:
	default:
		return "Unknown";
	}
}

#ifdef WIN32
bool Hit()
{
	bool hit = false;
	while (_kbhit())
	{
		getchar();
		hit = true;
	}
	return hit;
}
#endif
#pragma endregion Global variables - VICON

#pragma region
//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------
// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// this function renders the scene
void updateGraphics(void);

//Print headset position to file
void PrintHMDPos(void);
void PrintMarkerPos(void);
void PrintCubePos(void);

void UpdateViconFrame(void);

//Handle what to do when cube button pressed
void OnButtonUp(void);

//Move Cube
void UpdateIMUCube(void);
void MoveLeft(void);
void MoveRight(void);
void RotateCube(int x, int y, int z, double degrees);
void MoveLeftSine();
void MoveRightSine();
void MoveDirection(char axis, double amnt);

// this function closes the application
void close(void);

//==============================================================================
#pragma endregion Declared functions

int main(int argc, char* argv[])
{
	// parse first arg to try and locate resources
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);

#pragma region
	//LOGGING--------------------------
	SetConsoleTextAttribute(hConsole, 7);
	startTime = clock();

	struct tm * timeinfo;
	time(&startTime);
	timeinfo = localtime(&startTime);

	string datetime = (string)asctime(timeinfo);
	std::cout << "Session Start: " << datetime << endl;
	std::cout << "----------------------------------------" << endl << endl;
	std::cout << "Make sure all I/O devices are correctly connected now." << endl;

	std::cout << "Type file name for logging (no spaces)" << endl;
	string filename;
	cin >> filename;
	cout << "Enter the number of trials required:" << endl;
	cin >> numTrials;

	for (int i = 0; i < numTrials; i++) {
		trial newTrial;
		newTrial.moveType = rand() % 4 + 1;
		cout << "i: " << i << ", trials: " << newTrial.moveType << endl;
		trialList.push_back(newTrial);
	}

	string oculusFilename = filename + "_HMD.csv";
	string viconFilename = filename + "_markers.csv";
	string cubeFilename = filename + "_cube.csv";

	chaifile.open(oculusFilename, ios::app);
	viconfile.open(viconFilename, ios::app);
	cubefile.open(cubeFilename, ios::app);

#pragma endregion Log_Setup

#pragma region

	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	world->m_backgroundColor.setBlack();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	camera->set(
		cVector3d(0, 1, 1.3),       // Local Position of camera.
		cVector3d(0, 0, 1.3),      // Local Look At position
		cVector3d(0, 0, 1)        // Local Up Vector
	);

	// position and orient the camera
/*	camera->set(
		cVector3d(-1.0, 0.5, 0.0),    // camera position (eye)
		cVector3d(1.0, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 1.0, 0.0)
	);   // direction of the (up) vector*/

	// set the near and far clipping planes of the camera
	camera->setClippingPlanes(0.01, 10.0);

	camera->setUseMultipassTransparency(true);

	// create a directional light source
	light = new cSpotLight(world);

	// insert light source inside world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// define direction of light beam
	light->setLocalPos(0.0, 2.0, 1.0);

	// define the direction of the light beam
	light->setDir(0, -1, 0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);
#pragma endregion CHAIWorld_Setup

#pragma region


	//--------------------------------------------------------------------------
	// CREATING SHAPES
	//-------------------------------------------------------------------------
	my_cube = new cMesh();
	world->addChild(my_cube);
	// set position
	my_cube->setLocalPos(mycube_posX, mycube_posY, mycube_posZ);
	chai3d::cCreateBox(my_cube, cube_size, cube_size, cube_size);

	target_cube = new cMesh();
	world->addChild(target_cube);
	// set position
	target_cube->setLocalPos(targetcube_posX, targetcube_posY, targetcube_posZ);
	chai3d::cCreateBox(target_cube, cube_size, cube_size, cube_size);

	target_cube->setTransparencyLevel(0.3);
	target_cube->setUseCulling(false);

	if (serialOK) {
		// create a texture
		cTexture2dPtr texture = cTexture2d::create();

		std::cout << "Loading textures..." << endl;

		bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/brick-color.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/brick-color.png");
#endif
		}
		if (!fileload)
		{
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Warning: Cube texture failed to load correctly. Check file location." << endl;
			SetConsoleTextAttribute(hConsole, 7);
			// set material color
			my_cube->m_material->setRedFireBrick();
		}
		// apply texture to object
		my_cube->setTexture(texture);
		// enable texture rendering 
		my_cube->setUseTexture(true);
		// Since we don't need to see our polygons from both sides, we enable culling.
		my_cube->setUseCulling(true);

		std::cout << "Loading normal map..." << endl;

		// create a normal texture
		cNormalMapPtr normalMap = cNormalMap::create();

		// load normal map from file
		fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/brick-normal.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = normalMap->loadFromFile("../../../bin/resources/images/brick-normal.png");
#endif
		}
		if (!fileload)
		{
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Warning: Normal map failed to load correctly. Check file location." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}

		// assign normal map to object
		my_cube->m_normalMap = normalMap;
		// compute surface normals
		my_cube->computeAllNormals();
		// compute tangent vectors
		my_cube->computeBTN();
	}

	//---------------------OBJECT TEST 
	// a virtual object
	cMultiMesh* object;
	// create a virtual mesh
	object = new cMultiMesh();
	// add object to world
	my_cube->addChild(object);
	object->setLocalPos(0, 0, 0.0);
	// load an object file
	bool fileload = object->loadFromFile(RESOURCE_PATH("../resources/Spooder.obj"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = object->loadFromFile("../../../bin/resources/Spooder.obj");
#endif
	}
	if (!fileload)
	{
		cout << "Error - 3D Model failed to load correctly" << endl;
		//close();
		//return (-1);
	}

	// disable culling so that faces are rendered on both sides
	object->setUseCulling(false);

	// resize object to screen
	double size = cSub(object->getBoundaryMax(), object->getBoundaryMin()).length();
	object->scale(0.02);

#pragma endregion CHAIShape_Setup

#pragma region
	std::cout << "Looking for Serial Connection..." << std::endl;
	serialOK = serialPort.connect();

	if (serialOK) {
		Sleep(1000);
		cout << "Found USB device!" << endl << endl;
		cThread *serialThread;
		serialThread = new cThread();
		serialThread->start(UpdateIMUCube, CTHREAD_PRIORITY_GRAPHICS);
	}
	else {
		SetConsoleTextAttribute(hConsole, 0x0e);
		std::cout << "No USB peripheral found. Keyboard interaction only." << endl << endl;
		SetConsoleTextAttribute(hConsole, 7);
	}
#pragma endregion Serial_Setup

#pragma region
	//VICON-------------------------------------------
	//#define output_stream if(!LogFile.empty()) ; else std::cout 

	string HostName = "localhost:801"; //"134.225.86.151"
	unsigned int ClientBufferSize = 0;
	std::string AxisMapping = "ZUp";

	//Make a new client
	static int connectAttempts = 0;
	std::cout << "VICON Connection Test:" << endl;
	// Connect to a server
	std::cout << "Connecting to " << HostName << " ..." << endl << std::flush;

	for (int i = 0; i != 3; ++i) // repeat to check disconnecting doesn't wreck next connect
	{
		while (!MyClient.IsConnected().Connected)
		{
			// Direct connection
			viconConnected = (MyClient.Connect(HostName).Result == Result::Success);

			if (!viconConnected)
			{
				connectAttempts++;
				std::cout << ".";
				if (connectAttempts > 2) { break; }
			}
#ifdef WIN32
			Sleep(1000);
#else
			Sleep(200);
			//sleep(1);
#endif
		}
	}
	if (!viconConnected) {
		SetConsoleTextAttribute(hConsole, 0x0e);
		std::cout << endl << "Unable to connect to VICON. Marker tracking disabled." << endl << endl;
		SetConsoleTextAttribute(hConsole, 7);
	}
	else {
		// Enable some different data types
		MyClient.EnableSegmentData();
		MyClient.EnableMarkerData();
		MyClient.EnableUnlabeledMarkerData();

		// Set the streaming mode
		MyClient.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ServerPush);

		// Set the global up axis
		MyClient.SetAxisMapping(Direction::Forward,
			Direction::Left,
			Direction::Up); // Z-up

		if (AxisMapping == "YUp")
		{
			MyClient.SetAxisMapping(Direction::Forward,
				Direction::Up,
				Direction::Right); // Y-up
		}
		else if (AxisMapping == "XUp")
		{
			MyClient.SetAxisMapping(Direction::Up,
				Direction::Forward,
				Direction::Left); // Y-up
		}

		Output_GetAxisMapping _Output_GetAxisMapping = MyClient.GetAxisMapping();
		std::cout << "Vicon: Axis Mapping: X-" << Adapt(_Output_GetAxisMapping.XAxis)
			<< " Y-" << Adapt(_Output_GetAxisMapping.YAxis)
			<< " Z-" << Adapt(_Output_GetAxisMapping.ZAxis) << std::endl;

		if (ClientBufferSize > 0)
		{
			MyClient.SetBufferSize(ClientBufferSize);
			std::cout << "Vicon: Setting client buffer size to " << ClientBufferSize << std::endl;
		}
		clock_t LastTime = clock();

		//New thread to update VICON position
		viconThread = new cThread();
		viconThread->start(UpdateViconFrame, CTHREAD_PRIORITY_GRAPHICS);



	}

#pragma endregion VICON_Setup

#pragma region
	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------
	// initialize GLFW library
	std::cout << "Checking for OpenGL libraries..." << endl;
	if (!glfwInit())
	{
		std::cout << "GLFW initialisation failed - Check included libraries" << endl;
		cSleepMs(1000);
		return 1;
	}

	// set error callback
	glfwSetErrorCallback(errorCallback);

	// compute desired size of window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int w = 0.8 * mode->height;
	int h = 0.5 * mode->height;
	int x = 0.5 * (mode->width - w);
	int y = 0.5 * (mode->height - h);

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// create display context
	std::cout << "Creating context window..." << endl;
	window = glfwCreateWindow(w, h, "CHAI3D Test", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create window." << endl;
		std::cout << "Please close existing windows and retry" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// set key callback
	glfwSetKeyCallback(window, keyCallback);

	// set current display context
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(0);


#ifdef GLEW_VERSION
	// initialize GLEW library
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW library" << endl;
		glfwTerminate();
		return 1;
	}
#endif

#pragma endregion GLFW_Setup

#pragma region

	//initialise oculus
	std::cout << "Searching for Oculus Rift..." << endl;
	if (!oculusVR.initVR())
	{
		SetConsoleTextAttribute(hConsole, 0x0e);
		std::cout << "Failed to initialize Oculus." << endl;
		std::cout << "Check HDMI and USB are connected" << endl << endl;
		oculusInit = false;
		cSleepMs(1000);
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << "Opening in static screen mode." << endl << endl;
	}
	else {
		// get oculus display resolution
		ovrSizei hmdResolution = oculusVR.getResolution();

		// setup mirror display on computer screen
		ovrSizei windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };

		// inialize buffers
		if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h))
		{
			SetConsoleTextAttribute(hConsole, 4);
			std::cout << "Failed to initialize Oculus buffers. Check the headset view for troubleshooting tips." << endl;
			oculusInit = false;
			cSleepMs(1000);
			oculusVR.destroyVR();
			//renderContext.destroy();
			//glfwTerminate();
			SetConsoleTextAttribute(hConsole, 7);
		}
		else {
			oculusInit = true;
			glfwSetWindowSize(window, windowSize.w, windowSize.h);


		}
	}

	if (!oculusInit) {
		// get width and height of window
		glfwGetWindowSize(window, &width, &height);
		// set position of window
		glfwSetWindowPos(window, x, y);
		// set resize callback
		glfwSetWindowSizeCallback(window, windowSizeCallback);
	}
#pragma endregion OCULUS_Setup

#pragma region

#pragma endregion Run Trials

	for (int i = 0; i < numTrials; i++) {
		cout << "Trial #" << i + 1 << ": " << trialList[i].moveType << endl;
	}

#pragma region

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------
	if (oculusInit) {
		//Create a virtual mesh
		cout << "Setup globe" << endl;
		cMesh* globe = new cMesh();

		world->addChild(globe);
		globe->setLocalPos(0, 0, 0);
		cCreateSphere(globe, 6.0, 360, 360);
		globe->setUseDisplayList(true);
		globe->deleteCollisionDetector();
		cTexture2dPtr textureW = cTexture2d::create();
		cout << "Loading world texture..." << endl;

		bool fileload = textureW->loadFromFile(RESOURCE_PATH("../resources/infinity.jpg"));
		if (!fileload) {
			SetConsoleTextAttribute(hConsole, 0x0e);
			cout << "Warning: failed to load world texture. Check file location." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}
		globe->setTexture(textureW);
		globe->setUseTexture(true);
		globe->setUseCulling(false);
		globe->setUseMaterial(false);
	}
	
	// setup callback when application exits
	atexit(close);

	// call window size callback at initialization
	//windowSizeCallback(window, width, height);
	std::cout << "Solution loaded." << endl << endl;

	std::cout << "Press [1] for trial 1, [2] for trial 2." << endl;

	if (oculusInit) {
		oculusVR.recenterPose();
		std::cout << "Centred HMD view." << endl;
	}

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		if (oculusInit) {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			oculusVR.onRenderStart();
			if ((_Output_GetUnlabeledMarkerGlobalTranslation.Translation[0] != 0) && (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[1] != 0) && (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[2] != 0)) {
				PrintHMDPos();
				PrintMarkerPos();
			}

			// render frame for each eye
			for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
			{
				// retrieve projection and modelview matrix from oculus
				cTransform projectionMatrix, modelViewMatrix;
				oculusVR.onEyeRender(eyeIndex, projectionMatrix, modelViewMatrix);

				camera->m_useCustomProjectionMatrix = true;
				camera->m_projectionMatrix = projectionMatrix;

				camera->m_useCustomModelViewMatrix = true;
				camera->m_modelViewMatrix = modelViewMatrix;

				// render world
				ovrSizei size = oculusVR.getEyeTextureSize(eyeIndex);
				camera->renderView(size.w, size.h, C_STEREO_LEFT_EYE, false);

				// finalize rendering  
				oculusVR.onEyeRenderFinish(eyeIndex);

				// render graphics

			}
			updateGraphics();
			// update frames
			oculusVR.submitFrame();
			oculusVR.blitMirror();
		}
		else {
			// get width and height of window
			glfwGetWindowSize(window, &width, &height);

			// render graphics
			updateGraphics();
		}

		// swap buffers
		glfwSwapBuffers(window);

		// process events
		glfwPollEvents();
	}
#pragma endregion Simulation

	// close window
	glfwDestroyWindow(window);

	// terminate GLFW library
	glfwTerminate();

	// exit
	return (0);
}

#pragma region

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// update window size
	width = a_width;
	height = a_height;
}

void errorCallback(int a_error, const char* a_description)
{
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "Error: " << a_description << endl;
	SetConsoleTextAttribute(hConsole, 7);
}

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		return;
	}

	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	// option - spacebar
	else if (a_key == GLFW_KEY_SPACE)
	{
		if (oculusInit) {
			oculusVR.recenterPose();
		}
	}

	// option - trial 1
	else if (a_key == GLFW_KEY_1) {
		//Start trial 1
		std::cout << "Starting Trial 1: Right to Left" << endl;
		targetcube_posX = 0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveLeft, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Warning: Finish previous trial before next one." << endl;
		}
		SetConsoleTextAttribute(hConsole, 7);
	}

	else if (a_key == GLFW_KEY_2) {
		//Start trial 2
		std::cout << "Starting Trial 2: Left to Right" << endl;
		targetcube_posX = -0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveRight, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Error: Finish previous trial before next one." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}
	}
	else if (a_key == GLFW_KEY_3) {
		//Start trial 2
		std::cout << "Starting Trial 3: Left to Right (Sine Wave)" << endl;
		targetcube_posX = -0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveRightSine, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Error: Finish previous trial before next one." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}
	}
	else if (a_key == GLFW_KEY_4) {
		//Start trial 2
		std::cout << "Starting Trial 4: Right to Left (Sine Wave)" << endl;
		targetcube_posX = 0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveLeftSine, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			SetConsoleTextAttribute(hConsole, 0x0e);
			std::cout << "Error: Finish previous trial before next one." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}
	}
	else if (a_key == GLFW_KEY_5) {
		//Rotate X
		RotateCube(1, 0, 0, 10);
	}
	else if (a_key == GLFW_KEY_6) {
		//Rotate Y
		RotateCube(0, 1, 0, 10);
	}
	else if (a_key == GLFW_KEY_7) {
		//Rotate Z
		RotateCube(0, 0, 1, 10);
	}

	else if (a_key == GLFW_KEY_W) {
		cout << "W";
		MoveDirection('y', -0.1);
	}
	else if (a_key == GLFW_KEY_A) {
		//Move left
		MoveDirection('x', 0.1);
	}
	else if (a_key == GLFW_KEY_S) {
		//Move backward
		MoveDirection('y', 0.1);
	}
	else if (a_key == GLFW_KEY_D) {
		//Move right
		MoveDirection('x', -0.1);
	}
	else if (a_key == GLFW_KEY_R) {
	//Move up
		MoveDirection('z', 0.1);
	}
	else if (a_key == GLFW_KEY_F) {
	//Move down
		MoveDirection('z', -0.1);
	}
}

#pragma endregion Callbacks

//------------------------------------------------------------------------------

void close(void)
{
	if (serialOK) {
		serialPort.flush();
		serialPort.disconnect();
	}

	if (viconConnected) {
		MyClient.DisableSegmentData();
		MyClient.DisableMarkerData();
		MyClient.DisableUnlabeledMarkerData();
		MyClient.DisableDeviceData();

		// Disconnect and dispose
		int t = clock();
		std::cout << " Disconnecting VICON..." << std::endl;
		MyClient.Disconnect();
		int dt = clock() - t;
		double secs = (double)(dt) / (double)CLOCKS_PER_SEC;
		std::cout << " Disconnect time = " << secs << " secs" << std::endl;
	}

	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	// delete resources
	delete world;
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{
	// RENDER SCENE

	// render world
	camera->renderView(width, height);

	//set cube pos
	my_cube->setLocalPos(mycube_posX, mycube_posY, mycube_posZ+1.3);
	target_cube->setLocalPos(targetcube_posX, targetcube_posY, targetcube_posZ+1.0);

	//PrintCubePos();


	if (serialOK) {
		cQuaternion qRotation = quaternion;
		//cout << quaternion[0] << "," << quaternion[1] << "," << quaternion[2] << "," << quaternion[3] <<endl;
		cMatrix3d qRotMatrix;
		cMatrix3d localRotA = my_cube->getLocalRot();
		qRotation.toRotMat(qRotMatrix);
		my_cube->setLocalRot(qRotMatrix);
	}

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	SetConsoleTextAttribute(hConsole, 4);
	if (err != GL_NO_ERROR) std::cout << "Error:  %s\n" << gluErrorString(err);
	SetConsoleTextAttribute(hConsole, 7);
}
//------------------------------------------------------------------------------

//------MOVE CUBE METHODS----------------------

void UpdateIMUCube() {
	vector <string> lastStringBuffer;
	vector <string> stringBuffer;
	static string string;

	bool dataReceived = false;

	cout << "Serial: IMU thread created - Arduino mode." << endl;
	while (true)
	{
		int buffer;

		while (buffer = serialPort.readByte())
		{
			//	cout << "Buffer enter";
			if (!dataReceived) {
				if (buffer = 59) { dataReceived = true; } //All 'packets' are ; terminated
			}

			else {
				if (buffer > 44 && buffer < 58) { // If 0 - 9 (or - .)
					string += (char)buffer;
				}
				else if (buffer == 65) {
					OnButtonUp();
				}
				else if (buffer == 44) {
					//cout << string;
					stringBuffer.push_back(string);
					string = "";
				}
				else if (buffer == 13 || buffer == 10) //13 CR or 10 LF
				{
					//	cout << " endline ";
				}

				else if (buffer = 59) {
					string::size_type sz;

					stringBuffer.push_back(string);
					string = "";

					lastStringBuffer = stringBuffer;
					stringBuffer.resize(0);
					if (lastStringBuffer.size() == 4) {
						try {
							for (int i = 0; i < lastStringBuffer.size(); i++) {
								quaternion[i] = stod(lastStringBuffer[i], &sz);
							}
						}
						catch (exception e) {}
					}
					else cout << "Serial: Wrong size array of data." << endl;
				}
				else cout << "?";
			}
		}
	}

}

void MoveLeftSine() {
	trialRunning = true;
	while (targetcube_posX > -0.5) {
		targetcube_posX -= 0.0005;
		targetcube_posZ = 0.1*(std::sin(10 * targetcube_posX));
		cSleepMs(1);
	}
	trialRunning = false;
	return;
}

void MoveRightSine() {
	trialRunning = true;
	while (targetcube_posX < 0.5) {
		targetcube_posX += 0.0005;
		targetcube_posZ = 0.1*(sin(10 * targetcube_posX));
		cSleepMs(1);
	}
	trialRunning = false;
	return;
}

void MoveLeft() {
	trialRunning = true;
	while (targetcube_posX > -0.5) {
		targetcube_posX -= 0.0005;
		cSleepMs(1);
	}
	trialRunning = false;
	return;
}

void MoveRight() {
	trialRunning = true;
	while (targetcube_posX < 0.5) {
		targetcube_posX += 0.0005;
		cSleepMs(1);
	}
	trialRunning = false;
	return;
}

void MoveDirection(char axis, double amnt) {
	if (axis == 'x') {
		mycube_posX += amnt;
		cout << amnt;
	}
	else if (axis == 'y') {
		mycube_posY += amnt;
		cout << amnt;
	}
	else if (axis == 'z') {
		mycube_posZ += amnt;
		cout << "x";
	}
	else return;
}

void RotateCube(int x, int y, int z, double degrees)
{
	int Xaxis = x, Yaxis = y, Zaxis = z;
	double angle = degrees;

	cMatrix3d Rotator = cMatrix3d();
	Rotator.setAxisAngleRotationDeg(Xaxis, Yaxis, Zaxis, angle);
	cMatrix3d CurrentAngle = my_cube->getLocalRot();
	my_cube->setLocalRot(Rotator*CurrentAngle);
}

void PrintHMDPos() {
	try {
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.x << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.y << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.z << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.w << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.x << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.y << ",";
		chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.z << endl;
	}
	catch (exception e) {
		std::cout << "Couldn't find log file!" << endl;
	}
}

void PrintMarkerPos() {
	viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[0]);
	viconfile << (',');
	viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[1]);
	viconfile << (',');
	viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[2]);
	viconfile << endl;
}

void PrintCubePos() {
	double myx = my_cube->getLocalPos().x();
	double myy = my_cube->getLocalPos().y();
	double myz = my_cube->getLocalPos().z();
	double targetx = target_cube->getLocalPos().x();
	double targety = target_cube->getLocalPos().y();
	double targetz = target_cube->getLocalPos().z();
	edistance = sqrt(pow((targetx - myx), 2) + pow((targety - myy), 2) + pow((targetz - myz), 2));
	cubefile << targetx << "," << targety << "," << targetz << ",";
	cubefile << myx << "," << myy << "," << myz << ",";
	cubefile << edistance << endl;
}

void OnButtonUp() {
	cubeTransparent = !cubeTransparent;
	if (cubeTransparent) {
		my_cube->setTransparencyLevel(0.2);
		my_cube->setUseCulling(false);
	}
	else {
		my_cube->setTransparencyLevel(1);
		my_cube->setUseCulling(true);
	}
}

void createTrials(int total) {
	
}

void CallMotor() {
	//serialPort.
}

void UpdateViconFrame() {
	// Loop until a key is pressed
#ifdef WIN32
	while (!Hit())
#else
	while (true)
#endif
	{
		// Get a frame
		//std::cout << "Vicon: Waiting for new frame...";
		while (MyClient.GetFrame().Result != Result::Success)
		{
			// Sleep a little so that we don't lumber the CPU with a busy poll
#ifdef WIN32
			Sleep(200);
#else
			Sleep(200);
			//sleep(1);
#endif
		}

		// Get the frame number
		Output_GetFrameNumber _Output_GetFrameNumber = MyClient.GetFrameNumber();
		// Count the number of subjects
		unsigned int SubjectCount = MyClient.GetSubjectCount().SubjectCount;

		for (unsigned int SubjectIndex = 0; SubjectIndex < SubjectCount; ++SubjectIndex)
		{
			// Get the subject name
			std::string SubjectName = MyClient.GetSubjectName(SubjectIndex).SubjectName;

			// Count the number of markers
			unsigned int MarkerCount = MyClient.GetMarkerCount(SubjectName).MarkerCount;
			for (unsigned int MarkerIndex = 0; MarkerIndex < MarkerCount; ++MarkerIndex)
			{
				// Get the marker name
				std::string MarkerName = MyClient.GetMarkerName(SubjectName, MarkerIndex).MarkerName;

				// Get the global marker translation
				_Output_GetMarkerGlobalTranslation =
					MyClient.GetMarkerGlobalTranslation(SubjectName, MarkerName);
			}
		}
		// Get the unlabeled markers
		unsigned int UnlabeledMarkerCount = MyClient.GetUnlabeledMarkerCount().MarkerCount;
		std::cout << "Unlabeled Markers (" << UnlabeledMarkerCount << "):" << std::endl;
		//for (unsigned int LabeledMarkerIndex = 0; LabeledMarkerIndex < LabeledMarkerCount; ++LabeledMarkerIndex)
		for (unsigned int UnlabeledMarkerIndex = 0; UnlabeledMarkerIndex < 1; ++UnlabeledMarkerIndex)
		{
			// Get the global marker translation
			_Output_GetUnlabeledMarkerGlobalTranslation =
				MyClient.GetUnlabeledMarkerGlobalTranslation(UnlabeledMarkerIndex);
			if (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[0] != 0 && _Output_GetUnlabeledMarkerGlobalTranslation.Translation[1] != 0 && _Output_GetUnlabeledMarkerGlobalTranslation.Translation[2] != 0){
				double HMDX = -(_Output_GetUnlabeledMarkerGlobalTranslation.Translation[0] / 1000);
				double HMDY = -(_Output_GetUnlabeledMarkerGlobalTranslation.Translation[1] / 1000); //- 1.5;
				double HMDZ = (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[2] / 1000);// - 1;
			}
		}

		// Get the labeled markers
		unsigned int LabeledMarkerCount = MyClient.GetLabeledMarkerCount().MarkerCount;
		std::cout << "    Labeled Markers (" << LabeledMarkerCount << "):" << std::endl;
		//for (unsigned int LabeledMarkerIndex = 0; LabeledMarkerIndex < LabeledMarkerCount; ++LabeledMarkerIndex)
		for (unsigned int LabeledMarkerIndex = 0; LabeledMarkerIndex < 1; ++LabeledMarkerIndex)
		{
			// Get the global marker translation
			_Output_GetLabeledMarkerGlobalTranslation =
				MyClient.GetLabeledMarkerGlobalTranslation(LabeledMarkerIndex);
			double Marker1X = -(_Output_GetLabeledMarkerGlobalTranslation.Translation[0] / 1000);
			double Marker1Y = -(_Output_GetLabeledMarkerGlobalTranslation.Translation[1] / 1000); //- 1.5;
			double Marker1Z = (_Output_GetLabeledMarkerGlobalTranslation.Translation[2] / 1000);// - 1;



			cout << "Marker 1: XYZ = " << Marker1X << ", " << Marker1Y << ", " << Marker1Z << "!" << endl;
			/*	std::cout << "      Marker #" << LabeledMarkerIndex << ": ("
			<< _Output_GetLabeledMarkerGlobalTranslation.Translation[0] << ", "
			<< _Output_GetLabeledMarkerGlobalTranslation.Translation[1] << ", "
			<< _Output_GetLabeledMarkerGlobalTranslation.Translation[2] << ")" << std::endl;
			viconfile << (_Output_GetLabeledMarkerGlobalTranslation.Translation[0]);
			viconfile << (',');
			viconfile << (_Output_GetLabeledMarkerGlobalTranslation.Translation[1]);
			viconfile << (',');
			viconfile << (_Output_GetLabeledMarkerGlobalTranslation.Translation[2]);
			viconfile << endl;*/
			mycube_posX = Marker1X;
			mycube_posY = Marker1Y;
			mycube_posZ = Marker1Z;
		}
	}
}


//------------------------------------------------------------------------------

