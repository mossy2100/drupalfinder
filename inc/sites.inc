<?php
/**
 * Reusable functions related to URLs and websites.
 * 
 * @author Shaun Moss <shaun@astromultimedia.com>
 * @version 2011-11-12
 */

/**
 * Get encoding from content type.
 * e.g. if $content type == "text/html; charset=UTF-8"
 * then return "UTF-8"
 * 
 * @param string $content_type
 * @return string
 */
function get_encoding_from_content_type($content_type) {
  $rx = "/charset=([a-z0-9\-\_\.\:\#]*)/i";
  $n = preg_match($rx, $content_type, $matches);
  return $n ? normalise_encoding($matches[1]) : FALSE;
}

/**
 * Use curl to get a web page (HTML, XHTML, XML, image, etc.) from a URL.
 * Returns an array containing the HTTP server response header fields and content.
 * 
 * @param string $url
 * @return array
 */
function get_web_page($url) {
  $options = array(
    CURLOPT_RETURNTRANSFER => TRUE,           // return web page as a string
    CURLOPT_HEADER         => FALSE,          // don't include headers
    CURLOPT_ENCODING       => '',             // handle all encodings
    CURLOPT_USERAGENT      => 'DrupalFinder', // who am i
    CURLOPT_CONNECTTIMEOUT => 60,             // timeout on connect
    CURLOPT_TIMEOUT        => 60,             // timeout on response

//    CURLOPT_FOLLOWLOCATION => FALSE,          // don't follow redirects

    CURLOPT_FOLLOWLOCATION => TRUE,           // follow redirects
    CURLOPT_AUTOREFERER    => TRUE,           // set referer on redirect
    CURLOPT_MAXREDIRS      => 5,              // stop after x redirects
  );

  $ch = curl_init($url);
  curl_setopt_array($ch, $options);
  $content = curl_exec($ch);
  $err = curl_errno($ch);
  $errmsg = curl_error($ch);
  $response = curl_getinfo($ch);
  curl_close($ch);

  // Add to results array:
  $response['errno']   = $err;
  $response['errmsg']  = $errmsg;
  $response['content'] = $content;
  $response['valid']   = $response['http_code'] == 200 && $response['errno'] == 0;
  
  return $response;
}

/**
 * Extracts the title from a web page.
 * 
 * @param string $html
 * @param string $encoding
 * @return string
 */
function get_title_from_html($html, $encoding = 'UTF-8') {
  $rx = "/<title>([^<]*)<\/title>/si";
  $n = preg_match($rx, $html, $matches);
  if ($n) {
    return clean_text($matches[1], $encoding);
  }
  return FALSE;
}

/**
 * Extracts meta tags from a web page.
 * 
 * @param string $html
 * @return string
 */
function get_meta_tags_from_html($html) {
  // Regular expression for finding meta tags:
  $rx_tag = "/<meta[^>]*>/is";
  
  // Regular expression for finding attributes and values:
  $rx_attr = "/([a-z\-\:]+)\=([\"][^\"]*[\"]|[\'][^\']*[\']|[^\'\"\s]*)/is";
  $n = preg_match_all($rx_tag, $html, $matches_tags);
  
  // Results:
  $meta_tags = array();
  
  if ($n) {
    foreach ($matches_tags[0] as $i => $meta_tag) {
      $attributes = substr($meta_tag, 5, strlen($meta_tag) - 6);
      $n_attrs = preg_match_all($rx_attr, $attributes, $matches_attr);
      
      foreach ($matches_attr[1] as $j => $name) {
        $value = $matches_attr[2][$j];
        
        // Remove quotes if present:
        if ($value[0] == '"' || $value[0] == "'") {
          $value = substr($value, 1, strlen($value) - 2);
        }
        
        $meta_tags[$i][$name] = $value;
      }
    }
  }
  
  return $meta_tags;
}

