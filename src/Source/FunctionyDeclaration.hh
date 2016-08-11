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

use FredEmmott\DefinitionFinder\ScannedFunctionAbstract;
use Hphpdoc\Doc\ParameterTag;
use Hphpdoc\Doc\TypedTag;

/**
 * A thing and its corresponding doc block
 */
interface FunctionyDeclaration
{
    /**
     * Gets the scanned token.
     *
     * @return - The scanned token
     */
    public function getToken(): ScannedFunctionAbstract;

    /**
     * Gets the parameter tags.
     *
     * @return - The parameter tags
     */
    public function getParameterTags(): \ConstMap<string,ParameterTag>;

    /**
     * Gets the list of exceptions thrown.
     *
     * @return - The throws tags
     */
    public function getThrowsTags(): \ConstVector<TypedTag>;

    /**
     * Gets the returns tag.
     *
     * @return - The return tag
     */
    public function getReturnsTag(): ?TypedTag;
}
