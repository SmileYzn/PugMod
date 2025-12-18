#pragma once

constexpr hudtextparms_t g_DM_HudInfo = {-1.0f, 0.90f, 0, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 1.1f, 0.0f, 1};
constexpr hudtextparms_t g_DM_HudHeal = {-1.0f, 0.65f, 0, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 2.0f, 0.0f, 2};
constexpr hudtextparms_t g_DM_HudHits = {-1.0f, 0.55f, 0, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.8f, 0.0f, 3};

constexpr const char* g_DM_SoundHeadshot = "spk \"sound/fvox/blip\"";

enum DM_OPTION
{
    DM_OPT_KILL_FEED = 0,
    DM_OPT_HIT_DMG,
    DM_OPT_HS_MODE,
    DM_OPT_HUD_KD,
    DM_OPT_KILL_FADE,
    DM_OPT_KILL_SOUND,
    DM_OPT_MONEY_FRAG,
    DM_OPT_RESET_SCORE,
    DM_OPT_RESPAWN
};

typedef struct S_SPAWN
{
    vec3_t Vecs;
    vec3_t Angles;
    int Team;
    vec3_t VAngles;
} P_SPAWN, *LP_SPAWN;

typedef struct S_WEAPON
{
    WeaponSlotInfo *SlotInfo;
    std::string Label;
    int Bot;
} P_WEAPON, *LP_WEAPON;

typedef struct S_DM_INFO
{
    bool m_HideMenu = false;
    std::array<WeaponIdType, C4_SLOT + 1U> m_WeaponLast  = {};
    std::map<int, int> m_WeaponState  = {};
    float m_Headshots = 0.0f;

    std::array<bool, DM_OPT_RESPAWN + 1U> m_Option = {};

    void Clear()
    {
        this->m_HideMenu = false;
        this->m_WeaponLast.fill(WEAPON_NONE);
        this->m_WeaponState.clear();
        this->m_Headshots = 0.0f;
        this->m_Option.fill(false);
    }
} P_DM_INFO, *LP_DM_INFO;

class CPugDM
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void SetSpawn(vec3_t Vecs, vec3_t Angles, int Team, vec3_t VAngles);
    void SetWeapon(std::string Name, std::string Label, int Bot);
    void Load();
    void Init();
    void Stop();
    std::map<size_t, P_SPAWN> GetSpawns();
    bool GetPlayerSpawnSpot(CBasePlayer *Player);
    bool CheckDistance(CBasePlayer *Player, vec3_t Origin, float Distance);
    void CheckMapConditions();
    void GetIntoGame(CBasePlayer *Player);
    void PlayerSpawn(CBasePlayer *Player);
    void OnSpawnEquip(CBasePlayer *Player, bool addDefault, bool equipGame);
    bool DropItem(CBasePlayer *Player);
    bool FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity);
    void TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType);
    bool SendDeathMessage(CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
    void UpdateClientData(CBasePlayer *Player);
    bool AddAccount(CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange);
    void SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation);
    void MenuOption(CBasePlayer *Player);
    void MenuOptionHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    bool ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *pszMenuText);
    void Menu(CBasePlayer *Player);
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    void MenuEquip(CBasePlayer *Player, int Slot);
    void MenuEquipHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    void EquipItem(CBasePlayer *Player, WeaponIdType WeaponIndex);
    bool EquipRandomItem(CBasePlayer *Player, int Slot);
    bool Respawn(CBasePlayer *Player);
    bool ResetScore(CBasePlayer *Player);
    bool IsEntityStuck(edict_t *pEntity);

private:
    bool m_Run = false;
    std::map<size_t, P_SPAWN> m_Spawn = {};
    std::vector<P_WEAPON> m_Weapon = {};
    std::map<int, P_DM_INFO> m_Info = {};
};

extern CPugDM gPugDM;