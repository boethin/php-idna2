<?php
exit("This is a stub file for phpDocumentor\n");

/**
 * PHP IDNA extension
 *
 * A PHP extension for dealing with internationalized domain names, providing both IDNA 2003 and IDNA 2008 processing functions. 
 *
 * @see http://www.xn--domain.net
 * @author Sebastian Böthin <boethin@xn--domain.net>
 */


/**
 * Check whether the string contains non-ASCII characters. 
 */
const IDNA2_CHECK_ASCII = 0;

/**
 * Check whether the string contains HYPHEN_MINUS '-' at the 3rd and 4th position.
 */
const IDNA2_CHECK_HYPHEN34 = 0;

/**
 * Check whether the string does not start nor end with HYPHEN_MINUS '-'.
 */
const IDNA2_CHECK_HYPHEN = 0;

/**
 * Apply length checks on labels and domain names according to the IDNA standard.
 */
const IDNA2_CHECK_LENGTH = 0;

/**
 * Apply checks for non-LDH ASCII characters.
 */
const IDNA2_CHECK_LDH = 0;

/**
 * Check for Std3 ASCII rules.
 */
const IDNA2_CHECK_STD3 = 0;

/**
 * Check for PROHIBITED code points.
 */
const IDNA2_CHECK_PROHIBITED = 0;

/**
 * Check for UNASSIGNED code points.
 */
const IDNA2_CHECK_UNASSIGNED = 0;

/**
 * Check for labels beginning with a combining mark.
 */
const IDNA2_CHECK_COMBINING = 0;

/**
 * Check requirements specified in [IDNA2008-BIDI].
 */
const IDNA2_CHECK_BIDI = 0;

/**
 * Check labels are in NFC.
 */
const IDNA2_CHECK_NFC = 0;

/**
 * Check CONTEXTJ code points.
 */
const IDNA2_CHECK_CONTEXTJ = 0;

/**
 * Check CONTEXTO code points for the registration protocol.
 */
const IDNA2_CHECK_CONTEXTO_REGIST = 0;

/**
 * Check CONTEXTO code points for the lookup protocol.
 */
const IDNA2_CHECK_CONTEXTO_LOOKUP = 0;

/**
 * Check requirements as specified by the IDNA 2008 lookup protocol.
 */
const IDNA2_CHECK_2008_LOOKUP = 0;

/**
 * Check requirements as specified by the IDNA 2008 registration protocol.
 */
const IDNA2_CHECK_2008_REGIST = 0;

/**
 * Apply the IDNA 2003 Nameprep algorithm.
 */
const IDNA2_MAP_NAMEPREP = 0;

/**
 * Normalize strings with the Unicode Normalization form KC.
 */
const IDNA2_MAP_NFKC = 0;

/**
 * Apply Unicode normalization form C and map characters to lower case.
 */
const IDNA2_MAP_NFCLOWER = 0;

/**
 * Apply the Punycode algorithm.
 */
const IDNA2_MAP_PUNYCODE = 0;

/**
 * Perform IDNA 2003 mapping procedures.
 */
const IDNA2_MAP_IDNA2003 = 0;

/**
 * Perform IDNA 2008 mapping procedures.
 */
const IDNA2_MAP_IDNA2008 = 0;

/**
 * Get UTF-8 encoded string from an array of Unicode codepoints.
 *
 * This is the reverse of {@link idna2_from_utf8()}.
 *
 * @param array $codepoints An array of integers representing Unicode code points.
 * @return string
 * @see idna2_from_utf8
 */
function idna2_to_utf8($codepoints) {}

/**
 * Convert an UTF-8 encoded string to an array of Unicode code points.
 *
 * @param string $label
 * @return array
 * @see idna2_to_utf8
 */
function idna2_from_utf8($label) {}

/**
 * Split a domain name into labels.
 *
 * The IDNA label delimiters '.', U+3002, U+FF0E, U+FF61 are considered.
 *
 * @param string $domain
 * @return array
 */
function idna2_labels($domain) {}

/**
 * Apply the Puncode algorithm to a UTF-8 string.
 *
 * Note that strings are considered without the "xn--" prefix here.
 *
 * This is the reverse of {@link idna2_punycode_decode()}
 *
 * @param string $label Unicode label
 * @return string|false
 */
function idna2_punycode_encode($label) {}

/**
 * Decode an ASCII string by way of the Puncode algorithm.
 *
 * Note that strings are considered without the "xn--" prefix here.
 *
 * This is the reverse of {@link idna2_punycode_encode()}
 *
 * @param string $label Encoded ASCII label
 * @return string|false
 */
function idna2_punycode_decode($label) {}

/**
 * Apply the Nameprep Algorithm (IDNA 2003).
 *
 * @param string $domain
 * @param boolean $allowUnassigned Whether to allow UNASSIGNED code points.
 * @return string
 */
function idna2_nameprep($domain,$allowUnassigned=false) {}

/**
 * IDNA 2003 ToAscii method.
 *
 * @param string $domain
 * @param boolean $allowUnassigned Whether to allow UNASSIGNED code points.
 * @param boolean $useStd3Ascii Whether to apply Std3ASCII rules.
 * @return string
 */
function idna2_to_ascii($domain,$allowUnassigned=false,$useStd3Ascii=true) {}

/**
 * IDNA 2003 ToUnicode method.
 *
 * @param string $domain
 * @param boolean $allowUnassigned Whether to allow UNASSIGNED code points.
 * @param boolean $useStd3Ascii Whether to apply Std3ASCII rules.
 * @return string
 */
function idna2_to_unicode($domain,$allowUnassigned=false,$useStd3Ascii=true) {}

/**
 * Generic IDNA 2008 domain encoding.
 *
 * @param string $domain
 * @return string
 */
function idna2_encode($domain) {}

/**
 * Generic IDNA 2008 domain decoding.
 *
 * @param string $domain
 * @return string
 */
function idna2_decode($domain) {}

/**
 * Test an UTF-8 encoded string against various IDNA requirements, specified by the flags parameter.
 *
 * The return value contains
 *
 * @param string $domain
 * @param int $flags
 * @return int
 * @see IDNA2_* constants 
 */
function idna2_test($domain,$flags) {}

/**
 * Get error message from last function call.
 *
 * @return string|false
 */
function idna2_error() {}






