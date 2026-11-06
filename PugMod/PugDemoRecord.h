#pragma once

class CPugDemoRecord
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void GetIntoGame(CBasePlayer *Player);
	void PlayerSpawn(CBasePlayer *Player);

	void Menu(CBasePlayer *Player);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

private:
	std::array<bool, MAX_CLIENTS + 1U> m_Asked = {};
};

extern CPugDemoRecord gPugDemoRecord;