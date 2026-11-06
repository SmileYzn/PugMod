#pragma once

constexpr auto PUG_STATUS_MIN_UPDATE_TIME = 20.0f;

class CPugStatus
{
public:
	void ServerActivate();
	void StartFrame();
	void SendStatus();
private:
	float m_NextUpdate = 0.0f;
	nlohmann::ordered_json m_Data = {};
};

extern CPugStatus gPugStatus;