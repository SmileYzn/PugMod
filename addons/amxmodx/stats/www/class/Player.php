<?php

class Player extends Base
{
    function get($auth = '')
    {
        $player = [];
        
        if(!empty($auth))
        {
            $auth = filter_var($auth,FILTER_SANITIZE_STRING);
            
            if(!empty($auth))
            {
                return $this->consult("auth = '{$auth}'")->fetch();
            }
        }
        
        return $player;
    }
}