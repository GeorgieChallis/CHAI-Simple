//-----------------------------------
//CHAI3D
#include "chai3d.h"
//OpenGL Wrapper
#include <GLFW/glfw3.h>

//OCULUS SDK
#include "COculus.h"

//For VICON Code
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <conio.h>   // For _kbhit()
#include <cstdio>   // For getchar()
#include <windows.h> // For Sleep()
#endif // WIN32

#define _CRT_SECURE_NO_WARNINGS

//#include "DataStreamClient.h"

//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

//------------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------
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

//----------------------------------------
//	SETUP MICROBIT - Serial
//----------------------------------------




//----------------------------------------
// SETUP RIFT
//----------------------------------------
cOVRRenderContext renderContext;
cOVRDevice oculusVR;
bool oculusInit = false;

// ------ LOGGING -----------
ofstream chaifile("hmd-chaipos.csv", ios::app);
ofstream viconfile("hmd-viconpos.txt", ios::app);


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
void PrintHMDPos();

//Move Cube
void MoveLeft(void);
void MoveRight(void);
void RotateCube(int x, int y, int z, double degrees);

// this function closes the application
void close(void);

//==============================================================================
//---------------------------------------------------------------


static bool trialRunning = false;
int trialNumber = 0;

cThread *cubeThread;

//Values for changing the cube's position/size
static double cube_posX = 0.0;
static double cube_posY = 0.0;
static double cube_posZ = 0.0;
static double cube_size = 0.2;

//-----------------------------------------------------------------

int main(int argc, char* argv[])
{
	cout << endl;
	cout << "----------------------------------------" << endl;
	// parse first arg to try and locate resources
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);

	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLFW library
	cout << "Checking for OpenGL libraries..." << endl;
	if (!glfwInit())
	{
		cout << "GLFW initialisation failed - Check included libraries" << endl;
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
	cout << "Creating context window..." << endl;
	window = glfwCreateWindow(w, h, "CHAI3D Test", NULL, NULL);
	if (!window)
	{
		cout << "Failed to create window." << endl;
		cout << "Please close existing windows and retry" << endl;
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
		cout << "Failed to initialize GLEW library - did you mean to include this?" << endl;
		glfwTerminate();
		return 1;
	}
#endif

	//initialise oculus
	cout << "Searching for Oculus Rift..." << endl;
	if (!oculusVR.initVR())
	{
		SetConsoleTextAttribute(hConsole, 0x0e);
		cout << "Failed to initialize Oculus." << endl;
		cout << "Check HDMI and USB are connected" << endl;
		oculusInit = false;
		cSleepMs(1000);
		SetConsoleTextAttribute(hConsole, 7);
		cout << "Opening in static screen mode." << endl << endl;
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
			cout << "Failed to initialize Oculus buffers. Check the headset view for troubleshooting tips." << endl;
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

	// position and orient the camera
	camera->set(cVector3d(0.0, 1.5, 0.0),    // camera position (eye)
		cVector3d(0.2, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 1.0, 0.0));   // direction of the (up) vector

// set the near and far clipping planes of the camera
	camera->setClippingPlanes(0.01, 10.0);

	// create a directional light source
	light = new cSpotLight(world);

	// insert light source inside world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// define direction of light beam
	light->setLocalPos(3.5, 2.0, 0.0);

	// define the direction of the light beam
	light->setDir(-3.5, -2.0, 0.0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);

	//--------------------------------------------------------------------------
	// CREATING SHAPES
	//-------------------------------------------------------------------------
	my_cube = new cMesh();
	world->addChild(my_cube);

	// set position
	my_cube->setLocalPos(0, 0, 0);

	cCreateBox(my_cube, cube_size, cube_size, cube_size);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	cout << "Loading textures..." << endl;

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
		cout << "Warning: Cube texture failed to load correctly. Check file location." << endl;
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

	cout << "Loading normal map..." << endl;

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
		cout << "Warning: Normal map failed to load correctly. Check file location." << endl;
		SetConsoleTextAttribute(hConsole, 7);
	}

	// assign normal map to object
	my_cube->m_normalMap = normalMap;

	// compute surface normals
	my_cube->computeAllNormals();

	// compute tangent vectors
	my_cube->computeBTN();

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------
	// setup callback when application exits
	atexit(close);

	// call window size callback at initialization
	//windowSizeCallback(window, width, height);
	cout << "Solution loaded." << endl << endl;

	cout << "Press [1] for trial 1, [2] for trial 2." << endl;

	if (oculusInit) {		
		oculusVR.recenterPose();
		cout << "Centred HMD view." << endl;
	}

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		if (oculusInit) {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			oculusVR.onRenderStart();
			PrintHMDPos();

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
			}

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

	// close window
	glfwDestroyWindow(window);

	// terminate GLFW library
	glfwTerminate();

	// exit
	return (0);
}

//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// update window size
	width = a_width;
	height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
	SetConsoleTextAttribute(hConsole, 4);
	cout << "Error: " << a_description << endl;
	SetConsoleTextAttribute(hConsole, 7);
}

//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------

void close(void)
{
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
	my_cube->setLocalPos(cube_posX, cube_posY, cube_posZ);

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	SetConsoleTextAttribute(hConsole, 4);
	if (err != GL_NO_ERROR) cout << "Error:  %s\n" << gluErrorString(err);
	SetConsoleTextAttribute(hConsole, 7);
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------

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
		if(oculusInit) { 	
			PrintHMDPos();
			oculusVR.recenterPose(); 
		}
	}


	// option - trial 1
	else if (a_key == GLFW_KEY_1) {
		//Start trial 1
		cout << "Starting Trial 1: Right to Left" << endl;
		cube_posZ = 0.5;
		cube_posY = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveLeft, CTHREAD_PRIORITY_GRAPHICS);
		}
		else { 
			SetConsoleTextAttribute(hConsole, 0x0e);
			cout << "Warning: Finish previous trial before next one." << endl; }
			SetConsoleTextAttribute(hConsole, 7);
	}

	else if (a_key == GLFW_KEY_2) {
		//Start trial 2
		cout << "Starting Trial 2: Left to Right" << endl;
		cube_posZ = -0.5;
		cube_posY = 0.2;
		if (!trialRunning) {
			cubeThread = new cThread();
			cubeThread->start(MoveRight, CTHREAD_PRIORITY_GRAPHICS);
		}
		else { 
			SetConsoleTextAttribute(hConsole, 0x0e);
			cout << "Error: Finish previous trial before next one." << endl;
			SetConsoleTextAttribute(hConsole, 7);
		}
	}
	else if (a_key == GLFW_KEY_3) {
		//Rotate X
		RotateCube(1, 0, 0, 10);
	}
	else if (a_key == GLFW_KEY_4) {
		//Rotate Y
		RotateCube(0, 1, 0, 10);
	}
	else if (a_key == GLFW_KEY_5) {
		//Rotate Z
		RotateCube(0, 0, 1, 10);
	}
}

//------MOVE CUBE METHODS----------------------

void MoveLeft() {
	trialRunning = true;
	while (cube_posZ > -0.5) {
		cube_posZ -= 0.001;
		cSleepMs(1);
	}
	trialRunning = false;
	return;
}

void MoveRight() {
	trialRunning = true;
	while (cube_posZ < 0.5) {
		cube_posZ += 0.001;
		cSleepMs(1);
	}
	trialRunning = false;
	return;
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
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.x << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.y << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Position.z << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.w << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.x << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.y << ",";
	chaifile << oculusVR.m_trackingState.HeadPose.ThePose.Orientation.z << endl << endl;
}


//------------------------------------------------------------------------------

