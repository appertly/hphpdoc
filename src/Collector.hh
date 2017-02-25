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

use Facebook\DefinitionFinder\FileParser;
use Facebook\DefinitionFinder\TreeParser;
use Facebook\DefinitionFinder\ScannedBase;
use Psr\Log\LoggerInterface;
use Psr\Log\LoggerAwareTrait;

/**
 * Collects tokens
 */
class Collector
{
    use LoggerAwareTrait;

    /**
     * Create a new Collector.
     *
     * @param $logger - The logger
     */
    public function __construct(LoggerInterface $logger)
    {
        $this->logger = $logger;
    }

    /**
     * Collect all tokens
     *
     * @param $arguments - The list of files and directories to scan
     * @param $excludes - The list of files and directories to exclude
     * @return - All tokens found
     */
    public function collect(\ConstVector<string> $arguments, \ConstSet<string> $excludes): \ConstVector<ScannedBase>
    {
        $log = $this->logger;
        invariant($log !== null, "Log cannot be null");
        $tokens = Vector{};
        foreach ($arguments as $argument) {
            if ($excludes->contains($argument)) {
                $log->debug("Excluding file: $argument");
                continue;
            }
            foreach ($this->parsePath($argument, $log) as $token) {
                $omit = false;
                foreach ($excludes as $exclude) {
                    if (substr($token->getFileName(), 0, strlen($exclude)) === $exclude) {
                        $omit = true;
                        $log->debug("Excluding token in file: " . $token->getFileName());
                        break;
                    }
                }
                if (!$omit) {
                    $tokens[] = $token;
                }
            }
        }
        // https://github.com/facebook/hhvm/issues/7037
        /* HH_IGNORE_ERROR[1002]: Bug in the type checker! */
        usort($tokens, ($a, $b) ==> $a->getName() <=> $b->getName());
        return $tokens;
    }

    protected function parsePath(string $argument, LoggerInterface $log): \ConstVector<ScannedBase>
    {
        $file = new \SplFileInfo($argument);
        $realPath = $file->isReadable() ? $file->getRealPath() : $argument;
        $tokens = Vector{};
        if ($file->isReadable()) {
            $log->info("Including path: $realPath");
            $parser = $file->isDir() ?
                TreeParser::FromPath($realPath) :
                FileParser::FromFile($realPath);

            $constants = $parser->getConstants();
            $log->info("Found " . count($constants) . " constants");
            $tokens->addAll($constants);

            $functions = $parser->getFunctions();
            $log->info("Found " . count($functions) . " functions");
            $tokens->addAll($functions);

            $interfaces = $parser->getInterfaces();
            $log->info("Found " . count($interfaces) . " interfaces");
            $tokens->addAll($interfaces);

            $traits = $parser->getTraits();
            $log->info("Found " . count($traits) . " traits");
            $tokens->addAll($traits);

            $classes = $parser->getClasses();
            $log->info("Found " . count($classes) . " classes");
            $tokens->addAll($classes);

            $enums = $parser->getEnums();
            $log->info("Found " . count($enums) . " enums");
            $tokens->addAll($enums);

            $types = $parser->getTypes();
            $log->info("Found " . count($types) . " types");
            $tokens->addAll($types);

            $newtypes = $parser->getNewtypes();
            $log->info("Found " . count($types) . " newtypes");
            $tokens->addAll($newtypes);
        } else {
            $log->warning("File not readable: $realPath");
        }
        return $tokens;
    }
}
