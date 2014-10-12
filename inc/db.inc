<?php
/**
 * Database functions used by DrupalFinder.
 * 
 * @author Shaun Moss <shaun@astromultimedia.com>
 * @version 2011-11-12
 */

/**
 * Open the database.
 */
function open_db() {
  global $dbh;
  if ($_SERVER['HTTP_HOST'] == 'drupalfinder') {
    $db = 'drupalfinder';
    $user = 'drupalfinder';
    $pwd = '$I<3Drupal$';
  }
  else {
    $db = 'drupalfinder';
    $user = 'drupalfinder';
    $pwd = '$I<3Drupal$';
  }
  $dbh = new PDO("mysql:host=localhost;dbname=$db", $user, $pwd);
}

/**
 * Gets a site's URL given the site record.
 * 
 * @param object $site
 * @return string
 */
function get_site_url($site) {
  return 'http' . ($site->https ? 's' : '') . '://' . $site->host;
}

/**
 * Get a site from the database given an id.
 * 
 * @param int $site_id
 * @return object
 */
function select_site($site_id) {
  global $dbh;
  
  // Get the database record as an object:
  $sql = "SELECT * FROM site WHERE site_id = :site_id";
  $stmt = $dbh->prepare($sql);
  $stmt->execute(array(
    ':site_id' => $site_id,
  ));
  return $stmt->fetchObject();
}

/**
 * Get a site from the database given a URL.
 * 
 * @param string $url
 * @return object
 */
function get_site($url) {
  global $dbh;
  
  // Parse the URL:
  $url_info = parse_url2($url);

  // This check shouldn't be necessary, because we always call this function with a valid URL,
  // but we'll leave it here just in case.
  if (!$url_info['valid']) {
    return FALSE;
  }

  // Get the first matching database record as an object:
  $sql = "SELECT * FROM site WHERE https = :https AND host IN (:host, :www_host) ORDER BY site_id";
  $sql_values = array(
    ':https'    => $url_info['https'],
    ':host'     => $url_info['host'],
    ':www_host' => $url_info['www_host'],
  );

  $stmt = $dbh->prepare($sql);
  $stmt->execute($sql_values);
  return $stmt->fetchObject();
}

/**
 * Compare 2 sites, for ordering search results.
 * 
 * @param object $site1
 * @param object $site2
 * @return int
 */
function compare_sites($site1, $site2) {
  $host1 = $site1->host;
  $host2 = $site2->host;
  if ($host1 == $host2) {
    if (!$site1->https && $site2->https) {
      return -1;
    }
    elseif ($site1->https && !$site2->https) {
      return 1;
    }
    else {
      // Same scheme, same host:
      return 0;
    }
  }
  else {
    // Compare host parts in reverse order, i.e. tld, domain, subdomain, etc.
    $host1parts = array_reverse(explode('.', $host1));
    $host2parts = array_reverse(explode('.', $host2));
    $n_parts = max(count($host1parts), count($host2parts));
    for ($i = 0; $i < $n_parts; $i++) {
      if ($host1parts[$i] < $host2parts[$i]) {
        return -1;
      }
      elseif ($host1parts[$i] > $host2parts[$i]) {
        return 1;
      }
    }
    // They must be the same - should never happen because equal hosts should be caught earlier.
    return 0;
  }  
}

/**
 * Extract the host from a URL if necessary, and remove any invalid chars.
 * 
 * @return string 
 */
function clean_host($host) {
  
  // Get the host as lower case:
  $host = strtolower((string) $host);
  
  // Strip http:// or https:// if present:
  if (substr($host, 0, 7) == 'http://') {
    $host = substr($host, 7);
  }
  else if (substr($host, 0, 8) == 'https://') {
    $host = substr($host, 8);
  }
  
  // Look for the first forward slash:
  $slash_pos = strpos($host, '/');
  if ($slash_pos !== FALSE) {
    $host = substr($host, 0, $slash_pos);
  }
  
  // Remove invalid chars:
  $host2 = '';
  for ($i = 0; $i < strlen($host); $i++) {
    $ch = $host[$i];
    if (ctype_alnum($ch) || $ch == '-' || $ch == '.') {
      $host2 .= $ch;
    }
  }
  return $host2;
}

/**
 * Search sites.
 * Store the search params and the results in the session.
 * 
 * @param array $params
 */
