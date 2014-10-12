<?php
require_once 'inc/settings.inc';
require_once 'inc/strings.inc';
require_once 'inc/debug.inc';
debugOn();

?>
<!doctype html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>DrupalFinder</title>
    <meta name='description' content="Web crawler/search engine that detects character encoding, title, description,
      keywords, hosting location, if a site is running Drupal, and, if so, the Drupal version.">
    <meta name='keywords' content="search engine, web crawler, drupal">
    <link rel='stylesheet' href='drupalfinder.css'>
  </head>
  <body>
    
    <div id='header'>
      <div id='header-content'>
        <img id='banner' src='images/drupalfinder-banner.jpg'>
        <?php
        require "menu.php";
        ?>
      </div>
    </div>
    
    <div id='main'>
      <div id='main-content'>
