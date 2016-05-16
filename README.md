# hphpdoc
API Documentation Generator for Hack/HHVM and PHP

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
