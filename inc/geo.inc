<?php
/**
 * GeoIP functions used by DrupalFinder.
 * 
 * @author Shaun Moss <shaun@astromultimedia.com>
 * @version 2011-11-12
 */

/**
 * Gets the single Net_GeoIP instance.
 * 
 * @staticvar Net_GeoIP $geoip
 * @return Net_GeoIP
 */
function get_geoip() {
  global $base_path;
  static $geoip;
  if (!$geoip) {
    $geoip = Net_GeoIP::getInstance("$base_path/data/GeoLiteCity.dat", Net_GeoIP::MEMORY_CACHE);
  }
  return $geoip;
}

/**
 * Get location details from an IP address.
 * 
 * @param string $ip_addr
 * @return object
 *   The location, or FALSE of not found.
 */
function get_location($ip_addr) {
  global $GEOIP_REGION_NAME;
  
  // Look up the location using the MaxMind GeoIP database:
  $geoip = get_geoip();
  $location = $geoip->lookupLocation($ip_addr);
  
  if ($location) {
    // Create another, simple object with results:
    $location2 = new stdClass();
    
    // Get the country name:
    $location2->country_code = $location->countryCode;
    $location2->country_name = $location->countryName;
    
    // Get the subcountry name:
    $location2->subcountry_code = $location->region;
    $location2->subcountry_name = $location2->subcountry_code ? $GEOIP_REGION_NAME[$location2->country_code][$location2->subcountry_code] : NULL;
    
    // Convert city from ISO-8859-1 to UTF-8:
    $location2->city = $location->city ? convert_to_utf8($location->city, 'ISO-8859-1') : NULL;
    
    // Construct location string:
    $location2->address = implode(', ', array_filter(array(
      $location2->city,
      $location2->subcountry_name,
      $location2->country_name
    )));
    return $location2;
  }
  
  return FALSE;
}

/**
 * Fast lookup array for country codes to names, created using the following code:
 * 
 * $country_names = array_combine(Net_GeoIP::$COUNTRY_CODES, Net_GeoIP::$COUNTRY_NAMES);
 * echo "<pre>";
 * var_export($country_names);
 * echo "</pre>";
 */