/**
 * Scan meta tags for a certain type of information.
 * 
 * @see http://www.w3schools.com/tags/tag_meta.asp
 * 
 * @param array $meta_tags
 * @param string $info_type
 *   This can be any value of the http-equiv or name attribute, e.g.
 *     - content-type
 *     - cache-control
 *     - description
 *     - keywords
 *     - author
 *     - generator
 *     etc.
 * @param string $encoding
 * @return string
 *   The requested info, hopefully as UTF-8. FALSE if not found.
 */
function get_meta_info($meta_tags, $info_type, $encoding = 'UTF-8') {

  // Loop through meta tags:
  foreach ($meta_tags as $meta_tag) {
    
    // Reset name and content for this tag:
    $name = NULL;
    $content = NULL;
    
    // Loop through attributes:
    foreach ($meta_tag as $attr => $value) {
      
      if (strtolower($attr) == 'name' || strtolower($attr) == 'http-equiv') {
        $name = $value;
      }
      elseif (strtolower($attr) == 'content') {
        $content = $value;
      }

      // If we have a name matching the requested info type, and we have content, then we have what we're looking for:
      if ($name && strcasecmp($name, $info_type) == 0 && $content) {
        return clean_text($content, $encoding);
      }
    }
  }

  // Description not found:
  return FALSE;
}

/**
 * Looks through meta tags for the charset (HTML5).
 * 
 * @param array $meta_tags
 * @return string
 *   The charset, or FALSE if not found.
 */
function get_meta_charset($meta_tags) {
  foreach ($meta_tags as $meta_tag) {
    foreach ($meta_tag as $attr => $value) {
      if (strtolower($attr) == 'charset') {
        return normalise_encoding($value);
      }
    }
  }
  // Not found:
  return FALSE;
}

/**
 * If we don't already have the character encoding, get the meta tags and inspect.
 * 
 * @param array $response
 */
function get_web_page_encoding(&$response) {
  // Get info from the response array:
  $content_type = $response['content_type'];
  $encoding     = $response['encoding'];
  $meta_tags    = $response['meta_tags'];

  // If we have the content type already, try extracting the encoding:
  if ($content_type) {
    $encoding = get_encoding_from_content_type($content_type);
  }

  // If we don't have the encoding yet, look at meta tags:
  if (!$encoding) {
    
    // If don't have meta tags yet, get them now:
    if (!$meta_tags) {
      $meta_tags = get_meta_tags_from_html($response['content']);
    }
    
    // Try getting the encoding from the HTML5 meta tag with charset attribute:
    $encoding = get_meta_charset($meta_tags);

    // If we still don't have the encoding, try getting it from the HTML4 meta tags:
    if (!$encoding) {
      $content_type2 = get_meta_info($meta_tags, 'content-type');
      if ($content_type2) {
        $content_type = $content_type2;
        $encoding = get_encoding_from_content_type($content_type);
      }
    }
  }
  
  // Update response array with the new info:
  $response['content_type'] = $content_type;
  $response['encoding']     = $encoding;
  $response['meta_tags']    = $meta_tags;
  
//  print_response($response);
}

/**
 * Get description fields (title, description, keywords) from the web page.
 * 
 * @param array $response
 */
function get_web_page_description(&$response) {
  // Get meta tags if not already:
  $meta_tags = $response['meta_tags'];
  if (!$meta_tags) {
    $meta_tags = get_meta_tags_from_html($response['content']);
  }

  // Get title, description and keywords:
  $title = get_title_from_html($response['content'], $response['encoding']);
  $description = get_meta_info($meta_tags, 'description', $response['encoding']);
  $keywords = get_meta_info($meta_tags, 'keywords', $response['encoding']);

  // Update response array with the new info:
  $response['meta_tags'] = $meta_tags;
  $response['title'] = $title;
  $response['description'] = $description;
  $response['keywords'] = $keywords;
}

