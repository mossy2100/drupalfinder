<?php
// Generate the Atom feed.

require_once 'Net/GeoIP.php';
require_once 'Net/GeoIP/geoipregionvars.php';
require_once 'inc/geo.inc';
require_once 'inc/db.inc';
require_once 'inc/sites.inc';
require_once 'inc/verbose.inc';
require_once 'inc/strings.inc';
require_once 'inc/debug.inc';
require_once 'classes/FeedWriter.php';
//debugOn();
open_db();


// IMPORTANT : No need to add id for feed or channel. It will be automatically created from link.

//Creating an instance of FeedWriter class. 
//The constant ATOM is passed to mention the version
$feed = new FeedWriter(ATOM);

//Setting the channel elements
//Use wrapper functions for common elements
$feed->setTitle('DrupalFinder feed');
$feed->setLink('http://drupalfinder.com');
$feed->setSelfLink('http://drupalfinder.com/feed.php?' . $_SERVER['QUERY_STRING']);

//For other channel elements, use setChannelElement() function
$feed->setChannelElement('updated', date(DATE_ATOM , time()));
$feed->setChannelElement('author', array('name' => 'Shaun Moss'));

// Get the search params.
$params = $_GET;
// Decode a couple of the params:
foreach ($params as $param => $value) {
  if ($param == 'host' || $param == 'search_string') {
    $params[$param] = base64_decode($value);
  }
}
// Set min_dt_examined to 1 day in the past:
$params['min_dt_examined'] = date('Y-m-d H:i:s', time() - 86400);
//debug($params);
// Search sites:
$sites = search_sites($params);

// Add sites to the feed:
foreach ($sites as $site) {
  //Create an empty FeedItem
  $newItem = $feed->createNewItem();

  //Add elements to the feed item
  //Use wrapper functions to add common feed elements
  $url = get_site_url($site);
  $newItem->setTitle($site->title);
  $newItem->setLink($url);
  $newItem->setDate($site->dt_examined);
  
// Internally changed to "summary" tag for ATOM feed
  $newItem->setDescription($site->description);
  
  // Add other fields as content:
  $content = array(
    'encoding' => $site->encoding,
    'description' => $site->description,
    'keywords' => format_keywords($site->keywords),
    'ip_addr' => $site->ip_addr,
    'country_code' => $site->country_code,
    'country' => $site->country_code ? $GEOIP_COUNTRY_NAME[$site->country_code] : '',
    'subcountry_code' => $site->subcountry_code,
    'subcountry' => $site->subcountry_code ? $GEOIP_REGION_NAME[$site->country_code][$site->subcountry_code] : '',
    'city' => $site->city,
    'drupal' => $site->drupal,
    'drupal_ver' => ($site->drupal && $site->drupal_ver) ? ver_float_to_string($site->drupal_ver) : '',
    'drupal_ver_float' => ($site->drupal && $site->drupal_ver) ? $site->drupal_ver : '',
  );
  $pairs = array();
  foreach ($content as $key => $value) {
    $pairs[] = "$key=$value";
  }
  // Place each key-value pair on its own line:
  $newItem->addElement('content', implode("\n", $pairs));

  // Now add the feed item:
  $feed->addItem($newItem);
}

//OK. Everything is done. Now genarate the feed.
$feed->genarateFeed();
