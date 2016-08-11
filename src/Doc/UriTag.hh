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
 * Any tag that includes a URI or a symbol and a description.
 */
class UriTag extends DescribedTag
{
    private static ImmSet<string> $tagNames = ImmSet{'license', 'see', 'example'};

    private string $uri;

    /**
     * Creates a new UriTag.
     *
     * @param $name - The tag name
     * @param $uri - The URI or symbol
     * @param $description - The tag description
     */
    public function __construct(string $name, string $uri, string $description)
    {
        parent::__construct($name, $description);
        $this->uri = $uri;
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
     * Gets the tag types
     *
     * @return - The tag types
     */
    public function getUri(): string
    {
        return $this->uri;
    }

    /**
     * {@inheritDoc}
     */
    public function isNeeded(\ConstVector<Tag> $tags): bool
    {
        if ($this->getName() !== 'license') {
            return false;
        }
        foreach ($tags as $tag) {
            if ($tag instanceof UriTag && $tag->getName() === 'license' &&
                $tag->getUri() === $this->uri) {
                return false;
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public function __toString(): string
    {
        return '@' . $this->getName() . ' ' . $this->uri . ' ' .
            $this->getDescription();
    }
}
