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

use FredEmmott\DefinitionFinder\ScannedTypehint;

/**
 * Any tag that includes only a typehint and a description.
 */
class TypedTag extends DescribedTag
{
    private static ImmSet<string> $tagNames = ImmSet{'return', 'throws'};

    private ImmVector<ScannedTypehint> $types;

    /**
     * Creates a new TypedTag.
     *
     * @param $name - The tag name
     * @param $type - The typehint
     * @param $description - The tag description
     */
    public function __construct(string $name, Traversable<ScannedTypehint> $type, string $description)
    {
        parent::__construct($name, $description);
        $this->types = new ImmVector($type);
        $types = Vector{};
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
    public function getTypes(): ImmVector<ScannedTypehint>
    {
        return $this->types;
    }

    /**
     * {@inheritDoc}
     */
    public function isNeeded(\ConstVector<Tag> $tags): bool
    {
        if ($this->getName() === 'return') {
            return parent::isNeeded($tags);
        }
        foreach ($tags as $tag) {
            if ($tag instanceof TypedTag &&
                $tag->getName() === $this->getName() &&
                $tag->getTypes() == $this->types) {
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
        return '@' . $this->getName() . ' ' .
            implode('|', $this->types->map($a ==> $a->getTypeText())) . ' ' .
            $this->getDescription();
    }
}
