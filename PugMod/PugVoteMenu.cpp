#include "precompiled.h"

CPugVoteMenu gPugVoteMenu;

void CPugVoteMenu::ServerActivate()
{
	this->m_VoteKick = {};
	this->m_VoteMap = {};
	this->m_VotePause = {};
	this->m_VoteRestart = {};
	this->m_VoteCancel = {};
	this->m_VoteSurrender = {};
}

void CPugVoteMenu::RestartRound()
{
	if (g_pGameRules)
	{
		if (CSGameRules()->m_bCompleteReset)
		{
			this->m_VoteKick = {};
			this->m_VoteMap = {};
			this->m_VotePause = {};
			this->m_VoteRestart = {};
			this->m_VoteCancel = {};
			this->m_VoteSurrender = {};
		}
	}
}

void CPugVoteMenu::DropClient(edict_t *pEntity)
{
	auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

	if (Player)
	{
		this->m_VoteKick[Player->entindex()].fill(false);

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			this->m_VoteKick[i][Player->entindex()] = false;
		}

		this->m_VoteMap[Player->entindex()].fill(false);

		this->m_VotePause[Player->entindex()].fill(false);

		this->m_VoteRestart[Player->entindex()] = false;

		this->m_VoteCancel[Player->entindex()]= false;

		this->m_VoteSurrender[Player->entindex()].fill(false);
	}
}

bool CPugVoteMenu::Menu(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State != STATE_DEAD && State != STATE_VOTEMAP && State != STATE_VOTETEAM && State != STATE_CAPTAIN && State != STATE_END)
		{
			gPugMenu[Player->entindex()].Create(true, E_MENU::ME_PLAYER_VOTE_MENU, _T("Vote Menu:"));

			gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_KICK, false, 0, _T("Kick Player"));
			gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_MAP, false, 0, _T("Change Map"));

			if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
			{
				gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_PAUSE, false, 0, _T("Pause Match"));
				gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_RESTART, false, 0, _T("Restart Match"));
				gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_CANCEL, false, 0, _T("Cancel Match"));
				gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_SURRENDER, false, 0, _T("Vote Surrender"));
			}

			gPugMenu[Player->entindex()].Show(Player);

			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Choose an option to vote."), gPugCvar.m_Tag->string);

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	switch (Item.Info)
	{
		case PLAYER_VOTE_KICK:
		{
			this->VoteKick(Player);
			return true;
		}
		case PLAYER_VOTE_MAP:
		{
			this->VoteMap(Player);
			return true;
		}
		case PLAYER_VOTE_PAUSE:
		{
			this->VotePause(Player);
			return true;
		}
		case PLAYER_VOTE_RESTART:
		{
			this->VoteRestart(Player);
			return true;
		}
		case PLAYER_VOTE_CANCEL:
		{
			this->VoteCancel(Player);
			return true;
		}
		case PLAYER_VOTE_SURRENDER:
		{
			this->VoteSurrender(Player);
			return true;
		}
	}

	return false;
}

bool CPugVoteMenu::VoteKick(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State != STATE_DEAD && State != STATE_VOTEMAP && State != STATE_VOTETEAM && State != STATE_CAPTAIN && State != STATE_END)
		{
			if (gPugCvar.m_PlayerVoteKick->value <= 0.0f)
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote kick disabled."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers();

			if (Players[Player->m_iTeam].size() < (size_t)(gPugCvar.m_PlayersMin->value))
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote kick need more than ^3%d^1 players to be used."), gPugCvar.m_Tag->string, static_cast<int>(gPugCvar.m_PlayersMin->value));
				return false;
			}

			gPugMenu[Player->entindex()].Create(true, E_MENU::ME_PLAYER_VOTE_KICK, _T("Vote Kick:"));

			auto Needs = (static_cast<int>(gPugCvar.m_PlayersMin->value) - 1);

			for (auto Target : Players[Player->m_iTeam])
			{
				if (Player->entindex() != Target->entindex())
				{
					if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
					{
						auto Count = 0;

						for (int i = 1; i <= gpGlobals->maxClients; ++i)
						{
							if (this->m_VoteKick[i][Target->entindex()])
							{
								Count += 1;
							}
						}

						gPugMenu[Player->entindex()].AddItem(Target->entindex(), this->m_VoteKick[Player->entindex()][Target->entindex()], 0, "%s \\y\\R%2.0f%%", STRING(Target->edict()->v.netname), ((Count * 100.0f) / Needs));
					}
				}
			}

			gPugMenu[Player->entindex()].Show(Player);

			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Choose an player to vote kick."), gPugCvar.m_Tag->string);

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::VoteKickHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	auto Target = UTIL_PlayerByIndexSafe(Item.Info);

	if (Target)
	{
		if (Item.Disabled)
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Already voted to kick: ^3%s^1."), gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname));
			return false;
		}

		auto Players = gPugUtil.GetPlayers();

		if (Players[Player->m_iTeam].size() < (size_t)(gPugCvar.m_PlayersMin->value))
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote kick need more than ^3%d^1 players to be used."), gPugCvar.m_Tag->string, static_cast<int>(gPugCvar.m_PlayersMin->value));
			return false;
		}

		this->m_VoteKick[Player->entindex()][Target->entindex()] = true;

		auto Count = 0;
		auto Needs = (Players[Player->m_iTeam].size() - 1);

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			if (this->m_VoteKick[i][Target->entindex()] )
			{
				Count += 1;
			}
		}

		auto Progress = ((Count * 100.0f) / Needs);

		if (Progress < 100.0f)
		{
			gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 voted to kick ^3%s^1: %2.0f%% of votes to kick."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname), Progress);

			auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

			if (pCommand)
			{
				for (auto const & Temp : Players[Player->m_iTeam])
				{
					gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
				}
			}
		}
		else
		{
			gPugUtil.PrintColor(Player->edict(), Player->entindex(), _T("^4[%s]^1 ^3%s^1 Kicked: ^4%d^1 votes reached."), gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname), Needs);

			gPugUtil.ServerCommand("kick #%d %s", GETPLAYERUSERID(Target->edict()), _T("Kicked by Vote Kick."));
		}

		return true;
	}

	return false;
}

