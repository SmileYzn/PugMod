#include "precompiled.h"

CPugStats gPugStats;

void CPugStats::ServerActivate()
{
	// Match
	this->m_Match.Reset();

	// Player
	this->m_Player.clear();

	// Round events
	this->m_RoundEvent.clear();

	// Register Say Text messages
	gPugEngine.RegisterHook("SayText", this->SayText);
}

void CPugStats::SetState()
{
	// State
	auto State = gPugMod.GetState();

	// Switch
    switch (State)
    {
        case STATE_DEAD:
        {
            // Clear match data
            this->m_Match.Reset();

            // Clear player data
            this->m_Player.clear();

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_DEATHMATCH:
        case STATE_VOTEMAP:
        case STATE_VOTETEAM:
        case STATE_CAPTAIN:
		{
			// Clear match data
			this->m_Match.Reset();

			// Loop each player
			for (auto& Player : this->m_Player)
			{
				// Clear Chat Log
				Player.second.ChatLog.clear();

				// Clear player round stats
				Player.second.Round.Reset();
			}

			// Clear event data
			this->m_RoundEvent.clear();
			break;
		}
        case STATE_KNIFE_ROUND:
        {
            // Clear match data
            this->m_Match.Reset();

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_FIRST_HALF:
        {
            // Set start time
            this->m_Match.StartTime = time(NULL);
            
            // Set end time
            this->m_Match.EndTime = 0;
            
            // Set hostname
            this->m_Match.HostName = CVAR_GET_STRING("hostname");
            
            // Set map name
            this->m_Match.Map = STRING(gpGlobals->mapname);
            
            // Set address
            this->m_Match.Address = CVAR_GET_STRING("net_address");
            
            // Set score
            this->m_Match.Score.fill(0);
            
            // Set winner
            this->m_Match.Winner = 0;
            
            // Set rounds played
            this->m_Match.Rounds = 0;

            // Number of rounds in match
            this->m_Match.MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value);

            // Number of overtime rounds
            this->m_Match.MaxRoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value);

            // Match Game Mode
            this->m_Match.GameMode = static_cast<int>(gPugCvar.m_PlayGameMode->value);

            // Match will have Knife Round
            this->m_Match.KnifeRound = static_cast<int>(gPugCvar.m_PlayKnifeRound->value);

			// Mininum Players
			this->m_Match.MinPlayers = static_cast<int>(gPugCvar.m_PlayersMin->value);

			// Maximum Players
			this->m_Match.MaxPlayers = static_cast<int>(gPugCvar.m_PlayersMax->value);
            
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[State].Reset();

                // Clear Chat Log
                Player.second.ChatLog.clear();
            
                // Clear player round stats
                Player.second.Round.Reset();
            }

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_HALFTIME:
        {
			if (gPugMod.GetRound() >= static_cast<int>(gPugCvar.m_Rounds->value))
			{
				if (gPugMod.GetScore(TERRORIST) == gPugMod.GetScore(CT))
				{
					return;
				}
			}

			this->m_Match.SwapScores();
            break;
        }
        case STATE_SECOND_HALF:
        {
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[State].Reset();

                // Clear player round stats
                Player.second.Round.Reset();
            }
            break;
        }
        case STATE_OVERTIME:
        {
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[State].Reset();

                // Clear player round stats
                Player.second.Round.Reset();
            }
            break;
        }
        case STATE_END:
        {
            // Set end time
            this->m_Match.EndTime = time(NULL);

			// Generate Data
			this->DumpData();
            break;
        }
    }
}

const char* CPugStats::GetAuthId(CBasePlayer *Player)
{
    if (Player)
    {
        if (!FNullEnt(Player->edict()))
        {
            if (!(Player->edict()->v.flags & FL_FAKECLIENT))
            {
                return GETPLAYERAUTHID(Player->edict());
            }

            return STRING(Player->edict()->v.netname);
        }
    }

    return "";
}

void CPugStats::GetIntoGame(CBasePlayer *Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].JoinGameTime = time(NULL);

		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;
	}
}

void CPugStats::DropClient(edict_t *pEntity)
{
    auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

    if (Player)
    {
        auto Auth = this->GetAuthId(Player);

        if (Auth)
        {
            this->m_Player[Auth].DisconnectTime = time(NULL);
        }
    }
}

void CPugStats::ChooseTeam(CBasePlayer* Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;
	}
}

void CPugStats::SwitchTeam(CBasePlayer* Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;
	}
}

void CPugStats::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if ((playerAnim == PLAYER_ATTACK1) || (playerAnim == PLAYER_ATTACK2))
		{
			if (Player->m_pActiveItem)
			{
				if (Player->m_pActiveItem->m_iId != WEAPON_NONE)
				{
					if (Player->m_pActiveItem->m_iId != WEAPON_HEGRENADE && Player->m_pActiveItem->m_iId != WEAPON_C4 && Player->m_pActiveItem->m_iId != WEAPON_SMOKEGRENADE && Player->m_pActiveItem->m_iId != WEAPON_FLASHBANG && Player->m_pActiveItem->m_iId != WEAPON_SHIELDGUN)
					{
						auto Auth = this->GetAuthId(Player);

						if (Auth)
						{
							this->m_Player[Auth].Stats[State].Shots++;

							this->m_Player[Auth].Stats[State].Weapon[Player->m_pActiveItem->m_iId].Shots++;

							this->m_Player[Auth].Round.Shots++;
						}
					}
				}
			}
		}
	}
}

