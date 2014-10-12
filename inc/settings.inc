<?php
// Error settings:
error_reporting(E_ALL & ~E_STRICT & ~E_NOTICE);
ini_set('display_errors', 0);
ini_set('display_startup_errors', 0);
ini_set('log_errors', 1);

// Set the site base path and add it to the include path:
$base_path = "/var/aegir/platforms/drupalfinder.com";
ini_set('include_path', ini_get('include_path') . ":$base_path");
