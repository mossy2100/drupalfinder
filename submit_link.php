<?php
require_once 'Net/GeoIP.php';
require_once 'Net/GeoIP/geoipregionvars.php';
require_once 'inc/geo.inc';
require_once 'inc/db.inc';
require_once 'inc/sites.inc';
require_once 'inc/verbose.inc';
require_once 'template_top.php';
open_db();
?>

<h2>Submit a link</h2>

<p>This can be any valid link. The link's site will be added to the database, as will any other sites this page links to.</p>

<form method='POST' action='submit_link.php'>

  <label for='link'><strong>URL:</strong></label>
  <input id='link' name='link'>
  <input type='submit' value='Submit'>
  
</form>

<pre>

<?php
    
//debug($_REQUEST);

// Get the link from POST or GET:
if ($_POST['link']) {
  $link = $_POST['link'];
}
elseif ($_GET['link']) {
  $link = base64_decode($_GET['link']);
}


// If a seed URL was provided via the form or the querystring, crawl it:
if ($link) {
  
  echo "<hr>";
  
  // Check that it starts with a scheme:
  if (strtolower(substr($link, 0, 4)) != 'http') {
    // It doesn't, so add 'http://' as the default scheme:
    $link = "http://$link";
  }
  
  // Strip off any trailing slashes:
  $link = rtrim($link, '/');
  
  echo "<h2>Submitted URL: $link</h2>\n";
  
  // Process for examining a submitted link:
  //   1. Check if the link is a site URL, and can therefore be added to the database.
  //   2. If yes, add the site to the database, and examine as above.
  //   3. If no, get all links from the front page, and add to the database.

  // Get the site URL from the seed URL:
  $url_info = parse_url2($link);
  
  if (!$url_info['valid']) {
    echo "Error - {$url_info['error']}.\n";
  }
  else {
    $n_new_sites = 0;
    
    // Get the URLs of the link's site:
    $site_url = $url_info['site'];
    $www_site_url = $url_info['www_site'];

    // Make sure we have this site:
    $site = add_site($site_url);
    if ($site->new) {
      $n_new_sites++;
    }

    // Check if the submitted link is a valid site URL:
    $lower_case_link = strtolower($link);
    $link_is_site = $lower_case_link == $site_url || $lower_case_link == $www_site_url;

    // If the link is not a site, crawl it. (If it is, it will get crawled by examine_site() below.)
    if (!$link_is_site) {
      $n_new_sites += crawl_site($link);
    }
    else {
      // Examine the submitted site:
      $n_new_sites += examine_site($site);
    }

    if (!$n_new_sites) {
      echo "<span class='red'>In total, no new sites added.</span>\n";
    }
    else {
      echo "<span class='green'>In total, <span class='blue'>$n_new_sites</span> new " . ($n_new_sites == 1 ? 'site' : 'sites') . " added.</span>\n";
    }
  }
}
?>

</pre>
    
<?php
require_once 'template_bottom.php';