$GEOIP_COUNTRY_NAME = array (
  '' => '',
  'AP' => 'Asia/Pacific Region',
  'EU' => 'Europe',
  'AD' => 'Andorra',
  'AE' => 'United Arab Emirates',
  'AF' => 'Afghanistan',
  'AG' => 'Antigua and Barbuda',
  'AI' => 'Anguilla',
  'AL' => 'Albania',
  'AM' => 'Armenia',
  'AN' => 'Netherlands Antilles',
  'AO' => 'Angola',
  'AQ' => 'Antarctica',
  'AR' => 'Argentina',
  'AS' => 'American Samoa',
  'AT' => 'Austria',
  'AU' => 'Australia',
  'AW' => 'Aruba',
  'AZ' => 'Azerbaijan',
  'BA' => 'Bosnia and Herzegovina',
  'BB' => 'Barbados',
  'BD' => 'Bangladesh',
  'BE' => 'Belgium',
  'BF' => 'Burkina Faso',
  'BG' => 'Bulgaria',
  'BH' => 'Bahrain',
  'BI' => 'Burundi',
  'BJ' => 'Benin',
  'BM' => 'Bermuda',
  'BN' => 'Brunei Darussalam',
  'BO' => 'Bolivia',
  'BR' => 'Brazil',
  'BS' => 'Bahamas',
  'BT' => 'Bhutan',
  'BV' => 'Bouvet Island',
  'BW' => 'Botswana',
  'BY' => 'Belarus',
  'BZ' => 'Belize',
  'CA' => 'Canada',
  'CC' => 'Cocos (Keeling) Islands',
  'CD' => 'Congo, The Democratic Republic of the',
  'CF' => 'Central African Republic',
  'CG' => 'Congo',
  'CH' => 'Switzerland',
  'CI' => 'Cote D\'Ivoire',
  'CK' => 'Cook Islands',
  'CL' => 'Chile',
  'CM' => 'Cameroon',
  'CN' => 'China',
  'CO' => 'Colombia',
  'CR' => 'Costa Rica',
  'CU' => 'Cuba',
  'CV' => 'Cape Verde',
  'CX' => 'Christmas Island',
  'CY' => 'Cyprus',
  'CZ' => 'Czech Republic',
  'DE' => 'Germany',
  'DJ' => 'Djibouti',
  'DK' => 'Denmark',
  'DM' => 'Dominica',
  'DO' => 'Dominican Republic',
  'DZ' => 'Algeria',
  'EC' => 'Ecuador',
  'EE' => 'Estonia',
  'EG' => 'Egypt',
  'EH' => 'Western Sahara',
  'ER' => 'Eritrea',
  'ES' => 'Spain',
  'ET' => 'Ethiopia',
  'FI' => 'Finland',
  'FJ' => 'Fiji',
  'FK' => 'Falkland Islands (Malvinas)',
  'FM' => 'Micronesia, Federated States of',
  'FO' => 'Faroe Islands',
  'FR' => 'France',
  'FX' => 'France, Metropolitan',
  'GA' => 'Gabon',
  'GB' => 'United Kingdom',
  'GD' => 'Grenada',
  'GE' => 'Georgia',
  'GF' => 'French Guiana',
  'GH' => 'Ghana',
  'GI' => 'Gibraltar',
  'GL' => 'Greenland',
  'GM' => 'Gambia',
  'GN' => 'Guinea',
  'GP' => 'Guadeloupe',
  'GQ' => 'Equatorial Guinea',
  'GR' => 'Greece',
  'GS' => 'South Georgia and the South Sandwich Islands',
  'GT' => 'Guatemala',
  'GU' => 'Guam',
  'GW' => 'Guinea-Bissau',
  'GY' => 'Guyana',
  'HK' => 'Hong Kong',
  'HM' => 'Heard Island and McDonald Islands',
  'HN' => 'Honduras',
  'HR' => 'Croatia',
  'HT' => 'Haiti',
  'HU' => 'Hungary',
  'ID' => 'Indonesia',
  'IE' => 'Ireland',
  'IL' => 'Israel',
  'IN' => 'India',
  'IO' => 'British Indian Ocean Territory',
  'IQ' => 'Iraq',
  'IR' => 'Iran, Islamic Republic of',
  'IS' => 'Iceland',
  'IT' => 'Italy',
  'JM' => 'Jamaica',
  'JO' => 'Jordan',
  'JP' => 'Japan',
  'KE' => 'Kenya',
  'KG' => 'Kyrgyzstan',
  'KH' => 'Cambodia',
  'KI' => 'Kiribati',
  'KM' => 'Comoros',
  'KN' => 'Saint Kitts and Nevis',
  'KP' => 'Korea, Democratic People\'s Republic of',
  'KR' => 'Korea, Republic of',
  'KW' => 'Kuwait',
  'KY' => 'Cayman Islands',
  'KZ' => 'Kazakstan',
  'LA' => 'Lao People\'s Democratic Republic',
  'LB' => 'Lebanon',
  'LC' => 'Saint Lucia',
  'LI' => 'Liechtenstein',
  'LK' => 'Sri Lanka',
  'LR' => 'Liberia',
  'LS' => 'Lesotho',
  'LT' => 'Lithuania',
  'LU' => 'Luxembourg',
  'LV' => 'Latvia',
  'LY' => 'Libyan Arab Jamahiriya',
  'MA' => 'Morocco',
  'MC' => 'Monaco',
  'MD' => 'Moldova, Republic of',
  'MG' => 'Madagascar',
  'MH' => 'Marshall Islands',
  'MK' => 'Macedonia',
  'ML' => 'Mali',
  'MM' => 'Myanmar',
  'MN' => 'Mongolia',
  'MO' => 'Macau',
  'MP' => 'Northern Mariana Islands',
  'MQ' => 'Martinique',
  'MR' => 'Mauritania',
  'MS' => 'Montserrat',
  'MT' => 'Malta',
  'MU' => 'Mauritius',
  'MV' => 'Maldives',
  'MW' => 'Malawi',
  'MX' => 'Mexico',
  'MY' => 'Malaysia',
  'MZ' => 'Mozambique',
  'NA' => 'Namibia',
  'NC' => 'New Caledonia',
  'NE' => 'Niger',
  'NF' => 'Norfolk Island',
  'NG' => 'Nigeria',
  'NI' => 'Nicaragua',
  'NL' => 'Netherlands',
  'NO' => 'Norway',
  'NP' => 'Nepal',
  'NR' => 'Nauru',
  'NU' => 'Niue',
  'NZ' => 'New Zealand',
  'OM' => 'Oman',
  'PA' => 'Panama',
  'PE' => 'Peru',
  'PF' => 'French Polynesia',
  'PG' => 'Papua New Guinea',
  'PH' => 'Philippines',
  'PK' => 'Pakistan',
  'PL' => 'Poland',
  'PM' => 'Saint Pierre and Miquelon',
  'PN' => 'Pitcairn Islands',
  'PR' => 'Puerto Rico',
  'PS' => 'Palestinian Territory',
  'PT' => 'Portugal',
  'PW' => 'Palau',
  'PY' => 'Paraguay',
  'QA' => 'Qatar',
  'RE' => 'Reunion',
  'RO' => 'Romania',
  'RU' => 'Russian Federation',
  'RW' => 'Rwanda',
  'SA' => 'Saudi Arabia',
  'SB' => 'Solomon Islands',
  'SC' => 'Seychelles',
  'SD' => 'Sudan',
  'SE' => 'Sweden',
  'SG' => 'Singapore',
  'SH' => 'Saint Helena',
  'SI' => 'Slovenia',
  'SJ' => 'Svalbard and Jan Mayen',
  'SK' => 'Slovakia',
  'SL' => 'Sierra Leone',
  'SM' => 'San Marino',
  'SN' => 'Senegal',
  'SO' => 'Somalia',
  'SR' => 'Suriname',
  'ST' => 'Sao Tome and Principe',
  'SV' => 'El Salvador',
  'SY' => 'Syrian Arab Republic',
  'SZ' => 'Swaziland',
  'TC' => 'Turks and Caicos Islands',
  'TD' => 'Chad',
  'TF' => 'French Southern Territories',
  'TG' => 'Togo',
  'TH' => 'Thailand',
  'TJ' => 'Tajikistan',
  'TK' => 'Tokelau',
  'TM' => 'Turkmenistan',
  'TN' => 'Tunisia',
  'TO' => 'Tonga',
  'TL' => 'Timor-Leste',
  'TR' => 'Turkey',
  'TT' => 'Trinidad and Tobago',
  'TV' => 'Tuvalu',
  'TW' => 'Taiwan',
  'TZ' => 'Tanzania, United Republic of',
  'UA' => 'Ukraine',
  'UG' => 'Uganda',
  'UM' => 'United States Minor Outlying Islands',
  'US' => 'United States',
  'UY' => 'Uruguay',
  'UZ' => 'Uzbekistan',
  'VA' => 'Holy See (Vatican City State)',
  'VC' => 'Saint Vincent and the Grenadines',
  'VE' => 'Venezuela',
  'VG' => 'Virgin Islands, British',
  'VI' => 'Virgin Islands, U.S.',
  'VN' => 'Vietnam',
  'VU' => 'Vanuatu',
  'WF' => 'Wallis and Futuna',
  'WS' => 'Samoa',
  'YE' => 'Yemen',
  'YT' => 'Mayotte',
  'RS' => 'Serbia',
  'ZA' => 'South Africa',
  'ZM' => 'Zambia',
  'ME' => 'Montenegro',
  'ZW' => 'Zimbabwe',
  'A1' => 'Anonymous Proxy',
  'A2' => 'Satellite Provider',
  'O1' => 'Other',
  'AX' => 'Aland Islands',
  'GG' => 'Guernsey',
  'IM' => 'Isle of Man',
  'JE' => 'Jersey',
  'BL' => 'Saint Barthelemy',
  'MF' => 'Saint Martin',
);