#pragma once
#include <chai3d.h>
#include <SerialPort.h>

class ubCube {
private:

public:
	double posX;
	double posY;
	double posZ;
	
	SerialPort m_serialPort;
	chai3d::cMesh *m_cubeMesh;
	bool m_cubeTransparent;
	bool m_serialOK;
	double m_quaternion[4];
	double m_meshSize = 0.2;

	ubCube();
	void CallMotor(int);
	void OnButtonUp(chai3d::cMesh *cube);
	void UpdateIMUCube();
};