void CPugStats::TakeDamage(CBasePlayer* Victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (!Victim->m_bKilledByBomb)
		{
			auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

			if (Attacker)
			{
				if (Victim->entindex() != Attacker->entindex())
				{
					if (Victim->IsPlayer() && Attacker->IsPlayer())
					{
						if (CSGameRules()->FPlayerCanTakeDamage(Victim, Attacker))
						{
							auto VictimAuth = this->GetAuthId(Victim);

							auto AttackerAuth = this->GetAuthId(Attacker);

							auto DamageTaken = (int)(Victim->m_iLastClientHealth - clamp(Victim->edict()->v.health, 0.0f, Victim->edict()->v.health));

							auto ItemIndex = (bitsDamageType & DMG_EXPLOSION) ? WEAPON_HEGRENADE : ((Attacker->m_pActiveItem) ? Attacker->m_pActiveItem->m_iId : WEAPON_NONE);

							this->m_Player[AttackerAuth].Stats[State].Hits++;
							this->m_Player[AttackerAuth].Stats[State].Damage += DamageTaken;

							this->m_Player[AttackerAuth].Stats[State].HitBox[Victim->m_LastHitGroup].Hits++;
							this->m_Player[AttackerAuth].Stats[State].HitBox[Victim->m_LastHitGroup].Damage += DamageTaken;

							this->m_Player[AttackerAuth].Stats[State].Weapon[ItemIndex].Hits++;
							this->m_Player[AttackerAuth].Stats[State].Weapon[ItemIndex].Damage += DamageTaken;

							this->m_Player[VictimAuth].Stats[State].HitsReceived++;
							this->m_Player[VictimAuth].Stats[State].DamageReceived += DamageTaken;

							this->m_Player[VictimAuth].Stats[State].HitBox[Victim->m_LastHitGroup].HitsReceived++;
							this->m_Player[VictimAuth].Stats[State].HitBox[Victim->m_LastHitGroup].DamageReceived += DamageTaken;

							this->m_Player[VictimAuth].Stats[State].Weapon[ItemIndex].HitsReceived++;
							this->m_Player[VictimAuth].Stats[State].Weapon[ItemIndex].DamageReceived += DamageTaken;

							this->m_Player[AttackerAuth].Round.Hits++;
							this->m_Player[AttackerAuth].Round.Damage += DamageTaken;

							this->m_Player[VictimAuth].Round.HitsReceived++;
							this->m_Player[VictimAuth].Round.DamageReceived += DamageTaken;

							this->m_Player[AttackerAuth].Round.PlayerDamage[VictimAuth] += DamageTaken;
						}
					}
				}
			}
		}
	}
}

