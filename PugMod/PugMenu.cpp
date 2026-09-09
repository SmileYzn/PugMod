#include "precompiled.h"

std::array<CPugMenu, MAX_CLIENTS + 1U> gPugMenu;

void CPugMenu::Clear()
{
	this->m_Text = "";

	this->m_Data.clear();

	this->m_Page = -1;

	this->m_Exit = false;

	this->m_Handle = 0;
}

void CPugMenu::Create(bool Exit, int HandleType, const char *Format, ...)
{
    va_list argList;

    va_start(argList, Format);

    char Buffer[128] = {0};

    vsnprintf(Buffer, sizeof(Buffer), Format, argList);

    va_end(argList);

    if (Buffer[0] != '\0')
    {
        this->m_Text = std::string(Buffer);

        this->m_Data.clear();

        this->m_Page = -1;

        this->m_Exit = Exit;

        this->m_Handle = HandleType;
    }
}

void CPugMenu::AddItem(int Info, bool Disabled, int Extra, const char *Format, ...)
{
    va_list argList;

    va_start(argList, Format);

    char Buffer[128] = {0};

    vsnprintf(Buffer, sizeof(Buffer), Format, argList);

    va_end(argList);

    if (Buffer[0] != '\0')
    {
        P_MENU_ITEM ItemData = {Info, std::string(Buffer), Disabled, Extra};

        this->m_Data.push_back(ItemData);
    }
}

void CPugMenu::Show(CBasePlayer *Player)
{
    if (this->m_Data.size())
    {
        this->Display(Player, 0);
    }
}