/**
 * Extracts sites from a web page.
 * The sites include the scheme (http or https) and the host name, but no path.
 * Only detects full/absolute URLs, not relative ones.
 * Returns unique sites in lower-case.
 * 
 * @param string $html
 * @return array
 */
function get_linked_sites($html) {
  // @todo This may need to be updated to support non-Latin domain names. Depends if we care.
  $rx = "/\bhttp(s?)\:\/\/([a-z0-9\-]+\.)+([a-z]{2}|aero|asia|biz|cat|com|coop|edu|gov|info|int|jobs|mil|mobi|museum|name|net|org|pro|tel|travel|xxx)\b/i";
  $n_matches = preg_match_all($rx, $html, $matches);
  if ($n_matches) {
    $linked_sites = $matches[0];
    // Convert all to lower case:
    foreach ($linked_sites as $i => $linked_site) {
      // Only return unique results:
      $linked_sites[$i] = strtolower($linked_site);
    }
    return array_unique($linked_sites);
  }
  // None found:
  return array();
}

/**
 * Display a response from get_web_page(), excluding content.
 * 
 * @param array $response 
 */
function print_response($response) {
  unset($response['content']);
  var_export($response);
}

/**
 * Improved version of parse_url().
 * 
 * @param string $url
 * @return array
 */
function parse_url2($url) {
  $url_info = parse_url($url);
  
  // Check if the URL is valid:
  $url_info['valid'] = TRUE;
  if (!$url_info['scheme']) {
    // No valid scheme provided.
    $url_info['valid'] = FALSE;
    $url_info['error'] = "Invalid scheme";
  }
  else {
    $rx = "/[a-z0-9\-\.]+\.[a-z]{2,6}/i";
    if (!preg_match($rx, $url_info['host'])) {
      // The host is invalid.
      $url_info['valid'] = FALSE;
      $url_info['error'] = "Invalid host";
    }
  }

  // If so, get the https and site values we need:
  if ($url_info['valid']) {
    $url_info['scheme'] = strtolower($url_info['scheme']);
    $url_info['https'] = (int) ($url_info['scheme'] == 'https');

    // Make host lower-case:
    $host = strtolower($url_info['host']);
    // If the host begins with a www, remove it:
    if (substr($host, 0, 4) == 'www.') {
      $host = substr($host, 4);
    }
    $url_info['host'] = $host;
    
    // Get the host with the www:
    $url_info['www_host'] = "www.$host";
    
    // Get the full site URL with and without the www:
    $url_info['site'] = $url_info['scheme'] . '://' . $url_info['host'];
    $url_info['www_site'] = $url_info['scheme'] . '://' . $url_info['www_host'];
  }
  
  return $url_info;
}

/**
 * Check if a website is Drupal.
 * 
 * @param string $html
 * @return bool
 */
function detect_drupal($html) {
  // Regex from Wappalyzer:
  $rx = "/(<script [^>]+drupal\.js|jQuery\.extend\(Drupal\.settings, \{|Drupal\.extend\(\{ settings: \{|<link[^>]+sites\/(default|all)\/themes\/|<style[^>]+sites\/(default|all)\/(themes|modules)\/)/i";
  return (bool) preg_match($rx, $html);
}

/**
 * Attempts to detect Drupal version by looking for CHANGELOG.txt.
 * 
 * @param string $url
 *   Must be a site URL (i.e. scheme and host only), not any old link.
 * @return string
 */
function detect_drupal_ver($url) {
  // If this is a Drupal site, see if we can find the version by looking in CHANGELOG.txt.
  $response = get_web_page("$url/CHANGELOG.txt");
  if ($response['valid']) {
    // Look for the first Drupal version.
    // Note that we have not necessarily found CHANGELOG.txt. Some websites will return another page with no error.
    $rx = "/Drupal ([0-9\.]+)/";
    $n_matches = preg_match($rx, $response['content'], $matches);
    if ($n_matches) {
      return $matches[1];
    }
  }
  return FALSE;
}
