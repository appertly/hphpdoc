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
namespace Hphpdoc;

use Axe\Page;
use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedClass;
use FredEmmott\DefinitionFinder\ScannedEnum;
use FredEmmott\DefinitionFinder\ScannedMethod;
use FredEmmott\DefinitionFinder\ScannedProperty;
use FredEmmott\DefinitionFinder\ScannedTypehint;

/**
 * Contains helper methods for anything which generates content.
 */
trait Producer
{
    static private ImmSet<string> $primitives = ImmSet{'null', 'string', 'int',
        'integer', 'float', 'real', 'double', 'bool', 'boolean', 'resource',
        'array', 'arraykey', 'num', 'callable', 'mixed', 'true', 'false'};
    static private ImmSet<string> $thisy = ImmSet{'self', 'this', '$this',
        'static'};

    /**
     * Gets whether the provided type is primitive.
     *
     * @param $type - The type to test
     * @return true if the type is primitive.
     */
    protected function isPrimitive(string $type): bool
    {
        return self::$primitives->contains($type);
    }

    /**
     * Gets whether the provided typehint is void.
     *
     * @param $type - The typehint to test
     * @return true if the typehint is void
     */
    protected function isVoid(?ScannedTypehint $type): bool
    {
        return $type === null || $type->getTypeName() == 'void' || $type->getTypeName() == 'noreturn';
    }

    /**
     * Gets whether the provided typehint refers to `$this`
     *
     * @param $type - The typehint to test
     * @return true if the typehint is *thisy*
     */
    protected function isThisy(?ScannedTypehint $type): bool
    {
        return $type !== null && self::$thisy->contains($type->getTypeName());
    }

    /**
     * Gets a page builder.
     *
     * @return The page builder
     */
    protected function getPage(string $title): Page
    {
        $page = new Page();
        return $page->setTitle($title)
            ->addMeta('generator', 'hphpdoc ' . Version::VERSION)
            ->addStylesheet('styles.css');
    }

    /**
     * Cleans up a filename.
     *
     * @param $c - The scanned token
     * @return The cleaned up filename
     */
    protected function getFilename(ScannedBase $c): string
    {
        if ($c instanceof ScannedClass || $c instanceof ScannedEnum) {
            return str_replace("\\", '_', $c->getName()) . '.html';
        }
        throw new \InvalidArgumentException("Cannot generate a filename for this scanned token");
    }
}
