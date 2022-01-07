<?php

class PlayerStats extends Base
{
    function getPosition($auth = '')
    {
        $position = [];
        
        if(!empty($auth))
        {
            $run = $this->query("SELECT auth, @rws := ((SUM(roundWinShare) / (SELECT rounds FROM game WHERE id = playerStats.gameFK)) * 100.0) rws, RANK() OVER (ORDER BY rws DESC) AS rank FROM playerStats INNER JOIN player ON playerStats.playerFK = player.id INNER JOIN game ON playerStats.gameFK = game.id WHERE player.active = 1 AND roundWinShare > 0 GROUP BY playerFK ORDER BY rws DESC");

            if ($run)
            {
                foreach ($run as $row)
                {
                    if ($row['auth'] == $auth)
                    {
                        $position['rank']   = $row['rank'];
                        $position['ranked'] = $run->rowCount();
                    }
                }
            }
        }
        
        return $position;
    }
    
    function getRank($where = '',$order = 'GROUP BY playerFK ORDER BY rws DESC',$limit = 'LIMIT 100')
    {
        $rank = [];
        
        if(!empty($where))
        {
            $where = "AND $where";
        }
        
        $run = $this->query("SELECT player.id, player.auth, name player, SUM(kills) kills, SUM(assist) assist, SUM(death) death, SUM(bombPlant) AS bp, SUM(bombDefuse) AS bd, SUM(kills2) 2k, SUM(kills3) 3k, SUM(kills4) 4k, SUM(kills5) 5k, SUM(versus1) v1, SUM(versus2) v2, SUM(versus3) v3, SUM(versus4) v4, SUM(versus5) v5, IF(SUM(headshot), (SUM(headshot) / SUM(kills)), 0) hsp, SUM(roundPlay) rp, SUM(damage) / SUM(roundPlay) adr, IF(SUM(kills),SUM(kills) / SUM(roundPlay),0.0) fpr, @rws := ((SUM(roundWinShare) / (SELECT SUM(score1) + SUM(score2) FROM game WHERE id = playerStats.gameFK)) * 100.0) rws, COUNT(IF(winner = team, 1, NULL)) wins, COUNT(IF(winner <> team, 1, NULL)) loses, (100.0 * (COUNT(IF(winner = team, 1, NULL)) / COUNT(gameFK))) winPct FROM playerStats INNER JOIN player ON playerStats.playerFK = player.id INNER JOIN game ON playerStats.gameFK = game.id WHERE player.active = 1 $where $order $limit");
        
        if($run)
        {
            $rank = $run->fetchAll();
        }
        
        return $rank;
    }
    
    function getMapRank($auth)
    {
        $rank = [];
        
        if(!empty($auth))
        {
            $run = $this->query("SELECT game.map, SUM(kills) kills, SUM(assist) assist, SUM(death) death, IF(SUM(kills),SUM(kills) / SUM(roundPlay),0.0) fpr, (100.0 * (COUNT(IF(winner = team, 1, NULL)) / COUNT(gameFK))) winPct FROM playerStats INNER JOIN player ON playerStats.playerFK = player.id INNER JOIN game ON playerStats.gameFK = game.id WHERE player.active = 1 AND player.auth = '{$auth}' GROUP BY playerStats.playerFK, game.map ORDER BY SUM(kills) DESC");

            if($run)
            {
                $rank = $run->fetchAll();
            }
        }
        
        return $rank;
    }
}