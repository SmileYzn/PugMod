<?php

class Base
{
    private $pdo   = null;
    private $table = null;

    function __construct()
    {
        $this->pdo   = Connection::get();
        $this->table = lcfirst(get_called_class());
    }
    
    function query($sql)
    {
        try
        {
            return $this->pdo->query($sql);
        }
        catch (PDOException $e)
        {
            /**/
        }
        
        return false;
    }
    
    function id($id)
    {
        try
        {
            $run = $this->pdo->prepare("SELECT * FROM $this->table WHERE id = ?");

            $run->execute([$id]);

            return $run->fetch();
        }
        catch (PDOException $e)
        {
            /**/
        }
        
        return false;
    }
    
    function consult($where = '', $order = '', $limit = '')
    {
        if (!empty($where))
        {
            $where = "WHERE $where";
        }

        return $this->query("SELECT * FROM $this->table $where $order $limit");
    }
}
