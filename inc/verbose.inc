<?php
/**
 * Verbose functions for DrupalFinder.
 * 
 * @author Shaun Moss <shaun@astromultimedia.com>
 * @version 2011-11-12
 */

/**
 * The number of results to display per page.
 */
define('PAGE_SIZE', 10);

/**
 * The maximum number of search results to get from the database.
 */
define('MAX_NUM_RESULTS', 200);

/**
 * Displays a message, but only if we're not if in the cron job/CLI.
 * 
 * @param string $msg
 */
function echo_msg($msg) {
  if (isset($_SERVER['HTTP_HOST']) && $_SERVER['HTTP_HOST']) {
    echo $msg, "\n";
  }
}

/**
 * Add a new site to the database.
 * The site will only be added if it is actually new.
 * 
 * @param string $url
 * @param string $encoding
 * @return int
 *   The new site_id, or existing site record.
 */
function add_site($url, $encoding = 'UTF-8') {
  global $dbh;
  
  // Convert to UTF-8 if we need to:
  $url = convert_to_utf8($url, $encoding);
  
  // Parse the URL:
  $url_info = parse_url2($url);

  // Check if this site is in the database:
  echo_msg("Checking <span class='blue'>$url</span>...");
  $site = get_site($url);

  if ($site) {
    // We already have this site:
    echo_msg("<span class='red'>Already in the database.</span>");

    // Return the site object:
    return $site;
  }
  else {
    // This is a new site:
    echo_msg("<span class='green'>Not in the database, adding...</span>");

    // Insert new site into the database, using (for now) the host without the www:
    insert_site($url_info['https'], $url_info['host']);

    // Return new site object:
    $site = new stdClass();
    $site->site_id = $dbh->lastInsertId();
    $site->new = TRUE;
    return $site;
  }
}

/**
 * Get a web page, with some messages for the user.
 * 
 * @param string $url 
 * @return array
 */
function get_web_page_verbose($url) {
  echo_msg("Requesting <span class='blue'>$url</span>...");
  $response = get_web_page($url);
  
//  print_response($response);
  
  if (!$response['valid']) {
    $error_code = $response['http_code'] != 200 ? "HTTP Status Code {$response['http_code']}" : "Error {$response['errno']}";
    echo_msg("<span class='red'><span class='blue'>$url</span> is invalid. $error_code.</span>");
  }
  else {
    echo_msg("<span class='green'><span class='blue'>$url</span> is valid, received.</span>");
  }
  
  return $response;
}

/**
 * Scans a web page's HTML looking for linked sites, then adds any new ones to the database.
 * Returns the number of new sites added.
 * 
 * @param string $html
 * @param string $encoding 
 * @return int
 */
function add_linked_sites($html, $encoding) {
  // Get any linked sites:
  echo_msg("Scanning for linked sites...");
  $linked_sites = get_linked_sites($html);

  // Count new sites:
  $n_new_sites = 0;
  $n_linked_sites = count($linked_sites);

  if (!$n_linked_sites) {
    echo_msg("<span class='red'>No linked sites found.</span>");
  }
  else {
    echo_msg("<span class='green'><span class='blue'>$n_linked_sites</span> linked " . ($n_linked_sites == 1 ? 'site' : 'sites') . " found.</span>");

//    // Display linked sites:
//    echo_msg("<ol>";
//    foreach ($linked_sites as $linked_site) {
//      echo_msg("<li>$linked_site</li>";
//    }
//    echo_msg("</ol>";

    // Add all linked sites to the database:
    foreach ($linked_sites as $linked_site) {

      // Add this site to the database if necessary:
      $linked_site = add_site($linked_site, $encoding);

      // If a new site was added, count it:
      if ($linked_site->new) {
        $n_new_sites++;
      }
    }
    
    if (!$n_new_sites) {
      echo_msg("<span class='red'>No new sites added.</span>");
    }
    else {
      echo_msg("<span class='green'><span class='blue'>$n_new_sites</span> new " . ($n_new_sites == 1 ? 'site' : 'sites') . " added.</span>");
    }
    
  }

  // Return number of new sites found:
  return $n_new_sites;
}