void CPugMenu::Hide(CBasePlayer *Player)
{
    this->m_Page = -1;

    if (Player)
    {
        Player->m_iMenu = Menu_OFF;

        if (!Player->IsDormant() && !Player->IsBot())
        {
            static int iMsgShowMenu;

            if (iMsgShowMenu || (iMsgShowMenu = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ShowMenu", NULL)))
            {
                MESSAGE_BEGIN(MSG_ONE, iMsgShowMenu, nullptr, Player->edict());
                WRITE_SHORT(0);
                WRITE_CHAR(0);
                WRITE_BYTE(0);
                WRITE_STRING("");
                MESSAGE_END();
            }
        }
    }
}

bool CPugMenu::Handle(CBasePlayer *Player, int Key)
{
    if (this->m_Page != -1)
    {
        if (Player)
        {
            if (Player->m_iMenu == Menu_OFF)
            {
                if (Key == 9)
                {
                    this->Display(Player, ++this->m_Page);

                    if (gPugCvar.m_SoundEnable->value != 0.0f)
                    {
                        gPugUtil.ClientCommand(Player->edict(), g_Menu_Sound[0]);
                    }
                }
                else if (Key == 10)
                {
                    this->Display(Player, --this->m_Page);

                    if (gPugCvar.m_SoundEnable->value != 0.0f)
                    {
                        gPugUtil.ClientCommand(Player->edict(), g_Menu_Sound[0]);
                    }
                }
                else
                {
                    unsigned int ItemId = (Key + (this->m_Page * this->m_MaxPageOption)) - 1;

                    if (ItemId < this->m_Data.size())
                    {
                        if (this->m_Data[ItemId].Disabled)
                        {
                            if (gPugCvar.m_SoundEnable->value != 0.0f)
                            {
                                gPugUtil.ClientCommand(Player->edict(), g_Menu_Sound[2]);
                            }
                        }
                        else
                        {
                            if (gPugCvar.m_SoundEnable->value != 0.0f)
                            {
                                gPugUtil.ClientCommand(Player->edict(), g_Menu_Sound[1]);
                            }
                        }

                        this->Hide(Player);

                        switch (this->m_Handle)
                        {
                            case E_MENU::DM_OPTIONS:
                            {
                                gPugDM.MenuOptionHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::DM_EQUIP:
                            {
                                gPugDM.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::DM_WEAPONS:
                            {
                                gPugDM.MenuEquipHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::DM_SPAWN_EDIT:
                            {
                                gPugSpawnEdit.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::DM_SPAWN_EDIT_ADD_SPAWN:
                            {
                                gPugSpawnEdit.AddSpawnMenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::DM_SPAWN_EDIT_DISCARD:
                            {
                                gPugSpawnEdit.DiscardChangesHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_MAP:
                            {
                                gPugVoteMap.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_TEAM:
                            {
                                gPugVoteTeam.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_LEADER:
                            {
                                gPugLeader.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_SWAP_TEAM:
                            {
                                gPugVoteSwapTeam.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_OVERTIME:
                            {
                                gPugVoteOvertime.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_VOTE_END:
                            {
                                gPugVoteEnd.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_DEMO_RECORD:
                            {
                                gPugDemoRecord.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU:
                            {
                                gPugAdminMenu.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_KICK:
                            {
                                gPugAdminMenu.KickHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_BAN:
                            {
                                gPugAdminMenu.BanHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_SLAP:
                            {
                                gPugAdminMenu.SlapHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_TEAM:
                            {
                                gPugAdminMenu.TeamHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_MAP:
                            {
                                gPugAdminMenu.MapHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_PUG:
                            {
                                gPugAdminMenu.PugHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_ADMIN_MENU_CVAR:
                            {
                                gPugCvarControl.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_PLAYER_VOTE_MENU:
                            {
                                gPugVoteMenu.MenuHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_PLAYER_VOTE_KICK:
                            {
                                gPugVoteMenu.VoteKickHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                            case E_MENU::ME_PLAYER_VOTE_MAP:
                            {
                                gPugVoteMenu.VoteMapHandle(Player, this->m_Data[ItemId]);
                                break;
                            }
                        }
                    }
                }

                return true;
            }
            else
            {
                this->m_Page = -1;
            }
        }
    }

    return false;
}

void CPugMenu::Display(CBasePlayer *Player, int Page)
{
    if (Page < 0)
    {
        Page = 0;

        this->m_Page = 0;

        if (this->m_Exit)
        {
            this->m_Page = -1;
            return;
        }
    }
    else
    {
        this->m_Page = Page;
    }

    unsigned int Start = (Page * this->m_MaxPageOption);

    if (Start >= this->m_Data.size())
    {
        Start = Page = this->m_Page = 0;
    }

    auto PageCount = (int)this->m_Data.size() > this->m_MaxPageOption ? (this->m_Data.size() / this->m_MaxPageOption + ((this->m_Data.size() % this->m_MaxPageOption) ? 1 : 0)) : 1;

    std::string MenuText = "";

    MenuText = "\\y" + this->m_Text;

    if (PageCount > 1)
    {
        MenuText += "\\R";
        MenuText += std::to_string(Page + 1);
        MenuText += "/";
        MenuText += std::to_string(PageCount);
    }

    MenuText += "\n\\w\n";

    unsigned int End = (Start + this->m_MaxPageOption);

    if (End > this->m_Data.size())
    {
        End = this->m_Data.size();
    }

    int Slots = MENU_KEY_0; // MENU_KEY_0
    int BitSum = 0;

    for (unsigned int b = Start; b < End; b++)
    {
        Slots |= (1 << BitSum);

        MenuText += "\\r";
        MenuText += std::to_string(++BitSum);
        MenuText += this->m_Data[b].Disabled ? ".\\d " : ".\\w ";
        MenuText += this->m_Data[b].Text;
        MenuText += "\n";
    }

    if (End != this->m_Data.size())
    {
        Slots |= MENU_KEY_9; // MENU_KEY_9;

        if (Page)
        {
            MenuText += "\n\\r9.\\w Next\n\\r0.\\w Back";
        }
        else
        {
            MenuText += "\n\\r9.\\w Next";

            if (this->m_Exit)
            {
                MenuText += "\n\\r0.\\w Exit";
            }
        }
    }
    else
    {
        if (Page)
        {
            MenuText += "\n\\r0.\\w Back";
        }
        else
        {
            if (this->m_Exit)
            {
                MenuText += "\n\\r0.\\w Exit";
            }
        }
    }

    this->ShowMenu(Player, Slots, -1, MenuText);
}

void CPugMenu::ReplaceAll(std::string &String, const std::string &From, const std::string &To)
{
    if (!From.empty())
    {
        size_t StartPos = 0;

        while ((StartPos = String.find(From, StartPos)) != std::string::npos)
        {
            String.replace(StartPos, From.length(), To);

            StartPos += To.length();
        }
    }
}

void CPugMenu::ShowMenu(CBasePlayer *Player, int Slots, int Time, std::string Text)
{
    if (Player)
    {
        if (!Player->IsDormant())
        {
            if (!Player->IsBot())
            {
                static int iMsgShowMenu;

                if (iMsgShowMenu || (iMsgShowMenu = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ShowMenu", NULL)))
                {
                    Player->m_iMenu = Menu_OFF;

                    this->ReplaceAll(Text, "^w", "\\w");
                    this->ReplaceAll(Text, "^y", "\\y");
                    this->ReplaceAll(Text, "^r", "\\r");
                    this->ReplaceAll(Text, "^R", "\\R");

                    char BufferMenu[MAX_BUFFER_MENU * 6] = {0};

                    Text.copy(BufferMenu, Text.length() + 1);

                    char *pMenuList = BufferMenu;
                    char *aMenuList = BufferMenu;

                    int iCharCount = 0;

                    while (pMenuList && *pMenuList)
                    {
                        char szChunk[MAX_BUFFER_MENU + 1] = {0};

                        strncpy(szChunk, pMenuList, MAX_BUFFER_MENU);

                        szChunk[MAX_BUFFER_MENU] = 0;

                        iCharCount += strlen(szChunk);

                        pMenuList = aMenuList + iCharCount;

                        MESSAGE_BEGIN(MSG_ONE, iMsgShowMenu, nullptr, Player->edict());
                        WRITE_SHORT(Slots);
                        WRITE_CHAR(Time);
                        WRITE_BYTE(*pMenuList ? TRUE : FALSE);
                        WRITE_STRING(szChunk);
                        MESSAGE_END();
                    }
                }
            }
        }
    }
}
