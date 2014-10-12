<?php
// Debugging functions:

$debugMode = false;

function debugOn() {
	global $debugMode;
	$debugMode = true;
}

function debugOff() {
	global $debugMode;
	$debugMode = false;
}

function debugMode() {
	global $debugMode;
	return $debugMode;
}

function debugBeginPrint() {
	print("<pre style='color:Red'>\n");
}

function debugEndPrint() {
	print("</pre>\n");
}

function debug($var, $funcName = '') {
	global $debugMode;
	if ($debugMode) {
		debugBeginPrint();
		if ($funcName != '') {
			print "<b>$funcName:</b> ";
		}
		if (is_array($var)) {
			print_r($var);
		}
		elseif (is_object($var)) {
			var_dump($var);
		}
		elseif (is_bool($var)) {
			print(($var ? 'TRUE' : 'FALSE')."<br />\n");
		}
		else {
			print(htmlspecialchars($var)."<br />\n");
		}
		debugEndPrint();
	}
}

function debugAll($printPreTags = true) {
	global $debugMode;
	if ($debugMode) {
		if ($printPreTags) {
			debugBeginPrint();
		}
		var_dump(get_defined_vars());
		if ($printPreTags) {
			debugEndPrint();
		}
	}
}

function debugExit($str = '') {
	global $debugMode;
	if ($debugMode) {
		exit($str);
	}
}

/**
 * Same as db_query, but will show the actual query being executed, for debug purposes.
 * I copied db_query to avoid hacking core.
 */
function debug_query($query) {
  $args = func_get_args();
  array_shift($args);
  $query = db_prefix_tables($query);
  if (isset($args[0]) and is_array($args[0])) { // 'All arguments in one array' syntax
    $args = $args[0];
  }
  _db_query_callback($args, TRUE);
  $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
  return _db_query($query, TRUE);
}
