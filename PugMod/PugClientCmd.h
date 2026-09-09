#pragma once

enum E_CLIENT_CMD_ID
{
    // Players
    CMD_STATUS = 0,
    CMD_SCORE,
    CMD_READY,
    CMD_NOTREADY,
    CMD_MENU,
    CMD_GUNS,
    CMD_RESPAWN,
    CMD_RESET_SCORE,
    CMD_HELP,
    CMD_RECORD,
    CMD_HP,
    CMD_DMG,
    CMD_RDMG,
    CMD_SUM,
    CMD_STATS,
    CMD_VOTE_MENU,
    //
    // Admins
    CMD_HELP_ADMIN = 100,
    CMD_ADMIN_MENU,
    CMD_ADMIN_KICK,
    CMD_ADMIN_BAN,
    CMD_ADMIN_SLAP,
    CMD_ADMIN_TEAM,
    CMD_ADMIN_MAP,
    CMD_ADMIN_MESSAGE,
    CMD_ADMIN_RCON,
    CMD_ADMIN_CVAR,
    CMD_DM_SPAWN_EDITOR,
    CMD_PAUSE_MATCH
};

constexpr std::array<const char*, 8U> g_Pug_Blocked_Cmds =
{
    "rcon_password",
    "sv_password",
    "quit",
    "exit",
    "exec",
    "cmdlist",
    "cvarlist",
};

typedef struct S_CLIENT_CMD
{
    int Index;
    std::string Name;
    int Flags;
} P_CLIENT_CMD, *LP_CLIENT_CMD;

class CPugClientCmd
{
public:
    void ServerActivate();

    void Set(std::string Index, std::string Name, std::string Flags);
    LP_CLIENT_CMD Get(const char* Name);
    LP_CLIENT_CMD Get(int Index);

    bool FilterFlood(const char *pszCommand, int EntityIndex);
    bool Command(edict_t *pEntity);

    bool Help(CBasePlayer *Player);
    bool HelpAdmin(CBasePlayer *Player);
    bool Message(CBasePlayer *Player);
    bool Rcon(CBasePlayer *Player);
private:
    std::map<std::string, P_CLIENT_CMD> m_Data = {};

    std::array<float, MAX_CLIENTS + 1U> m_FloodCount = {};
    std::array<float, MAX_CLIENTS + 1U> m_FloodTimer = {};
};

extern CPugClientCmd gPugClientCmd;