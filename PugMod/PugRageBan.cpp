#include "precompiled.h"

CPugRageBan gPugRageBan;

void CPugRageBan::DropClient(edict_t *pEntity)
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

						if (AuthId)
						{
							if (AuthId[0u] != '\0')
							{
								if (this->m_Data.find(AuthId) == this->m_Data.end())
								{
									P_RAGE_BAN_INFO Info = {};

									Info.Name = STRING(pEntity->v.netname);

									Info.CheckTime = (gpGlobals->time + gPugCvar.m_RageQuitLimit->value);

									this->m_Data.insert(std::make_pair(AuthId, Info));

									gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Player ^3%s^1 has been added to the rage quit. He need to back in ^3%d^1 seconds before ban.",  gPugCvar.m_Tag->string, STRING(pEntity->v.netname), static_cast<int>(gPugCvar.m_RageQuitLimit->value));

									gPugTask.Create(E_TASK::RAGE_BAN_CHECK, 1.0f, true, State);
								}
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
			auto Players = gPugUtil.GetPlayers(false, false);

			if (!this->m_Data.empty())
			{
				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						auto AuthId = GETPLAYERAUTHID(Player->edict());

						if (AuthId)
						{
							if (AuthId[0u] != '\0')
							{
								if (this->m_Data.find(AuthId) != this->m_Data.end())
								{
									this->m_Data.erase(AuthId);
								}
							}
						}
					}
				}

				for (auto it = this->m_Data.begin(); it != this->m_Data.end(); )
				{
					if (gpGlobals->time >= it->second.CheckTime)
					{
						gPugUtil.ServerCommand(_T("banid %d %s"), static_cast<int>(gPugCvar.m_RageQuitBanTime->value), it->first.c_str());

						gPugUtil.ServerCommand(_T("wait;wait;writeid;writeip"));

						gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Player ^3%s^1 was banned duo rage quit after ^3%d^1 seconds.",  gPugCvar.m_Tag->string, it->second.Name.c_str(), static_cast<int>(gPugCvar.m_RageQuitLimit->value));

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