<?php
session_start();

require_once 'Net/GeoIP.php';
require_once 'Net/GeoIP/geoipregionvars.php';
require_once 'inc/geo.inc';
require_once 'inc/db.inc';
require_once 'inc/verbose.inc';
require_once 'inc/stats.inc';
require_once 'template_top.php';

open_db();

// If the search form was submitted, do search:
if (!empty($_POST)) {
  search_sites($_POST);
}

// Get stats:
$stats = get_stats();
//var_export($stats['countries']);

?>

<h2>Search Websites</h2>

<p>You can use this search engine to find Drupal sites, or in fact <em>any</em> sites with a host that matches a certain
pattern, or with a title, description or keywords that match a search string, or that are hosted in a given country.
Enjoy! And please <a href='<?php echo html_entities_all("mailto:shaun@astromultimedia.com"); ?>'>submit feedback</a>.
If you want. No pressure.</p>


<form method='POST' action='index.php'>

  <table id='search_table'>
    <tr>
      <th>
        <label for='keyword'>Title/description/keywords</label>
      </th>
      <td>
        <input id='search_string' name='search_string' value='<?php echo utf8_to_html($_SESSION['search_params']['search_string']); ?>'>
      </td>
    </tr>
    <tr>
      <th>
        <label for='scheme'>Scheme</label>
      </th>
      <td>
        <select id='scheme' name='scheme'>
          <option value='0' <?php if ($_SESSION['search_params']['scheme'] == '0') echo 'selected'; ?>>- Any -</option>
          <option value='1' <?php if ($_SESSION['search_params']['scheme'] == '1') echo 'selected'; ?>>http</option>
          <option value='2' <?php if ($_SESSION['search_params']['scheme'] == '2') echo 'selected'; ?>>https</option>
        </select>
      </td>
    </tr>
    <tr>
      <th>
        <label for='host'>Host</label>
      </th>
      <td>
        <select id='host_pattern' name='host_pattern'>
          <option value='0' <?php if ($_SESSION['search_params']['host_pattern'] == '0') echo 'selected'; ?>>Contains</option>
          <option value='1' <?php if ($_SESSION['search_params']['host_pattern'] == '1') echo 'selected'; ?>>Begins with</option>
          <option value='2' <?php if ($_SESSION['search_params']['host_pattern'] == '2') echo 'selected'; ?>>Ends with</option>
        </select>
        <input id='host' name='host' value='<?php echo utf8_to_html($_SESSION['search_params']['host']); ?>'>
      </td>
    </tr>
    <tr>
      <th>
        <label for='country_code'>Country where hosted</label>
      </th>
      <td>

        <?php
        /*
        <table id='where_hosted_table'>
          <tr>
            <th>
              <label for='country_code'>Country</label>
            </th>
            <td>
         */
        ?>
              <select id='country_code' name='country_code'>
<?php
// Create options HTML:
$country_options = "<option value=''>- Any -</option>\n";
foreach ($stats['countries'] as $country_code => $country_name) {
  $country_options .= "<option value='$country_code'";
  if ($_SESSION['search_params']['country_code'] == $country_code) {
    $country_options .= " selected";
  }
  $country_options .= ">$country_name</option>\n";
}
echo $country_options;
?>
              </select>
        
<?php
/*
            </td>
          </tr>
          <tr>
            <th>
              <label for='subcountry_code'>Subcountry</label>
            </th>
            <td>
              <select id='subcountry_code' name='subcountry_code'>
              </select>
            </td>
          </tr>
          <tr>
            <th>
              <label for='city'>City</label>
            </th>
            <td>
              <select id='city' name='city'>
              </select>
            </td>
          </tr>
        </table>
 */
?>
      </td>
    </tr>
    <tr>
      <th>
        <label for='drupal'>Drupal sites only</label>
      </th>
      <td>
        <input type='checkbox' id='drupal' name='drupal' <?php if ($_SESSION['search_params']['drupal']) echo 'checked'; ?>>
      </td>
    </tr>
    <tr id='drupal_ver_row'>
      <th>
        <label for='drupal_ver'>Drupal version</label>
      </th>
      <td>
        <select id='drupal_ver' name='drupal_ver'>
          <option value=''>- Any -</option>
          <?php
          // Show the major Drupal versions:
          foreach ($stats['total_drupal_ver'] as $drupal_ver => $count) {
            // Skip Unknown version:
            if (!$drupal_ver) {
              continue;
            }
            // Add option:
            echo "<option value='$drupal_ver'";
            if ($drupal_ver == $_SESSION['search_params']['drupal_ver']) {
              echo " selected";
            }
            echo ">$drupal_ver.x</option>\n";
          }
          ?>
        </select>
      </td>
    </tr>
    <tr>
      <td colspan='2' id='search-btn-row'>
        <input type='submit' value='Search'>
      </td>
    </tr>
  </table>
</form>

