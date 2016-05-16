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

/**
 * Superclass for any parsed PHPDoc.
 *
 * Implementors are meant to be immutable.
 *
 * @copyright 2016 Appertly
 * @license   Apache-2.0
 */
class Tag
{
    /**
     * Creates a new Tag.
     *
     * @param $name - The type of tag
     */
    public function __construct(private string $name)
    {
    }

    /**
     * Gets the name of the tag
     *
     * @return The name
     */
    public function getName(): string
    {
        return $this->name;
    }

    /**
     * Returns a string representation.
     *
     * @return The string representation
     */
    public function __toString(): string
    {
        return "@" . $this->name;
    }
}
