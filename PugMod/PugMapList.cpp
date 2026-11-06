#include "precompiled.h"

CPugMapList gPugMapList;

void CPugMapList::ServerActivate()
{
	this->m_Data.clear();
}

void CPugMapList::Add(const char *Map)
{
	if (Map)
	{
		if (Map[0u] != '\0')
		{
			if (this->m_Data.size() < MAX_VOTE_MAPS)
			{
				char *File = strdup(Map);

				if (IS_MAP_VALID(File))
				{
					this->m_Data.insert(std::make_pair(this->m_Data.size(), Map));
				}

				free(File);
			}
			else
			{
				LOG_CONSOLE(PLID, "[%s] Map list is full (%d maps), ignoring: %s", Plugin_info.logtag, MAX_VOTE_MAPS, Map);
			}
		}
	}
}

std::map<int, std::string> CPugMapList::Get()
{
	return this->m_Data;
}

void CPugMapList::ChangeMap(int Index)
{
	auto MapList = gPugMapList.Get();

	if (!MapList.empty())
	{
		auto it = MapList.find(Index);

		if (it != MapList.end())
		{
			g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");

			gPugUtil.ServerCommand("changelevel %s", it->second.c_str());
		}
	}
}
