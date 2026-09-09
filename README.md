<h3 align="center">Counter-Strike 1.6 Match Plugin</h3>
<p align="center">Counter-Strike 1.6 Match Plugin for ReHLDS and MetaMod</p>

<p align="center">
    <a href=""><img src="https://img.shields.io/github/actions/workflow/status/SmileYzn/pugmod/build.yml?branch=main&label=Build&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/pugmod/issues"><img src="https://img.shields.io/github/issues-raw/smileyzn/pugmod?style=flat-square"></a>
</p>

<h4>Description</h4>
<p>A metamod plugin to run automnated competitive Counter-Strike 1.6 matches</p>

<h4>Requisites</h4>
<ul>
    <li>ReHLDS</li>
    <li>ReGameDLL_CS</li>
    <li>Metamod</li>
</ul> 

<h4>Features</h4>
<ul>
    <li>CS:DM mode that can be enabled while match is not started</li>
    <li>CS:DM mode with options via <b>'M'</b> key</li>
    <li>CS:DM weapons menu <b>'G'</b> key</li>
    <li>CS:DM spawn editor</li>
    <li>Ready System: Player must type <b>.ready</b> on chat to start game</li>
    <li>Automatic Timer: Match starts while all palyers are connected</li>
    <li>Support maxrounds (mr) match with best of 30 rounds</li>
    <li>Infinite Overtime support</li>
    <li>Vote for Overtime play, sudden death round, or end game tied</li>
    <li>Server slots management with spectators and HLTV slots support</li>
    <li>Vote Map</li>
    <li>Players can choose the method of picking teams</li>
    <li>Knife Round supported</li>
    <li>A complete admin menu to manage match and server</li>
    <li>Admin can kick, ban, kill, change map, change team of player, control match</li>
    <li>Player and Admin commands in game</li>
    <li>Commands in server console</li>
    <li>Settings and configurations via console variable cvars</li>
    <li>Weapon restriction control</li>
    <li>Game name changer on server list change</li>
    <li>Ask to record a demo on client side while match is started/running</li>
    <li>Ask to end pug while an player disconnect from game</li>
    <li>Round end stats in screen and commands</li>
    <li>A complete menu for players to: Vote Kick, Vote Map or Vote to stop match</li>
    <li>Translation system support</li>
    <li>Anti Flood system for chat and radio messages</li>
    <li>Automatic ban of players that rage quit from game</li>
    <li>Smoke Bug Fix</li>
    <li>Supports for Windows and Linux Systems</li>
</ul>

<h4>Settings</h4>

<details>
    <summary>Pug Mod Variables</summary>

| Variable | Default | Description |
|:----|:---:|:----|
| pug_tag | PUG | Tag used in logs |
| pug_language | en | Server language |
| pug_cmd_prefix_player | . | Command prefix for players |
| pug_cmd_prefix_admin | ! | Command prefix for admins |
| pug_cmd_flood_time | 0.75 | Time between commands to detect flood |
| pug_cmd_flood_repeat | 3.0 | Max flood attempts before blocking |
| pug_cmd_flood_next_time | 4.0 | Block duration after flood detection |
| pug_players_min | 5 | Minimum players per team |
| pug_players_max | 5 | Maximum players per team |
| pug_rounds | 30 | Total rounds per map |
| pug_rounds_ot | 6 | Total overtime rounds |
| pug_ot_type | 0 | Overtime type (0 Vote / 1 Play / 2 Allow tie / 3 Sudden death) |
| pug_ready_type | 1 | Ready system mode (0 Off / 1 Auto / 2 Manual) |
| pug_ready_list | 0 | Ready list display type (0 Full list / 1 Only numbers) |
| pug_ready_time_limit | 60.0 | Time in seconds before starting once all players are in game |
| pug_team_type | 0 | Team selection mode (Vote, Captains, Shuffle, etc.) |
| pug_team_option | abcdef | Options available in team vote menu |
| pug_restrict_item | 000000000000000000000000000000000000000 | Weapon/item restriction per slot |
| pug_sound_enable | 1 | Enable sounds |
| pug_vote_delay | 15.0 | Vote time limit |
| pug_vote_percent | 0.70 | Vote success percentage requirement |
| pug_knife_round | 0 | Force a knife round |
| pug_vote_end | 1 | Prompt match end when a player disconnects |
| pug_score_text | 0 | Chat score text type |
| pug_score_teams | 1 | Keep total team scores on scoreboard (TAB) |
| pug_score_players | 1 | Keep player stats on scoreboard (TAB) |
| pug_motd_file | cstrike/addons/pugmod/motd.html | Help file for user commands |
| pug_motd_admin_file | cstrike/addons/pugmod/motd_admin.html | Help file for admin commands |
| pug_demo_record | 1 | Ask players to record demo at match start |
| pug_rage_quit_time_limit | 0 | Rage quit detection time (0 = disabled) |
| pug_rage_quit_ban_time | 1440 | Rage quit ban duration in minutes (0 = permanent) |
| pug_round_stats | abcde | Flags enabling stats commands |
| pug_round_end_stats | e | Stats displayed at round end |
| pug_player_vote_kick | 1 | Allow players to use votekick |
| pug_player_vote_map | 1 | Allow players to use votemap |
| pug_player_vote_pause | 1 | Allow players to vote for pause |
| pug_player_vote_restart | 1 | Allow players to vote for restart |
| pug_player_vote_cancel | 1 | Allow players to vote to cancel |
| pug_player_vote_surrender | 1 | Allow players to vote to surrender |
| pug_pause_timeout | 60 | Allowed pause duration |
| pug_smoke_bug_fix | 1 | Fix smoke grenade multiple-cloud bug |
| pug_dm_enable | 1 | Enable Deathmatch mode |
| pug_dm_hide_kill_feed | 0 | Hide kill feed from players |
| pug_dm_hit_indicator | 1 | Hit indicator type (0 Off / 1 Normal / 2 Through walls) |
| pug_dm_hs_mode | 0 | Headshot-only mode |
| pug_dm_hud_kd_ratio | 1 | Show kill/death and HS rate on HUD |
| pug_dm_kill_fade | 1 | Screen flash on kill (HS) |
| pug_dm_kill_hp | 15 | Health recovered per kill |
| pug_dm_kill_hp_hs | 40 | Health recovered per headshot kill |
| pug_dm_kill_repair_armor | 1 | Armor recovery on kill |
| pug_dm_kill_hp_msg | 1 | Show HP recovered on kill |
| pug_dm_kill_sound | 1 | Play sound on kill (HS) |
| pug_dm_money_frag | 1 | Show frags instead of money |
</details>