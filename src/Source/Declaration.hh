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
namespace Hphpdoc\Source;

use Hphpdoc\Doc\Block;

/**
 * A thing and its corresponding doc block
 */
interface Declaration
{
    /**
     * Gets the source filename.
     *
     * @return - The filename
     */
    public function getFilename(): string;

    /**
     * Gets the doc comment block.
     *
     * @return - The doc block, which could be empty
     */
    public function getDocBlock(): Block;
}
