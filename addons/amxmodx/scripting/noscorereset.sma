#include <amxmodx>
#include <fakemeta> 
#include <hamsandwich>

#define PLUGIN "No Score Reset on sv_restart 1"
#define VERSION "1.0"
#define AUTHOR "EFFx"

enum dData
{
    iFrags,
    iDeaths
}

new g_iUserData[33][dData], bool:g_bCanSet[33]

const m_iDeaths =        444

public plugin_init() 
{
    register_plugin(PLUGIN, VERSION, AUTHOR)
    
    RegisterHam(Ham_Spawn, "player", "ham_PlayerSpawn_Post", 1)
    
    register_event("TextMsg","EventRestart","a","2&#Game_w")
}

public ham_PlayerSpawn_Post(id)
{
    if(g_bCanSet[id] && is_user_alive(id))
    {
        updateScore(id)
    }
}

public EventRestart()
{
    new iPlayers[32], iNum
    get_players(iPlayers, iNum)
    if(!iNum)
        return

    for(new i, id;i < iNum;i++)
    {
        id = iPlayers[i]
        
        g_bCanSet[id] = true

        g_iUserData[id][iFrags] = get_user_frags(id)
        g_iUserData[id][iDeaths] = get_user_deaths(id)
    }
}  

updateScore(id)
{
    g_bCanSet[id] = false

    set_pdata_int(id, m_iDeaths, g_iUserData[id][iDeaths]) 
    ExecuteHam(Ham_AddPoints, id, g_iUserData[id][iFrags], true)
} 