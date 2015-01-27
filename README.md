# php-idna2
PHP IDNA extension

PHP extension for dealing with internationalized domain names, providing both IDNA 2003 and IDNA 2008 processing functions.

## Description

Dealing with Internationalized Domain Names (IDN) basically means to encode Non-ASCII domain labels into ASCII-compatible encoding (prefixed with "xn--") and vice versa. For example, the domain name name *www.böthin.de* is encoded to *www.xn--bthin-jua.de* by way of the well known Punycode algorithm. However, the question of how to consider arbitrary Unicode strings as domain labels in general is more complex and has to do with normalization processes and Unicode character classifications.

This extension makes the functionallity of two C-librarys available: 
- GNU Libidn (IDNA 2003) and 
- Idnkit-2 from the JPRS registry (IDNA 2008).

There is an online [IDN tool](http://xn--domain.net) based on this extension.

Note:

This is an experimantal tool suitable for dealing with advanced IDNA tasks. For more simple tasks, there are other solutions for handling IDN in PHP, for example [PHP's intl framework](http://php.net/manual/en/ref.intl.idn.php).

## Sample usage

```php
    // 㯙㯜㯙㯟 is the Punycode decoded string of "domain"
    $utf8 = idna2_to_utf8(array(0x3BD9,0x3BDC,0x3BD9,0x3BDF));
    assert($utf8 === "\xe3\xaf\x99\xe3\xaf\x9c\xe3\xaf\x99\xe3\xaf\x9f");
    $codepoints = idna2_from_utf8($utf8); // array(0x3BD9,0x3BDC,0x3BD9,0x3BDF)
    
    $punycode = idna2_punycode_encode($utf8);
    assert($punycode === "domain"); 
    assert($utf8 === idna2_punycode_decode($punycode));
    
    $encoded = idna2_encode("㯙㯜㯙㯟.net");
    assert("xn--domain.net" === $encoded);

```
    

See the [API documentation](http://boethin.github.io/php-idna2/phpdoc/packages/Default.html) for an overview.

## Prerequisites

- Install the [GNU Libidn library](http://www.gnu.org/software/libidn/)
- Install the [idnkit-2.3 library](http://jprs.co.jp/idn/index-e.html)
- You need the PHP5 developer tools (phpize) in order to build the extension.

## Installation

Within the source directory, the following should work:

    $ phpize
    $ ./configure
    $ make
    $ make install
    
After successful installation you should have *idna2.so* within your php extension directory. In order to make this extension usable to your php code, add the following line to your php.ini:

    extension=idna2.so







