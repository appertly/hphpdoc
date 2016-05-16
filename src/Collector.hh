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

use FredEmmott\DefinitionFinder\FileParser;
use FredEmmott\DefinitionFinder\TreeParser;
use FredEmmott\DefinitionFinder\ScannedBase;
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
     * @return All tokens found
     */
    public function collect(\ConstVector<string> $arguments, \ConstSet<string> $excludes): \ConstVector<ScannedBase>
    {
        $tokens = $this->getTokens($arguments, $excludes);
        $rtokens = Vector{};
        foreach ($tokens as $class) {
            $omit = false;
            foreach ($excludes as $exclude) {
                if (substr($class->getFileName(), 0, strlen($exclude)) === $exclude) {
                    $omit = true;
                    /* HH_IGNORE_ERROR[4064]: We set logger in constructor */
                    $this->logger->debug("Excluding token in file: " . $class->getFileName());
                    break;
                }
            }
            if (!$omit) {
                $rtokens[] = $class;
            }
        }
        return $rtokens;
    }

    /**
     * Gets all tokens found in the provided files
     *
     * @param $arguments - The list of files and directories to scan
     * @param $excludes - The list of files and directories to exclude
     * @return The scanned tokens
     */
    protected function getTokens(\ConstVector<string> $arguments, \ConstSet<string> $excludes): \ConstVector<ScannedBase>
    {
        $tokens = Vector{};
        foreach ($arguments as $argument) {
            if ($excludes->contains($argument)) {
                /* HH_IGNORE_ERROR[4064]: We set logger in constructor */
                $this->logger->debug("Excluding file: $argument");
                continue;
            }
            $file = new \SplFileInfo($argument);
            $realPath = $file->isReadable() ? $file->getRealPath() : $argument;
            if ($file->isReadable()) {
                /* HH_IGNORE_ERROR[4064]: We set logger in constructor */
                $this->logger->info("Including path: $realPath");
                $parser = $file->isDir() ?
                    TreeParser::FromPath($realPath) :
                    FileParser::FromFile($realPath);
                $tokens->addAll($parser->getClasses());
                $tokens->addAll($parser->getInterfaces());
                $tokens->addAll($parser->getTraits());
                $tokens->addAll($parser->getFunctions());
                $tokens->addAll($parser->getConstants());
                $tokens->addAll($parser->getEnums());
                $tokens->addAll($parser->getTypes());
                $tokens->addAll($parser->getNewtypes());
            } else {
                /* HH_IGNORE_ERROR[4064]: We set logger in constructor */
                $this->logger->warning("File not readable: $realPath");
            }
        }
        // https://github.com/facebook/hhvm/issues/7037
        /* HH_IGNORE_ERROR[1002]: Bug in the type checker! */
        usort($tokens, ($a, $b) ==> $a->getName() <=> $b->getName());
        return $tokens;
    }
}