<!-- Include the JS -->
<script>
<?php
/*
// Output the hosting locations in JS for fast client-side interaction.
$sql2 = "
  SELECT DISTINCT country_code, subcountry_code, city
  FROM site
  WHERE country_code IS NOT NULL
  ORDER BY country_code, subcountry_code, city";
$locations = array();
foreach ($dbh->query($sql2, PDO::FETCH_OBJ) as $site) {

  $subcountry_code = $site->subcountry_code ? $site->subcountry_code : 'XX';
  $subcountry_name = array_key_exists($site->country_code, $GEOIP_REGION_NAME) &&
    array_key_exists($site->subcountry_code, $GEOIP_REGION_NAME[$site->country_code]) ?
    $GEOIP_REGION_NAME[$site->country_code][$site->subcountry_code] : '- Unknown -';
  $city_name = $site->city ? $site->city : '- Unknown -';
  
  if (!array_key_exists($site->country_code, $locations)) {
    $locations[$site->country_code] = array();
  }
  
  if (!array_key_exists($subcountry_code, $locations[$site->country_code])) {
    $locations[$site->country_code][$subcountry_code] = array(
      'name' => $subcountry_name,
      'cities' => array(),
    );
  }
  
  $locations[$site->country_code][$subcountry_code]['cities'][] = $city_name;
}
echo format_json(json_encode($locations));
*/
?>
</script>

<?php
// Search results:
if (array_key_exists('search_results', $_SESSION)) {
  echo "<hr>\n";
  
  // Atom feed link:
  $pairs = array();
  $params = $_SESSION['search_params'];
  if (!$params['host']) {
    unset($params['host_pattern']);
  }
  if (!$params['drupal']) {
    unset($params['drupal_ver']);
  }
  foreach ($params as $param => $value) {
    if ($value) {
      if ($param == 'host' || $param == 'search_string') {
        $value = urlencode(base64_encode($value));
      }

      $pairs[] = "$param=$value";
    }
  }
  echo "<p><a href='feed.php?", implode('&', $pairs), "'>Get this search as an Atom feed</a></p>\n";

  // Display results if there are any.
  if (!empty($_SESSION['search_results'])) {

    // Pagination variables:
    $page = (int) $_GET['page'];
    $n_total = $_SESSION['n_total_search_results'];
    $n_results = count($_SESSION['search_results']);
    $min_index = $page * PAGE_SIZE;
    $max_index = min(($page + 1) * PAGE_SIZE - 1, $n_results - 1);
    $n_pages = ceil($n_results / PAGE_SIZE);

    // Number of results message:
    if (is_numeric($n_total)) {
      echo "<p><strong>", number_format($n_total), ' ', ($n_total == 1 ? 'match' : 'matches') . " found.</strong>";
      if ($n_total > $n_results) {
        echo " A maximum of ", MAX_NUM_RESULTS, " results are listed, so you may want to refine your search.\n";
      }
    }
    else {
      echo "<p>The total number of matches could not be determined.";
    }
    
    echo "</p>\n";
    

    // Sorting message:
    echo "<p>Results are listed alphabetically by top-level domain, domain, subdomain, etc. Links open up in a new tab or window.</p>\n";

    // Show 1 page of results:
    for ($i = $min_index; $i <= $max_index; $i++) {
      $site = $_SESSION['search_results'][$i];

      echo "<div class='search_result ", ($i % 2 == 0 ? 'even' : ''), "'>\n";

      // URL:
      $url = get_site_url($site);
      echo "<h3><a href='$url' target='_blank'>" . utf8_to_html($url) . "</a></h3>\n";

      // Title:
      if ($site->title) {
        echo "<strong>" . utf8_to_html($site->title) . "</strong>\n";
      }

      // Description:
      if ($site->description) {
        echo "<p>" . utf8_to_html($site->description) . "</p>\n";
      }

      // Keywords:
      $keywords = utf8_to_html(format_keywords($site->keywords));
      if ($keywords) {
        echo "<p><em>$keywords</em></p>\n";
      }

      // Hosting location:
      if ($site->country_code) {
        $country_name = $site->country_code ? $GEOIP_COUNTRY_NAME[$site->country_code] : '';
        $subcountry_name = $site->subcountry_code ? $GEOIP_REGION_NAME[$site->country_code][$site->subcountry_code] : '';
        $location = implode(', ', array_filter(array($site->city, $subcountry_name, $country_name)));
        echo "<p><span class='green'>Hosted in " . utf8_to_html($location) . ".</span></p>\n";
      }
      else {
        echo "<p><span class='grey'>Hosting location could not be detected.</span></p>\n";
      }

      // Drupal info:
      if ($site->drupal) {
        if ($site->drupal_ver > 0) {
          echo "<p><span class='green'>This is a Drupal " . ver_float_to_string($site->drupal_ver) . " site.</span></p>\n";
        }
        else {
          echo "<p><span class='green'>This is a Drupal site, version unknown.</span></p>\n";
        }
      }
      else {
        echo "<p><span class='grey'>This is not a Drupal site.</span></p>\n";
      }
      
      // Re-examine link.
      // Note:
      //   - We use base64_encode() here because urlencode() does not encode ".", which causes problems, at least on hostgator.
      //   - We still use urlencode() to encode potential "+", "/" and "=" characters in the result from base64_encode().
      echo "<p class='grey'><em>Last examined ", substr($site->dt_examined, 0, 10), ". <a href='submit_link.php?link=", urlencode(base64_encode($url)), "'>Re-examine</a></em></p>\n";

      echo "</div>\n";
    }

    // Pagination:
    if ($n_pages > 1) {
      $links = array();
      for ($j = 0; $j < $n_pages; $j++) {
        $class = $page == $j ? 'active' : '';
        $links[] = "<a href='index.php?page=$j' class='$class'>" . ($j + 1) . "</a>";
      }
      echo "<div id='pagination'><div id='page'>Page:</div>" . implode($links) . "</div>\n";
    }

  }
  else {
    echo "<p>No matches found.</p>\n";
  }
}

require_once 'template_bottom.php';
