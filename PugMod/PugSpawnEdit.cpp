#include "precompiled.h"

CPugSpawnEdit gPugSpawnEdit;

void CPugSpawnEdit::ServerActivate()
{
	this->m_Spawns.clear();

	this->m_Entities.clear();

	this->m_Marked.clear();

	this->m_InfoTarget = ALLOC_STRING("info_target");
}

void CPugSpawnEdit::ServerDeactivate()
{
	this->m_Spawns.clear();

	this->m_Entities.clear();

	this->m_Marked.clear();
}

void CPugSpawnEdit::EditSpawns(CBasePlayer* Player)
{
	if (gPugAdmin.CheckAccess(Player, ADMIN_LEVEL_D))
	{
		if (gPugMod.GetState() == STATE_DEATHMATCH)
		{
			if (this->m_Spawns.empty())
			{
				this->m_Spawns = gPugDM.GetSpawns();
			}
			
			this->MakeEntity(-1);

			this->Menu(Player, 0);
		}
		else
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);
		}
	}
}

void CPugSpawnEdit::Menu(CBasePlayer* Player, int Page)
{
	gPugMenu[Player->entindex()].Create(true, E_MENU::DM_SPAWN_EDIT, _T("CSDM: Spawn Manager"));

	gPugMenu[Player->entindex()].AddItem(1, false, 0, _T("Add Spawn at current position"));

	if (this->m_Spawns.empty())
	{
		gPugMenu[Player->entindex()].AddItem(2, true, 0, _T("Edit Spawn - No Spawn"));
		gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Delete Spawn - No Spawn"));
	}
	else if (FNullEnt(this->m_Entities[this->m_Marked[Player->entindex()]]))
	{
		gPugMenu[Player->entindex()].AddItem(2, true, 0, _T("Edit Spawn - No Spawn Selected"));
		gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Delete Spawn - No Spawn Selected"));
	}
	else
	{
		gPugMenu[Player->entindex()].AddItem(2, false, 0, _T("Move nearest Spawn (^yYellow^w) to current position"));

		if ((Player->edict()->v.origin - this->m_Entities[this->m_Marked[Player->entindex()]]->v.origin).Length() > 200.0f)
		{
			gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Delete Spawn - Selected Spawn too far"));
		}
		else
		{
			gPugMenu[Player->entindex()].AddItem(3, false, 0, _T("Delete nearest Spawn"));
		}
	}

	gPugMenu[Player->entindex()].AddItem(4, false, 0, _T("Select nearest Spawn"));

	gPugMenu[Player->entindex()].AddItem(5, false, 0, _T("Show Statistics"));

	if (Player->edict()->v.movetype == MOVETYPE_WALK)
	{
		gPugMenu[Player->entindex()].AddItem(6, false, 0, _T("Enable No Clip"));
	}
	else if(Player->edict()->v.movetype == MOVETYPE_NOCLIP)
	{
		gPugMenu[Player->entindex()].AddItem(6, false, 0, _T("Disable No Clip"));
	}

	gPugMenu[Player->entindex()].AddItem(7, false, 0, _T("Discard all changes, and exit"));

	gPugMenu[Player->entindex()].AddItem(8, false, 0, _T("Save all changes"));

	gPugMenu[Player->entindex()].AddItem(9, false, 0, _T("Show Stucked Spawns"));

	gPugMenu[Player->entindex()].Display(Player, Page);
}

void CPugSpawnEdit::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		if (!Item.Disabled)
		{
			switch (Item.Info)
			{
				case 1:
				{
					this->AddSpawnMenu(Player);
					break;
				}
				case 2:
				{
					this->EditSpawn(Player);
					this->Menu(Player, 0);
					break;
				}
				case 3:
				{
					this->DeleteSpawn(Player->entindex());
					this->Menu(Player, 0);
					break;
				}
				case 4:
				{
					this->Refresh(Player);
					this->Menu(Player, 0);
					break;
				}
				case 5:
				{
					this->ShowStats(Player);
					this->Menu(Player, 0);
					break;
				}
				case 6:
				{
					this->ToggleNoClip(Player);
					this->Menu(Player, 0);
					break;
				}
				case 7:
				{
					this->DiscardChanges(Player);
					break;
				}
				case 8:
				{
					this->Save(Player);
					this->Menu(Player, 1);
					break;
				}
				case 9:
				{
					this->ShowStuckedSpawns(Player);
					this->Menu(Player, 1);
					break;
				}
			}
		}
	}
}

