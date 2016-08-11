<?hh // strict
/**
 * Hphpdoc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @copyright 2016 Appertly
 * @license   Apache-2.0
 */
namespace Hphpdoc;

use Cleopatra\Command;
use Cleopatra\OptionSet;
use Cleopatra\Option;
use Cleopatra\Parser;
use Psr\Log\LoggerInterface;
use Psr\Log\LogLevel;

/**
 * Runs the generator
 */
class Runner
{
    private OptionSet $optionSet;

    /**
     * Creates a new Runner.
     */
    public function __construct()
    {
        $this->optionSet = new OptionSet(
            new Option("h|help", "Show this help screen"),
            new Option("version", "Displays version information"),
            new Option("v|verbose+", "Increases verbosity level (can be used more than once, e.g. -vvv)"),
            new Option("q|quiet", "Prevents any output except errors; supercedes the verbose setting"),
            new Option("x|exclude:s@", "Excludes specific files and folders from scanning (can be used more than once), wildcards are not supported"),
            new Option("o|output:s", "Specifies the directory for generated documentation; defaults to PWD")
        );
    }

    /**
     * Prints the version information
     */
    protected function version(): void
    {
        echo "hphpdoc ", Version::VERSION, " Copyright 2016 Appertly", PHP_EOL;
    }

    /**
     * Prints the help information
     */
    protected function help(): void
    {
        echo "Usage:  hphpdoc [options] [--] [args...]", PHP_EOL, PHP_EOL;
        echo "hphpdoc generates API documentation for Hack and PHP files.", PHP_EOL, PHP_EOL;
        echo "Options:", PHP_EOL;
        echo $this->optionSet->getHelp();
    }

    /**
     * Runs the program.
     *
     * @param $args - The command line arguments
     */
    public function run(Traversable<string> $args): void
    {
        set_error_handler(function (int $severity, string $message, string $file = '', int $line = 0) {
            if (error_reporting() & $severity) {
                throw new \ErrorException($message, 0, $severity, $file, $line);
            }
        });

        $cmd = $this->getCommand($args);
        $arguments = $cmd->getArguments();
        $options = $cmd->getOptions();

        $log = $this->getLogger($options);
        $log->debug("Command line options: " . json_encode($options));

        try {
            if ($options->containsKey('version')) {
                $this->version();
            } elseif ($options->containsKey('h') || $options->containsKey('help')) {
                $this->help();
            } elseif ($arguments->isEmpty()) {
                fwrite(STDERR, "No arguments specified; please specify the files or directories to scan" . PHP_EOL . PHP_EOL);
                $this->help();
            } else {
                $this->version();
                $excludes = $this->getExcludes($options);
                $log->info("Excluding paths: [ " . implode(", ", $excludes) . ' ]');
                $collector = new Collector($log);
                $publisher = new Publisher($log);
                $publisher->publish(
                    (string)($options->get('o') ?? $options->get('output') ?? getcwd()),
                    $collector->collect($arguments, $excludes),
                );
            }
        } catch (\Exception $e) {
            $log->error($e->getMessage());
            $log->notice("An exception occurred", ['exception' => $e]);
            exit(1);
        }

        restore_error_handler();
    }

    /**
     * Gets the command.
     *
     * @param $args - The command line arguments
     * @return - The parsed command to execute
     */
    /* HH_IGNORE_ERROR[4110]: Exception catcher will exit */
    protected function getCommand(Traversable<string> $args): Command
    {
        try {
            $parser = new Parser($this->optionSet);
            return $parser->parse($args);
        } catch (\UnexpectedValueException $e) {
            $this->unknownOption($e);
        }
    }

    /**
     * Creates a new Logger based on passed options.
     *
     * @param $options - The options
     * @return - The created logger
     */
    protected function getLogger(ImmMap<string,mixed> $options): LoggerInterface
    {
        $level = 4;
        $verb = (int)$options->get('v') + (int)$options->get('verbose');
        if ($options->get('q') || $options->get('quiet')) {
            $verb = -1;
        }
        if ($verb > 0) {
            $level += $verb;
        }
        return new StderrLogger($level);
    }

    /**
     * Gets the list of excluded paths
     *
     * @param $options - The options
     * @return - The excluded paths
     */
    protected function getExcludes(ImmMap<string,mixed> $options): Set<string>
    {
        $excludes = Set{};
        $x = $options->get('x');
        if ($x instanceof Vector) {
            $excludes->addAll($x);
        }
        $x = $options->get('exclude');
        if ($x instanceof Vector) {
            $excludes->addAll($x);
        }
        return $excludes->map($e ==> realpath($e));
    }

    /**
     * Handles an unknown option exception.
     *
     * @param $e - The exception
     */
    protected function unknownOption(\UnexpectedValueException $e): noreturn
    {
        fwrite(STDERR, $e->getMessage());
        $this->help();
        exit(1);
    }
}
