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
 * Any tag that includes a version number and a description.
 *
 * @copyright 2016 Appertly
 * @license   Apache-2.0
 */
class VersionTag extends DescribedTag
{
    private static ImmSet<string> $tagNames = ImmSet{'since', 'version', 'deprecated'};

    private string $version;

    /**
     * Creates a new UriTag.
     *
     * @param $name - The tag name
     * @param $version - The URI or symbol
     * @param $description - The tag description
     */
    public function __construct(string $name, string $version, string $description)
    {
        parent::__construct($name, $description);
        $this->version = $version;
    }

    /**
     * Whether the given tag name applies to this class.
     *
     * @param $tagName - The tag name to test
     * @return - whether the tag name applies
     */
    public static function applies(string $tagName): bool
    {
        return self::$tagNames->contains($tagName);
    }

    /**
     * Gets the version
     *
     * @return - The version
     */
    public function getVersion(): string
    {
        return $this->version;
    }

    /**
     * Returns a string representation.
     *
     * @return - The string representation
     */
    public function __toString(): string
    {
        return '@' . $this->getName() . ' ' . $this->version . ' ' .
            $this->getDescription();
    }
}
