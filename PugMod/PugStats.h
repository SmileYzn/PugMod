#pragma once

// Match Stats
typedef struct S_MATCH_STATS
{
	// BETA: Match Start Time
	time_t StartTime;

	// BETA: Match End Time
	time_t EndTime;

	// BETA: Server Name
	std::string HostName;

	// BETA: Map Name
	std::string Map;

	// BETA: Server Address
	std::string Address;

	// BETA: Team Score
	std::array<int, SPECTATOR + 1U> Score = { };

	// BETA: Winner of match
	int Winner;

	// BETA: Rounds Played
	int Rounds;

	// Number of rounds in match
	int MaxRounds;

	// Number of overtime rounds
	int MaxRoundsOT;

	// Game Mode: 0 Leaders Sorted, 1 Random Teams, 2 Not Sorted, 3 Skill Sorted, 4 Swap Teams, 5 Knife Round
	int GameMode;

	// Match will have Knife Round
	int KnifeRound;

	// Minimum Players
	int MinPlayers;

	// Maximum Players
	int MaxPlayers;

	// Reset
	void Reset()
	{
		this->StartTime = 0;
		this->EndTime = 0;
		this->HostName = "";
		this->Map = "";
		this->Address = "";
		this->Score.fill(0);
		this->Winner = 0;
		this->Rounds = 0;
		this->MaxRounds = 0;
		this->MaxRoundsOT = 0;
		this->GameMode = 0;
		this->KnifeRound = 0;
		this->MinPlayers = 0;
		this->MaxPlayers = 0;
	}

	// Swap Scores
	void SwapScores()
	{
		// Swap Scores
		SWAP(this->Score[TERRORIST], this->Score[CT]);
	}
} P_MATCH_STATS, *LP_MATCH_STATS;

// Weapon Stats
typedef struct S_WEAPON_STATS
{
	int Frags;
	int Deaths;
	int Headshots;
	int Shots;
	int Hits;
	int HitsReceived;
	int Damage;
	int DamageReceived;
} P_WEAPON_STATS, *LP_WEAPON_STATS;

// Hitbox Stats
typedef struct S_HITBOX_STATS
{
	int Hits;
	int Damage;
	int HitsReceived;
	int DamageReceived;
	int Frags;
	int Deaths;
} P_HITBOX_STATS, *LP_HITBOX_STATS;

// Dominations
typedef struct S_DOMINATION_STATS
{
	int DominationBegin;
	int Domination;
	int Revenge;
} P_DOMINATION_STATS, *LP_DOMINATION_STATS;

// Round Stats
typedef struct S_ROUND_STATS
{
	int Frags;				// BETA: Player Frags
	int Deaths;				// BETA: Player Deaths
	int Headshots;			// BETA: Headshots
	int Shots;				// BETA: Shots
	int Hits;				// BETA: Hits
	int HitsReceived;		// BETA: Hits Received
	int Damage;				// BETA: Damage Taken
	int DamageReceived;		// BETA: Damage Receeived
	bool BombDefused;		// BETA: Bomb Defused
	bool BombExploded;		// BETA: Bomb Exploded
	int Versus;				// BETA: Player is versus X players
	float KillTime;			// BETA: Last Kill Time
	float DeathTime;		// BETA: Last Death Time

	// Damage dealt to other players
	std::map<std::string, int> PlayerDamage;

	// Reset
	void Reset()
	{
		this->Frags = 0;
		this->Deaths = 0;
		this->Headshots = 0;
		this->Shots = 0;
		this->Hits = 0;
		this->HitsReceived = 0;
		this->Damage = 0;
		this->DamageReceived = 0;
		this->BombDefused = 0;
		this->BombExploded = 0;
		this->Versus = 0;
		this->KillTime = 0;
		this->DeathTime = 0;
		this->PlayerDamage.clear();
	}
} P_ROUND_STATS, *LP_ROUND_STATS;

