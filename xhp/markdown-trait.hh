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

use League\CommonMark\DocParser;
use League\CommonMark\Environment;

/**
 * Helper to be used by XHP that needs the Markdown parser.
 */
trait MarkdownHelper
{
    require implements XHPRoot;

    /**
     * Gets the Markdown parser
     *
     * @return - The Markdown parser
     */
    protected function getMarkdownParser(): DocParser
    {
        $mdParser = $this->getContext('markdownParser');
        if (!($mdParser instanceof DocParser)) {
            $mdParser = new DocParser(Environment::createCommonMarkEnvironment());
        }
        return $mdParser;
    }
}
