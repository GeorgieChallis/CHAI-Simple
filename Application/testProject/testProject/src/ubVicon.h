#pragma once
#include "DataStreamClient.h"

class ubVicon
{
public:
	bool viconConnected;
	ViconDataStreamSDK::CPP::Client MyClient;

	ubVicon();
	~ubVicon();

	std::string HostName;

	ViconDataStreamSDK::CPP::Output_GetMarkerGlobalTranslation _Output_GetMarkerGlobalTranslation;
	ViconDataStreamSDK::CPP::Output_GetLabeledMarkerGlobalTranslation _Output_GetLabeledMarkerGlobalTranslation;
	ViconDataStreamSDK::CPP::Output_GetUnlabeledMarkerGlobalTranslation _Output_GetUnlabeledMarkerGlobalTranslation;

	std::string Adapt(const bool i_Value);
	std::string Adapt(const ViconDataStreamSDK::CPP::TimecodeStandard::Enum i_Standard);
	std::string Adapt(const ViconDataStreamSDK::CPP::Direction::Enum i_Direction);
	std::string Adapt(const ViconDataStreamSDK::CPP::DeviceType::Enum i_DeviceType);
	std::string Adapt(const ViconDataStreamSDK::CPP::Unit::Enum i_Unit);
};