bool CPugVoteMenu::VoteMap(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (gPugMod.GetState() != STATE_END)
		{
			if (gPugCvar.m_PlayerVoteMap->value <= 0.0f)
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote map disabled."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Maps = gPugMapList.Get();

			if (Maps.size() < 1)
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Map list is empty."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers(true, true);

			if (Players.size() < 2)
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote map unavailable."), gPugCvar.m_Tag->string);
				return false;
			}

			gPugMenu[Player->entindex()].Create(true, E_MENU::ME_PLAYER_VOTE_MAP, _T("Vote Map:"));

			auto Needs = (static_cast<float>(Players.size()) * gPugCvar.m_VotePercent->value);

			for (auto const & Map : Maps)
			{
				auto Count = 0.0f;

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					if (this->m_VoteMap[i][Map.first])
					{
						Count += 1.0f;
					}
				}

				gPugMenu[Player->entindex()].AddItem(Map.first, this->m_VoteMap[Player->entindex()][Map.first], 0, "%s \\y\\R%2.0f%%", Map.second.c_str(), ((Count * 100.0f) / Needs));
			}

			gPugMenu[Player->entindex()].Show(Player);

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::VoteMapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	auto Maps = gPugMapList.Get();

	if (Maps.size() < 1)
	{
		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Map list is empty."), gPugCvar.m_Tag->string);
		return false;
	}

	auto Map = Maps.find(Item.Info);

	if (Map != Maps.end())
	{
		if (Item.Disabled)
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Already voted to change map to: ^3%s^1."), gPugCvar.m_Tag->string, Map->second.c_str());
			return false;
		}

		auto Players = gPugUtil.GetPlayers(true, true);

		if (Players.size() < 2)
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote map unavailable."), gPugCvar.m_Tag->string);
			return false;
		}

		this->m_VoteMap[Player->entindex()][Map->first] = true;

		auto Count = 0.0f;
		auto Needs = (static_cast<float>(Players.size()) * gPugCvar.m_VotePercent->value);

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			if (this->m_VoteMap[i][Map->first])
			{
				Count += 1.0f;
			}
		}

		auto Progress = ((Count * 100.0f) / Needs);

		if (Progress < 100.0f)
		{
			gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 voted to change map to ^3%s^1: %2.0f%% of votes to change."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), Map->second.c_str(), Progress);

			auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

			if (pCommand)
			{
				for (auto const & Temp : Players)
				{
					gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
				}
			}
		}
		else
		{
			gPugTask.Create(E_TASK::MAP_CHANGE, 5.0f, false, Map->first);

			gPugUtil.PrintColor(Player->edict(), Player->entindex(), _T("^4[%s]^1 ^3%s^1 Changing map to: ^4%s^1, ^4%d^1 votes reached."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), Map->second.c_str(), static_cast<int>(Needs));
		}

		return false;
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);
    
	return false;
}