void CPugSpawnEdit::AddSpawnMenu(CBasePlayer* Player)
{
	gPugMenu[Player->entindex()].Create(false, E_MENU::DM_SPAWN_EDIT_ADD_SPAWN, _T("CSDM: Add Spawn Menu"));

	gPugMenu[Player->entindex()].AddItem(0, false, 0, _T("Add current position as a Random Spawn"));
	gPugMenu[Player->entindex()].AddItem(1, false, 1, _T("Add current position as a TR Spawn"));
	gPugMenu[Player->entindex()].AddItem(2, false, 2, _T("Add current position as a CT Spawn"));
	gPugMenu[Player->entindex()].AddItem(3, false, 3, _T("Cancel"));

	gPugMenu[Player->entindex()].Show(Player);
}

void CPugSpawnEdit::AddSpawnMenuHandle(CBasePlayer* Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		switch (Item.Info)
		{
			case 3:
			{
				this->Menu(Player, 0);
				break;
			}
			default:
			{
				this->AddSpawn(Player, Item.Info);
				this->AddSpawnMenu(Player);
				break;
			}
		}
	}
}

void CPugSpawnEdit::ToggleNoClip(CBasePlayer* Player)
{
	if (Player->edict()->v.movetype == MOVETYPE_WALK)
	{
		Player->edict()->v.movetype = MOVETYPE_NOCLIP;

		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::BLUE, _T("^4[%s]^1 ^3No-Clip Enabled."), gPugCvar.m_Tag->string);
	}
	else if (Player->edict()->v.movetype == MOVETYPE_NOCLIP)
	{
		Player->edict()->v.movetype = MOVETYPE_WALK;

		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3No-Clip Disabled."), gPugCvar.m_Tag->string);
	}
}

void CPugSpawnEdit::DiscardChanges(CBasePlayer* Player)
{
	gPugMenu[Player->entindex()].Create(false, E_MENU::DM_SPAWN_EDIT_DISCARD, _T("Discard all changes and exit the editor?"));

	gPugMenu[Player->entindex()].AddItem(1, false, 0, _T("No, continue editing"));

	gPugMenu[Player->entindex()].AddItem(2, false, 0, _T("Yes, discard all and exit the editor"));

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3Discard all changes and exit the editor?"), gPugCvar.m_Tag->string);

	gPugMenu[Player->entindex()].Show(Player);
}

void CPugSpawnEdit::DiscardChangesHandle(CBasePlayer* Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		switch (Item.Info)
		{
			case 1:
			{
				this->Menu(Player, 0);
				break;
			}
			case 2:
			{
				this->RemoveEntity(-1);

				Player->edict()->v.movetype = MOVETYPE_WALK;

                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 All changes have been discarded."), gPugCvar.m_Tag->string);

				auto pCommand = gPugClientCmd.Get(CMD_DM_SPAWN_EDITOR);

				if (pCommand)
				{
					if (!pCommand->Name.empty())
					{
						gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Use the command ^3%s^1 to open the editor again."), gPugCvar.m_Tag->string, pCommand->Name.c_str());
					}
				}
                
                break;
			}
		}
	}
}

edict_t* CPugSpawnEdit::MakeEntity(int EntityIndex)
{
	edict_t* pEntity = nullptr;

	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			pEntity = CREATE_NAMED_ENTITY(this->m_InfoTarget);

			if (!FNullEnt(pEntity))
			{
				pEntity->v.classname = ALLOC_STRING("view_spawn");

				switch (Spawn.second.Team)
				{
					case 0:
					{
						SET_MODEL(pEntity, "models/player/vip/vip.mdl");
						break;
					}
					case 1:
					{
						SET_MODEL(pEntity, "models/player/terror/terror.mdl");
						break;
					}
					case 2:
					{
						SET_MODEL(pEntity, "models/player/urban/urban.mdl");
						break;
					}
				}

				pEntity->v.solid = SOLID_SLIDEBOX;
				pEntity->v.movetype = MOVETYPE_NOCLIP;
				pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
				pEntity->v.sequence = 1;

				if (this->m_Entities.find(Spawn.first) != this->m_Entities.end())
				{
					if (!FNullEnt(this->m_Entities[Spawn.first]))
					{
						REMOVE_ENTITY(this->m_Entities[Spawn.first]);
					}
				}

				this->m_Entities[Spawn.first] = pEntity;

				this->UnGlowEnt(Spawn.first);
			}
		}
	}
	else
	{
		pEntity = CREATE_NAMED_ENTITY(this->m_InfoTarget);

		if (!FNullEnt(pEntity))
		{
			pEntity->v.classname = ALLOC_STRING("view_spawn");

			SET_MODEL(pEntity, "models/player/vip/vip.mdl");

			switch (this->m_Spawns[EntityIndex].Team)
			{
				case 0:
				{
					SET_MODEL(pEntity, "models/player/vip/vip.mdl");
					break;
				}
				case 1:
				{
					SET_MODEL(pEntity, "models/player/terror/terror.mdl");
					break;
				}
				case 2:
				{
					SET_MODEL(pEntity, "models/player/urban/urban.mdl");
					break;
				}
			}

			pEntity->v.solid = SOLID_SLIDEBOX;
			pEntity->v.movetype = MOVETYPE_NOCLIP;
			pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
			pEntity->v.sequence = 1;

			if (this->m_Entities.find(EntityIndex) != this->m_Entities.end())
			{
				if (!FNullEnt(this->m_Entities[EntityIndex]))
				{
					REMOVE_ENTITY(this->m_Entities[EntityIndex]);
				}
			}

			this->m_Entities[EntityIndex] = pEntity;

			this->UnGlowEnt(EntityIndex);
		}
	}

	return pEntity;
}

