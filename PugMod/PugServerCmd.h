#pragma once

typedef struct S_SERVER_CMD
{
	const char* Name;
	void (*Function) (void);
} P_SERVER_CMD, *LP_SERVER_CMD;

class CPugServerCmd
{
public:
	void ServerActivate();

	static void AddAdmin();
	static void AddClientCmd();
	static void AddSpawn();
	static void AddWeapon();
	static void AddCvar();
	static void AddMap();
	static void Status();

private:
	const std::vector<P_SERVER_CMD> m_CommandList = 
	{{
		{"pug_add_admin", this->AddAdmin},
		{"pug_add_clcmd", this->AddClientCmd},
		{"pug_add_spawn", this->AddSpawn},
		{"pug_add_weapon", this->AddWeapon},
		{"pug_add_cvar", this->AddCvar},
		{"pug_add_map", this->AddMap},
		{"pug_status", this->Status}
	}};
};

extern CPugServerCmd gPugServerCmd;