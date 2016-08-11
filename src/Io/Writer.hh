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
namespace Hphpdoc\Io;

use Psr\Log\LoggerAwareTrait;
use Psr\Log\LoggerInterface;

/**
 * Writes stuff to the filesystem.
 */
class Writer
{
    use LoggerAwareTrait;

    /**
     * Creates a new Writer.
     *
     * @param $logger - The logger
     */
    public function __construct(LoggerInterface $logger)
    {
        $this->logger = $logger;
    }

    /**
     * Copies a file from one place to another.
     *
     * @param $source - The source file
     * @param $destination - The destination
     */
    public function copy(string $source, string $destination): void
    {
        if (!file_exists($destination)) {
            $this->mkdir(dirname($destination));
        }
        try {
            copy($source, $destination);
        } catch (\Exception $e) {
            throw new IoException("Could not copy: $source to $destination", 0, $e);
        }
    }

    /**
     * Writes a file.
     *
     * @param $filename - The file to write
     * @param $contents - The contents to export
     * @throws \Hphpdoc\Io\IoException if anything goes wrong
     */
    public async function write(string $filename, \XHPRoot $contents): Awaitable<void>
    {
        $out = '';
        if (method_exists($contents, 'asyncToString')) {
            /* HH_IGNORE_ERROR[4053]: Yup */
            $out = await $contents->asyncToString();
        } else {
            $out = (string)$contents;
        }
        if (!file_exists($filename)) {
            $this->mkdir(dirname($filename));
        }
        $r = null;
        try {
            /* HH_IGNORE_ERROR[4064]: Logger gets set in constructor */
            $this->logger->debug("Writing to: $filename");
            file_put_contents($filename, $out);
        } catch (\Exception $e) {
            throw new IoException("Could not write: $filename", 0, $e);
        }
    }

    /**
     * Creates a directory; does nothing if path already exists
     *
     * @param $path - The path to create
     * @throws \Hphpdoc\Io\IoException if anything goes wrong or if $path is a file
     */
    public function mkdir(string $path): void
    {
        if (is_dir($path)) {
            return;
        } elseif (is_file($path)) {
            throw new IoException("Path exists but is not a directory: $path");
        }
        /* HH_IGNORE_ERROR[4064]: Logger gets set in constructor */
        $this->logger->debug("Creating directory: $path");
        try {
            mkdir($path, 0755, true);
        } catch (\Exception $e) {
            throw new IoException("Could not create $path", 0, $e);
        }
    }
}
