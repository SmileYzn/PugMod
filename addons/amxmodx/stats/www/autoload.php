<?php
// Error Display
ini_set('display_errors', 0);

// Error Reporting
error_reporting(E_ERROR|E_PARSE);

// Timezone
date_default_timezone_set('America/Sao_Paulo');

// Locale
setlocale(LC_TIME, 'pt_BR', 'pt_BR.utf-8', 'pt_BR.utf-8', 'portuguese');

// Autoloader para classess do PHP
spl_autoload_register(function($class)
{
    // Construir o caminho
    $path = __DIR__ . DIRECTORY_SEPARATOR . 'class' . DIRECTORY_SEPARATOR . $class . '.php';
    
    // Se existir
    if(file_exists($path))
    {
        // Incluir
        include_once($path);
    }
});
