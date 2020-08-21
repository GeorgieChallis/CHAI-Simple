#include "ubVicon.h"

ubVicon::ubVicon()
{
	viconConnected = false;
	HostName = "localhost:801";
}

ubVicon::~ubVicon()
{

}

std::string ubVicon::Adapt(const bool i_Value)
{
	return i_Value ? "True" : "False";
}

std::string ubVicon::Adapt(const ViconDataStreamSDK::CPP::TimecodeStandard::Enum i_Standard)
{
	switch (i_Standard)
	{
	default:
	case ViconDataStreamSDK::CPP::TimecodeStandard::None:
		return "0";
	case ViconDataStreamSDK::CPP::TimecodeStandard::PAL:
		return "1";
	case ViconDataStreamSDK::CPP::TimecodeStandard::NTSC:
		return "2";
	case ViconDataStreamSDK::CPP::TimecodeStandard::NTSCDrop:
		return "3";
	case ViconDataStreamSDK::CPP::TimecodeStandard::Film:
		return "4";
	case ViconDataStreamSDK::CPP::TimecodeStandard::NTSCFilm:
		return "5";
	case ViconDataStreamSDK::CPP::TimecodeStandard::ATSC:
		return "6";
	}
}

std::string ubVicon::Adapt(const ViconDataStreamSDK::CPP::Direction::Enum i_Direction)
{
	switch (i_Direction)
	{
	case ViconDataStreamSDK::CPP::Direction::Forward:
		return "Forward";
	case ViconDataStreamSDK::CPP::Direction::Backward:
		return "Backward";
	case ViconDataStreamSDK::CPP::Direction::Left:
		return "Left";
	case ViconDataStreamSDK::CPP::Direction::Right:
		return "Right";
	case ViconDataStreamSDK::CPP::Direction::Up:
		return "Up";
	case ViconDataStreamSDK::CPP::Direction::Down:
		return "Down";
	default:
		return "Unknown";
	}
}

std::string ubVicon::Adapt(const ViconDataStreamSDK::CPP::DeviceType::Enum i_DeviceType)
{
	switch (i_DeviceType)
	{
	case ViconDataStreamSDK::CPP::DeviceType::ForcePlate:
		return "ForcePlate";
	case ViconDataStreamSDK::CPP::DeviceType::Unknown:
	default:
		return "Unknown";
	}
}

std::string ubVicon::Adapt(const ViconDataStreamSDK::CPP::Unit::Enum i_Unit)
{
	switch (i_Unit)
	{
	case ViconDataStreamSDK::CPP::Unit::Meter:
		return "Meter";
	case ViconDataStreamSDK::CPP::Unit::Volt:
		return "Volt";
	case ViconDataStreamSDK::CPP::Unit::NewtonMeter:
		return "NewtonMeter";
	case ViconDataStreamSDK::CPP::Unit::Newton:
		return "Newton";
	case ViconDataStreamSDK::CPP::Unit::Kilogram:
		return "Kilogram";
	case ViconDataStreamSDK::CPP::Unit::Second:
		return "Second";
	case ViconDataStreamSDK::CPP::Unit::Ampere:
		return "Ampere";
	case ViconDataStreamSDK::CPP::Unit::Kelvin:
		return "Kelvin";
	case ViconDataStreamSDK::CPP::Unit::Mole:
		return "Mole";
	case ViconDataStreamSDK::CPP::Unit::Candela:
		return "Candela";
	case ViconDataStreamSDK::CPP::Unit::Radian:
		return "Radian";
	case ViconDataStreamSDK::CPP::Unit::Steradian:
		return "Steradian";
	case ViconDataStreamSDK::CPP::Unit::MeterSquared:
		return "MeterSquared";
	case ViconDataStreamSDK::CPP::Unit::MeterCubed:
		return "MeterCubed";
	case ViconDataStreamSDK::CPP::Unit::MeterPerSecond:
		return "MeterPerSecond";
	case ViconDataStreamSDK::CPP::Unit::MeterPerSecondSquared:
		return "MeterPerSecondSquared";
	case ViconDataStreamSDK::CPP::Unit::RadianPerSecond:
		return "RadianPerSecond";
	case ViconDataStreamSDK::CPP::Unit::RadianPerSecondSquared:
		return "RadianPerSecondSquared";
	case ViconDataStreamSDK::CPP::Unit::Hertz:
		return "Hertz";
	case ViconDataStreamSDK::CPP::Unit::Joule:
		return "Joule";
	case ViconDataStreamSDK::CPP::Unit::Watt:
		return "Watt";
	case ViconDataStreamSDK::CPP::Unit::Pascal:
		return "Pascal";
	case ViconDataStreamSDK::CPP::Unit::Lumen:
		return "Lumen";
	case ViconDataStreamSDK::CPP::Unit::Lux:
		return "Lux";
	case ViconDataStreamSDK::CPP::Unit::Coulomb:
		return "Coulomb";
	case ViconDataStreamSDK::CPP::Unit::Ohm:
		return "Ohm";
	case ViconDataStreamSDK::CPP::Unit::Farad:
		return "Farad";
	case ViconDataStreamSDK::CPP::Unit::Weber:
		return "Weber";
	case ViconDataStreamSDK::CPP::Unit::Tesla:
		return "Tesla";
	case ViconDataStreamSDK::CPP::Unit::Henry:
		return "Henry";
	case ViconDataStreamSDK::CPP::Unit::Siemens:
		return "Siemens";
	case ViconDataStreamSDK::CPP::Unit::Becquerel:
		return "Becquerel";
	case ViconDataStreamSDK::CPP::Unit::Gray:
		return "Gray";
	case ViconDataStreamSDK::CPP::Unit::Sievert:
		return "Sievert";
	case ViconDataStreamSDK::CPP::Unit::Katal:
		return "Katal";

	case ViconDataStreamSDK::CPP::Unit::Unknown:
	default:
		return "Unknown";
	}
}