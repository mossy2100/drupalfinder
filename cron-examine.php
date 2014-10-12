<?php
require_once 'inc/settings.inc';

// Ensure this script is not being run from the browser, unless local/dev or it's me:
$dev = FALSE;

if (isset($_SERVER['HTTP_HOST']) && $_SERVER['HTTP_HOST']) {
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

$script_t1 = time();

require_once 'Net/GeoIP.php';
require_once 'Net/GeoIP/geoipregionvars.php';
require_once 'inc/geo.inc';
require_once 'inc/db.inc';
require_once 'inc/sites.inc';
require_once 'inc/verbose.inc';
require_once 'inc/strings.inc';
require_once 'inc/debug.inc';
debugOn();
open_db();


/**
 * Add a line to the log file.
 * 
 * @param string $msg 
 */
function log_msg($msg) {
  global $log;
  $date = date('Y-m-d H:i:s');
  $log_msg = "$date $msg\n";
  echo $log_msg;
}

log_msg("Started cron job.");


///////////////////////////////////////////////////////////////////////////////
// Parameters.

// Set maximum script execution time to 5 minutes. For some reason the cron job
// is timing out approximately every 6.5 minutes.
$max_script_time = 300; // seconds

// Max sites should be more than we can do in $max_script_time seconds, but not so
// many that the script gets slowed down by the database select query.
// Value is based on best performance of about 0.5 second per host (actually it
// takes around 2 seconds per host in average)
$max_sites = $max_script_time * 2;

///////////////////////////////////////////////////////////////////////////////
// Examine any unexamined sites, maximum of $max_sites.
// Note, I changed the order-by from dt_created to site_id, because it should
// be faster as site_id is indexed (primary key).
$sql = "
  SELECT *
  FROM site
  WHERE examined = 0
  ORDER BY site_id
  LIMIT $max_sites";
$count = 0;
log_msg("Started loop...");
foreach ($dbh->query($sql, PDO::FETCH_OBJ) as $site) {
  $site_t1 = time();
  $url = get_site_url($site);
  log_msg("Started examining $url...");
  examine_site($site);
  log_msg("Finished examining $url");

  $site_t2 = time();
  log_msg("It took " . ($site_t2 - $site_t1) . " seconds to examine $url");

  $count++;
  log_msg("$count sites examined by this script so far.");
  
  // Should we stop?
  $script_dt = $site_t2 - $script_t1;
  if ($script_dt >= $max_script_time) {
    break;
  }
}
log_msg("Finished loop.");

// Calculate script time:
$script_t2 = time();
$script_dt = $script_t2 - $script_t1;
$avg = number_format($script_dt / $count, 1);
log_msg("It took $script_dt seconds to run this script and $count sites were examined, an average of $avg seconds per site.");

log_msg("Finished cron job.");

if ($dev) {
  echo '</pre>';
}
