<?php

class Demos Extends Base
{
    function zip($path)
    {
        if(file_exists($path))
        {
            $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
            
            foreach ($rii as $file)
            {
                if($file->isFile() && $file->getExtension() == 'dem')
                {
                    $zipFile = new ZipArchive();
                    $zipName = str_replace('.dem','.zip', $file->getRealPath());
                    
                    if($zipFile->open($zipName, ZIPARCHIVE::CREATE) === TRUE)
                    {
                        $zipFile->addFile($file->getRealPath(),$file->getFileName());
                        
                        $zipFile->close(); 
                        
                        unlink($file->getRealPath());
                    }
                }
            }   
            
            unset($rii);
        }
    }
    
    function get($path)
    {
        $demos = [];
        
        if(file_exists($path))
        {
            $this->zip($path);
            
            $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
            
            foreach ($rii as $file)
            {
                if($file->isFile())
                {
                    $demoInfo = explode('-', $file->getBasename('.zip'));
                    
                    $demos[$demoInfo[1]] =
                    [
                        'prefix'    => $demoInfo[0],
                        'map'       => $demoInfo[2],
                        'date'      => DateTime::createFromFormat('ymdHi', $demoInfo[1])->format('m/d/Y H:i'),
                        'file'      => $file->getFileName()
                    ];
                    
                    rsort($demos);
                }
            }
        }
        
        return $demos;
    }
}