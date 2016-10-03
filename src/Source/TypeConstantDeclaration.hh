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

use FredEmmott\DefinitionFinder\ScannedClass;
use FredEmmott\DefinitionFinder\ScannedTypeConstant;
use FredEmmott\DefinitionFinder\ScannedTypehint;
use Hphpdoc\Doc\Block;
use Hphpdoc\Doc\TypedTag;

/**
 * A declared class type constant.
 */
class TypeConstantDeclaration implements TokenDeclaration<ScannedTypeConstant>
{
    private string $summary;
    private ?ScannedTypehint $type;

    /**
     * Creates a new ConstantDeclaration.
     *
     * @param $constant - The property
     * @param $block - The doc comment
     * @param $class - Optional. The interface, trait, or class
     */
    public function __construct(
        private ScannedTypeConstant $constant,
        private Block $block,
        private ?ScannedClass $class = null,
    ) {
        $rt = Vector{$constant->getValue()};
        $summary = trim($block->getSummary());
        foreach ($block->getTags() as $t) {
            if ($t->getName() === 'var' && $t instanceof TypedTag) {
                if (strlen($summary) === 0) {
                    $summary = $t->getDescription();
                }
                break;
            }
        }
        $this->summary = $summary;
        $this->type = $rt->firstValue();
    }

    /**
     * {@inheritDoc}
     */
    public function getFilename(): string
    {
        return $this->constant->getFileName();
    }

    /**
     * {@inheritDoc}
     */
    public function getDocBlock(): Block
    {
        return $this->block;
    }

    /**
     * {@inheritDoc}
     */
    public function getName(): string
    {
        return $this->constant->getName();
    }

    /**
     * {@inheritDoc}
     */
    public function getToken(): ScannedTypeConstant
    {
        return $this->constant;
    }

    /**
     * Gets the containing class.
     *
     * @return - The containing class
     */
    public function getClass(): ?ScannedClass
    {
        return $this->class;
    }

    /**
     * Gets the best summary for the property.
     *
     * Either the PHPDoc summary, or the description of its `var` tag.
     *
     * @return - The summary
     */
    public function getSummary(): string
    {
        return $this->summary;
    }

    /**
     * Gets the best type hints for the property.
     *
     * Either the inline typehint, or if omitted or `mixed`, we use the tags.
     *
     * @return - The type hints
     */
    public function getType(): ?ScannedTypehint
    {
        return $this->type;
    }
}
