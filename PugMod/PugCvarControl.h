#pragma once

class CPugCvarControl
{
public:
	void ServerActivate();
	void Add(const char *Name, const char *String);
	void Menu(CBasePlayer *Player);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

private:
	std::map<cvar_t*, std::vector<std::string>> m_Data;
};

extern CPugCvarControl gPugCvarControl;
