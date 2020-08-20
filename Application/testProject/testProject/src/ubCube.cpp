#include "ubCube.h"

ubCube::ubCube() {
	m_cubeTransparent = false;
	m_serialOK = false;
}

void ubCube::CallMotor(int pwm) {
	if (m_serialOK) {
		if (pwm >= 0 && pwm < 256) {
			m_serialPort.writeByte(pwm);
		}
	}
}

void ubCube::OnButtonUp(chai3d::cMesh *cube) {
	m_cubeMesh = cube;
	m_cubeTransparent = !m_cubeTransparent;
	if (m_cubeTransparent) {
		m_cubeMesh->setTransparencyLevel(0.2);
		m_cubeMesh->setUseCulling(false);
	}
	else {
		m_cubeMesh->setTransparencyLevel(1);
		m_cubeMesh->setUseCulling(true);
	}
}

