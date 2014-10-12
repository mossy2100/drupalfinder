<?php
require_once "template_top.php";
?>
    
<h2>About DrupalFinder</h2>

<p>This is a very simple and lightweight web crawler that examines websites and checks for a few things:</p>

<ul>
  <li>If the site URL is valid, i.e. does not trigger a server-side redirect or an error (e.g. 404, page forbidden, etc.).</li>
  <li>The character encoding.</li>
  <li>The site's title, description and keywords.</li>
  <li>Where the site is hosted.</li>
  <li>If the site is running Drupal, and if possible, the version.</li>
</ul>

<p>This program was created to enable searches like "Find Drupal sites on a .au domain",
or "Find Drupal sites hosted in Australia".</p>

<p>There is currently only one database table, which contains information about websites. This is
<strong>not</strong> a collection of <em>links</em>, such as you might find on Google, but of <em>sites</em>, which
means simply a scheme (http or https) plus a host (domain or subdomain), i.e. only the top-level URL of the site.</p>

<p>Domains and subdomains are treated as separate sites. However, <em>http://www.example.com</em> and <em>http://example.com</em>
are treated as equivalent. Each host is first examined without the 'www' prefix. If that fails, then it's examined with the 'www'.
Therefore, if a host is shown starting with 'www', it needs it.</p>

<p>Website URLs are extracted from submitted and crawled links. The character encoding is obtained from the HTTP header
or from meta tags. Description and keywords are harvested from meta tags.</p>

<p>To detect where a site is hosted, the host is first resolved to an IP address. The location is then
looked up using <a href='http://www.maxmind.com/app/geolitecity' target="_blank">MaxMind's free GeoLite City database</a>.</p>

<p>To detect if a website is using Drupal, the regular expression from
<a href='http://wappalyzer.com/' target='_blank'>Wappalyzer</a> is used, which basically just looks for
&quot;drupal.js&quot; or &quot;Drupal.settings&quot;. If it's a Drupal site, the version is
detected by looking for a <em>CHANGELOG.txt</em> file, and scanning for the first Drupal version in that file. This is
not a flawless method, as that file does not need to be present on a Drupal site, and the web server may be configured
not to serve <em>.txt</em> files. However, it works in most cases (see <a href='stats.php'>Stats</a>).</p>

<p>Because the program only examines front pages, Drupal sites running in a subdirectory will not be detected or added
to the database. Websites that perform a server-side redirect to another site or a subdirectory are currently treated as invalid
and will not appear in search results.</p>

<p>Ironically, this website does not use Drupal, which would have been overkill for this simple program.
It uses PHP 5.2, PEAR, MySQL 5, HTML5, CSS3, JavaScript, and jQuery 1.7.
PHP's <a href='http://au2.php.net/manual/en/book.mbstring.php' target="_blank">multibyte string</a> and
<a href='http://au2.php.net/manual/en/book.iconv.php' target="_blank">iconv</a> extensions
provide support for non-Latin characters. Everything is converted to UTF-8 before being stored in the database, and
then to HTML entities for display in the browser.</p>

<p>If you have any suggestions for this little project, feel free to <a href='<?php echo html_entities_all("mailto:shaun@astromultimedia.com"); ?>'>email me</a>! :)</p>

<p>Shaun Moss<br>
CEO, <a href='http://iwda.biz' target="_blank">International Web Development Academy</a></p>

<p>
Facebook: <a href="http://facebook.com/mossy2100" target='_blank'>facebook.com/mossy2100</a><br>
Twitter: <a href="http://twitter.com/mossy2100" target='_blank'>twitter.com/mossy2100</a><br>
Skype: shaun.moss1
</p>

<br>
<h3>Possible future ideas</h3>

<!--
<p><strong>Inbound link counter</strong><br>
A link counter to count inbound links to each site, so search results can be ordered by popularity
instead of alphabetically.</p>
-->

<p><strong>Customised RSS feed</strong><br>
Allow people to set up an RSS feed of newly discovered sites, based on their search preferences.</p>

<?php
require_once 'template_bottom.php';
