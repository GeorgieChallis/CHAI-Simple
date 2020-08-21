#pragma once
#include <chai3d.h>
#include <SerialPort.h>

class ubCube {
private:

public:
	double posX;
	double posY;
	double posZ;
	double m_quaternion[4];
	
	SerialPort m_serialPort;
	bool m_serialOK;

	chai3d::cMesh *m_cubeMesh;
	chai3d::cTexture2dPtr m_texture;
	bool m_cubeTransparent;
	double m_meshSize = 0.2;

	ubCube();
	void CallMotor(int);
	void OnButtonUp(chai3d::cMesh *cube);
	void UpdateIMUCube();
	void SetTexture();
};

