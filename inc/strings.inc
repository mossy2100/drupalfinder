<?php
/**
 * String-related functions used by DrupalFinder.
 * 
 * @author Shaun Moss <shaun@astromultimedia.com>
 * @version 2011-11-12
 */

/**
 * Converts the string to a string of numerical Unicode html entities.
 * Only works with single-byte/ASCII strings.
 * This function *will* double-encode existing HTML entities in the string.
 * 
 * @param string $str
 * @return string 
 */
function html_entities_all($str) {
	$result = '';
	for ($i = 0; $i < strlen($str); $i++) {
		$result .= '&#' . ord($str[$i]) . ';';
  }
	return $result;
}

/**
 * Convert a UTF-8 string to UTF-8 with HTML character entities (only for special chars).
 * 
 * @param string $str
 * @return string
 */
function utf8_to_html($str) {
  // Decode (twice, just to be sure)
  $str = html_entity_decode($str, ENT_QUOTES, 'UTF-8');
  $str = html_entity_decode($str, ENT_QUOTES, 'UTF-8');
  // Encode:
  $str = htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
  // Replace single quotes with named entity:
  $str = str_replace('&#039;', '&apos;', $str);
  return $str;
}

/**
 * Convert $str to UTF-8 if we need to and if we can.
 * 
 * @param string $str
 * @param string $from_encoding
 * @return string
 */
function convert_to_utf8($str, $from_encoding) {
  
  // Only convert if we have a different encoding:
  if ($from_encoding && $from_encoding != 'UTF-8') {
    
    if (function_exists('mb_convert_encoding') && in_array($from_encoding, mb_list_encodings())) {
      // Use mb if available, and if encoding is supported:
      return mb_convert_encoding($str, 'UTF-8', $from_encoding);
    }
    elseif (function_exists('iconv')) {
      // Otherwise try iconv:
      return iconv($from_encoding, 'UTF-8', $str);
    }
  }

  // Return the string unchanged:
  return $str;
}

/**
 * Cleans a piece of text.
 *   - Trim
 *   - Remove line breaks and compress whitespace, like browsers do.
 *   - Convert to UTF-8.
 * 
 * @param string $str
 * @return string
 */
function clean_text($str, $encoding) {
  return convert_to_utf8(preg_replace("/\s+/s", ' ', trim($str)), $encoding);
}

/**
 * Makes sure that each keyword is separated by ", ".
 * 
 * @param string $keywords
 * @return string
 */
function format_keywords($keywords) {
  $a1 = explode(',', $keywords);
  $a2 = array();
  foreach ($a1 as $keyword) {
    $keyword2 = trim($keyword);
    if ($keyword2) {
      $a2[] = $keyword2;
    }
  }
  return implode(', ', $a2);
}

/**
 * Convert encoding to its standard case.
 * 
 * @param string $encoding
 * @return string
 */
function normalise_encoding($encoding) {
  global $ENCODINGS;
  $encoding = trim($encoding);
  
  // Special cases:
  if (strtolower($encoding) == 'shift-jis') {
    $encoding = 'Shift_JIS';
  }
  else {
    // Find a match in the known encodings:
    foreach ($ENCODINGS as $std_encoding) {
      if (strcasecmp($encoding, $std_encoding) == 0) {
        return $std_encoding;
      }
    }
  }

  return $encoding;
}

/**
 * Convert a version number as a float to a string.
 * 
 * @param float $ver_float
 * @return string 
 */
function ver_float_to_string($ver_float) {
  $ver_float = (string) $ver_float;
  $parts = explode('.', $ver_float);
  $major = (int) $parts[0];
  $minor = (int) substr($parts[1], 0, 2);
  $revision = (int) substr($parts[1], 2, 4);
  return "$major.$minor" . ($revision ? ".$revision" : '');
}

/**
 * Convert a version number as a string to a float.
 * 
 * @param string $ver_string
 * @return float
 */
function ver_string_to_float($ver_string) {
  list($major, $minor, $revision) = explode('.', $ver_string);
  return $major + ($minor / 100) + ($revision / 10000);
}

/**
 * Indents a flat JSON string to make it more human-readable.
 *
 * @param string $json
 *   The original JSON string to process.
 * @return string
 *   Indented version of the original JSON string.
 */
function format_json($json) {
  $result = '';
  $pos = 0;
  $strLen = strlen($json);
  $indentStr = '  ';
  $newLine = "\n";
  $prevChar = '';
  $outOfQuotes = true;

  for ($i = 0; $i <= $strLen; $i++) {

    // Grab the next character in the string.
    $char = substr($json, $i, 1);

    // Are we inside a quoted string?
    if ($char == '"' && $prevChar != '\\') {
      $outOfQuotes = !$outOfQuotes;

      // If this character is the end of an element, 
      // output a new line and indent the next line.
    }
    else if (($char == '}' || $char == ']') && $outOfQuotes) {
      $result .= $newLine;
      $pos--;
      for ($j = 0; $j < $pos; $j++) {
        $result .= $indentStr;
      }
    }

    // Add the character to the result string.
    $result .= $char;

    // If the last character was the beginning of an element, 
    // output a new line and indent the next line.
    if (($char == ',' || $char == '{' || $char == '[') && $outOfQuotes) {
      $result .= $newLine;
      if ($char == '{' || $char == '[') {
        $pos++;
      }

      for ($j = 0; $j < $pos; $j++) {
        $result .= $indentStr;
      }
    }

    $prevChar = $char;
  }

  return $result;
}

/**
 * Character encodings.
 */
$ENCODINGS = array (
  'pass',
  'auto',
  'wchar',
  'byte2be',
  'byte2le',
  'byte4be',
  'byte4le',
  'BASE64',
  'UUENCODE',
  'HTML-ENTITIES',
  'Quoted-Printable',
  '7bit',
  '8bit',
  'UCS-4',
  'UCS-4BE',
  'UCS-4LE',
  'UCS-2',
  'UCS-2BE',
  'UCS-2LE',
  'UTF-32',
  'UTF-32BE',
  'UTF-32LE',
  'UTF-16',
  'UTF-16BE',
  'UTF-16LE',
  'UTF-8',
  'UTF-7',
  'UTF7-IMAP',
  'ASCII',
  'EUC-JP',
  'SJIS',       // Same as Shift_JIS
  'Shift_JIS',  // Preferred
  'eucJP-win',
  'SJIS-win',
  'CP932',
  'CP51932',
  'JIS',
  'ISO-2022-JP',
  'ISO-2022-JP-MS',
  'Windows-1250',
  'Windows-1251',
  'Windows-1252',
  'Windows-1253',
  'Windows-1254',
  'Windows-1255',
  'Windows-1256',
  'Windows-1257',
  'ISO-8859-1',
  'ISO-8859-2',
  'ISO-8859-3',
  'ISO-8859-4',
  'ISO-8859-5',
  'ISO-8859-6',
  'ISO-8859-7',
  'ISO-8859-8',
  'ISO-8859-9',
  'ISO-8859-10',
  'ISO-8859-13',
  'ISO-8859-14',
  'ISO-8859-15',
  'ISO-8859-16',
  'EUC-CN',
  'CP936',
  'HZ',
  'EUC-TW',
  'BIG-5',
  'EUC-KR',
  'UHC',
  'ISO-2022-KR',
  'CP866',
  'KOI8-R',
  'KOI8-U',
  'ArmSCII-8',
  'CP850',
  'JIS-ms',
  'CP50220',
  'CP50220raw',
  'CP50221',
  'CP50222',
  'GB2312',
  'KS_C_5601-1987',
);
