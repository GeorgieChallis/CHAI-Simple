//Minimal Program for OpenGL to work (display a sphere)
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

#include <iostream>

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


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// this function renders the scene
void updateGraphics(void);

// this function closes the application
void close(void);

//==============================================================================

int main(int argc, char* argv[])
{
	// parse first arg to try and locate resources
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);

	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLFW library
	if (!glfwInit())
	{
		cout << "failed initialization" << endl;
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// create display context
	window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// get width and height of window
	glfwGetWindowSize(window, &width, &height);

	// set position of window
	glfwSetWindowPos(window, x, y);

	// set resize callback
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	// set current display contextn
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(swapInterval);

#ifdef GLEW_VERSION
	// initialize GLEW library
	if (glewInit() != GLEW_OK)
	{
		cout << "failed to initialize GLEW library" << endl;
		glfwTerminate();
		return 1;
	}
#endif

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
	camera->set(cVector3d(0.0, 0.5, 0.0),    // camera position (eye)
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
	my_cube->setLocalPos(0.0, 0.0, 0.0);

	cCreateBox(my_cube, 0.4, 0.4, 0.4);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/brick-color.png"));
	if (!fileload)
	{
	#if defined(_MSVC)
		fileload = texture->loadFromFile("../../../bin/resources/brick-color.png");
	#endif
	}
	if (!fileload)
	{
		// set material color
		my_cube->m_material->setRedFireBrick();
	}

	// apply texture to object
	my_cube->setTexture(texture);

	// enable texture rendering 
	my_cube->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	my_cube->setUseCulling(true);

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------
	// setup callback when application exits
	atexit(close);

	// call window size callback at initialization
	windowSizeCallback(window, width, height);

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		// get width and height of window
		glfwGetWindowSize(window, &width, &height);

		// render graphics
		updateGraphics();

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
	cout << "Error: " << a_description << endl;
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
	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE

	// render world
	camera->renderView(width, height);

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error:  %s\n" << gluErrorString(err);
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
