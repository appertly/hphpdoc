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
 * Any tag that includes a typehint, a name, and a description.
 */
class ParameterTag extends TypedTag
{
    private static ImmSet<string> $tagNames = ImmSet{'var', 'param', 'property'};

    private string $variable;

    /**
     * Creates a new ParameterTag.
     *
     * @param $name - The tag name
     * @param $type - The typehint
     * @param $variable - The variable name
     * @param $description - The tag description
     */
    public function __construct(string $name, Traversable<ScannedTypehint> $type, string $variable, string $description)
    {
        parent::__construct($name, $type, $description);
        $this->variable = substr($variable, 0, 1) === '$' ? substr($variable, 1) : $variable;
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
     * Gets the tag variable name
     *
     * @return - The tag variable name
     */
    public function getVariable(): string
    {
        return $this->variable;
    }

    /**
     * {@inheritDoc}
     */
    public function isNeeded(\ConstVector<Tag> $tags): bool
    {
        if ($this->getName() !== 'param') {
            return parent::isNeeded($tags);
        }
        foreach ($tags as $tag) {
            if ($tag instanceof ParameterTag &&
                $tag->getName() === 'param' &&
                $tag->getVariable() === $this->variable) {
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
            implode('|', $this->getTypes()->map($a ==> $a->getTypeText())) .
            ' $' . $this->variable . ' ' .
            $this->getDescription();
    }
}