/**
 * Crawl a web page, looking for other sites.
 * Returns the number of new sites found, or FALSE if invalid.
 * 
 * @param type $url
 * @return int
 *   The number of linked sites found, or FALSE.
 */
function crawl_site($url) {
  // Get the HTML of this site's front page:
  $response = get_web_page_verbose($url);
  
  // If the URL could be loaded, scan for links and add:
  if ($response['valid']) {
    // Get the encoding:
    get_web_page_encoding($response);
    
    return add_linked_sites($response['content'], $response['encoding']);
  }
  
  return FALSE;
}

/**
 * Examine a site that's in the database.
 * 
 * @param object $site
 *   A site object, which may be a database record, or may just contain the site_id and new flag.
 * @return int
 *   The number of new sites added, or FALSE if the site or site_id is invalid.
 */
function examine_site($site) {
  global $dbh;
  
  // 1. Try to load the front page. If successful:
  // 2.   Do a reverse DNS lookup on the host, get the IP address.
  // 3.   Get the location from the IP address.
  // 4.   Get the encoding, title, description and keywords.
  // 5.   Detect if it's Drupal, and the version.
  // 6.   Get all links from the front page, and add to the database.
  
  // If the parameter is a site_id, load the site record:
  if ($site->new) {
    // Get the site record:
    $site = select_site($site->site_id);
  }
  
  // Check param is valid:
  if (!$site || !is_object($site)) {
    return FALSE;
  }
  
  // Values for SQL update query:
  $sql_values = array();
  $sql_values[':site_id'] = $site->site_id;

  /////////////////////////////////////////////////////////////////////////////
  // Try to get the web page.

  // Get the site URL and the URL info:
  $url = get_site_url($site);
  $url_info = parse_url2($url);

  // Default result is *without* the www (regardless of what is currently in the database).
  $url = $url_info['site'];
  $host = $url_info['host'];

  // Try *without* the www:
  $response = get_web_page_verbose($url_info['site']);
  
  // If that didn't work, try *with* the www:
  if (!$response['valid']) {
    $response = get_web_page_verbose($url_info['www_site']);
    if ($response['valid']) {
      $url = $url_info['www_site'];
      $host = $url_info['www_host'];
    }
  }
  
  // At this point we know the value for the host field, which may or may not have changed:
  $sql_values[':host'] = $host;

  if ($response['valid']) {
    // Site is valid.
    $sql_values[':valid'] = 1;

    /////////////////////////////////////////////////////////////////////////////
    // Get the IP address:
    
    // Theoretically we could do this even if the site was invalid, but there's no point.
    // It will just slow the spider down, collecting data that won't be used.
    echo_msg("Finding IP address...");
    $ip_addr = gethostbyname($host);
    if ($ip_addr == $host) {
      $ip_addr = NULL;
      echo_msg("<span class='red'>The IP address for <span class='blue'>$host</span> could not be detected.<span>");
    }
    else {
      echo_msg("<span class='green'>The IP address for <span class='blue'>$host</span> is <span class='blue'>$ip_addr</span>.</span>");
    }
    $sql_values[':ip_addr'] = $ip_addr;

    /////////////////////////////////////////////////////////////////////////////
    // Find out where it's hosted:
    echo_msg("Detecting hosting location...");
    $location = NULL;

    if ($ip_addr) {
      $location = get_location($ip_addr);
      if ($location) {
        echo_msg("<span class='green'>The site <span class='blue'>$url</span> is hosted in <span class='blue'>" . utf8_to_html($location->address) . "</span></span>");
        // Add location fields to SQL values:
        $sql_values[':country_code']    = $location->country_code;
        $sql_values[':subcountry_code'] = $location->subcountry_code;
        $sql_values[':city']            = $location->city;
      }
    }

    if (!$location) {
      echo_msg("<span class='red'>Could not determine where $url is hosted.</span>");
      $sql_values[':country_code']    = NULL;
      $sql_values[':subcountry_code'] = NULL;
      $sql_values[':city']            = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Get the encoding, title, description and keywords:
    echo_msg("Getting additional site info...");
    get_web_page_encoding($response);
    get_web_page_description($response);
    
    foreach (array('encoding', 'title', 'description', 'keywords') as $property) {
      $value = $response[$property];
      
      if ($value) {
        echo_msg("<span class='green'>" . ucfirst($property) . ": <span class='blue'>" . utf8_to_html($value) . "</span></span>");
      }
      else {
        echo_msg("<span class='red'>" . ucfirst($property) . " not found.</span>");
      }
      
      $sql_values[":$property"] = $value ? $value : NULL;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Detect Drupal
    $drupal = detect_drupal($response['content']);
    
    if (!$drupal) {
      echo_msg("<span class='red'>This is not a Drupal site.</span>");
    }
    else {
      echo_msg("<span class='green'>This is a Drupal site.</span>");
  
      // Detect the Drupal version:
      echo_msg("Detecting Drupal version...");
      $drupal_ver = detect_drupal_ver($url);
    
      if ($drupal_ver) {
        echo_msg("<span class='green'>Drupal version <span class='blue'>$drupal_ver</span>.</span>");
      }
      else {
        echo_msg("<span class='red'>Drupal version could not be detected.</span>");
      }
    }
    
    $sql_values[':drupal'] = $drupal;
    
    // Convert Drupal version to string for storing in the database:
    $sql_values[':drupal_ver'] = $drupal_ver ? ver_string_to_float($drupal_ver) : NULL;
    
    ///////////////////////////////////////////////////////////////////////////
    // Crawl the site, i.e. scan for links and add to the database:
    $n_new_sites = add_linked_sites($response['content'], $response['encoding']);
    
  }
  else {
    // Site is invalid.
    $sql_values[':valid']           = 0;
    $sql_values[':ip_addr']         = NULL;
    $sql_values[':country_code']    = NULL;
    $sql_values[':subcountry_code'] = NULL;
    $sql_values[':city']            = NULL;
    $sql_values[':encoding']        = NULL;
    $sql_values[':title']           = NULL;
    $sql_values[':description']     = NULL;
    $sql_values[':keywords']        = NULL;
    $sql_values[':drupal']          = NULL;
    $sql_values[':drupal_ver']      = NULL;
    $n_new_sites = 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Update the database record:
  echo_msg("Updating database record for <span class='blue'>$url</span>...");
  $sql = "
    UPDATE site SET
      host = :host,
      examined = 1,
      valid = :valid,
      ip_addr = :ip_addr,
      country_code = :country_code,
      subcountry_code = :subcountry_code,
      city = :city,
      encoding = :encoding,
      title = :title,
      description = :description,
      keywords = :keywords,
      drupal = :drupal,
      drupal_ver = :drupal_ver,
      dt_examined = NOW(),
      dt_updated = NOW()
    WHERE site_id = :site_id";
  $stmt = $dbh->prepare($sql);
  $stmt->execute($sql_values);

  /////////////////////////////////////////////////////////////////////////////
  // Do some clean-up. Check for any equal hosts (with or without 'www'), and delete them.
  // This shouldn't be needed any more, actually.
//  $sql2 = "
//    SELECT site_id
//    FROM site
//    WHERE https = :https AND host IN (:host, :www_host) AND site_id != :site_id";
//  $stmt2 = $dbh->prepare($sql2);
//  $stmt2->execute(array(
//    ':https' => $url_info['https'],
//    ':host' => $url_info['host'],
//    ':www_host' => $url_info['www_host'],
//    ':site_id' => $site->site_id,
//  ));
//  while ($stmt2) {
//    $site = $stmt2->fetchObject();
//    if (!$site) {
//      break;
//    }
//    $stmt3 = $dbh->prepare("DELETE FROM site WHERE site_id = :site_id");
//    $stmt3->execute(array(':site_id' => $site->site_id));
//  }

  return $n_new_sites;
}