function search_sites($params) {
  global $dbh;
  
  // Valid param keys:
  //   - search_string
  //   - scheme
  //   - host_match
  //   - host
  //   - country_code
  //   - drupal
  //   - drupal_ver
  
  
  /////////////////////////////////////////////////////////////////////////////
  // Construct the SQL:
  $where = array();
  $sql_values = array();
  
  // Return valid and examined sites only:
  $where[] = "valid = 1 AND examined = 1";
  
  // search_string:
  if ($params['search_string']) {
    
//    $words = array_filter(explode(' ', $params['search_string']));
//    foreach ($words as $i => $word) {
//    
    // Wrap search string in word boundaries so we don't match partial words:
    $rx = "[[:<:]]{$params['search_string']}[[:>:]]";
    $where[] = "(title REGEXP :rx OR description LIKE :rx OR keywords LIKE :rx)";
    $sql_values[":rx"] = $rx;
      
//    }
    
//    $search_string = '%' . $params['search_string'] . '%';
//    $where[] = "(title LIKE :search_string OR description LIKE :search_string OR keywords LIKE :search_string)";
//    $sql_values[':search_string'] = $search_string;
  }
  
  // https:
  if ($params['scheme'] == 1) {
    $where[] = "https = 0";
  }
  elseif ($params['scheme'] == 2) {
    $where[] = "https = 1";
  }
  
  // host:
  // Clean host param, make it only letters, digits, dots or dashes, and lower-case.
  $params['host'] = clean_host($params['host']);
  if ($params['host']) {
    switch ($params['host_pattern']) {
      case 0:
        $host_pattern = '%' . $params['host'] . '%';
        break;
      
      case 1:
        $host_pattern = $params['host'] . '%';
        break;
      
      case 2:
        $host_pattern = '%' . $params['host'];
        break;
    }
    $where[] = "host LIKE :host_pattern";
    $sql_values[':host_pattern'] = $host_pattern;
  }
  
  // country_code:
  if ($params['country_code']) {
    $where[] = "country_code = :country_code";
    $sql_values[':country_code'] = $params['country_code'];
  }
  
  // subcountry_code:
//  if ($params['subcountry_code']) {
//    $where[] = "subcountry_code = :subcountry_code";
//    $sql_values[':subcountry_code'] = $params['subcountry_code'];
//  }
//  
//  // city:
//  if ($params['city']) {
//    $where[] = "city = :city";
//    $sql_values[':city'] = $params['city'];
//  }
  
  // drupal:
  $params['drupal'] = $params['drupal'] ? 1 : 0;
  
  if ($params['drupal']) {
    $where[] = "drupal = 1";

    if ($params['drupal_ver']) {
      $where[] = "drupal_ver >= :min_drupal_ver";
      $sql_values[':min_drupal_ver'] = $params['drupal_ver'];
      
      $where[] = "drupal_ver < :max_drupal_ver";
      $sql_values[':max_drupal_ver'] = $params['drupal_ver'] + 1;
    }

//    // min_drupal_ver:
//    if ($params['min_drupal_ver']) {
//      $where[] = "drupal_ver >= :min_drupal_ver";
//      $sql_values[':min_drupal_ver'] = $params['min_drupal_ver'];
//    }
//    
//    // max_drupal_ver:
//    if ($params['max_drupal_ver']) {
//      $where[] = "drupal_ver <= :max_drupal_ver";
//      $sql_values[':max_drupal_ver'] = $params['max_drupal_ver'];
//    }
  }
  
  // min_dt_examined:
  if ($params['min_dt_examined']) {
    $where[] = "dt_examined >= :min_dt_examined";
    $sql_values[':min_dt_examined'] = $params['min_dt_examined'];
  }

  // Get the WHERE clause:
  $where_clause = empty($where) ? '' : ('WHERE ' . implode(' AND ', $where));
  
  // Run the SQL, limit to MAX_NUM_RESULTS results.
  $sql = "SELECT * FROM site $where_clause ORDER BY host, https LIMIT " . MAX_NUM_RESULTS;
  $stmt = $dbh->prepare($sql);
  $stmt->execute($sql_values);
  
  // Collect all the sites:
  $sites = array();
  while ($stmt) {
    $site = $stmt->fetchObject();
    if (!$site) {
      break;
    }
    $sites[] = $site;
  }
  
  // Sort the sites:
  usort($sites, 'compare_sites');

  /////////////////////////////////////////////////////////////////////////////
  // Remember the params and the results in the session:
  $_SESSION['search_params'] = $params;
  $_SESSION['search_results'] = $sites;
  
  /////////////////////////////////////////////////////////////////////////////
  // Get the total number of results:
  $_SESSION['n_total_search_results'] = 'Unknown';
  $sql_count = "SELECT COUNT(*) AS count FROM site $where_clause";
  $stmt_count = $dbh->prepare($sql_count);
  $stmt_count->execute($sql_values);
  if ($stmt_count) {
    $rec_count = $stmt_count->fetchObject();
    if ($rec_count) {
      $_SESSION['n_total_search_results'] = $rec_count->count;
    }
  }
  
  return $sites;
}

/**
 * Insert a new site into the database.
 * 
 * @param bool $https
 * @param string $host 
 */
function insert_site($https, $host) {
  global $dbh;
  $sql = "INSERT INTO site (https, host, dt_created, dt_updated) VALUES (:https, :host, NOW(), NOW())";
  $stmt = $dbh->prepare($sql);
  $stmt->execute(array(
    ':https'  => $https,
    ':host'   => $host,
  ));
}
