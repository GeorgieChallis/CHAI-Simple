//-----------------------------------
#pragma region
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <string.h>

#include <cmath>

#ifdef WIN32
#include <conio.h>   // For _kbhit()
#include <cstdio>   // For getchar()
#include <windows.h> // For Sleep()
#endif // WIN32

#include <chai3d.h>
//OpenGL Wrapper
#include <GLFW/glfw3.h>
//OCULUS SDK
#include <COculus.h>
//For VICON Code
#include "DataStreamClient.h"
// FOR C++ Serial Port Class
#include "SerialPort.cpp"
#include "Defaults.h"

#include "ubLogger.h"
#include "ubCube.h"
#pragma endregion Includes

#define RESOURCE_PATH(p)	(char*)((resourceRoot+std::string(p)).c_str())

#pragma region
//-Namespaces-------------------------------------------------------------------
using namespace chai3d;
using namespace ViconDataStreamSDK::CPP;
//------------------------------------------------------------------------------
#pragma  endregion Namespaces

#pragma region
static bool trialRunning = false;
static int numTrials = 0;
double edistance; //Euclidean distance between cubes

struct trial {int moveType;};
std::vector<trial> trialList;
#pragma endregion Trial Data

#pragma region
//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------
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
cThread *serialThread;

//Values for changing the visible cube's position/size
static double mycube_posX = 0.0;
static double mycube_posY = 0.0;
static double mycube_posZ = 0.0;

static double targetcube_posX = 0.0;
static double targetcube_posY = 0.0;
static double targetcube_posZ = 0.0;

//-----------------------------------------------------------------

#pragma endregion Global variables - CHAI

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

static ubCube ubiCube;
ubLogger ubLog;

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

void ChaiWorldSetup();
void ChaiShapeSetup();

//Print headset position to file
void PrintHMDPos(void);
void PrintMarkerPos(void);

void UpdateMyCube(void);
void UpdateViconFrame(void);

//Move Cube
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
	std::string resourceRoot = std::string(argv[0]).substr(0, std::string(argv[0]).find_last_of("/\\") + 1);

	//Introductory text and filename/trial nums
	numTrials = ubLog.LogInit();

	if (numTrials > 0) {
		for (int i = 0; i < numTrials; i++) {
			trial newTrial;
			newTrial.moveType = rand() % 4 + 1;
			std::cout << "i: " << i << ", trials: " << newTrial.moveType << std::endl;
			trialList.push_back(newTrial);
		}
	}

	ChaiWorldSetup();
	ChaiShapeSetup();

	if (ubiCube.m_serialOK) {
		// create a texture
		cTexture2dPtr texture = cTexture2d::create();
		ubLog.Info("Loading textures...");
		bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/brick-color.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/brick-color.png");
#endif
		}
		if (!fileload)
		{
			ubLog.Warn("Cube texture failed to load correctly. Check file location.");
		}
		// apply texture to object
		my_cube->setTexture(texture);
		// enable texture rendering 
		my_cube->setUseTexture(true);
		// Since we don't need to see our polygons from both sides, we enable culling.
		my_cube->setUseCulling(true);
		
		ubLog.Info("Loading normal map...");
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
			ubLog.Warn("Normal map failed to load correctly. Check file location.");
		}
		else {
			// assign normal map to object
			my_cube->m_normalMap = normalMap;
			// compute surface normals
			my_cube->computeAllNormals();
			// compute tangent vectors
			my_cube->computeBTN();
		}
	}

	// Spider object as child of main cube:
	cMultiMesh* object;
	object = new cMultiMesh();
	// add object to cube
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
		ubLog.Warn("Error - 3D Model failed to load correctly");
	}

	// disable culling so that faces are rendered on both sides
	object->setUseCulling(false);

	// resize object to screen
	double size = cSub(object->getBoundaryMax(), object->getBoundaryMin()).length();
	object->scale(0.02);

#pragma endregion CHAIShape_Setup

