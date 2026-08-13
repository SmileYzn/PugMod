#pragma once

enum E_PRINT_TEAM
{
    DEFAULT = 0,
    GREY = -1,
    RED = -2,
    BLUE = -3
};

enum E_PRINT
{
    NOTIFY = 1,
    CONSOLE = 2,
    CHAT = 3,
    CENTER = 4,
    RADIO = 5
};

class CPugUtil
{
public:
    const char *GetPath();
    const char *GetFullPath();
    int MakeDirectory(const char* Path);
    void ServerCommand(const char *Format, ...);
    bool IsNetClient(edict_t *pEntity);
    void PrintColor(edict_t *pEntity, int Sender, const char *Format, ...);
    void TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName);
    void ParseColor(char *Buffer);
    void ClientPrint(edict_t *pEntity, int iMsgDest, const char *Format, ...);
    void ClientCommand(edict_t *pEntity, const char *Format, ...);
    void ClientDrop(int EntityIndex, const char *Format, ...);
    void ClientSlap(edict_t *pEntity, float Damage, bool RandomDirection);
    unsigned short FixedUnsigned16(float Value, float Scale);
    short FixedSigned16(float Value, float Scale);
    hudtextparms_t SetHud(vec3_t FromColor, vec3_t ToColor, float X, float Y, int Effect, float FxTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel);
    void SendHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void SendDHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void SendDeathMsg(edict_t *pEntity, CBaseEntity *pKiller, CBasePlayer *pVictim, CBasePlayer *pAssister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
    void ScreenFade(edict_t *pEntity, float Duration, float HoldTime, int FadeFlags, int Red, int Green, int Blue, int Alpha);
    void ScreenShake(edict_t *pEntity, float Amplitude, float Duration, float Frequency);
    void DrawTracer(Vector Start, Vector End);
    void DrawLine(Vector Start, Vector End, int Life, int Red, int Green, int Blue);
    std::vector<CBasePlayer *> GetPlayers(bool InGame, bool Bots);
    std::array<std::vector<CBasePlayer *>, SPECTATOR + 1U> GetPlayers();
    void ShowMotd(edict_t* pEntity, char* Motd, unsigned int MotdLength);
    bool IsPlayerVisible(CBasePlayer* Player, CBasePlayer* Target);

private:
    std::string m_Path;
    std::string m_FullPath;
};

extern CPugUtil gPugUtil;