// Player Stats
typedef struct S_PLAYER_STATS
{
	// Stats
	int Frags;					// BETA: Player Frags
	int Deaths;					// BETA: Player Deaths
	int Assists;				// BETA: Player Kill Assists
	int Headshots;				// BETA: Headshots by player
	int Shots;					// BETA: Shots by player
	int Hits;					// BETA: Hits done by player
	int HitsReceived;			// BETA: Hits received by player
	int Damage;					// BETA: Damage taken by player
	int DamageReceived;			// BETA: Damage received by player
	long Money;					// BETA: Money Balance from player
	int Suicides;				// BETA: Suicide Count
	int Survives; 				// BETA: Round Survive Count
	int TradeKills;				// BETA: Trade Kills
	int TradeDeaths; 			// BETA: Trade Deaths

	// BETA: Round Win Share stats
	float RoundWinShare;

	// TODO: Rating
	float RatingTR;				// TODO: Rating as TR Player
	float RatingCT;				// TODO: Rating as CT Player

	// Sick Stats
	int BlindFrags;				// BETA: Player frags when blinded by flashbang
	int BlindDeaths;			// BETA: Player deaths when blinded by flashbang
	int OneShot;				// BETA: One Shot Frags (Except for AWP)
	int NoScope;				// BETA: No Scope Frags
	int FlyFrags;				// BETA: Flying Frags
	int WallFrags;				// BETA: Wallbang Frags
	int DoubleKill;				// BETA: Double Kill
	int SmokeFrags;				// BETA: Smoke grenade penetration kill (bullets went through smoke)
	int AssistedFlash;			// BETA: Assister helped with a flash

	// BETA: Count of Knife Duels in match
	int KnifeDuelWin;
	int KnifeDuelLose;

	// Entries
	int EntryFrags;				// BETA: Entry Frag of round
	int EntryDeaths;			// BETA: Entry Death of round

	// Rounds
	int RoundPlay;				// BETA: Rounds Play
	int RoundWin;				// BETA: Rounds Won
	int RoundLose;				// BETA: Rounds Lose

	// Bomb
	int BombSpawn;				// BETA: Bomb Spawns
	int BombDrop;				// BETA: Bomb Drops
	int BombPlanting;			// BETA: Bomb Plant Attempts
	int BombPlanted;			// BETA: Bomb Plants
	int BombExploded;			// BETA: Bomb Explosions
	int BombDefusing;			// BETA: Bomb Defusing Attempts
	int BombDefusingKit;		// BETA: Bomb Defusing with Kit Attempts
	int BombDefused;			// BETA: Bomb Defuses
	int BombDefusedKit;			// BETA: Bomb Defuses with Kit

	// BETA: Kill streak
	std::array<int, MAX_CLIENTS + 1U> KillStreak;

	// BETA: Versus: 1 vs X win situations
	std::array<int, MAX_CLIENTS + 1U> Versus;
	
	// BETA: HitBox
	std::array<P_HITBOX_STATS, 9U> HitBox;

	// BETA: Weapon Stats
	std::map<int, P_WEAPON_STATS> Weapon;

	// BETA: Dominations / Revenge
	std::map<std::string, P_DOMINATION_STATS> Domination;
	
	// Clear stats
	void Reset()
	{
		// Stats
		this->Frags = 0;
		this->Deaths = 0;
		this->Assists = 0;
		this->Headshots = 0;
		this->Shots = 0;
		this->Hits = 0;
		this->HitsReceived = 0;
		this->Damage = 0;
		this->DamageReceived = 0;
		this->Money = 0;
		this->Suicides = 0;
		this->Survives = 0;
		this->TradeKills = 0;
		this->TradeDeaths = 0;

		// Round Win Share stats
		this->RoundWinShare = 0.0f;

		// Rating
		this->RatingTR = 0.0f;
		this->RatingCT = 0.0f;

		// Sick Frags
		this->BlindFrags = 0;
		this->BlindDeaths = 0;
		this->OneShot = 0;
		this->NoScope = 0;
		this->FlyFrags = 0;
		this->WallFrags = 0;
		this->DoubleKill = 0;
		this->SmokeFrags = 0;
		this->AssistedFlash = 0;

		// Knife Duels
		this->KnifeDuelWin = 0;
		this->KnifeDuelLose = 0;

		// Entries
		this->EntryFrags = 0;
		this->EntryDeaths = 0;

		// Rounds
		this->RoundPlay = 0;
		this->RoundWin = 0;
		this->RoundLose = 0;

		// Bomb
		this->BombSpawn = 0;
		this->BombDrop = 0;
		this->BombPlanting = 0;
		this->BombPlanted = 0;
		this->BombExploded = 0;
		this->BombDefusing = 0;
		this->BombDefusingKit = 0;
		this->BombDefused = 0;
		this->BombDefusedKit = 0;

		// Kill streak
		this->KillStreak = {};

		// Versus: 1 vs X win situations
		this->Versus = {};

		// HitBox (0 Hits, 1 Damage, 2 Hits Received, 3 Damage Received, 4 Frags, 5 Deaths)
		this->HitBox = {};

		// Weapon Stats
		this->Weapon.clear();

		// Dominations
		this->Domination.clear();
	}
} P_PLAYER_STATS, *LP_PLAYER_STATS;

