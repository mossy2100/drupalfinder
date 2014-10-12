<div id='menu'>
  <?php
  $links = array(
    "Search" => 'index.php',
    "Submit a link" => 'submit_link.php',
    "Stats" => 'stats.php',
    "About" => 'about.php',
    "Contact" => html_entities_all("mailto:shaun@astromultimedia.com"),
  );
  $this_page = ltrim($_SERVER['SCRIPT_NAME'], '/\\');
  foreach ($links as $label => $href) {
    echo "<a href='$href'", ($this_page == $href ? " class='active'" : ''), ">$label</a>";
  }
  ?>
  <a href='http://drupal.org' target='_blank'>drupal.org</a>
</div>
