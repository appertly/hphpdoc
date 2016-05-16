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
namespace Hphpdoc\Doc;

use FredEmmott\DefinitionFinder\ScannedBase;

/**
 * Parses doc comments
 */
class Parser
{
    private TagFactory $tagFactory;
    private Map<string,Block> $cache = Map{};

    /**
     * Creates a new Parser.
     *
     * @param $tagFactory - Optional. The factory to use.
     */
    public function __construct(?TagFactory $tagFactory = null)
    {
        $this->tagFactory = $tagFactory === null ? new TagFactory() : $tagFactory;
    }

    /**
     * Parses a doc comment.
     *
     * @param $token - The token whose doc comment will be evaluated
     * @return The parsed block
     */
    public function parse(?ScannedBase $token): Block
    {
        $comment = $this->extract((string)$token?->getDocComment());
        if ($this->cache->containsKey($comment)) {
            return $this->cache[$comment];
        }
        $opener = '';
        $description = '';
        $tags = Vector{};
        foreach (mb_split("\n", trim($comment)) as $line) {
            if (substr($line, 0, 1) === '@') {
                $tags[] = $line;
            } elseif (count($tags) > 0) {
                $tags[count($tags) - 1] .= " $line";
            } elseif (strlen($description) > 0) {
                $description .= "\n$line";
            } elseif (strlen($line) === 0) {
                $description = "\n";
            } elseif (strlen($opener) > 0 && substr($opener, -1, 1) === '.') {
                $description = $line;
            } else {
                $opener .= $line;
            }
        }
        $b = new Block($opener, $description, $this->tagFactory->convert($token, $tags));
        $this->cache[$comment] = $b;
        return $b;
    }

    /**
     * Extract the content from the comment.
     *
     * @param $comment - The comment to clean up
     * @return The extracted content
     */
    protected function extract(string $comment): string
    {
        // get rid of potential carriage returns
        $comment = str_replace("\r", "", $comment);
        // remove leading spaces
        $comment = preg_replace("/^\s+/m", "", $comment);
        // remove leading asterisks
        $comment = preg_replace("/^\*([\t ]+|$)/m", "", $comment);
        // remove leading slash
        $comment = preg_replace("#^/\*\*#", "", $comment);
        // remove ending slash
        return preg_replace("#\*/$#", "", $comment);
    }
}
