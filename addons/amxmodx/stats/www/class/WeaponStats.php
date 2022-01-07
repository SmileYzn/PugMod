<?php

class WeaponStats extends Base
{
    function getStats($where = '',$order = 'GROUP BY weaponStats.playerFK, weaponStats.weaponFK ORDER BY SUM(weaponStats.kills) DESC',$limit = '')
    {
        $rank = [];
        
        if(!empty($where))
        {
            $where = "AND $where";
        }

        $run = $this->query("SELECT player.name, weapon.name weapon, weaponStats.weaponFK id, SUM(kills) kills, SUM(death) death, SUM(headshot) hs, SUM(hit) hit, IF(SUM(headshot), (SUM(headshot) / SUM(kills)), 0) hsp FROM weaponStats INNER JOIN player ON weaponStats.playerFK = player.id INNER JOIN weapon ON weaponStats.weaponFK = weapon.id WHERE player.active = 1 $where $order $limit");

        if($run)
        {
            $rank = $run->fetchAll();
        }
        
        return $rank;
    }
}