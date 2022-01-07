<?php

class Game extends Base
{
    function getGames($auth = '')
    {
        $games = [];
        
        if(!empty($auth))
        {
            $run = $this->query("SELECT game.id, game.map, rounds, score1, score2, IF(team = winner,1,0) winner, team, kills, assist,death, kills5 5k, ((SUM(roundWinShare) / (SELECT rounds FROM game WHERE id = playerStats.gameFK)) * 100.0) rws, game.now FROM playerstats INNER JOIN player ON playerstats.playerFK = player.id INNER JOIN game ON playerstats.gameFK = game.id WHERE player.active = 1 AND player.auth = '{$auth}' GROUP BY game.id ORDER BY game.id DESC LIMIT 10");
            
            if(!empty($run))
            {
                $games = $run->fetchAll();
            }
        }
        
        return $games;
    }
}