#pragma region
	ubLog.Info("Looking for Serial Connection...");
	ubiCube.m_serialOK = ubiCube.m_serialPort.connect();

	if (ubiCube.m_serialOK) {
		Sleep(1000);
		ubLog.Info("Found USB device!");
		serialThread = new cThread();
		serialThread->start(UpdateMyCube, CTHREAD_PRIORITY_GRAPHICS);
	}
	else {
		ubLog.Warn("No USB peripheral found. Keyboard interaction only.");
	}
#pragma endregion Serial_Setup

#pragma region
	//VICON-------------------------------------------
	std::string HostName = "localhost:801";
	unsigned int ClientBufferSize = 0;
	std::string AxisMapping = "ZUp";

	//Make a new client
	static int connectAttempts = 0;
	ubLog.Info("VICON Connection Test:");
	// Connect to a server
	std::cout << std::endl << "Connecting to " << HostName << " ..." << std::endl << std::flush;

	for (int i = 0; i != 3; ++i) // repeat to check disconnecting doesn't wreck next connect
	{
		while (!MyClient.IsConnected().Connected)
		{
			// Direct connection
			viconConnected = (MyClient.Connect(HostName).Result == Result::Success);

			if (!viconConnected)
			{
				connectAttempts++;
				std::cout  << ".";
				if (connectAttempts > 2) { break; }
			}
#ifdef WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
		}
	}
	if (!viconConnected) {
		ubLog.Warn("Unable to connect to VICON. Marker tracking disabled.");
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
		std::cout  << "Vicon: Axis Mapping: X-" << Adapt(_Output_GetAxisMapping.XAxis)
			<< " Y-" << Adapt(_Output_GetAxisMapping.YAxis)
			<< " Z-" << Adapt(_Output_GetAxisMapping.ZAxis) << std::endl;
		if (ClientBufferSize > 0)
		{
			MyClient.SetBufferSize(ClientBufferSize);
			std::cout  << "Vicon: Setting client buffer size to " << ClientBufferSize << std::endl;
		}
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
	ubLog.Info("Checking for OpenGL libraries...");
	if (!glfwInit())
	{
		ubLog.Error("GLFW initialisation failed - Check included libraries");
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
	ubLog.Info("Creating context window...");
	window = glfwCreateWindow(w, h, "CHAI3D Test", NULL, NULL);
	if (!window)
	{
		ubLog.Error("Failed to create window.");
		std::cout  << "Please close existing windows and retry" << std::endl;
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
		ubLog.Error("Failed to initialize GLEW library");
		glfwTerminate();
		return 1;
	}
#endif

#pragma endregion GLFW_Setup

#pragma region

	//initialise oculus
	ubLog.Info("Searching for Oculus Rift...");
	if (!oculusVR.initVR())
	{
		ubLog.Warn("Failed to initialize Oculus.\nCheck HDMI and USB are connected");
		oculusInit = false;
		cSleepMs(1000);
		SetConsoleTextAttribute(ubLog.hConsole, 7);
		ubLog.Info("Opening in static screen mode.");
	}
	else {
		// get oculus display resolution
		ovrSizei hmdResolution = oculusVR.getResolution();

		// setup mirror display on computer screen
		ovrSizei windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };

		// inialize buffers
		if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h))
		{
			ubLog.Error("Failed to initialize Oculus buffers.\nCheck the headset view for troubleshooting tips.");
			oculusInit = false;
			cSleepMs(1000);
			oculusVR.destroyVR();
			//renderContext.destroy();
			//glfwTerminate();
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
		std::cout  << "Trial #" << i + 1 << ": " << trialList[i].moveType << std::endl;
	}