// Player Chat
typedef struct S_PLAYER_CHAT
{
	time_t Time;
	int State;
	int Team;
	int Alive;
	std::string Message;
} P_PLAYER_CHAT, *LPP_PLAYER_CHAT;

// Player Data
typedef struct S_PLAYER_DATA
{
	// Timers
	time_t JoinGameTime;		// BETA: Joined Game Time
	time_t DisconnectTime;		// BETA: Disconnected Time

	// Player Data
	std::string Name;			// BETA: Player Name
	int Team;					// BETA: Plyer Team
	int	Winner;					// BETA: Is Winner of match
	int IsBot;					// BETA: Is BOT

	// Player Stats (Per Half)
	std::map<int, P_PLAYER_STATS> Stats;

	// Player chat log
	std::vector<P_PLAYER_CHAT> ChatLog;

	// Round Stats
	P_ROUND_STATS Round;
} P_PLAYER_DATA, *LP_PLAYER_DATA;

// Round Events
typedef struct S_ROUND_EVENT
{
	int			Round;			// Round Count
	float		Time;			// Round Time Seconds;
	int			Type;			// ROUND_NONE for player events, ScenarioEventEndRound for round events
	int			ScenarioEvent;	// Scenario Event End Round
	int 		State;			// Match State
	int			Winner;			// Winner team of event
	int			Loser;			// Loser team of event
	std::string Killer;			// Killer
	std::string Victim;			// Victim
	Vector		KillerOrigin;	// Killer Position
	Vector		VictimOrigin;	// Victim Position
	int			IsHeadShot;		// HeadShot
	int			ItemIndex;		// Item Index of event
} P_ROUND_EVENT, * LP_ROUND_EVENT;

class CPugStats
{
public:
	void ServerActivate();
	void SetState();
    const char* GetAuthId(CBasePlayer* Player);
	void GetIntoGame(CBasePlayer* Player);
	void DropClient(edict_t *pEntity);
	void ChooseTeam(CBasePlayer* Player);
	void SwitchTeam(CBasePlayer* Player);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim);
	void TakeDamage(CBasePlayer* Victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void SendDeathMessage(CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
	void AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	void RestartRound();
	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay);
	void MakeBomber(CBasePlayer* Player);
	void DropPlayerItem(CBasePlayer* Player, const char* pszItemName);
	void PlantBomb(entvars_t* pevOwner, bool Planted);
	void DefuseBombStart(CBasePlayer* Player);
	void DefuseBombEnd(CBasePlayer* Player, bool Defused);
	void ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType);
	static bool SayText(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity);
	void OnEvent(GameEventType event, int ScenarioEvent, CBaseEntity* pEntity, CBaseEntity* pEntityOther);
	void DumpData();
	void SaveData(nlohmann::json Data);
	void UploadData(nlohmann::json Data);
private:
	P_MATCH_STATS m_Match;
	std::map<std::string, P_PLAYER_DATA> m_Player;
	std::vector<P_ROUND_EVENT> m_RoundEvent;
};

extern CPugStats gPugStats;