void CPugSpawnEdit::RemoveEntity(int EntityIndex)
{
	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if (!FNullEnt(this->m_Entities[Spawn.first]))
			{
				REMOVE_ENTITY(this->m_Entities[Spawn.first]);
			}
		}

		this->m_Spawns.clear();

		this->m_Entities.clear();

		this->m_Marked.clear();
	}
	else
	{
		if (!FNullEnt(this->m_Entities[EntityIndex]))
		{
			REMOVE_ENTITY(this->m_Entities[EntityIndex]);
		}

		this->m_Spawns.erase(EntityIndex);

		this->m_Entities[EntityIndex] = nullptr;

		this->m_Marked[EntityIndex] = -1;
	}
}

void CPugSpawnEdit::GlowEnt(int EntityIndex, vec3_t Color)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxGlowShell;

		pEntity->v.renderamt = 127.0f;

		pEntity->v.rendermode = kRenderTransAlpha;

		pEntity->v.rendercolor = Color;
	}
}

void CPugSpawnEdit::UnGlowEnt(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxNone;

		pEntity->v.renderamt = 255.0f;

		pEntity->v.rendermode = kRenderTransAlpha;
	}
}

bool CPugSpawnEdit::IsStuck(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

    if (!FNullEnt(pEntity))
    {
        TraceResult trResult = {};

        TRACE_HULL(pEntity->v.origin, pEntity->v.origin, dont_ignore_monsters, (pEntity->v.flags & FL_DUCKING) ? 3 : 1, pEntity, &trResult);

        return (trResult.fStartSolid || trResult.fAllSolid || !trResult.fInOpen);
    }

    return false;
}

void CPugSpawnEdit::SetPosition(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		pEntity->v.origin = this->m_Spawns[EntityIndex].Vecs;

		pEntity->v.angles = this->m_Spawns[EntityIndex].Angles;

		pEntity->v.v_angle = this->m_Spawns[EntityIndex].VAngles;

		pEntity->v.fixangle = 1;
	}
}

void CPugSpawnEdit::AddSpawn(CBasePlayer* Player, int Team)
{
	P_SPAWN Info = {};

	Info.Vecs = Player->edict()->v.origin;

	Info.Vecs.z += 15.0f; // test

	Info.Angles = Player->edict()->v.angles;

	Info.Team = Team;

	Info.VAngles = Player->edict()->v.v_angle;

	size_t Index = this->m_Spawns.size() + 1;

	this->m_Spawns[Index] = Info;

	auto pEntity = this->MakeEntity(Index);

	if (!FNullEnt(pEntity))
	{
		this->Refresh(Player);
	}
}

void CPugSpawnEdit::EditSpawn(CBasePlayer *Player)
{
	auto EntityIndex = Player->entindex();

	auto pEntity = this->m_Marked[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if ((this->m_Spawns[EntityIndex].Vecs[0] == Spawn.second.Vecs[0]) && (this->m_Spawns[EntityIndex].Vecs[1] == Spawn.second.Vecs[1]) && ((this->m_Spawns[EntityIndex].Vecs[2] - Spawn.second.Vecs[2]) <= 15.0f))
			{
				this->RemoveEntity(this->m_Marked[EntityIndex]);

				this->m_Spawns[this->m_Marked[EntityIndex]].Vecs = Player->edict()->v.origin;

				this->m_Spawns[this->m_Marked[EntityIndex]].Angles = Player->edict()->v.angles;

				this->m_Spawns[this->m_Marked[EntityIndex]].Team = 0;

				this->m_Spawns[this->m_Marked[EntityIndex]].VAngles = Player->edict()->v.v_angle;

				this->MakeEntity(this->m_Marked[EntityIndex]);

				this->GlowEnt(this->m_Marked[EntityIndex], Vector(255.0f, 0.0f, 0.0f));

				break;
			}
		}
	}
}