#pragma region

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------
	if (oculusInit) {
		//Create a virtual mesh
		ubLog.Info("Setup globe...");
		cMesh* globe = new cMesh();

		world->addChild(globe);
		globe->setLocalPos(0, 0, 0);
		cCreateSphere(globe, 6.0, 360, 360);
		globe->setUseDisplayList(true);
		globe->deleteCollisionDetector();
		cTexture2dPtr textureW = cTexture2d::create();
		ubLog.Info("Loading world texture...");

		bool fileload = textureW->loadFromFile(RESOURCE_PATH("../resources/infinity.jpg"));
		if (!fileload) {
			ubLog.Warn("ailed to load world texture. Check file location.");
		}
		globe->setTexture(textureW);
		globe->setUseTexture(true);
		globe->setUseCulling(false);
		globe->setUseMaterial(false);
	}
	
	// setup callback when application exits
	atexit(close);

	// call window size callback at initialization //windowSizeCallback(window, width, height);
	ubLog.Info("Solution loaded!");

	std::cout  << "Press [1] for trial 1, [2] for trial 2." << std::endl;

	if (oculusInit) {
		oculusVR.recenterPose();
		ubLog.Info("Centred HMD view.");
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
	ubLog.Error(a_description);
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
		std::cout  << "Starting Trial 1: Right to Left" << std::endl;
		targetcube_posX = 0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveLeft, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			ubLog.Warn("Finish previous trial before next one.");
		}
		SetConsoleTextAttribute(ubLog.hConsole, 7);
	}

	else if (a_key == GLFW_KEY_2) {
		//Start trial 2
		std::cout  << "Starting Trial 2: Left to Right" << std::endl;
		targetcube_posX = -0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveRight, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			ubLog.Warn("Error: Finish previous trial before next one.");
		}
	}
	else if (a_key == GLFW_KEY_3) {
		//Start trial 2
		std::cout  << "Starting Trial 3: Left to Right (Sine Wave)" << std::endl;
		targetcube_posX = -0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveRightSine, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			ubLog.Warn("Error: Finish previous trial before next one.");
		}
	}
	else if (a_key == GLFW_KEY_4) {
		//Start trial 2
		std::cout  << "Starting Trial 4: Right to Left (Sine Wave)" << std::endl;
		targetcube_posX = 0.5;
		//cube_posZ = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveLeftSine, CTHREAD_PRIORITY_GRAPHICS);
		}
		else {
			ubLog.Warn("Error: Finish previous trial before next one.");
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
		std::cout  << "W";
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
	if (ubiCube.m_serialOK) {
		ubiCube.m_serialPort.flush();
		ubiCube.m_serialPort.disconnect();
	}

	if (viconConnected) {
		MyClient.DisableSegmentData();
		MyClient.DisableMarkerData();
		MyClient.DisableUnlabeledMarkerData();
		MyClient.DisableDeviceData();

		// Disconnect and dispose
		int t = clock();
		ubLog.Info("Disconnecting VICON...");
		MyClient.Disconnect();
		int dt = clock() - t;
		double secs = (double)(dt) / (double)CLOCKS_PER_SEC;
		std::cout  << " Disconnect time = " << secs << " secs" << std::endl;
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
	my_cube->setLocalPos(mycube_posX, mycube_posY, mycube_posZ);
	target_cube->setLocalPos(targetcube_posX, targetcube_posY, targetcube_posZ+1.4);

	if (ubiCube.m_serialOK) {
		cQuaternion qRotation = ubiCube.m_quaternion;
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
	if (err != GL_NO_ERROR) {
		ubLog.Error((const char*)gluErrorString(err));
	}
}

//------MOVE CUBE METHODS----------------------
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
		std::cout  << amnt;
	}
	else if (axis == 'y') {
		mycube_posY += amnt;
		std::cout  << amnt;
	}
	else if (axis == 'z') {
		mycube_posZ += amnt;
		std::cout  << "x";
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
		ubLog.Info("Eriting Pos to file...");
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.y << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.x << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.z << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.w << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.x << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.y << ",";
		ubLog.chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.z << std::endl;
	}
	catch (std::exception e) {
		ubLog.Error("Couldn't find HMD log file!");
	}
}

