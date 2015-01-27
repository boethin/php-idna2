<?php
/*
 * Copyright (C) 2011 Sebastian Boethin <sebastian@boethin.eu>
 *
 * This file is part of idna2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


abstract class Idna2Test
{

  abstract protected function _view($arg);
  abstract protected function _comp($arg1, $arg2);

  public function test($command, $expected, $expected_error = NULL)
  {
    eval("\$result = $command;");
    $msg = NULL;
    if ( $result === FALSE )
    {
      $error = idna2_error();
      if ( FALSE === $expected )
      {
        if ( $error !== $expected_error )
          $msg = "Expected error '$expected_error' doesn't match actual error '$error'";
      }
      else
        $msg = "got unexpected error:\n\t$error";
    }
    else if ( $expected === FALSE )
    {
      $msg = "error expected, but got result:\n\t$result";
    }
    else if ( !$this->_comp($result, $expected) )
    {
      $msg = "Expected result ".$this->_view($expected).
        "\ndoesn't match actual result ".$this->_view($result);
    }

    if (!is_null($msg))
    {
      print "Assertion failed:\n\t$command\n$msg\n\n";
      //debug_print_backtrace();
      exit(1);
    }
    //return $res;
  }

  private static function __IDNA2_ADDFLAG(&$s, $t, $f, $n) { if ( $f == ($t & $f) ) $s[] = $n; }

  public static function IDNA2_FLAGS($f)
  {
    $s = array();
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_ASCII, 'IDNA2_CHECK_ASCII');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_HYPHEN, 'IDNA2_CHECK_HYPHEN');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_HYPHEN34, 'IDNA2_CHECK_HYPHEN34');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_LDH, 'IDNA2_CHECK_LDH');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_LENGTH, 'IDNA2_CHECK_LENGTH');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_STD3, 'IDNA2_CHECK_STD3');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_PROHIBITED, 'IDNA2_CHECK_PROHIBITED');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_UNASSIGNED, 'IDNA2_CHECK_UNASSIGNED');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_COMBINING, 'IDNA2_CHECK_COMBINING');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_BIDI, 'IDNA2_CHECK_BIDI');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_NFC, 'IDNA2_CHECK_NFC');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_CONTEXTJ, 'IDNA2_CHECK_CONTEXTJ');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_CONTEXTO_REGIST, 'IDNA2_CHECK_CONTEXTO_REGIST');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_CONTEXTO_LOOKUP, 'IDNA2_CHECK_CONTEXTO_LOOKUP');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_2008_REGIST, 'IDNA2_CHECK_2008_REGIST');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_CHECK_2008_LOOKUP, 'IDNA2_CHECK_2008_LOOKUP');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_NAMEPREP, 'IDNA2_MAP_NAMEPREP');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_NFKC, 'IDNA2_MAP_NFKC');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_NFCLOWER, 'IDNA2_MAP_NFCLOWER');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_PUNYCODE, 'IDNA2_MAP_PUNYCODE');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_IDNA2003, 'IDNA2_MAP_IDNA2003');
    Idna2Test::__IDNA2_ADDFLAG($s, $f, IDNA2_MAP_IDNA2008, 'IDNA2_MAP_IDNA2008');
    return "(($f): ".implode(' | ', $s).')';
  }

}

// test strings
class Idna2Test_STR extends Idna2Test
{
  protected function _view($arg)
  {
    return "'$arg'";
  }

  protected function _comp($arg1, $arg2)
  {
    return (0 === strcmp($arg1,$arg2));
  }

}

// test arrays
class Idna2Test_ARRAY extends Idna2Test
{
  protected function _view($arg)
  {
    return '('.implode(',', $arg).')';
  }

  protected function _comp($arg1, $arg2)
  {
    if (count($arg1) != count($arg2))
      return FALSE;
    for ($i = 0; $i < count($arg1); $i++)
      if ($arg1[$i] != $arg2[$i])
        return FALSE;
    return TRUE;
  }

}

// test IDNA2_ flags
class Idna2Test_F extends Idna2Test
{
  protected function _view($arg)
  {
    return Idna2Test::IDNA2_FLAGS($arg);
  }
  
  protected function _comp($arg1, $arg2)
  {
    return ($arg1 === $arg2);
  }

}


if ( TRUE )
{

print "test: idna2_labels() ...";
$t = new Idna2Test_ARRAY();

$t->test('idna2_labels(NULL)',array());
$t->test('idna2_labels("")',array());
$t->test('idna2_labels("a")',array('a'));
$t->test('idna2_labels("a.b")',array('a','b'));
$t->test('idna2_labels("a\xef\xbd\xa1b")',array('a','b'));
$t->test('idna2_labels("a\xef\xbd\xa1b\xe3\x80\x82c")',array('a','b','c'));

print " OK.\n";


print "test: idna2_to_utf8() ...";
$t = new Idna2Test_STR();

$t->test('idna2_to_utf8(array())','');
$t->test('idna2_to_utf8(array(0x61))','a');
$t->test('idna2_to_utf8(array(0x7f))',"\x7f");
$t->test('idna2_to_utf8(array(0xFF23))',"\xef\xbc\xa3");
$t->test('idna2_to_utf8(array(0x45,0xDF,0x7A,0xE4,0x74,0x74))',
  "E\xc3\x9fz\xc3\xa4tt");
$t->test('idna2_to_utf8(array(0x3BD9,0x3BDC,0x3BD9,0x3BDF))',
  "\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f");

// $t->test('idna2_to_utf8(array(0x0628,0x200C,0x0627,0x200C,0x0627))',
//   "\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f");



print " OK.\n";

print "test: idna2_from_utf8() ...";
$t = new Idna2Test_ARRAY();

$t->test('idna2_from_utf8(NULL)',array());
$t->test('idna2_from_utf8("")',array());
$t->test('idna2_from_utf8("a")',array(0x61));
$t->test('idna2_from_utf8("\x7f")',array(0x7F));
//$t->test('idna2_from_utf8("\x80")',array(0x80));
$t->test('idna2_from_utf8("\xef\xbc\xa3")',array(0xFF23));
$t->test('idna2_from_utf8("E\xc3\x9fz\xc3\xa4tt")',
  array(0x45,0xDF,0x7A,0xE4,0x74,0x74));
$t->test('idna2_from_utf8("\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f")',
  array(0x3BD9,0x3BDC,0x3BD9,0x3BDF));
$t->test('idna2_from_utf8("\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f")',
  array(0x3BD9,0x3BDC,0x3BD9,0x3BDF));

;
  

print " OK.\n";

print "test: idna2_punycode_encode() ...";
$t = new Idna2Test_STR();

$t->test('idna2_punycode_encode(NULL)','');
$t->test('idna2_punycode_encode("")','');
$t->test('idna2_punycode_encode("Ascii")','Ascii-');
$t->test('idna2_punycode_encode("\xc2\x80")',"a");
$t->test('idna2_punycode_encode("B\xc3\xb6thin")','Bthin-jua');
$t->test('idna2_punycode_encode("\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f")','domain');

print " OK.\n";

print "test: idna2_punycode_decode() ...";
$t = new Idna2Test_STR();

$t->test('idna2_punycode_decode(NULL)','');
$t->test('idna2_punycode_decode("")','');
$t->test('idna2_punycode_decode("a")',"\xc2\x80");
$t->test('idna2_punycode_decode("b")',FALSE,
  '(idna2_punycode_decode) decoding failed: (punycode_decode) Invalid input');
$t->test('idna2_punycode_decode("Bthin-jua")',"B\xc3\xb6thin");
$t->test('idna2_punycode_decode("domain")',"\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f");

print " OK.\n";

print "test: idna2_test() ...";
$t = new Idna2Test_F();

//$t->test('idna2_test(NULL,-1)', IDNA2_CHECK_LENGTH);
//$t->test('idna2_test("",-1)', IDNA2_CHECK_LENGTH);
$t->test('idna2_test(".",-1)', 0);
$t->test('idna2_test("..",-1)', IDNA2_CHECK_LENGTH);
$t->test('idna2_test(".x",-1)', IDNA2_CHECK_LENGTH);
$t->test('idna2_test("x.",-1)', 0);
$t->test('idna2_test("-",IDNA2_CHECK_2008_REGIST)', IDNA2_CHECK_HYPHEN);
$t->test('idna2_test("-",IDNA2_CHECK_2008_LOOKUP)', 0);
$t->test('idna2_test(" ",-1)', IDNA2_CHECK_LDH | IDNA2_CHECK_PROHIBITED);
$t->test('idna2_test("ASCII",-1)', 0);
$t->test('idna2_test("ASCII/",-1)', IDNA2_CHECK_LDH | IDNA2_CHECK_PROHIBITED);
$t->test('idna2_test("a_.-bc.da--4",-1)', IDNA2_CHECK_LDH | IDNA2_CHECK_PROHIBITED | IDNA2_CHECK_HYPHEN | IDNA2_CHECK_HYPHEN34);
$t->test('idna2_test("xn--",-1)', IDNA2_CHECK_HYPHEN | IDNA2_CHECK_HYPHEN34);
$t->test('idna2_test("ab--c",-1)', IDNA2_CHECK_HYPHEN34);
$t->test('idna2_test("ab--c",IDNA2_CHECK_STD3)', 0);

$t->test('idna2_test("xn--a",-1)', IDNA2_CHECK_ASCII | IDNA2_CHECK_PROHIBITED);


$t->test('idna2_test("www.xn--domain.net",-1)', IDNA2_CHECK_ASCII);
$t->test('idna2_test("www.xn--bthin-jua.eu",-1)', IDNA2_CHECK_ASCII);
$t->test('idna2_test("www.b\xc3\xb6thin.eu",-1)', IDNA2_CHECK_ASCII);

// bidi
$t->test('idna2_test("a\xd8\xa7",-1)', IDNA2_CHECK_ASCII | IDNA2_CHECK_BIDI);

$t->test('idna2_test("\xef\xbc\xa3",IDNA2_CHECK_2008_LOOKUP)', IDNA2_CHECK_PROHIBITED);
$t->test('idna2_test("\xef\xbc\xa3",IDNA2_CHECK_ASCII)', IDNA2_CHECK_ASCII);
$t->test('idna2_test("\xef\xbc\xa3",IDNA2_CHECK_STD3)', 0);

$t->test('idna2_test("www.xn--b.com",-1)', IDNA2_MAP_PUNYCODE);
$t->test('idna2_test("www.xn--ä.com",-1)', IDNA2_CHECK_ASCII | IDNA2_CHECK_HYPHEN34);



#$utf8 = idna2_to_utf8(array(0x0628,0x200C,0x0627,0x200C,0x0627));
#$t->test("idna2_test(\"$utf8\",-1)", 0);



print " OK.\n";

print "test: idna2_nameprep() ...";
$t = new Idna2Test_STR();

$t->test('idna2_nameprep(NULL)','');
$t->test('idna2_nameprep("")','');
$t->test('idna2_nameprep("Aa")',"aa");
$t->test('idna2_nameprep("E\xc3\x9fzett")',"esszett");
$t->test('idna2_nameprep("\xc2\x80")',FALSE,
  '(idna2_nameprep) nameprep failed: (stringprep) Prohibited code points in input');
$t->test('idna2_nameprep("\xf4\x8f\xbf\xbf")',FALSE,
  '(idna2_nameprep) nameprep failed: (stringprep) Prohibited code points in input');
$t->test('idna2_nameprep("\xf2\xbf\xbe\x80",TRUE)',"\xf2\xbf\xbe\x80");
$t->test('idna2_nameprep("\xf2\xbf\xbe\x80",FALSE)',FALSE,
  '(idna2_nameprep) nameprep failed: (stringprep) Forbidden unassigned code points in input');
$t->test('idna2_nameprep("x\xc3\x96\xc3\x9b\xc3\xa9")',"x\xc3\xb6\xc3\xbb\xc3\xa9");

print " OK.\n";

print "test: idna2_to_ascii() ...";
$t = new Idna2Test_STR();

$t->test('idna2_to_ascii(NULL)','');
$t->test('idna2_to_ascii("")','');
$t->test('idna2_to_ascii(".")','.');
$t->test('idna2_to_ascii("-",FALSE,FALSE)','-');
$t->test('idna2_to_ascii("-",FALSE,TRUE)',FALSE,
  "(idna2_to_ascii) encoding failure: (to_ascii) Forbidden leading or trailing minus sign (`-')");
$t->test('idna2_to_ascii("Aa")',"Aa");
$t->test('idna2_to_ascii("E\xc3\x9fzett")',"esszett");
$t->test('idna2_to_ascii("www.B\xc3\x96THIN.eu")','www.xn--bthin-jua.eu');
$t->test('idna2_to_ascii("\xc3\xa4\xc3\xb6\xc3\xb3\xc3\xb4\xc3\xaa")',"xn--4cam0aei");
$t->test('idna2_to_ascii("x\xc3\x96\xc3\x9b\xc3\xa9.com")',"xn--x-bga2bu.com");

print " OK.\n";


print "test: idna2_to_unicode() ...";
$t = new Idna2Test_STR();

$t->test('idna2_to_unicode(NULL)','');
$t->test('idna2_to_unicode("")','');
$t->test('idna2_to_unicode("www.xn--bthin-jua.eu")',"www.b\xc3\xb6thin.eu");
$t->test('idna2_to_unicode("www.xn--domain.net")',"www.\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f.net");
$t->test('idna2_to_unicode("xn--")','xn--');

print " OK.\n";


print "test: idna2_encode() ...";
$t = new Idna2Test_STR();

$t->test('idna2_encode(NULL)','');
$t->test('idna2_encode("")','');
$t->test('idna2_encode("\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f")',"xn--domain");
$t->test('idna2_encode("E\xc3\x9fz\xc3\x84tT.DE")',"xn--eztt-vna5a.de");

print " OK.\n";


print "test: idna2_decode() ...";
$t = new Idna2Test_STR();

$t->test('idna2_decode(NULL)','');
$t->test('idna2_decode("")','');
$t->test('idna2_decode("xn--domain")',"\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f");
$t->test('idna2_decode("xn--eztt-vna5a.de")',"e\xc3\x9fz\xc3\xa4tt.de");

print " OK.\n";



}

?>