void CPugSpawnEdit::DeleteSpawn(int EntityIndex)
{
	this->UnGlowEnt(this->m_Marked[EntityIndex]);

	this->RemoveEntity(this->m_Marked[EntityIndex]);

	auto it = this->m_Spawns.find(this->m_Marked[EntityIndex]);

	if (it != this->m_Spawns.end())
	{
		this->m_Spawns.erase(it);
	}

	this->MakeEntity(-1);

	this->m_Marked[EntityIndex] = -1;
}

int CPugSpawnEdit::ClosestSpawn(edict_t* pEdict)
{
	auto Result = 0;

	if (!FNullEnt(pEdict))
	{
		auto LastDistance = 9999.0f;

		for (auto const& Spawn : this->m_Spawns)
		{
			auto Distance = (pEdict->v.origin - Spawn.second.Vecs).Length();

			if (Distance < LastDistance)
			{
				LastDistance = Distance;

				Result = Spawn.first;
			}
		}
	}

	return Result;
}

void CPugSpawnEdit::Refresh(CBasePlayer* Player)
{
	this->UnGlowEnt(this->m_Marked[Player->entindex()]);

	this->m_Marked[Player->entindex()] = this->ClosestSpawn(Player->edict());

	this->GlowEnt(this->m_Marked[Player->entindex()], Vector(255.0f, 200.0f, 20.0f));

	auto Spawn = this->m_Spawns.find(this->m_Marked[Player->entindex()]);

	if (Spawn != this->m_Spawns.end())
	{
		gPugUtil.PrintColor
		(
			Player->edict(),
			E_PRINT_TEAM::DEFAULT,
			_T("^4[%s]^1 Nearest spawn: Number ^3%d^1 -> Team ^3%s^1"),
			gPugCvar.m_Tag->string,
			Spawn->first,
			(Spawn->second.Team == UNASSIGNED) ? "ANY" : (Spawn->second.Team == CT ? "CT" : "TR")
		);

		gPugUtil.PrintColor
		(
			Player->edict(),
			E_PRINT_TEAM::DEFAULT,
			_T("^4[%s]^1 Origin [%.0f %.0f %.0f] Angle [%.0f %.0f %.0f] VAngle [%.0f %.0f %.0f]"),
			gPugCvar.m_Tag->string,
			Spawn->second.Vecs[0],
			Spawn->second.Vecs[1],
			Spawn->second.Vecs[2],
			Spawn->second.Angles[0],
			Spawn->second.Angles[1],
			Spawn->second.Angles[2],
			Spawn->second.VAngles[0],
			Spawn->second.VAngles[1],
			Spawn->second.VAngles[2]
		);

		if (this->IsStuck(this->m_Marked[Player->entindex()]))
		{
			gPugUtil.PrintColor
			(
				Player->edict(),
				E_PRINT_TEAM::RED,
				_T("^4[%s]^1 This spawn (Number ^4%d^1) (Team ^4%s^1) may be stuck: ^3Please fix it."),
				gPugCvar.m_Tag->string,
				Spawn->first,
				(Spawn->second.Team == UNASSIGNED) ? "ANY" : (Spawn->second.Team == CT ? "CT" : "TR")
			);
		}
	}
}

void CPugSpawnEdit::ShowStats(CBasePlayer* Player)
{
	auto TotalSpawns = 0;
	auto TotalRandom = 0;
	auto TotalTR = 0;
	auto TotalCT = 0;

	if (!this->m_Spawns.empty())
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			TotalSpawns++;

			switch (Spawn.second.Team)
			{
				case UNASSIGNED:
				{
					TotalRandom++;
					break;
				}
				case TERRORIST:
				{
					TotalTR++;
					break;
				}
				case CT:
				{
					TotalCT++;
					break;
				}
			}
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Spawns: %d; Random: %d; TR: %d; CT: %d"), gPugCvar.m_Tag->string, TotalSpawns, TotalRandom, TotalTR, TotalCT);
	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Current Position: X: %.3f Y: %.3f Z: %.3f"), gPugCvar.m_Tag->string, Player->edict()->v.origin.x, Player->edict()->v.origin.y, Player->edict()->v.origin.z);
}

