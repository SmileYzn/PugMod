#pragma once


constexpr hudtextparms_t g_LO3_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 10.0f, 2.0f, 4};

constexpr std::array<const char*, 5U> g_LO3_HudText =
{
	"LIVE GOOD LUCK & HAVE FUN",
	"LIVE ON NEXT RESTART",
	"LIVE IN TWO RESTARTS",
	"LIVE IN THREE RESTARTS",
	"LIVE IN THREE RESTARTS"
};

constexpr std::array<const char*, 5U> g_LO3_Message =
{
	"^4[%s]^1 %s: ^3Live! Good Luck & Have Fun!!",
	"^4[%s]^1 %s: ^3Live on next restart!",
	"^4[%s]^1 %s: ^3Live in two restarts!",
	"^4[%s]^1 %s: ^3Live in three restarts!",
	"^4[%s]^1 %s: ^3Live in three restarts!"
};

constexpr std::array<const char*, 5U> g_LO3_Sound =
{
	"spk \"barney/letsgo\"",
	"spk \"fvox/blip, one\"",
	"spk \"fvox/blip, two\"",
	"spk \"fvox/blip, three\"",
	"spk \"fvox/blip, voice_off\"",
};

class CPugLO3
{
public:
	void Init(int RestartNum);

private:
	int m_RestartNum;
};

extern CPugLO3 gPugLO3;