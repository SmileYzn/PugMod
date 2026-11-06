#pragma once

typedef struct S_ADMIN
{
	std::string Auth;
	std::string Name;
	std::string Flag;
} P_ADMIN, *LP_ADMIN;

// AMX Mod X Team Flags
constexpr auto ADMIN_ALL			= 0;        /* everyone */
constexpr auto ADMIN_IMMUNITY		= BIT(0);   /* flag "a" */
constexpr auto ADMIN_RESERVATION	= BIT(1);	/* flag "b" */
constexpr auto ADMIN_KICK			= BIT(2);	/* flag "c" */
constexpr auto ADMIN_BAN			= BIT(3);	/* flag "d" */
constexpr auto ADMIN_SLAY			= BIT(4);	/* flag "e" */
constexpr auto ADMIN_MAP			= BIT(5);	/* flag "f" */
constexpr auto ADMIN_CVAR			= BIT(6);	/* flag "g" */
constexpr auto ADMIN_CFG			= BIT(7);	/* flag "h" */
constexpr auto ADMIN_CHAT			= BIT(8);	/* flag "i" */
constexpr auto ADMIN_VOTE			= BIT(9);	/* flag "j" */
constexpr auto ADMIN_PASSWORD		= BIT(10);	/* flag "k" */
constexpr auto ADMIN_RCON			= BIT(11);	/* flag "l" */
constexpr auto ADMIN_LEVEL_A		= BIT(12);	/* flag "m" */
constexpr auto ADMIN_LEVEL_B		= BIT(13);	/* flag "n" */
constexpr auto ADMIN_LEVEL_C		= BIT(14);	/* flag "o" */
constexpr auto ADMIN_LEVEL_D		= BIT(15);	/* flag "p" */
constexpr auto ADMIN_LEVEL_E		= BIT(16);	/* flag "q" */
constexpr auto ADMIN_LEVEL_F		= BIT(17);	/* flag "r" */
constexpr auto ADMIN_LEVEL_G		= BIT(18);	/* flag "s" */
constexpr auto ADMIN_LEVEL_H		= BIT(19);	/* flag "t" */
constexpr auto ADMIN_MENU			= BIT(20);	/* flag "u" */
constexpr auto ADMIN_BAN_TEMP		= BIT(21);	/* flag "v" */
constexpr auto ADMIN_ADMIN			= BIT(24);	/* flag "y" */
constexpr auto ADMIN_USER			= BIT(25);	/* flag "z" */

class CPugAdmin
{
public:
	void ServerActivate();
	void Set(std::string Auth, std::string Name, std::string Flag);
	int ReadFlags(const char* Flags);
	void PutInServer(edict_t* pEntity);
	int GetFlags(int EntityIndex);
	int Access(int EntityIndex, int Level);
	int Access(std::string Auth, int Level);
	std::string GetFlags(edict_t* pEdict);
	bool CheckAccess(CBasePlayer *Player, int Level);
private:
	std::map<std::string, P_ADMIN> m_Data = {};
	std::map<int, int> m_Flag = {};
};

extern CPugAdmin gPugAdmin;