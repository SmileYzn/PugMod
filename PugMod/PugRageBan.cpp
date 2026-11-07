#include "precompiled.h"

CPugRageBan gPugRageBan;

void CPugRageBan::ServerActivate()
{
	// Tasks are cleared on map change: resume the check if someone is still pending
	if (!this->m_Data.empty())
	{
		gPugTask.Create(E_TASK::RAGE_BAN_CHECK, 1.0f, true, 0);
	}
}

bool CPugRageBan::IsGenericAuthId(const char *AuthId)
{
	// IDs shared by many players: banning them would ban everyone with the same ID
	return (strstr(AuthId, "_ID_LAN") || strstr(AuthId, "_ID_PENDING") || strstr(AuthId, "UNKNOWN") || _stricmp(AuthId, "HLTV") == 0 || _stricmp(AuthId, "BOT") == 0);
}

std::string CPugRageBan::GetAddress(IGameClient *client)
{
	if (client)
	{
		auto NetChan = client->GetNetChan();

		if (NetChan)
		{
			auto Address = NetChan->GetRemoteAdr();

			if (Address && Address->type == NA_IP)
			{
				char Buffer[16] = {};

				snprintf(Buffer, sizeof(Buffer), "%d.%d.%d.%d", Address->ip[0], Address->ip[1], Address->ip[2], Address->ip[3]);

				return std::string(Buffer);
			}
		}
	}

	return std::string();
}

void CPugRageBan::DropClient(edict_t *pEntity, IGameClient *client)
{
	if (gPugCvar.m_RageQuitLimit->value > 0.0f)
	{
		auto State = gPugMod.GetState();

		if (State >= STATE_KNIFE_ROUND && State <= STATE_OVERTIME)
		{
			auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

			if (Player)
			{
				if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
				{
					if (!Player->IsBot())
					{
						auto AuthId = GETPLAYERAUTHID(pEntity);

						if (AuthId && AuthId[0u] != '\0')
						{
							P_RAGE_BAN_INFO Info = {};

							Info.Name = STRING(pEntity->v.netname);

							Info.Auth = AuthId;

							Info.Address = this->GetAddress(client);

							Info.GenericAuth = this->IsGenericAuthId(AuthId);

							Info.CheckTime = time(nullptr) + static_cast<time_t>(gPugCvar.m_RageQuitLimit->value);

							// With a generic SteamID, only the IP address identifies the player
							auto Key = Info.GenericAuth ? Info.Address : Info.Auth;

							if (!Key.empty() && this->m_Data.find(Key) == this->m_Data.end())
							{
								this->m_Data.insert(std::make_pair(Key, Info));

								gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 left the match and has ^3%d^1 seconds to come back before being banned."), gPugCvar.m_Tag->string, Info.Name.c_str(), static_cast<int>(gPugCvar.m_RageQuitLimit->value));

								gPugTask.Create(E_TASK::RAGE_BAN_CHECK, 1.0f, true, 0);
							}
						}
					}
				}
			}
		}
	}
}

void CPugRageBan::CheckList()
{
	if (gPugCvar.m_RageQuitLimit->value > 0.0f)
	{
		if (gPugCvar.m_RageQuitBanTime->value >= 0.0f)
		{
			if (!this->m_Data.empty())
			{
				// Remove from the list the players who came back (by SteamID, or by IP for generic SteamIDs)
				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						auto AuthId = GETPLAYERAUTHID(Player->edict());

						if (AuthId && AuthId[0u] != '\0')
						{
							if (this->IsGenericAuthId(AuthId))
							{
								auto Address = this->GetAddress(g_RehldsSvs->GetClient(i - 1));

								if (!Address.empty())
								{
									this->m_Data.erase(Address);
								}
							}
							else
							{
								this->m_Data.erase(AuthId);
							}
						}
					}
				}

				auto BanTime = static_cast<int>(gPugCvar.m_RageQuitBanTime->value);

				for (auto it = this->m_Data.begin(); it != this->m_Data.end();)
				{
					if (time(nullptr) >= it->second.CheckTime)
					{
						if (!it->second.GenericAuth)
						{
							gPugUtil.ServerCommand("banid %d %s", BanTime, it->second.Auth.c_str());

							gPugUtil.ServerCommand("wait;wait;writeid");
						}

						if (!it->second.Address.empty())
						{
							gPugUtil.ServerCommand("addip %d %s", BanTime, it->second.Address.c_str());

							gPugUtil.ServerCommand("wait;wait;writeip");
						}

						gPugStats.SetRageQuit(it->second.Auth.c_str());

						gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 was banned for leaving the match and not coming back in ^3%d^1 seconds."), gPugCvar.m_Tag->string, it->second.Name.c_str(), static_cast<int>(gPugCvar.m_RageQuitLimit->value));

						it = this->m_Data.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				gPugTask.Remove(E_TASK::RAGE_BAN_CHECK);
			}
		}
	}
}