void CPugStats::SendDeathMessage(CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
        if (KillerBaseEntity && Victim)
        {
            auto Killer = static_cast<CBasePlayer*>(KillerBaseEntity);

            if (Killer->IsPlayer() && Victim->IsPlayer())
			{
				auto VictimIndex = Victim->entindex();

				auto KillerIndex = Killer->entindex();

				auto ItemIndex = (Victim->m_bKilledByGrenade) ? WEAPON_HEGRENADE : ((Killer->m_pActiveItem) ? Killer->m_pActiveItem->m_iId : WEAPON_NONE);

				auto VictimAuth = this->GetAuthId(Victim);

				auto KillerAuth = this->GetAuthId(Killer);

				if (VictimIndex != KillerIndex)
				{
					this->m_Player[KillerAuth].Stats[State].Frags++;

					if (ItemIndex != WEAPON_NONE)
					{
						this->m_Player[KillerAuth].Stats[State].Weapon[ItemIndex].Frags++;
					}

					this->m_Player[VictimAuth].Stats[State].Deaths++;

					if (ItemIndex != WEAPON_NONE)
					{
						this->m_Player[VictimAuth].Stats[State].Weapon[ItemIndex].Deaths++;
					}

					this->m_Player[KillerAuth].Stats[State].HitBox[Victim->m_LastHitGroup].Frags++;

					this->m_Player[VictimAuth].Stats[State].HitBox[Victim->m_LastHitGroup].Deaths++;

					this->m_Player[KillerAuth].Round.Frags++;

					this->m_Player[VictimAuth].Round.Deaths++;

					if ((gpGlobals->time - this->m_Player[KillerAuth].Round.KillTime) < 0.25f)
					{
						this->m_Player[KillerAuth].Stats[State].DoubleKill++;
					}

					this->m_Player[KillerAuth].Round.KillTime = gpGlobals->time;

					if (iRarityOfKill & 0x001 /*KILLRARITY_HEADSHOT*/)
					{
						this->m_Player[KillerAuth].Stats[State].Headshots++;

						this->m_Player[KillerAuth].Stats[State].Weapon[ItemIndex].Headshots++;

						this->m_Player[KillerAuth].Round.Headshots++;
					}

					if (iRarityOfKill & 0x010 /*KILLRARITY_THRUSMOKE*/)
					{
						this->m_Player[KillerAuth].Stats[State].SmokeFrags++;
					}

					if (iRarityOfKill & 0x020 /*KILLRARITY_ASSISTEDFLASH*/)
					{
						if (Assister)
						{
							auto AssisterAuth = this->GetAuthId(Assister);

							if (AssisterAuth)
							{
								this->m_Player[AssisterAuth].Stats[State].AssistedFlash++;
							}
						}
					}

					if (iDeathMessageFlags & 0x002 /*PLAYERDEATH_ASSISTANT*/)
					{
						if (Assister)
						{
							auto AssisterAuth = this->GetAuthId(Assister);

							if (AssisterAuth)
							{
								this->m_Player[AssisterAuth].Stats[State].Assists++;
							}
						}
					}

					if (!Victim->m_bKilledByGrenade)
					{
						if (Killer->IsBlind())
						{
							this->m_Player[KillerAuth].Stats[State].BlindFrags++;
						}

						if (Victim->IsBlind())
						{
							this->m_Player[VictimAuth].Stats[State].BlindDeaths++;
						}
					}

					if (ItemIndex != WEAPON_AWP)
					{
						if (Victim->m_iLastClientHealth >= 100)
						{
							if (Killer->m_lastDamageAmount >= 100)
							{
								this->m_Player[KillerAuth].Stats[State].OneShot++;
							}
						}
					}

					if (ItemIndex == WEAPON_AWP || ItemIndex == WEAPON_SCOUT || ItemIndex == WEAPON_G3SG1 || ItemIndex == WEAPON_SG550)
					{
						if (iRarityOfKill & 0x004 /* KILLRARITY_NOSCOPE */)
						{
							this->m_Player[KillerAuth].Stats[State].NoScope++;
						}
					}

					if (iRarityOfKill & 0x200 /* KILLRARITY_INAIR */)
					{
						this->m_Player[KillerAuth].Stats[State].FlyFrags++;
					}

					if (iRarityOfKill & 0x008 /* KILLRARITY_PENETRATED */)
					{
						this->m_Player[KillerAuth].Stats[State].WallFrags++;
					}

					if (iRarityOfKill & 0x040 /* KILLRARITY_DOMINATION_BEGAN */)
					{
						this->m_Player[KillerAuth].Stats[State].Domination[VictimAuth].DominationBegin++;
					}
					
					if (iRarityOfKill & 0x080 /* KILLRARITY_DOMINATION */)
					{
						this->m_Player[KillerAuth].Stats[State].Domination[VictimAuth].Domination++;
					}

					if (iRarityOfKill & 0x100 /* KILLRARITY_REVENGE */)
					{
						this->m_Player[KillerAuth].Stats[State].Domination[VictimAuth].Revenge++;
					}

					if (g_pGameRules)
					{
						auto NumAliveTR = 0, NumAliveCT = 0, NumDeadTR = 0, NumDeadCT = 0;

						CSGameRules()->InitializePlayerCounts(NumAliveTR, NumAliveCT, NumDeadTR, NumDeadCT);

						if (!NumDeadTR && !NumDeadCT)
						{
							this->m_Player[KillerAuth].Stats[State].EntryFrags++;

							this->m_Player[VictimAuth].Stats[State].EntryDeaths++;
						}

						if (NumAliveTR == 1 || NumAliveCT == 1)
						{
							for (int i = 1; i <= gpGlobals->maxClients; ++i)
							{
								auto Player = UTIL_PlayerByIndexSafe(i);

								if (Player)
								{
									if (Player->IsAlive())
									{
										auto Auth = this->GetAuthId(Player);

										if (Auth)
										{
											if ((Player->m_iTeam == TERRORIST) && (NumAliveTR == 1))
											{
												this->m_Player[Auth].Round.Versus = NumAliveCT;
											}
											else if ((Player->m_iTeam == CT) && (NumAliveCT == 1))
											{
												this->m_Player[Auth].Round.Versus = NumAliveTR;
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					this->m_Player[VictimAuth].Stats[State].Suicides++;
				}

				this->OnEvent(EVENT_PLAYER_DIED, ROUND_NONE, Victim, Killer);
			}
		}
	}
}

void CPugStats::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (type == RT_ROUND_BONUS || type == RT_HOSTAGE_TOOK || type == RT_HOSTAGE_RESCUED || type == RT_ENEMY_KILLED || type == RT_VIP_KILLED || type == RT_VIP_RESCUED_MYSELF)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				this->m_Player[Auth].Stats[State].Money += amount;
			}
		}
	}
}

void CPugStats::RestartRound()
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (g_pGameRules)
		{
			// If is complete reset
			if (CSGameRules()->m_bCompleteReset)
			{
				// Loop all saved players
				for (auto& Player : this->m_Player)
				{
					// Reset player stats of this state
					Player.second.Stats[State].Reset();

					// Reset round stats
					Player.second.Round.Reset();
				}
			}
		}
	}
}

void CPugStats::RoundStart()
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		// For each player
		for (auto& Player : this->m_Player)
		{
			// Reset round stats
			Player.second.Round.Reset();
		}
	}
}

