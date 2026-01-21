#include "precompiled.h"

CPugDM gPugDM;

void CPugDM::ServerActivate()
{
    this->m_Run = false;

    this->Load();
}

void CPugDM::ServerDeactivate()
{
    this->m_Run = false;

    this->m_Spawn.clear();

    this->m_Weapon.clear();
}

void CPugDM::Load()
{
    this->m_Spawn.clear();

    this->m_Weapon.clear();

    gPugUtil.ServerCommand("exec %s/spawns/%s.cfg", gPugUtil.GetPath(), STRING(gpGlobals->mapname));
    
    gPugUtil.ServerCommand("exec %s/cfg/weaponlist.cfg", gPugUtil.GetPath());
}

void CPugDM::SetSpawn(P_SPAWN Spawn)
{
    if (!Spawn.Vecs.IsZero())
    {
        this->m_Spawn.insert(std::make_pair(this->m_Spawn.size(), Spawn));
    }
}

void CPugDM::SetWeapon(std::string Name, std::string Label, int Bot)
{
    if (!Name.empty() && !Label.empty())
    {
        if (Name.find("weapon_") == std::string::npos)
        {
            Name = "weapon_" + Name;
        }

        auto SlotInfo = g_ReGameApi->GetWeaponSlot(Name.c_str());

        if (SlotInfo)
        {
            this->m_Weapon.push_back({SlotInfo, Label, Bot});
        }  
    }
}

void CPugDM::Init()
{
    this->m_Run = true;

    if (g_pGameRules)
    {
        CSGameRules()->m_bMapHasBombTarget = false;
        CSGameRules()->m_bMapHasBombZone = false;
        CSGameRules()->m_bMapHasRescueZone = false;
        CSGameRules()->m_bMapHasBuyZone = false;
        CSGameRules()->m_bMapHasEscapeZone = false;
        CSGameRules()->m_bMapHasVIPSafetyZone = false;
    }

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3Deathmatch^1 enabled until match start."), gPugCvar.m_Tag->string);
}

void CPugDM::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        if (g_pGameRules)
        {
            CSGameRules()->CheckMapConditions();
        }
        
        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3Deathmatch^1 disabled."), gPugCvar.m_Tag->string);
    }
}

std::map<int, P_SPAWN> CPugDM::GetSpawns()
{
    return this->m_Spawn;
}

bool CPugDM::GetPlayerSpawnSpot(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
        {
            if (this->m_Spawn.size() > 0)
            {
                do
                {
                    auto Spawn = this->m_Spawn.begin();

                    std::advance(Spawn, RANDOM_LONG(0, this->m_Spawn.size()));

                    if (!Spawn->second.Vecs.IsZero())
                    {
                        if (this->CheckDistance(Player, Spawn->second.Vecs, gPugCvar.m_DM_SpawnDistance->value))
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

                            return true;
                        }
                    }
                } while (true);
            }
            else
            {
                if (gPugCvar.m_MpFreeForAll->value != 0.0f)
                {
                    g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_MpFreeForAll, "0");
                }
            }
        }
    }

    return false;
}

bool CPugDM::CheckDistance(CBasePlayer *Player, vec3_t Origin, float Distance)
{
    edict_t *pEntity = nullptr;

    if (Distance < 250.0f)
    {
        Distance = 250.0f;
    }
    
    while (!FNullEnt(pEntity = FIND_ENTITY_IN_SPHERE(pEntity, Origin, Distance)))
    {
        auto Target = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

        if (Target)
        {
            if (Target->IsPlayer() && Target->IsAlive())
            {
                if (Target->entindex() != Player->entindex())
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void CPugDM::GetIntoGame(CBasePlayer *Player)
{
    auto EntityIndex = Player->entindex();

    if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
    {
        this->m_Info[EntityIndex].Clear();

        this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_FEED] = gPugCvar.m_DM_HideKillFeed->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_HIT_DMG] = gPugCvar.m_DM_HitIndicator->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_HS_MODE] = gPugCvar.m_DM_HSOnlyMode->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_HUD_KD] = gPugCvar.m_DM_HudKDRatio->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_FADE] = gPugCvar.m_DM_KillFade->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_SOUND] = gPugCvar.m_DM_KillSound->value ? true : false;
        this->m_Info[EntityIndex].m_Option[DM_OPT_MONEY_FRAG] = gPugCvar.m_DM_MoneyFrag->value ? true : false;
    }
}

