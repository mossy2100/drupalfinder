
/**
 * Set the main content height so that the footer is flush with the bottom of the page.
 */
function setMinHeight() {
  // Get the window height:
  var windowHeight = $(window).height();
  var headerHeight = $('#header').height();
  var footerHeight = $('#footer').height();
  
  // Calculate the minHeight.
  // The 40 is padding (20px) on #main-content.
  var minHeight = windowHeight - headerHeight - footerHeight - 40;
  
  // Set the minHeight
  $('#main-content').css('min-height', minHeight + 'px');
}

/**
 * Hide/show the drupal version if the Drupal checkbox is checked.
 */
function hide_show_drupal_ver() {
  var checked = $('#drupal').is(':checked');
  var drupal_ver_row = $('#drupal_ver').closest('tr');
  if (checked) {
    $('#drupal_ver').closest('tr').removeClass('disabled');
    $('#drupal_ver').enable();
  }
  else {
    $('#drupal_ver').closest('tr').addClass('disabled');
    $('#drupal_ver').val('');
    $('#drupal_ver').disable();
  }
}

/**
 * Remove any invalid characters from the host.
 */
function extract_host_from_url() {
  // Get the host as lower case:
  var host = $('#host').val().toLowerCase();
  
  // Strip http:// or https:// if present:
  if (host.substr(0, 7) == 'http://') {
    host = host.substr(7);
  }
  else if (host.substr(0, 8) == 'https://') {
    host = host.substr(8);
  }
  
  // Look for the first forward slash:
  var slash_pos = host.indexOf('/');
  if (slash_pos != -1) {
    host = host.substr(0, slash_pos);
  }
  
  // Remove invalid chars:
  var host2 = '';
  var ch;
  for (var i = 0; i < host.length; i++) {
    ch = host.charAt(i);
    if (('a' <= ch && ch <= 'z') || ('0' <= ch && ch <= '9') || ch == '-' || ch == '.') {
      host2 += ch;
    }
  }
  $('#host').val(host2);
}


$(function() {
  if ($('#drupal').length) {
    // Do it on startup:
    hide_show_drupal_ver();
    // Also then whe checkbox is clicked:
    $('#drupal').click(hide_show_drupal_ver);
  }

  // Do it on startup:
  setMinHeight();
  // Also on resize:
  $(window).resize(setMinHeight);
  
  // When the host changes, check if we need to extract it from a URL:
  $('#host').change(extract_host_from_url);
  extract_host_from_url();
});