void CPugStats::RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (g_pGameRules)
		{
			if (winStatus == WINSTATUS_TERRORISTS || winStatus == WINSTATUS_CTS)
			{
				// Winner of Round
				auto Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

				// On Event Log
				this->OnEvent((Winner == TERRORIST) ? EVENT_TERRORISTS_WIN : EVENT_CTS_WIN, static_cast<int>(eventScenario), nullptr, nullptr);

				// Increment Score
				this->m_Match.Score[Winner]++;

				// Increment total rounds
				this->m_Match.Rounds++;

				// Calculate who is winning the match
				this->m_Match.Winner = (this->m_Match.Score[TERRORIST] != this->m_Match.Score[CT]) ? (this->m_Match.Score[TERRORIST] > this->m_Match.Score[CT] ? 1 : 2) : 0;

				// Loop player list
				for (auto & Player : this->m_Player)
				{
					// If is in winner team
					Player.second.Winner = (Player.second.Team == this->m_Match.Winner) ? 1 : 0;
				}

				// Team Round Damage
				std::array<float, SPECTATOR + 1U> TeamRoundDamage = { };

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
						{
							auto Auth = this->GetAuthId(Player);

							if (Auth)
							{
								if (this->m_Player[Auth].Round.Damage > 0)
								{
									TeamRoundDamage[Player->m_iTeam] += static_cast<float>(this->m_Player[Auth].Round.Damage);
								}
							}
						}
					}
				}

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
						{
							auto Auth = this->GetAuthId(Player);

							if (Auth)
							{
								this->m_Player[Auth].Stats[State].RoundPlay++;

								if (Player->IsAlive())
								{
									this->m_Player[Auth].Stats[State].Survives++;
								}

								if (Player->m_iTeam == Winner)
								{
									this->m_Player[Auth].Stats[State].RoundWin++;

									if (this->m_Player[Auth].Round.Versus > 0)
									{
										this->m_Player[Auth].Stats[State].Versus[this->m_Player[Auth].Round.Versus]++;
									}

									if (CSGameRules()->m_bBombDefused || CSGameRules()->m_bTargetBombed)
									{
										auto TotalPoints = gPugCvar.m_ST_RwsTotalPoints->value;

										if (this->m_Player[Auth].Round.BombDefused)
										{
											if (gPugCvar.m_ST_RwsC4Defused->value > 0.0f)
											{
												TotalPoints -= gPugCvar.m_ST_RwsC4Defused->value;

												this->m_Player[Auth].Stats[State].RoundWinShare += gPugCvar.m_ST_RwsC4Defused->value;
											}
										}

										if (this->m_Player[Auth].Round.BombExploded)
										{
											if (gPugCvar.m_ST_RwsC4Explode->value > 0.0f)
											{
												TotalPoints -= gPugCvar.m_ST_RwsC4Explode->value;

												this->m_Player[Auth].Stats[State].RoundWinShare += gPugCvar.m_ST_RwsC4Explode->value;
											}
										}

										if (this->m_Player[Auth].Round.Damage > 0)
										{
											this->m_Player[Auth].Stats[State].RoundWinShare += ((((float)this->m_Player[Auth].Round.Damage * (TotalPoints / 100.0f)) / TeamRoundDamage[Winner]) * 100.0f);
										}
									}
									else
									{
										if (this->m_Player[Auth].Round.Damage > 0)
										{
											this->m_Player[Auth].Stats[State].RoundWinShare += (((float)this->m_Player[Auth].Round.Damage / TeamRoundDamage[Winner]) * 100.0f);
										}
									}
								}
								else
								{
									this->m_Player[Auth].Stats[State].RoundLose++;
								}

								if (this->m_Player[Auth].Round.Frags > 0)
								{
									this->m_Player[Auth].Stats[State].KillStreak[this->m_Player[Auth].Round.Frags]++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CPugStats::MakeBomber(CBasePlayer* Player)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (Player->m_bHasC4)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				this->m_Player[Auth].Stats[State].BombSpawn++;
			}
		}
	}
}

void CPugStats::DropPlayerItem(CBasePlayer* Player, const char* pszItemName)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (pszItemName)
		{
			if (!Player->IsAlive())
			{
				if (Q_strcmp(pszItemName, "weapon_c4") == 0)
				{
					auto Auth = this->GetAuthId(Player);

					if (Auth)
					{
						this->m_Player[Auth].Stats[State].BombDrop++;
					}
				}
			}
		}
	}
}

void CPugStats::PlantBomb(entvars_t* pevOwner, bool Planted)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pevOwner));

		if (Player)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				if (Planted)
				{
					this->m_Player[Auth].Stats[State].BombPlanted++;

					this->OnEvent(EVENT_BOMB_PLANTED, ROUND_NONE, Player, nullptr);
				}
				else
				{
					this->m_Player[Auth].Stats[State].BombPlanting++;
				}
			}
		}
	}
}

void CPugStats::DefuseBombStart(CBasePlayer* Player)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		auto Auth = this->GetAuthId(Player);

		if (Auth)
		{
			this->m_Player[Auth].Stats[State].BombDefusing++;

			if (Player->m_bHasDefuser)
			{
				this->m_Player[Auth].Stats[State].BombDefusingKit++;
			}
		}
	}
}

void CPugStats::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		auto Auth = this->GetAuthId(Player);

		if (Auth)
		{
			if (Defused)
			{
				this->m_Player[Auth].Stats[State].BombDefused++;

				if (Player->m_bHasDefuser)
				{
					this->m_Player[Auth].Stats[State].BombDefusedKit++;
				}

				this->m_Player[Auth].Round.BombDefused = true;

				this->OnEvent(EVENT_BOMB_DEFUSED, ROUND_NONE, Player, nullptr);
			}
		}
	}
}

void CPugStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (pThis->m_bIsC4)
		{
			if (pThis->pev->owner)
			{
				auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pThis->pev->owner));

				if (Player)
				{
					auto Auth = this->GetAuthId(Player);

					if (Auth)
					{
						this->m_Player[Auth].Stats[State].BombExploded++;

						this->m_Player[Auth].Round.BombExploded = true;

						this->OnEvent(EVENT_BOMB_EXPLODED, ROUND_NONE, Player, nullptr);
					}
				}
			}
		}
	}
}

