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

use Psr\Log\LogLevel;
use Psr\Log\LoggerInterface;
use Psr\Log\LoggerTrait;

/**
 * PSR-3 Logger that sends to STDERR
 */
class StderrLogger implements LoggerInterface
{
    use LoggerTrait;

    private int $level;

    private static ImmMap<string,int> $rankings = ImmMap{
        LogLevel::EMERGENCY => 0,
        LogLevel::ALERT     => 1,
        LogLevel::CRITICAL  => 2,
        LogLevel::ERROR     => 3,
        LogLevel::WARNING   => 4,
        LogLevel::NOTICE    => 5,
        LogLevel::INFO      => 6,
        LogLevel::DEBUG     => 7,
    };

    /**
     * Create a new Logger
     *
     * @param $level - The log level to display
     */
    public function __construct(mixed $level)
    {
        if (is_int($level)) {
            $this->level = min(max($level, 0), 7);
        } else {
            $this->level = self::$rankings[(string)$level] ?? 8;
        }
    }

    /**
     * Logs with an arbitrary level.
     *
     * @param $level - Log level
     * @param $message - String message
     * @param $context - Optional context
     */
    /* HH_IGNORE_ERROR[4032]: Need to be compatible with PHP 7 */
    public function log(mixed $level, $message, array<string,mixed> $context = []): void
    {
        if (!array_key_exists($level, self::$rankings)) {
            throw new \Psr\Log\InvalidArgumentException("Unknown log level: " . (string)$level);
        }
        $ilevel = self::$rankings[(string)$level];
        if ($ilevel <= $this->level) {
            $smessage = $message;
            if (array_key_exists('exception', $context)) {
                $e = $context['exception'];
                if ($e instanceof \Exception) {
                    $smessage .= PHP_EOL;
                    while ($e !== null) {
                        $smessage .= "  " . get_class($e) . ': ' . $e->getMessage() . PHP_EOL;
                        foreach ($e->getTrace() as $i => $trace) {
                            $className = $trace['class'] ?? '';
                            $smessage .= "    #$i " . ($trace['file'] ?? '') . '(' . ($trace['line'] ?? '') . '): ' .
                                ($className ? "$className->" : '') . $trace['function'] . '()' . PHP_EOL;
                        }
                        $e = $e->getPrevious();
                    }
                }
                unset($context['exception']);
            }
            $smessage = sprintf('[%s] %s %s', date('Y-m-d H:i:s'), strtoupper($level), $smessage);
            if (count($context) > 0) {
                $smessage .= ' ' . json_encode($context);
            }
            fwrite(STDERR, $smessage . PHP_EOL);
        }
    }
}
