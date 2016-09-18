# hphpdoc
API Documentation Generator for Hack/HHVM and PHP

Check out some example documentation: actually, [it's our own API](https://appertly.github.io/hphpdoc/api/). Please have a look.

[![Packagist](https://img.shields.io/packagist/v/appertly/hphpdoc.svg)](https://packagist.org/packages/appertly/hphpdoc)
[![Build Status](https://travis-ci.org/appertly/hphpdoc.svg)](https://travis-ci.org/appertly/hphpdoc)

## Installation

You can install this library using Composer:

```console
$ composer require appertly/hphpdoc
```

* The master branch (version 0.x) of this project requires HHVM 3.12 and depends
  on `appertly/axe`, `appertly/cleopatra`, and `fredemmott/definition-finder`.

## Compliance

Releases of this library will conform to [Semantic Versioning](http://semver.org).

Our code is intended to comply with [PSR-1](http://www.php-fig.org/psr/psr-1/), [PSR-2](http://www.php-fig.org/psr/psr-2/), and [PSR-4](http://www.php-fig.org/psr/psr-4/). If you find any issues related to standards compliance, please send a pull request!

## Usage

You can view a list of all command line options with the `-h` or `--help` flags, or by simply calling the executable on its own.

```
Usage:  hphpdoc [options] [--] [args...]

hphpdoc generates API documentation for Hack and PHP files.

Options:
  -h --help     Show this help screen
  --version     Displays version information
  -v --verbose  Increases verbosity level (can be used more than once, e.g.
                -vvv)
  -q --quiet    Prevents any output except errors; supercedes the verbose
                setting
  -x --exclude  Excludes specific files and folders from scanning (can be used
                more than once), wildcards are not supported
  -o --output   Specifies the directory for generated documentation; defaults
                to PWD
```

For example:

```
hphpdoc -v -x tests -o build/api .
```
## PHPDoc Syntax

For the most part, we're trying to cover everything in
[PSR-5](https://github.com/phpDocumentor/fig-standards/blob/master/proposed/phpdoc.md).
This initial release covers many tags, but not all of them just yet.

Being that Hack has more strict typehints than PHP 5, you can omit types from your `@var`, `@param`, and `@return` PHPDoc tags if you choose!

```hack
/**
 * @var You can omit the type here, or…
 */
protected string $something = "nothing";
/**
 * …you can just specify a description here. The type is detected automatically!
 */
protected Vector<string> $test = Vector{'foo'};
/**
 * This is my method.
 *
 * This is a description.
 *
 * @param $name - The name
 * @param $numbers - The numbers
 * @param $log - The log
 * @return - The thing you need. Always specify a dash before description.
 * @throws \RuntimeException if anything goes wrong
 */
public function getFoo(string $name, ConstVector<int> $numbers, Psr\Log\LoggerInterface $log): ?Foo<Bar>
{
    return null;
}
```

If your return type or parameter typehint is `mixed`, hphpdoc will fall back to the PHPDoc tag type, if one is specified. Example:

```hack
/**
 * @return array<string>|string|null The return type
 */
function nope(): mixed
{
    return null;
}
```