bool CPugStats::SayText(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	// If has entity target
	if (!FNullEnt(pEntity))
	{
		// Get CBasePlayer data
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		// If is not null
		if (Player)
		{
			// If is in game
			if ((Player->m_iTeam == TERRORIST) || (Player->m_iTeam == CT))
			{
				// Get argument 1
				auto Format = gPugEngine.GetString(1);

				// If is not empty
				if (Format)
				{
					// Get argument 3
					auto TextMsg = gPugEngine.GetString(3);

					// If is not empty
					if (TextMsg)
					{
						// If is chat for all or for all dead
						if (!Q_stricmp("#Cstrike_Chat_All", Format) || !Q_stricmp("#Cstrike_Chat_AllDead", Format))
						{
							// Get State
							auto State = gPugMod.GetState();

                            // If match is running
                            if (State != STATE_DEAD)
                            {
                                // If player is not null
                                if (Player)
                                {
                                    // If text is not null
                                    if (TextMsg)
                                    {
                                        // Get player auth
                                        auto Auth = gPugStats.GetAuthId(Player);

                                        // If is not null
                                        if (Auth)
                                        {
                                            // Chat struct
                                            P_PLAYER_CHAT Chat = { };

                                            // Set time
                                            Chat.Time = time(NULL);

                                            // Set match state
                                            Chat.State = State;

                                            // Set team
                                            Chat.Team = static_cast<int>(Player->m_iTeam);

                                            // Player is alive
                                            Chat.Alive = Player->IsAlive() ? 1 : 0;

                                            // Set message
                                            Chat.Message = TextMsg;

                                            // Push to vector
                                            gPugStats.m_Player[Auth].ChatLog.push_back(Chat);
                                        }
                                    }
                                }
                            }
						}
					}
				}
			}
		}
	}

	// Do not block original message call
	return false;
}

void CPugStats::OnEvent(GameEventType event, int ScenarioEvent, CBaseEntity* pEntity, class CBaseEntity* pEntityOther)
{
	// Round event data
	P_ROUND_EVENT Event = { };
	
	// Set event round count plus 1
	Event.Round = ((this->m_Match.Score[TERRORIST] + this->m_Match.Score[CT]) + 1);

	// If has ReGameDLL API Game Rules
	if (g_pGameRules)
	{
		// Get round timer
		Event.Time = CSGameRules()->GetRoundRemainingTimeReal();
	}
	
	// Store type of event
	Event.Type = event;
	
	// Store event scenario
	Event.ScenarioEvent = ScenarioEvent;

	// Store State
	Event.State = gPugMod.GetState();
	
	// Switch of event
	switch (event)
	{
		// Tell bots the player is killed (argumens: 1 = victim, 2 = killer)
		case EVENT_PLAYER_DIED:
		{
			// Get Victim
			auto Victim = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// Get Killer
			auto Killer = UTIL_PlayerByIndexSafe(pEntityOther->entindex());

			// If event has victim abnd killer
			if (Victim && Killer)
			{
				// Get Killer auth index
				Event.Killer = this->GetAuthId(Killer);

				// Get Killer Origin
				Event.KillerOrigin = Killer->edict()->v.origin;

				// Get Victim auth index
				Event.Victim = this->GetAuthId(Victim);

				// Get Victim Origin
				Event.VictimOrigin = Victim->edict()->v.origin;

				// Get Winner of event (Team of Killer)
				Event.Winner = Killer->m_iTeam;

				// Get Loser of event (Team of Victim)
				Event.Loser = Victim->m_iTeam;

				// Check if victim was killed by a headshot
				Event.IsHeadShot = Victim->m_bHeadshotKilled ? 1 : 0;

				// Default weapon is empty
				Event.ItemIndex = WEAPON_NONE;

				// If killer has active item on hand
				if (Killer->m_pActiveItem)
				{
					// Store item index, and check if is HE Grenade
					Event.ItemIndex = (Victim->m_bKilledByGrenade ? WEAPON_HEGRENADE : Killer->m_pActiveItem->m_iId);
				}
			}

			//
			break;
		}
		// Tell bots the bomb has been planted (argumens: 1 = planter, 2 = NULL)
		case EVENT_BOMB_PLANTED:
		{
			// Get Bomb Planter
			auto Planter = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If found
			if (Planter)
			{
				// Store as killer auth index
				Event.Killer = this->GetAuthId(Planter);

				// Store killer origin
				Event.KillerOrigin = Planter->edict()->v.origin;
			}

			// Winner of that event is Terrorists
			Event.Winner = TERRORIST;

			// Loser of that event is CTs
			Event.Loser = CT;

			// Is not headshot, rofl
			Event.IsHeadShot = 0;

			// Weapon is C4
			Event.ItemIndex = WEAPON_C4;

			//
			break;
		}
		// Tell the bots the bomb is defused (argumens: 1 = defuser, 2 = NULL)
		case EVENT_BOMB_DEFUSED:
		{
			// Get Defuser of bomb
			auto Defuser = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If is not null
			if (Defuser)
			{
				// Set as killer
				Event.Killer = this->GetAuthId(Defuser);

				// Set origin
				Event.KillerOrigin = Defuser->edict()->v.origin;

				// If has defuser, set as headshot
				Event.IsHeadShot = Defuser->m_bHasDefuser ? 1 : 0;
			}

			// Winner is CT
			Event.Winner = CT;

			// Loser is TERRORIST
			Event.Loser = TERRORIST;

			// Defused item is C4
			Event.ItemIndex = WEAPON_C4;

			break;
		}
		// Let the bots hear the bomb exploding (argumens: 1 = NULL, 2 = NULL)
		case EVENT_BOMB_EXPLODED:
		{
			// Get Planter of the bomb
			auto Planter = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If found
			if (Planter)
			{
				// Set as killer
				Event.Killer = this->GetAuthId(Planter);

				// Set player origin
				Event.KillerOrigin = Planter->edict()->v.origin;
			}

			// Winner is TERRORIST
			Event.Winner = TERRORIST;

			// Loser is CT
			Event.Loser = CT;

			// Nothing to do here
			Event.IsHeadShot = 0;

			// Item index is C4
			Event.ItemIndex = WEAPON_C4;

			break;
		}
		// Tell bots the terrorists won the round (argumens: 1 = NULL, 2 = NULL)
		case EVENT_TERRORISTS_WIN:
		{
			// Set winner
			Event.Winner = TERRORIST;

			// Set loser
			Event.Loser = CT;

			// Nothing to do
			Event.IsHeadShot = false;

			// No weapons
			Event.ItemIndex = WEAPON_NONE;

			break;
		}
		// Tell bots the CTs won the round (argumens: 1 = NULL, 2 = NULL)
		case EVENT_CTS_WIN:
		{
			// Set winner
			Event.Winner = CT;

			// Set loser
			Event.Loser = TERRORIST;

			// Nothing to do
			Event.IsHeadShot = false;

			// No weapons
			Event.ItemIndex = WEAPON_NONE;

			break;
		}
		default:
		{
			// None
			break;
		}
	}

	// Insert Event Data
	this->m_RoundEvent.push_back(Event);
}

