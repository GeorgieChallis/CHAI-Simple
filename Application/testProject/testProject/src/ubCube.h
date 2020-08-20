#pragma once
#include <chai3d.h>
#include <SerialPort.h>

class ubCube {
private:

public:
	SerialPort m_serialPort;
	chai3d::cMesh *m_cubeMesh;
	bool m_cubeTransparent;
	bool m_serialOK;
	//static double m_quaternion[4];

	ubCube();
	void CallMotor(int);
	void OnButtonUp(chai3d::cMesh *cube);
};

