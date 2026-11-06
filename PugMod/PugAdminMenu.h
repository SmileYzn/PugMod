#pragma once

enum ADMIN_MENU_LIST
{
	ADMIN_MENU_KICK,
	ADMIN_MENU_BAN,
	ADMIN_MENU_SLAP,
	ADMIN_MENU_TEAM,
	ADMIN_MENU_MAP,
	ADMIN_MENU_PUG,
	ADMIN_MENU_CVAR,
	ADMIN_MENU_PAUSE,
	ADMIN_MENU_SPAWN_EDIT
};

class CPugAdminMenu
{
public:
	void Menu(CBasePlayer *Player);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Kick(CBasePlayer *Player);
	void KickHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Ban(CBasePlayer *Player);
	void BanHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Slap(CBasePlayer *Player);
	void SlapHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Team(CBasePlayer *Player);
	void TeamHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Map(CBasePlayer *Player);
	void MapHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void Pug(CBasePlayer *Player);
	void PugHandle(CBasePlayer *Player, P_MENU_ITEM Item);
};

extern CPugAdminMenu gPugAdminMenu;