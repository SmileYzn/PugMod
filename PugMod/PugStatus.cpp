#include "precompiled.h"

CPugStatus gPugStatus;

void CPugStatus::ServerActivate()
{
	this->m_NextUpdate = (gpGlobals->time + gPugCvar.m_API_Status->value);

	this->m_Data.clear();
}

void CPugStatus::StartFrame()
{
	if (gPugCvar.m_API_Enable->value > 0.0f)
	{
		if (gPugCvar.m_API_Address->string)
		{
			if (gPugCvar.m_API_Address->string[0u] != '\0')
			{
				if (gPugCvar.m_API_Status->value >= PUG_STATUS_MIN_UPDATE_TIME)
				{
					if (gpGlobals->time >= this->m_NextUpdate)
					{
						this->SendStatus();

						this->m_NextUpdate = (gpGlobals->time + gPugCvar.m_API_Status->value);
					}
				}
			}
		}
	}
}

void CPugStatus::SendStatus()
{
	this->m_Data["Time"] = time(NULL);

	this->m_Data["Address"] = CVAR_GET_STRING("net_address");

	this->m_Data["HostName"] = CVAR_GET_STRING("hostname");

	this->m_Data["Map"] = STRING(gpGlobals->mapname);

	this->m_Data["Game"] = (g_pGameRules ? CSGameRules()->m_GameDesc : "Counter-Strike");

	this->m_Data["ModName"] = (gPugCvar.m_ModName->string ? gPugCvar.m_ModName->string : "PUG");

	this->m_Data["KnifeRound"] = static_cast<int>(gPugCvar.m_PlayKnifeRound->value);

	this->m_Data["GameMode"] = static_cast<int>(gPugCvar.m_PlayGameMode->value);

	auto State = gPugMod.GetState();

	this->m_Data["State"] = gPugMod.GetString(State);

	this->m_Data["StateIndex"] = State;

	this->m_Data["IsLive"] = (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME);

	this->m_Data["ScoreTR"] = gPugMod.GetScore(TERRORIST);

	this->m_Data["ScoreCT"] = gPugMod.GetScore(CT);

	this->m_Data["Rounds"] = gPugMod.GetRound();

	this->m_Data["MaxRounds"] = static_cast<int>(gPugCvar.m_Rounds->value);

	this->m_Data["Slots"] = gpGlobals->maxClients;

	this->m_Data["MinPlayers"] = static_cast<int>(gPugCvar.m_PlayersMin->value);

	this->m_Data["MaxPlayers"] = static_cast<int>(gPugCvar.m_PlayersMax->value);

	auto Teams = gPugUtil.GetPlayers();

	this->m_Data["NumPlayers"] = 
	{
		{"0", Teams[UNASSIGNED].size()},
		{"1", Teams[TERRORIST].size()},
		{"2", Teams[CT].size()},
		{"3", Teams[SPECTATOR].size()}
	};

	this->m_Data["Players"] = nlohmann::ordered_json::object();

	for (auto const & Team : Teams)
	{
		if (Team.size() > 0)
		{
			for (auto const & Player : Team)
			{
				auto Auth = gPugStats.GetAuthId(Player);

				if (Auth)
				{
					if (Auth[0u] != '\0')
					{
						auto Frags = static_cast<int>(Player->edict()->v.frags);
						auto Deaths = Player->m_iDeaths;

						if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
						{
							Frags = gPugMod.GetPoint(Player->entindex(), 0);
							Deaths = gPugMod.GetPoint(Player->entindex(), 1);
						}

						this->m_Data["Players"][std::to_string(Player->entindex())] =
						{
							{"EntityId", Player->entindex()},
							{"Auth", Auth},
							{"Name", STRING(Player->edict()->v.netname)},
							{"UserId", GETPLAYERUSERID(Player->edict())},
							{"Team", Player->m_iTeam},
							{"Frags", Frags},
							{"Deaths", Deaths},
						};
					}
				}
			}   
		}
	}

	if (!this->m_Data.empty())
	{
		gPugCurl.PostJSON(gPugCvar.m_API_Address->string, (long)(gPugCvar.m_API_Timeout->value), gPugCvar.m_API_Bearer->string, this->m_Data.dump(-1, ' ', false, nlohmann::ordered_json::error_handler_t::replace));
	}
}