void CPugSpawnEdit::Save(CBasePlayer* Player)
{
	if (!this->m_Spawns.empty())
	{
		char Path[MAX_PATH] = { 0 };

		snprintf(Path, sizeof(Path), "%s/%s/spawns/%s.cfg", gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR), gPugUtil.GetPath(), STRING(gpGlobals->mapname));

		std::ofstream File(Path);

		if (File.is_open())
		{
			for (auto const& Spawn : this->m_Spawns)
			{
				File << "pug_add_spawn\t";
				File << static_cast<int>(Spawn.second.Vecs[0]) << "\t" << static_cast<int>(Spawn.second.Vecs[1]) << "\t" << static_cast<int>(Spawn.second.Vecs[2]) << "\t";
				File << static_cast<int>(Spawn.second.Angles[0]) << "\t" << static_cast<int>(Spawn.second.Angles[1]) << "\t" << static_cast<int>(Spawn.second.Angles[2]) << "\t";
				File << Spawn.second.Team << "\t";
				File << static_cast<int>(Spawn.second.VAngles[0]) << "\t" << static_cast<int>(Spawn.second.VAngles[1]) << "\t" << static_cast<int>(Spawn.second.VAngles[2]);
				File << "\n";
			}

			gPugDM.Load();
			
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%u^1 Spawns saved to file: ^3%s.cfg"), gPugCvar.m_Tag->string, this->m_Spawns.size(), STRING(gpGlobals->mapname));
		}
		else
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Failed to open: ^3%s.cfg"), gPugCvar.m_Tag->string, STRING(gpGlobals->mapname));
		}

		File.close();
	}
	else
	{
		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 Failed to save file: ^3No Spawn in the list."), gPugCvar.m_Tag->string);
	}	
}

void CPugSpawnEdit::ShowStuckedSpawns(CBasePlayer* Player)
{
	if (!this->m_Entities.empty())
	{
		auto StuckedSpawns = false;

		for (auto const & Entity : this->m_Entities)
		{
			auto Spawn = this->m_Spawns.find(Entity.first);

			if (Spawn != this->m_Spawns.end())
			{
				if (this->IsStuck(Entity.first))
				{
					if (!StuckedSpawns)
					{
						this->RespawnPlayer(Player, Entity.first);
					}

					StuckedSpawns = true;

					this->GlowEnt(Entity.first, Vector(255.0f, 0.0f, 255.0f));

					gPugUtil.PrintColor
					(
						Player->edict(),
						E_PRINT_TEAM::RED,
						_T("^4[%s]^1 This spawn (Number ^4%d^1) (Team ^4%s^1) may be stuck: ^3Please fix it."),
						gPugCvar.m_Tag->string,
						Entity.first,
						(Spawn->second.Team == UNASSIGNED) ? "ANY" : (Spawn->second.Team == CT ? "CT" : "TR")
					);
				}
				else
				{
					this->UnGlowEnt(Entity.first);
				}
			}
		}

		if (!StuckedSpawns)
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^4Good Job! No trapped Spawn were found!"), gPugCvar.m_Tag->string);
		}
	}
	else
	{
		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3No Spawn in the list."), gPugCvar.m_Tag->string);
	}
}

void CPugSpawnEdit::RespawnPlayer(CBasePlayer *Player, int SpawnIndex)
{
	auto Spawn = this->m_Spawns.find(SpawnIndex);

	if (Spawn != this->m_Spawns.end())
	{
		if (!Spawn->second.Vecs.IsZero())
		{
			Player->edict()->v.origin = Spawn->second.Vecs + Vector(0.0f, 0.0f, 1.0f);

			if (!Spawn->second.Angles.IsZero())
			{
				Player->edict()->v.angles = Spawn->second.Angles;
			}

			if (!Spawn->second.VAngles.IsZero())
			{
				Player->edict()->v.v_angle = Spawn->second.VAngles;

				Player->edict()->v.v_angle.z = 0;

				Player->edict()->v.angles = Player->edict()->v.v_angle;
			}

			Player->edict()->v.velocity = Vector(0.0f, 0.0f, 0.0f);

			Player->edict()->v.punchangle = Vector(0.0f, 0.0f, 0.0f);

			Player->edict()->v.fixangle = 1;

			Player->m_bloodColor = BLOOD_COLOR_RED;

			Player->m_modelIndexPlayer = Player->edict()->v.modelindex;

			if (Player->edict()->v.flags & FL_DUCKING)
			{
				SET_SIZE(Player->edict(), VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			}
			else
			{
				SET_SIZE(Player->edict(), VEC_HULL_MIN, VEC_HULL_MAX);
			}
		}
	}
}