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


void ubCube::UpdateIMUCube() {
	std::vector<std::string> lastStringBuffer;
	std::vector<std::string> stringBuffer;
	static std::string string;

	bool dataReceived = false;

	std::cout << "Serial: IMU thread created - Arduino mode." << std::endl;
	while (true)
	{
		int buffer;

		while (buffer = m_serialPort.readByte())
		{
			//	std::cout  << "Buffer enter";
			if (!dataReceived) {
				if (buffer = 59) { dataReceived = true; } //All 'packets' are ; terminated
			}

			else {
				if (buffer > 44 && buffer < 58) { // If 0 - 9 (or - .)
					string += (char)buffer;
				}
				else if (buffer == 65) {
					OnButtonUp(m_cubeMesh);
				}
				else if (buffer == 44) {
					//std::cout  << string;
					stringBuffer.push_back(string);
					string = "";
				}
				else if (buffer == 13 || buffer == 10) //13 CR or 10 LF
				{
					//	std::cout  << " std::endline ";
				}

				else if (buffer = 59) {
					std::string::size_type sz;

					stringBuffer.push_back(string);
					string = "";

					lastStringBuffer = stringBuffer;
					stringBuffer.resize(0);
					if (lastStringBuffer.size() == 4) {
						try {
							for (int i = 0; i < lastStringBuffer.size(); i++) {
								m_quaternion[i] = stod(lastStringBuffer[i], &sz);
							}
						}
						catch (std::exception e) {}
					}
					else std::cout << "Serial: Wrong size array of data." << std::endl;
				}
				else std::cout << "?";
			}
		}
	}

}

