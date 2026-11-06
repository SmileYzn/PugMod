#pragma once

constexpr std::array<const char*, 3U> g_Menu_Sound =
{
	"spk common/menu1", // 0 -> Next / Next
	"spk common/menu2", // 1 -> Select Normal
	"spk common/menu3", // 2 -> Select Disabled
};

enum E_MENU
{
	DM_OPTIONS,
	DM_EQUIP,
	DM_WEAPONS,
	DM_SPAWN_EDIT,
	DM_SPAWN_EDIT_ADD_SPAWN,
	DM_SPAWN_EDIT_DISCARD,
	ME_VOTE_MAP,
	ME_VOTE_TEAM,
	ME_VOTE_LEADER,
	ME_VOTE_SWAP_TEAM,
	ME_VOTE_OVERTIME,
	ME_VOTE_END,
	ME_DEMO_RECORD,
	ME_ADMIN_MENU,
	ME_ADMIN_MENU_KICK,
	ME_ADMIN_MENU_BAN,
	ME_ADMIN_MENU_SLAP,
	ME_ADMIN_MENU_TEAM,
	ME_ADMIN_MENU_MAP,
	ME_ADMIN_MENU_PUG,
	ME_ADMIN_MENU_CVAR,
	ME_PLAYER_VOTE_MENU,
	ME_PLAYER_VOTE_KICK,
	ME_PLAYER_VOTE_MAP
};

typedef struct S_MENU_ITEM
{
	int Info;
	std::string Text;
	bool Disabled;
	int Extra;
} P_MENU_ITEM, *LP_MENU_ITEM;

class CPugMenu
{
public:
	void Clear();
	void Create(bool Exit, int HandleType, const char *Format, ...);
	void AddItem(int Info, bool Disabled, int Extra, const char *Format, ...);
	void Show(CBasePlayer *Player);
	void Hide(CBasePlayer *Player);
	bool Handle(CBasePlayer *Player, int Key);
	void Display(CBasePlayer *Player, int Page);
	void ShowMenu(CBasePlayer *Player, int Slots, int Time, std::string Text);
	void ReplaceAll(std::string &String, const std::string &From, const std::string &To);

private:
	std::string m_Text = "";
	std::vector<P_MENU_ITEM> m_Data = {};
	int m_Page = 0;
	bool m_Exit = false;
	int m_MaxPageOption = 7;
	int m_Handle = 0;
};

extern std::array<CPugMenu, MAX_CLIENTS + 1U> gPugMenu;