void CPugDM::PlayerSpawn(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        if (Player->edict()->v.frags < 1.0f)
        {
            this->m_Info[Player->entindex()].m_Headshots = 0.0f;
        }
    }
}

void CPugDM::OnSpawnEquip(CBasePlayer *Player, bool addDefault, bool equipGame)
{
    if (this->m_Run)
    {
        this->Menu(Player);
    }
}

bool CPugDM::DropItem(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        auto EntityIndex = Player->entindex();

        if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
        {
            if (this->m_Info[EntityIndex].m_HideMenu)
            {
                this->m_Info[EntityIndex].m_HideMenu = false;

                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The weapons menu has been enabled."), gPugCvar.m_Tag->string);
            }
            else
            {
                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The weapons menu is already enabled."), gPugCvar.m_Tag->string);
            }
            
            if(!Player->m_rgpPlayerItems[PISTOL_SLOT])
            {
                this->MenuEquip(Player, PISTOL_SLOT);
            }
            else if (!Player->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
            {
                this->MenuEquip(Player, PRIMARY_WEAPON_SLOT);
            } 
        }

        return true;
    }

    return false;
}

bool CPugDM::FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity)
{
    if (this->m_Run)
    {
        if (this->m_Info[Entity->entindex()].m_Option[DM_OPT_HS_MODE])
        {
            return (Player->m_LastHitGroup != 1);
        }
    }

    return false;
}

