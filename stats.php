<?php
require_once 'inc/db.inc';
require_once 'inc/stats.inc';
require_once 'template_top.php';

open_db();
?>
    
<h2>Stats</h2>

<?php

// Get stats:
$stats = get_stats();
echo "<p><em>These stats are updated every hour. The last update was at {$stats['dt_updated']}.</em></p>";

echo "<p>There are ", number_format($stats['total']), " sites in the database.</p>";
echo "<p>", number_format($stats['total_examined']), " of these have been examined.</p>";
echo "<p>", number_format($stats['total_valid']), " valid sites have been found.</p>";
$percent = number_format($stats['total_drupal'] / $stats['total_valid'] * 100, 2);
echo "<p>", number_format($stats['total_drupal']), " of these are Drupal sites, which is $percent%.</p>";
?>

<br>

<h3>Drupal versions</h3>

<p>This graph shows the number of Drupal sites by version, and the percentage of the total number of Drupal sites.</p>

<?php

$n_total_drupal_ver = array();
$max = 0;

function add_stat($v, $n) {
  global $stats, $n_total_drupal_ver, $drupal_ver_percent, $max;
  $drupal_ver_percent[$v] = number_format($n / $stats['total_drupal'] * 100, 2);
  if ($n > $max) {
    $max = $n;
  }
}

foreach ($stats['total_drupal_ver'] as $drupal_ver => $num) {
  add_stat($drupal_ver, $num);
}

define('MAX_BAR_WIDTH', 600);
$colors = array('#eee', 'chocolate', 'turquoise', 'yellow', 'red', '#00cc00', '#ffbf00', '#36a0ea', 'orchid');

echo "<div id='stats_drupal_ver_graph'>\n";
foreach ($stats['total_drupal_ver'] as $v => $n) {
  $width = ceil($n * MAX_BAR_WIDTH / $max);
  echo "<div class='stats_bar'>\n";
  echo "<div class='stats_drupal_ver'>", ($v ? "$v.x" : '?'), "</div>\n";
  echo "<div class='stats_drupal_ver_bar' style='width: {$width}px; background-color: $colors[$v]'></div>\n";
  echo "<div class='stats_drupal_ver_info'>$n ", ($n == 1 ? 'site' : 'sites'), ", $drupal_ver_percent[$v]%</div>\n";
  echo "</div>\n";
}
echo "</div>\n";

require_once 'template_bottom.php';
