#include "precompiled.h"

CPugRestrictItem gPugRestrictItem;

bool CPugRestrictItem::HasRestrictItem(CBasePlayer* Player, ItemID Item, ItemRestType Type)
{
	auto State = gPugMod.GetState();

	if (State != STATE_DEAD)
	{
		if (Item >= ITEM_SHIELDGUN && Item <= ITEM_BATTERY)
		{
			if (State == STATE_KNIFE_ROUND)
			{
				if ((Item != ITEM_KEVLAR) && (Item != ITEM_ASSAULT) && (Item != ITEM_KNIFE))
				{
					gPugUtil.ClientPrint(Player->edict(), E_PRINT::CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
					return true;
				}
			}
            
			if (gPugCvar.m_RestrictItem->string)
			{
				if (gPugCvar.m_RestrictItem->string[Item] != '0')
				{
					gPugUtil.ClientPrint(Player->edict(), E_PRINT::CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
					return true;
				}
			}
		}
	}

	return false;
}