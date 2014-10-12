<?php
require_once 'inc/settings.inc';

// Ensure this script is not being run from the browser, unless local/dev or it's me:
$dev = FALSE;

if ($_SERVER['HTTP_HOST']) {
  if ($_SERVER['HTTP_HOST'] == 'drupalfinder' || $_GET['user'] == 'mossy2100') {
    $dev = TRUE;
  }
  else {
    echo "Access denied.";
    exit;
  }
}

if ($dev) {
  echo '<pre>';
}

require_once 'inc/db.inc';
require_once 'inc/strings.inc';
require_once 'inc/debug.inc';
require_once 'inc/stats.inc';
require_once 'inc/geo.inc';
debugOn();
open_db();

$stats = get_stats(TRUE);
var_export($stats);

if ($dev) {
  echo '</pre>';
}