bool CPugVoteMenu::VotePause(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
		{
			if (this->m_VotePause[Player->entindex()][Player->m_iTeam])
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Already voted to pause the game."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers();

			if (Players[Player->m_iTeam].size() < 2)
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote pause needs at least ^3%d^1 players in your team."), gPugCvar.m_Tag->string, 2);
				return false;
			}

			this->m_VotePause[Player->entindex()][Player->m_iTeam] = true;

			auto Count = 0.0f;

			auto Needs = (static_cast<float>(Players[Player->m_iTeam].size()) * gPugCvar.m_VotePercent->value);

			for (auto const &Temp : Players[Player->m_iTeam])
			{
				if (this->m_VotePause[Temp->entindex()][Player->m_iTeam])
				{
					Count += 1.0f;
				}
			}

			auto Progress = ((Count * 100.0f) / Needs);

			if (Progress < 100.0f)
			{
				gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 (^3%s^1) voted to pause match: %2.0f%% of votes to pause."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), g_Pug_TeamName[Player->m_iTeam], Progress);

				auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

				if (pCommand)
				{
					for (auto const & Temp : Players[Player->m_iTeam])
					{
						gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
					}
				}
			}
			else
			{
				this->m_VotePause = {};

				gPugPause.Init(Player->m_iTeam);
			}

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::VoteRestart(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State == STATE_FIRST_HALF)
		{
			if (this->m_VoteRestart[Player->entindex()])
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Already voted to restart match."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers(true, true);

			if (Players.size() < static_cast<size_t>(gPugCvar.m_PlayersMin->value))
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote restart need more than ^3%d^1 players to be used."), gPugCvar.m_Tag->string, static_cast<int>(gPugCvar.m_PlayersMin->value));
				return false;
			}

			this->m_VoteRestart[Player->entindex()] = true;

			auto Count = 0.0f;
			auto Needs = (static_cast<float>(Players.size()) * gPugCvar.m_VotePercent->value);

			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				if (this->m_VoteRestart[i])
				{
					Count += 1.0f;
				}
			}

			auto Progress = ((Count * 100.0f) / Needs);

			if (Progress < 100.0f)
			{
				gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 voted to restart match: %2.0f%% of votes to restart."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), Progress);

				auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

				if (pCommand)
				{
					for (auto const & Temp : Players)
					{
						gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
					}
				}
			}
			else
			{
				this->m_VoteRestart = {};

				gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_FIRST_HALF);

				gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The match will be restarted."), gPugCvar.m_Tag->string);
			}

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::VoteCancel(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State == STATE_FIRST_HALF)
		{
			if (this-> m_VoteCancel[Player->entindex()])
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Already voted to cancel match."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers(true, true);

			if (Players.size() < static_cast<size_t>(gPugCvar.m_PlayersMin->value))
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Vote cancel need more than ^3%d^1 players to be used."), gPugCvar.m_Tag->string, static_cast<int>(gPugCvar.m_PlayersMin->value));
				return false;
			}

			this->m_VoteCancel[Player->entindex()] = true;

			auto Count = 0.0f;
			auto Needs = (static_cast<float>(Players.size()) * gPugCvar.m_VotePercent->value);

			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				if (this->m_VoteCancel[i])
				{
					Count += 1.0f;
				}
			}

			auto Progress = ((Count * 100.0f) / Needs);

			if (Progress < 100.0f)
			{
				gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 voted to cancel match: %2.0f%% of votes to cancel."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), Progress);

				auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

				if (pCommand)
				{
					for (auto const & Temp : Players)
					{
						gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
					}
				}
			}
			else
			{
				this->m_VoteCancel = {};

				gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_DEATHMATCH);

				gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The match will be cancelled."), gPugCvar.m_Tag->string);
			}

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugVoteMenu::VoteSurrender(CBasePlayer *Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto State = gPugMod.GetState();

		if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF)
		{
			if (this->m_VoteSurrender[Player->entindex()][Player->m_iTeam])
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 You have already voted to surrender for your team."), gPugCvar.m_Tag->string);
				return false;
			}

			auto Players = gPugUtil.GetPlayers();

			if (Players[Player->m_iTeam].size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value))
			{
				gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The ^3%s^1 team is complete, cannot vote to surrender."), gPugCvar.m_Tag->string, g_Pug_TeamName[Player->m_iTeam]);
				return false;
			}

			this->m_VoteSurrender[Player->entindex()][Player->m_iTeam] = true;

			auto Count = 0.0f;
			auto Needs = (static_cast<float>(Players[Player->m_iTeam].size()) * gPugCvar.m_VotePercent->value);

			for (auto const &Temp : Players[Player->m_iTeam])
			{
				if (this->m_VoteSurrender[Temp->entindex()][Player->m_iTeam])
				{
					Count += 1.0f;
				}
			}

			auto Progress = ((Count * 100.0f) / Needs);

			if (Progress < 100.0f)
			{
				gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 (^3%s^1) voted to surrender: %2.0f%% of votes to stop match."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), g_Pug_TeamName[Player->m_iTeam], Progress);

				auto pCommand = gPugClientCmd.Get(CMD_VOTE_MENU);

				if (pCommand)
				{
					for (auto const & Temp : Players[Player->m_iTeam])
					{
						gPugUtil.PrintColor(Temp->edict(), Temp->entindex(), _T("^4[%s]^1 Say ^3%s^1 to open vote menu."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
					}
				}
			}
			else
			{
				this->m_VoteSurrender = {};

				gPugMod.Surrender(Player->m_iTeam);
			}

			return true;
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}