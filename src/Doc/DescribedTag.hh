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
 * Any tag that includes only a typehint and a description.
 */
class DescribedTag extends Tag
{
    private string $description;

    /**
     * Creates a new DescribedTag.
     *
     * @param $name - The tag name
     */
    public function __construct(string $name, string $description)
    {
        parent::__construct($name);
        $this->description = trim($description);
    }

    /**
     * Gets the tag description.
     *
     * @return - The tag description
     */
    public function getDescription(): string
    {
        return $this->description;
    }

    /**
     * {@inheritDoc}
     */
    public function __toString(): string
    {
        return parent::__toString() . ' ' . $this->getDescription();
    }
}