void CPugDM::TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType)
{
    if (this->m_Run)
    {
        auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

        if (Attacker)
        {
            if (!Attacker->IsBot())
            {
                if (Attacker->entindex() != Player->entindex())
                {
                    if (g_pGameRules)
                    {
                        if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker))
                        {
                            if (this->m_Info[Attacker->entindex()].m_Option[DM_OPT_HIT_DMG])
                            {
                                if (Attacker->CSEntity())
                                {
                                    if (Attacker->CSEntity()->m_pContainingEntity)
                                    {
                                        if (Attacker->CSEntity()->m_pContainingEntity->FVisible(Player))
                                        {
                                            gPugUtil.SendDHud(Attacker->edict(), g_DM_HudHits, "%d", static_cast<int>(flDamage));
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
}

bool CPugDM::SendDeathMessage(CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
    if (this->m_Run)
    {
        if (KillerBaseEntity && Victim)
        {
            auto Killer = (CBasePlayer*)(KillerBaseEntity);

            if (Killer->IsPlayer() && Victim->IsPlayer())
            {
                auto KillerIndex = Killer->entindex();
                auto VictimIndex = Victim->entindex();

                auto RestoreHealth = gPugCvar.m_DM_KillHP->value;

                if (Victim->m_bHeadshotKilled)
                {
                    this->m_Info[KillerIndex].m_Headshots += 1.0f;

                    if (gPugCvar.m_DM_KillHPHS->value > 0.0f)
                    {
                        RestoreHealth = gPugCvar.m_DM_KillHPHS->value;
                    }
                }

                if (RestoreHealth > 0.0f)
                {
                    Killer->edict()->v.health = clamp(Killer->edict()->v.health + RestoreHealth, 1.0f, 100.0f);
                }

                if (gPugCvar.m_DM_KillRepairArmor->value)
                {
                    Killer->m_iKevlar = ARMOR_VESTHELM;
                    Killer->edict()->v.armorvalue = MAX_NORMAL_BATTERY;
                }

                if (!(Killer->edict()->v.flags & (FL_DORMANT | FL_FAKECLIENT)))
                {
                    if (Victim->m_bHeadshotKilled && this->m_Info[KillerIndex].m_Option[DM_OPT_KILL_FADE])
                    {
                        gPugUtil.ScreenFade(Killer->edict(), 0.8f, 0.8f, 0x0002, 0, 0, 200, 50);
                    }

                    if (Victim->m_bHeadshotKilled && this->m_Info[KillerIndex].m_Option[DM_OPT_KILL_SOUND])
                    {
                        gPugUtil.ClientCommand(Killer->edict(), g_DM_SoundHeadshot);
                    }

                    if (gPugCvar.m_DM_KillHealedMsg->value > 0.0f)
                    {
                        auto Value = (int)(Killer->edict()->v.health - (float)(Killer->m_iLastClientHealth));

                        if (Value > 0)
                        {
                            gPugUtil.SendDHud(Killer->edict(), g_DM_HudHeal, "%+d HP", Value);
                        }
                    }
                }

                if (this->m_Info[KillerIndex].m_Option[DM_OPT_KILL_FEED])
                {
                    gPugUtil.SendDeathMsg(Killer->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                }

                if (this->m_Info[VictimIndex].m_Option[DM_OPT_KILL_FEED])
                {
                    gPugUtil.SendDeathMsg(Victim->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                }

                for (int i = 1; i <= gpGlobals->maxClients; ++i)
                {
                    auto Player = UTIL_PlayerByIndexSafe(i);

                    if (Player)
                    {
                        if (!this->m_Info[i].m_Option[DM_OPT_KILL_FEED])
                        {
                            if (!Player->IsDormant() && !Player->IsBot())
                            {
                                gPugUtil.SendDeathMsg(Player->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                            }
                        }
                    }
                }

                return true;
            }
        }
    }

    return false;
}

void CPugDM::UpdateClientData(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        if (!Player->IsBot())
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                auto EntityIndex = Player->entindex();

                if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
                {
                    if (this->m_Info[EntityIndex].m_Option[DM_OPT_HUD_KD])
                    {
                        if (gpGlobals->time >= Player->m_iUpdateTime)
                        {
                            gPugUtil.SendDHud
                            (
                                Player->edict(),
                                g_DM_HudInfo,
                                _T("KD: %.2f^nHSP: %.2f%%"),
                                (Player->m_iDeaths > 0) ? (Player->edict()->v.frags / (float)(Player->m_iDeaths)) : (Player->edict()->v.frags > 0.0f ? 100.0f : 0.0f),
                                (Player->edict()->v.frags > 0.0f) ? ((this->m_Info[EntityIndex].m_Headshots / Player->edict()->v.frags) * 100.0f) : 0.0f
                            );

                            Player->m_iUpdateTime = static_cast<int>(gpGlobals->time + 1.0f);
                        }
                    }
                }
            }
        }
    }
}

bool CPugDM::AddAccount(CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange)
{
    if (this->m_Run)
    {
        if (this->m_Info[Player->entindex()].m_Option[DM_OPT_MONEY_FRAG])
        {
            Player->m_iAccount = static_cast<int>(Player->edict()->v.frags);
        }

        return true;
    }

    return false;
}

void CPugDM::SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation)
{
    if (this->m_Run)
    {
        if ((playerAnimation == PLAYER_ATTACK1) || (playerAnimation == PLAYER_ATTACK2) || (playerAnimation == PLAYER_RELOAD))
        {
            if (Player->m_pActiveItem)
            {
                if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
                {
                    CBasePlayerWeapon *Weapon = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

                    if (Weapon)
                    {
                        this->m_Info[Player->entindex()].m_WeaponState[Weapon->m_iId] = Weapon->m_iWeaponState;
                    }
                }
            }
        }
    }
}

void CPugDM::MenuOption(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        if (Player)
        {
            auto EntityIndex = Player->entindex();

            if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
            {
                gPugMenu[EntityIndex].Create(true, E_MENU::DM_OPTIONS, _T("CSDM: Options"));

                gPugMenu[EntityIndex].AddItem(DM_OPT_KILL_FEED, false, 0, _T("Hide Kill Feed^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_FEED] ? "^y1" : "^r0");
                gPugMenu[EntityIndex].AddItem(DM_OPT_HIT_DMG, false, 0, _T("Hit indicator^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_HIT_DMG] ? "^y1" : "^r0");
                gPugMenu[EntityIndex].AddItem(DM_OPT_HS_MODE, false, 0, _T("Headshot mode^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_HS_MODE] ? "^y1" : "^r0");
                gPugMenu[EntityIndex].AddItem(DM_OPT_HUD_KD, false, 0, _T("Show stats^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_HUD_KD] ? "^y1" : "^rN");
                gPugMenu[EntityIndex].AddItem(DM_OPT_KILL_FADE, false, 0, _T("Flash on kill^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_FADE] ? "^y1" : "^r0");
                gPugMenu[EntityIndex].AddItem(DM_OPT_KILL_SOUND, false, 0, _T("Play sound on kill^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_KILL_SOUND] ? "^y1" : "^r0");
                gPugMenu[EntityIndex].AddItem(DM_OPT_MONEY_FRAG, false, 0, _T("Show frags instead of money^R%s"), this->m_Info[EntityIndex].m_Option[DM_OPT_MONEY_FRAG] ? "^y1" : "^r0");

                gPugMenu[EntityIndex].Show(Player);
            }
        }
    }
}

void CPugDM::MenuOptionHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto EntityIndex = Player->entindex();

        if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
        {
            this->m_Info[EntityIndex].m_Option[Item.Info] ^= true;

            this->MenuOption(Player);   
        }
    }
}

bool CPugDM::ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *pszMenuText)
{
    if (this->m_Run)
    {
        if (MenuType == VGUI_Menu_Team)
        {
            if (Player->m_iMenu == Menu_ChooseTeam)
            {
                if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                {
                    this->MenuOption(Player);
                    return true;
                }
            }
        }
    }

    return false;
}

void CPugDM::Menu(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        auto EntityIndex = Player->entindex();

        if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
        {
            if (gPugCvar.m_DM_KillRepairArmor->value > 0.0f)
            {
                Player->edict()->v.armorvalue = MAX_NORMAL_BATTERY;
                Player->m_iKevlar = ARMOR_VESTHELM;
            }

            if (!Player->IsBot())
            {
                if (!this->m_Info[EntityIndex].m_HideMenu)
                {
                    gPugMenu[EntityIndex].Create(false, E_MENU::DM_EQUIP, _T("CSDM: Equipment"));

                    gPugMenu[EntityIndex].AddItem(1, false, 0, _T("New Weapons"));

                    if ((this->m_Info[EntityIndex].m_WeaponLast[PRIMARY_WEAPON_SLOT] != WEAPON_NONE) && (this->m_Info[EntityIndex].m_WeaponLast[PISTOL_SLOT] != WEAPON_NONE))
                    {
                        gPugMenu[EntityIndex].AddItem(2, false, 0, _T("Previous Setup"));

                        gPugMenu[EntityIndex].AddItem(3, false, 0, _T("2 + Hide Menu"));
                    }

                    gPugMenu[EntityIndex].Show(Player);

                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Press ^3'M'^1 to view the training menu."), gPugCvar.m_Tag->string);
                }
                else
                {
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Press ^3'G'^1 to view the weapons menu."), gPugCvar.m_Tag->string);

                    this->EquipItem(Player, this->m_Info[EntityIndex].m_WeaponLast[PRIMARY_WEAPON_SLOT]);
                    this->EquipItem(Player, this->m_Info[EntityIndex].m_WeaponLast[PISTOL_SLOT]);
                }
            }
            else
            {
                gPugDM.EquipRandomItem(Player, 2);
                gPugDM.EquipRandomItem(Player, 1);
            }
        }
    }
}

void CPugDM::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto EntityIndex = Player->entindex();

        if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
        {
            if (Item.Info == 1)
            {
                this->MenuEquip(Player, PISTOL_SLOT);
                return;
            }
            else
            {
                if (Item.Info == 3)
                {
                    this->m_Info[EntityIndex].m_HideMenu = true;

                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Press ^3'G'^1 to show the menu again."), gPugCvar.m_Tag->string);
                }

                this->EquipItem(Player, gPugDM.m_Info[EntityIndex].m_WeaponLast[PRIMARY_WEAPON_SLOT]);
                this->EquipItem(Player, gPugDM.m_Info[EntityIndex].m_WeaponLast[PISTOL_SLOT]);

                return;
            }

            gPugMenu[EntityIndex].Show(Player);
        }
    }
}

void CPugDM::MenuEquip(CBasePlayer *Player, int Slot)
{
    if (this->m_Run)
    {
        if (Player)
        {
            if (this->m_Weapon.size() > 0)
            {
                auto EntityIndex = Player->entindex();

                if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
                {
                    gPugMenu[EntityIndex].Create(false, E_MENU::DM_WEAPONS, _T("CSDM: Weapons"));

                    for (auto const &Weapon : this->m_Weapon)
                    {
                        if (Weapon.SlotInfo)
                        {
                            if (Weapon.SlotInfo->slot == Slot)
                            {
                                gPugMenu[EntityIndex].AddItem(Weapon.SlotInfo->id, false, Weapon.SlotInfo->slot, Weapon.Label.c_str());
                            }
                        }
                    }

                    gPugMenu[EntityIndex].Show(Player);
                }
            }
        }
    }
}

void CPugDM::MenuEquipHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        this->EquipItem(Player, static_cast<WeaponIdType>(Item.Info));

        if (Item.Extra == 2)
        {
            this->MenuEquip(Player, PRIMARY_WEAPON_SLOT);
        }
    }
}

void CPugDM::EquipItem(CBasePlayer *Player, WeaponIdType WeaponIndex)
{
    if (this->m_Run)
    {
        if (Player)
        {
            if (g_ReGameApi)
            {
                auto SlotInfo = g_ReGameApi->GetWeaponSlot(WeaponIndex);

                if (SlotInfo)
                {
                    auto AutoWepSwitch = Player->m_iAutoWepSwitch;

                    Player->m_iAutoWepSwitch = 1;

                    auto NamedItem = static_cast<CBasePlayerItem *>(Player->CSPlayer()->GiveNamedItemEx(SlotInfo->weaponName));

                    if (NamedItem)
                    {
                        Player->GiveAmmo(NamedItem->CSPlayerItem()->m_ItemInfo.iMaxAmmo1, const_cast<char *>(NamedItem->CSPlayerItem()->m_ItemInfo.pszAmmo1), -1);

                        if (Player->m_pActiveItem)
                        {
                            this->m_Info[Player->entindex()].m_WeaponLast[SlotInfo->slot] = WeaponIndex;

                            if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
                            {
                                auto ActiveItem = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

                                if (ActiveItem)
                                {
                                    ActiveItem->m_iWeaponState = this->m_Info[Player->entindex()].m_WeaponState[ActiveItem->m_iId];
                                }
                            }
                        }
                    }

                    Player->m_iAutoWepSwitch = AutoWepSwitch;
                }
            }
        }
    }
}

bool CPugDM::EquipRandomItem(CBasePlayer *Player, int Slot)
{
    if (this->m_Run)
    {
        if (Player)
        {
            if (this->m_Weapon.size() > 0)
            {
                auto Weapon = std::next(std::begin(this->m_Weapon), RANDOM_LONG(0, this->m_Weapon.size() - 1));

                if (Weapon->SlotInfo)
                {
                    if (Player->IsBot() && !Weapon->Bot)
                    {
                        this->EquipRandomItem(Player, Slot);
                        return false;
                    }

                    if (Weapon->SlotInfo->slot != Slot)
                    {
                        this->EquipRandomItem(Player, Slot);
                        return false;
                    }

                    this->EquipItem(Player, Weapon->SlotInfo->id);
                    return true;
                }
            }
        }
    }

    return false;
}

bool CPugDM::Respawn(CBasePlayer *Player)
{
    if (Player)
    {
        if (this->m_Run)
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                if (!Player->IsAlive() || this->IsEntityStuck(Player->edict()))
                {
                    Player->RoundRespawn();

                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 You have revived."), gPugCvar.m_Tag->string);

                    return true;
                }
            }
        }

        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);
    }

    return false;
}

bool CPugDM::ResetScore(CBasePlayer *Player)
{
    if (Player)
    {
        if (this->m_Run)
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                if (Player->edict()->v.frags > 10.0f)
                {
                    this->m_Info[Player->entindex()].m_Headshots = 0.0f;

                    Player->CSPlayer()->Reset();

                    Player->RoundRespawn();

                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 reset the score."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname));

                    return true;
                }
                else
                {
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 There are no frags to reset."), gPugCvar.m_Tag->string);
                }

                return false;
            }
        }

        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);
    }

    return false;
}

bool CPugDM::IsEntityStuck(edict_t *pEntity)
{
    if (!FNullEnt(pEntity))
    {
        TraceResult trResult = {};

        TRACE_HULL(pEntity->v.origin, pEntity->v.origin, dont_ignore_monsters, (pEntity->v.flags & FL_DUCKING) ? 3 : 1, pEntity, &trResult);

        return (trResult.fStartSolid || trResult.fAllSolid || !trResult.fInOpen);
    }

    return false;
}