void CPugStats::DumpData()
{
	try
	{
		// Data
		nlohmann::json Data;
		//
		// Match (OK)
		Data["Match"] =
		{
			{"StartTime", this->m_Match.StartTime},
			{"EndTime", this->m_Match.EndTime},
			{"HostName", this->m_Match.HostName},
			{"Map", this->m_Match.Map},
			{"Address", this->m_Match.Address},
			{"ScoreTRs", this->m_Match.Score[TERRORIST]},
			{"ScoreCTs", this->m_Match.Score[CT]},
			{"Winner", this->m_Match.Winner},
			{"Rounds", this->m_Match.Rounds},
			{"MaxRounds", this->m_Match.MaxRounds},
			{"MaxRoundsOT", this->m_Match.MaxRoundsOT},
			{"GameMode", this->m_Match.GameMode},
			{"KnifeRound", this->m_Match.KnifeRound},
			{"MinPlayers", this->m_Match.MinPlayers},
			{"MaxPlayers", this->m_Match.MaxPlayers}
		};
		//
		// Players
		for (auto & Player : this->m_Player)
		{
			// Player Result Stats
			P_PLAYER_STATS Result = {};
			
			// Player Data (OK)
			Data["Player"][Player.first]["Data"] =
			{
				{"JoinGameTime", Player.second.JoinGameTime},
				{"DisconnectTime", Player.second.DisconnectTime},
				{"Auth", Player.first},
				{"Name", Player.second.Name},
				{"Team", Player.second.Team},
				{"Winner", Player.second.Winner},
				{"IsBot", Player.second.IsBot},
			};
			//
			// Player Stats (OK)
			for (auto & Stats : Player.second.Stats)
			{
				if (Stats.first == STATE_FIRST_HALF || Stats.first == STATE_SECOND_HALF || Stats.first == STATE_OVERTIME)
				{
					// Player stats
					Result.Frags += Stats.second.Frags;
					Result.Deaths += Stats.second.Deaths;
					Result.Assists += Stats.second.Assists;
					Result.Headshots += Stats.second.Headshots;
					Result.Shots += Stats.second.Shots;
					Result.Hits += Stats.second.Hits;
					Result.HitsReceived += Stats.second.HitsReceived;
					Result.Damage += Stats.second.Damage;
					Result.DamageReceived += Stats.second.DamageReceived;
					Result.Money += Stats.second.Money;
					Result.Suicides += Stats.second.Suicides;
					Result.Survives = Stats.second.Survives;
					//
					// Round Win Share
					Result.RoundWinShare += Stats.second.RoundWinShare;
					//
					// Rating
					Result.RatingTR += Stats.second.RatingTR,
					Result.RatingCT += Stats.second.RatingCT,
					//
					// Misc Frags
					Result.BlindFrags += Stats.second.BlindFrags;
					Result.BlindDeaths += Stats.second.BlindDeaths;
					Result.OneShot += Stats.second.OneShot;
					Result.NoScope += Stats.second.NoScope;
					Result.FlyFrags += Stats.second.FlyFrags;
					Result.WallFrags += Stats.second.WallFrags;
					Result.DoubleKill += Stats.second.DoubleKill;
					Result.SmokeFrags += Stats.second.SmokeFrags;
					Result.AssistedFlash += Stats.second.AssistedFlash;
					//
					// Knife Duels
					Result.KnifeDuelWin += Stats.second.KnifeDuelWin;
					Result.KnifeDuelLose += Stats.second.KnifeDuelLose;
					//
					// Entry Frags and Deaths
					Result.EntryFrags += Stats.second.EntryFrags;
					Result.EntryDeaths += Stats.second.EntryDeaths;
					//
					// Round counter
					Result.RoundPlay += Stats.second.RoundPlay;
					Result.RoundWin += Stats.second.RoundWin;
					Result.RoundLose += Stats.second.RoundLose;
					//
					// Bomb counter
					Result.BombSpawn += Stats.second.BombSpawn;
					Result.BombDrop += Stats.second.BombDrop;
					Result.BombPlanting += Stats.second.BombPlanting;
					Result.BombPlanted += Stats.second.BombPlanted;
					Result.BombExploded += Stats.second.BombExploded;
					Result.BombDefusing += Stats.second.BombDefusing;
					Result.BombDefusingKit += Stats.second.BombDefusingKit;
					Result.BombDefused += Stats.second.BombDefused;
					Result.BombDefusedKit += Stats.second.BombDefusedKit;
					//
					// Kill Streak
					for (size_t i = 0; i < Stats.second.KillStreak.size(); i++)
					{
						Result.KillStreak[i] += Stats.second.KillStreak[i];
					}
					//
					// Versus
					for (size_t i = 0; i < Stats.second.Versus.size(); i++)
					{
						Result.Versus[i] += Stats.second.Versus[i];
					}
					//
					// Hitbox Stats
					for (size_t i = 0; i < Stats.second.HitBox.size(); i++)
					{
						Result.HitBox[i].Hits += Stats.second.HitBox[i].Hits;
						Result.HitBox[i].Damage += Stats.second.HitBox[i].Damage;
						Result.HitBox[i].HitsReceived += Stats.second.HitBox[i].HitsReceived;
						Result.HitBox[i].DamageReceived += Stats.second.HitBox[i].DamageReceived;
						Result.HitBox[i].Frags += Stats.second.HitBox[i].Frags;
						Result.HitBox[i].Deaths += Stats.second.HitBox[i].Deaths;
					}
					//
					// Weapon Stats
					for (auto & Weapon : Stats.second.Weapon)
					{
						Result.Weapon[Weapon.first].Frags += Weapon.second.Frags;
						Result.Weapon[Weapon.first].Deaths += Weapon.second.Deaths;
						Result.Weapon[Weapon.first].Headshots += Weapon.second.Headshots;
						Result.Weapon[Weapon.first].Shots += Weapon.second.Shots;
						Result.Weapon[Weapon.first].Hits += Weapon.second.Hits;
						Result.Weapon[Weapon.first].HitsReceived += Weapon.second.HitsReceived;
						Result.Weapon[Weapon.first].Damage += Weapon.second.Damage;
						Result.Weapon[Weapon.first].DamageReceived += Weapon.second.DamageReceived;
					}
					//
					// Domination / Revenge
					for (auto & Domination : Stats.second.Domination)
					{
						Result.Domination[Domination.first].DominationBegin += Domination.second.DominationBegin;
						Result.Domination[Domination.first].Domination += Domination.second.Domination;
						Result.Domination[Domination.first].Revenge += Domination.second.Revenge;
					}
				}
			}
			//
			// Stats (OK)
			Data["Player"][Player.first]["Stats"] =
			{
				// Player stats
				{"Frags", Result.Frags},
				{"Deaths", Result.Deaths},
				{"Assists", Result.Assists},
				{"Headshots", Result.Headshots},
				{"Shots", Result.Shots},
				{"Hits", Result.Hits},
				{"HitsReceived", Result.HitsReceived},
				{"Damage", Result.Damage},
				{"DamageReceived", Result.DamageReceived},
				{"Money", Result.Money},
				{"Suicides", Result.Suicides},
				{"Survives", Result.Survives},
				//
				// Round Win Share
				{"RoundWinShare", Result.RoundWinShare > 0.0f ? (Result.RoundWinShare / static_cast<float>(Result.RoundWin)) : 0.0f},
				//
				// Rating
				{"RatingTR", Result.RatingTR},
				{"RatingCT", Result.RatingCT},
				//
				// Misc Frags
				{"BlindFrags", Result.BlindFrags},
				{"BlindDeaths", Result.BlindDeaths},
				{"OneShot", Result.OneShot},
				{"NoScope", Result.NoScope},
				{"FlyFrags", Result.FlyFrags},
				{"WallFrags", Result.WallFrags},
				{"DoubleKill", Result.DoubleKill},
				{"SmokeFrags", Result.SmokeFrags},
				{"AssistedFlash", Result.AssistedFlash},
				//
				// Knife Duels
				{"KnifeDuelWin", Result.KnifeDuelWin},
				{"KnifeDuelLose", Result.KnifeDuelLose},
				//
				// Entry Frags and Deaths
				{"EntryFrags", Result.EntryFrags},
				{"EntryDeaths", Result.EntryDeaths},
				//
				// Round counter
				{"RoundPlay", Result.RoundPlay},
				{"RoundWin", Result.RoundWin},
				{"RoundLose", Result.RoundLose},
				//
				// Bomb counter
				{"BombSpawn", Result.BombSpawn},
				{"BombDrop", Result.BombDrop},
				{"BombPlanting", Result.BombPlanting},
				{"BombPlanted", Result.BombPlanted},
				{"BombExploded", Result.BombExploded},
				{"BombDefusing", Result.BombDefusing},
				{"BombDefusingKit", Result.BombDefusingKit},
				{"BombDefused", Result.BombDefused},
				{"BombDefusedKit", Result.BombDefusedKit},
				//
				// Formulas (OK)
				{"DIF", (Result.Frags - Result.Deaths)},
				{"KDR", Result.Deaths > 0 ? (Result.Frags / static_cast<float>(Result.Deaths)) : 0.0f},
				{"KDA", Result.Deaths > 0 ? ((Result.Frags + Result.Assists) / static_cast<float>(Result.Deaths)) : 0.0f},
				{"HSP", Result.Frags > 0 ? (100.0f * Result.Headshots / static_cast<float>(Result.Frags)) : 0.0f},
				{"ADR", Result.RoundPlay > 0 ? (Result.Damage / static_cast<float>(Result.RoundPlay)) : 0.0f},
				{"ACC", Result.Shots > 0 ? (100.0f * Result.Hits / static_cast<float>(Result.Shots)) : 0.0f},
				{"EFF", (Result.Frags + Result.Deaths) > 0 ? (100.0f * Result.Frags / static_cast<float>(Result.Frags + Result.Deaths)) : 0.0f},
				{"KPR", Result.RoundPlay > 0 ? (Result.Frags / static_cast<float>(Result.RoundPlay)) : 0.0f},
				{"DPR", Result.RoundPlay > 0 ? (Result.Deaths / static_cast<float>(Result.RoundPlay)) : 0.0f},
			};
			//
			// Kill Streak (OK)
			Data["Player"][Player.first]["KillStreak"] = Result.KillStreak;
			//
			// Versus (OK)
			Data["Player"][Player.first]["Versus"] = Result.Versus;
			//
			// Hitbox Stats (OK)
			for (size_t Place = 0; Place < Result.HitBox.size(); Place++)
			{
				Data["Player"][Player.first]["Hitbox"] +=
				{
					{"Place", Place},
					{"Hits", Result.HitBox[Place].Hits},
					{"Damage", Result.HitBox[Place].Damage},
					{"HitsReceived", Result.HitBox[Place].HitsReceived},
					{"DamageReceived", Result.HitBox[Place].DamageReceived},
					{"Frags", Result.HitBox[Place].Frags},
					{"Deaths", Result.HitBox[Place].Deaths}
				};
			}
			//
			// Weapon Stats (OK)
			for (auto & Weapon : Result.Weapon)
			{
				if (Weapon.first)
				{
					Data["Player"][Player.first]["Weapon"] +=
					{
						{"Weapon", Weapon.first},
						{"Frags", Weapon.second.Frags},
						{"Deaths", Weapon.second.Deaths},
						{"Headshots", Weapon.second.Headshots},
						{"Shots", Weapon.second.Shots},
						{"Hits", Weapon.second.Hits},
						{"HitsReceived", Weapon.second.HitsReceived},
						{"Damage", Weapon.second.Damage},
						{"DamageReceived", Weapon.second.DamageReceived}
					};
				}
			}
			//
			// Domination / Revenge (OK)
			for (auto & Domination : Result.Domination)
			{
				Data["Player"][Player.first]["Domination"] +=
				{
					{"Auth", Domination.first},
					{"DominationBegin", Domination.second.DominationBegin},
					{"Domination", Domination.second.Domination},
					{"Revenge", Domination.second.Revenge}
				};
			}
			//
			// Chat Log (OK)
			for (auto & Chat : Player.second.ChatLog)
			{
				Data["Chat"] +=
				{
					{"Auth", Player.first},
					{"Name", Player.second.Name},
					{"Time", Chat.Time},
					{"State", Chat.State},
					{"Team", Chat.Team},
					{"Alive", Chat.Alive},
					{"Message", Chat.Message}
				};
			}
		}
		//
		// Round events (OK)
		for (auto & Event : this->m_RoundEvent)
		{
			Data["Events"] +=
			{
				{"Round",Event.Round},
				{"Time",Event.Time},
				{"Type",Event.Type},
				{"ScenarioEvent", Event.ScenarioEvent},
				{"State", Event.State},
				{"Winner",Event.Winner},
				{"Loser",Event.Loser},
				{"Killer",Event.Killer},
				{"KillerOrigin",{Event.KillerOrigin.x,Event.KillerOrigin.y,Event.KillerOrigin.z}},
				{"Victim",Event.Victim},
				{"VictimOrigin",{Event.VictimOrigin.x,Event.VictimOrigin.y,Event.VictimOrigin.z}},
				{"IsHeadShot",Event.IsHeadShot},
				{"ItemIndex",Event.ItemIndex}
			};
		}
		
		// If data is not empty
		if (!Data.empty())
		{
			// Save data in file
			this->SaveData(Data);

			// Upload data
			this->UploadData(Data);
		}

		Data.clear();
	}
	catch (nlohmann::json::exception & ex)
	{
		LOG_CONSOLE(PLID, "[%s] %d: %s", __func__, ex.id, ex.what());
	}
}