void PrintMarkerPos() {
	ubLog.viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[0]);
	ubLog.viconfile << (',');
	ubLog.viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[1]);
	ubLog.viconfile << (',');
	ubLog.viconfile << (_Output_GetUnlabeledMarkerGlobalTranslation.Translation[2]);
	ubLog.viconfile << std::endl;
}

void PrintCubePos() {
	double myx = my_cube->getLocalPos().x();
	double myy = my_cube->getLocalPos().y();
	double myz = my_cube->getLocalPos().z();
	double targetx = target_cube->getLocalPos().x();
	double targety = target_cube->getLocalPos().y();
	double targetz = target_cube->getLocalPos().z();
	edistance = sqrt(pow((targetx - myx), 2) + pow((targety - myy), 2) + pow((targetz - myz), 2));
	ubLog.cubefile << targetx << "," << targety << "," << targetz << ",";
	ubLog.cubefile << myx << "," << myy << "," << myz << ",";
	ubLog.cubefile << edistance << std::endl;
}

void createTrials(int total) {
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
		while (MyClient.GetFrame().Result != Result::Success)
		{
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
		std::cout  << "Unlabeled Markers (" << UnlabeledMarkerCount << "):" << std::endl;
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
		std::cout  << "    Labeled Markers (" << LabeledMarkerCount << "):" << std::endl;
		//for (unsigned int LabeledMarkerIndex = 0; LabeledMarkerIndex < LabeledMarkerCount; ++LabeledMarkerIndex)
		for (unsigned int LabeledMarkerIndex = 0; LabeledMarkerIndex < 1; ++LabeledMarkerIndex)
		{
			// Get the global marker translation
			_Output_GetLabeledMarkerGlobalTranslation =
				MyClient.GetLabeledMarkerGlobalTranslation(LabeledMarkerIndex);
			double Marker1X = -(_Output_GetLabeledMarkerGlobalTranslation.Translation[0] / 1000);
			double Marker1Y = -(_Output_GetLabeledMarkerGlobalTranslation.Translation[1] / 1000); //- 1.5;
			double Marker1Z = (_Output_GetLabeledMarkerGlobalTranslation.Translation[2] / 1000);// - 1;



			std::cout  << "Marker 1: XYZ = " << Marker1X << ", " << Marker1Y << ", " << Marker1Z << "!" << std::endl;
			mycube_posX = Marker1X;
			mycube_posY = Marker1Y;
			mycube_posZ = Marker1Z;
		}
	}
}

void UpdateMyCube() {
	ubiCube.UpdateIMUCube();
}

void ChaiWorldSetup() {
	// WORLD - CAMERA - LIGHTING
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

	// set the near and far clipping planes of the camera
	camera->setClippingPlanes(0.01, 10.0);
	camera->setUseMultipassTransparency(true);

	// create a directional light source
	light = new cSpotLight(world);
	// insert light source inside world
	world->addChild(light);
	// enable light source
	light->setEnabled(true);
	// define position of light beam
	light->setLocalPos(0.0, 2.0, 1.0);
	// define the direction of the light beam
	light->setDir(0, -1, 0);
	// set light cone half angle
	light->setCutOffAngleDeg(50);
}

void ChaiShapeSetup() {
	// CREATING SHAPES
	my_cube = new cMesh();
	world->addChild(my_cube);
	my_cube->setLocalPos(mycube_posX, mycube_posY, mycube_posZ);
	chai3d::cCreateBox(my_cube, ubiCube.m_meshSize, ubiCube.m_meshSize, ubiCube.m_meshSize);

	target_cube = new cMesh();
	world->addChild(target_cube);
	target_cube->setLocalPos(targetcube_posX, targetcube_posY, targetcube_posZ);
	chai3d::cCreateBox(target_cube, ubiCube.m_meshSize, ubiCube.m_meshSize, ubiCube.m_meshSize);

	target_cube->setTransparencyLevel(0.3);
	target_cube->setUseCulling(false);
}


