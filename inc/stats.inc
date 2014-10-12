<?php

/**
 * Get the path to the stats file.
 * 
 * @return string
 */
function stats_path() {
  return dirname(__FILE__) . '/../data/stats.txt';
}

/**
 * Regenerate the stats.
 * 
 * @return array
 */
function regenerate_stats() {
  global $dbh, $GEOIP_COUNTRY_NAME;
  
  $stats = array();

  // Note update time:
  $stats['dt_updated'] = date('Y-m-d H:i:s');
  
  // Get the total number of sites:
  $sql_total = "SELECT COUNT(*) AS total FROM site";
  $stmt_total = $dbh->query($sql_total);
  $rec_total = $stmt_total->fetchObject();
  $stats['total'] = (int) $rec_total->total;

  // Get the total number of examined sites:
  $sql_examined = "SELECT COUNT(*) AS total FROM site WHERE examined = 1";
  $stmt_examined = $dbh->query($sql_examined);
  $rec_examined = $stmt_examined->fetchObject();
  $stats['total_examined'] = (int) $rec_examined->total;

  // Get the total number of examined and valid sites:
  $sql_valid = "SELECT COUNT(*) AS total FROM site WHERE valid = 1 AND examined = 1";
  $stmt_valid = $dbh->query($sql_valid);
  $rec_valid = $stmt_valid->fetchObject();
  $stats['total_valid'] = (int) $rec_valid->total;

  // Get number of Drupal sites:
  $sql_drupal = "SELECT COUNT(*) AS total_drupal FROM site WHERE valid = 1 AND examined = 1 AND drupal = 1";
  $stmt_drupal = $dbh->query($sql_drupal);
  $rec_drupal = $stmt_drupal->fetchObject();
  $stats['total_drupal'] = (int) $rec_drupal->total_drupal;

  // Get the Drupal sites with unknown version:
  $sql3 = "
    SELECT COUNT(*) AS n
    FROM site
    WHERE valid = 1 AND examined = 1 AND drupal = 1 AND drupal_ver IS NULL";
  $stmt3 = $dbh->query($sql3);
  $rec3 = $stmt3->fetchObject();
  $stats['total_drupal_ver'][0] = (int) $rec3->n;

  // Get the Drupal sites with known version:
  $sql3 = "
    SELECT COUNT(*) AS n, FLOOR(drupal_ver) AS v
    FROM site
    WHERE examined = 1 AND valid = 1 AND drupal_ver IS NOT NULL
    GROUP BY FLOOR(drupal_ver)";
  $stmt3 = $dbh->query($sql3);
  while ($stmt3) {
    $rec3 = $stmt3->fetchObject();
    if (!$rec3) {
      break;
    }
    $stats['total_drupal_ver'][$rec3->v] = (int) $rec3->n;
  }

  // Get all country codes:
  $sql_countries = "
    SELECT DISTINCT country_code
    FROM site
    WHERE examined = 1 AND valid = 1 AND country_code IS NOT NULL";
  $countries = array();
  foreach ($dbh->query($sql_countries, PDO::FETCH_OBJ) as $site) {
    $countries[$site->country_code] = $GEOIP_COUNTRY_NAME[$site->country_code];
  }
  // Sort by country name:
  asort($countries);
  $stats['countries'] = $countries;

  // Save the stats to a file:
  $path = stats_path();
  file_put_contents($path, serialize($stats));
  
  return $stats;
}

/**
 * Load or regenerate the stats.
 * 
 * 
 * @param type $regenerate 
 */
function get_stats($force_regenerate = FALSE) {
  // Get path to stats file:
  $path = stats_path();

  // If not forcing a regenerate, try to load from file:
  if (!$force_regenerate) {
    if (file_exists($path)) {
      $stats = @file_get_contents($path);
    }
    if ($stats) {
      $stats = @unserialize($stats);
    }
  }
  
  // If we don't have the stats, regenerate:
  if (!$stats || !is_array($stats)) {
    $stats = regenerate_stats();
  }
  
  return $stats;
}
