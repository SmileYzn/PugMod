#include "precompiled.h"

CPugCvarControl gPugCvarControl;

void CPugCvarControl::ServerActivate()
{
	this->m_Data.clear();

	gPugUtil.ServerCommand("exec %s/cfg/cvars.cfg", gPugUtil.GetPath());
}

void CPugCvarControl::Add(const char *Name, const char *String)
{
	if (Name)
	{
		if (Name[0u] != '\0')
		{
			auto Pointer = CVAR_GET_POINTER(Name);

			if (Pointer)
			{
				std::string Temp = "";

				if (String)
				{
					if (String[0u] != '\0')
					{
						Temp = String;
					}
				}

				this->m_Data[Pointer].push_back(Temp);
			}
		}
	}
}

void CPugCvarControl::Menu(CBasePlayer *Player)
{
	if (gPugAdmin.CheckAccess(Player, ADMIN_CVAR))
	{
		gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_CVAR, _T("Cvar Control"));

		for (auto Item : this->m_Data)
		{
			if (Item.first)
			{
				gPugMenu[Player->entindex()].AddItem((int)(Item.first), false, 0, "%s \\R^y%s", Item.first->name, Item.first->string);
			}
		}

		gPugMenu[Player->entindex()].Show(Player);
	}
}

void CPugCvarControl::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		if (gPugAdmin.CheckAccess(Player, ADMIN_CVAR))
		{
			if (Item.Info)
			{
				auto Pointer = (cvar_t*)(Item.Info);

				if (Pointer)
				{
					if (this->m_Data.find(Pointer) != this->m_Data.end())
					{
						auto begin = this->m_Data[Pointer].begin();

						auto end = this->m_Data[Pointer].end();

						for (auto it = begin; ; ++it)
						{
							if (it < end)
							{
								if (it->compare(Pointer->string) == 0)
								{
									if (++it >= end)
									{
										it = begin;
									}

									g_engfuncs.pfnCvar_DirectSet(Pointer, it->c_str());
									break;
								}
							}
							else
							{
								g_engfuncs.pfnCvar_DirectSet(Pointer, begin->c_str());
								break;
							}
						}
					}

					gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 %s set to: ^3%s^1"), Plugin_info.logtag, Pointer->name, Pointer->string);
				}
			}

			this->Menu(Player);   
		}
	}
}
