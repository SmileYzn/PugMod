#pragma once

class CPugRestrictItem
{
public:
	bool HasRestrictItem(CBasePlayer *Player, ItemID Item, ItemRestType Type);
};

extern CPugRestrictItem gPugRestrictItem;