void CPugStats::SaveData(nlohmann::json Data)
{
	if (!Data.empty())
	{
		char Date[32] = {};
		strftime(Date, sizeof(Date), "%y%m%d%H%M", localtime(&this->m_Match.EndTime));

		char File[128] = {};
		Q_snprintf(File, sizeof(File), "pug-%s-%s.json", Date, this->m_Match.Map.c_str());

		char Path[MAX_PATH] = {};
		Q_snprintf(Path, sizeof(Path), "%s/stats", gPugUtil.GetFullPath());

		gPugUtil.MakeDirectory(Path);

		char FilePath[MAX_PATH * 2] = {};
		Q_snprintf(FilePath, sizeof(FilePath), "%s/%s", Path, File);

		std::ofstream DataFile(FilePath);

		DataFile << Data;

		DataFile.close();
	}
}

void CPugStats::UploadData(nlohmann::json Data)
{
	if (!Data.empty())
	{
		if (gPugCvar.m_API_Enable->value > 0.0f)
		{
			if (gPugCvar.m_API_Address->string)
			{
                if (gPugCvar.m_API_Address->string[0u] != '\0')
                {
                    gPugCurl.PostJSON(gPugCvar.m_API_Address->string, (long)(gPugCvar.m_API_Timeout->value), gPugCvar.m_API_Bearer->string, Data.dump());
                }
			}
		}
	}
}
