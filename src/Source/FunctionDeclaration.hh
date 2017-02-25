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

use Facebook\DefinitionFinder\ScannedFunction;
use Hphpdoc\Doc\Block;
use Hphpdoc\Doc\ParameterTag;
use Hphpdoc\Doc\TypedTag;

/**
 * A declared namespace function.
 */
class FunctionDeclaration implements FunctionyDeclaration, TokenDeclaration<ScannedFunction>
{
    private ImmMap<string,ParameterTag> $tags;
    private ImmVector<TypedTag> $throws;
    private ?TypedTag $returns;

    /**
     * Creates a new MethodDeclaration.
     *
     * @param $function - The method
     * @param $class - The interface, trait, or class
     * @param $block - The doc comment
     */
    public function __construct(
        private ScannedFunction $function,
        private Block $block,
    ) {
        $pnamed = Map{};
        $tags = $block->getTags() ?? ImmVector{};
        $params = $function->getParameters();
        foreach ($params as $p) {
            $name = $p->getName();
            foreach ($tags as $t) {
                if ($t->getName() === 'param' && $t instanceof ParameterTag && $t->getVariable() == $name) {
                    $pnamed[$name] = $t;
                    break;
                }
            }
            if (!$pnamed->containsKey($name)) {
                $th = $p->getTypehint();
                $pnamed[$name] = new ParameterTag('param', $th ? Vector{$th} : Vector{}, $name, '');
            }
        }
        $this->tags = $pnamed->immutable();
        /* HH_IGNORE_ERROR[4110]: This will only give TypedTag */
        $this->throws = $tags->filter($a ==> $a instanceof TypedTag && $a->getName() === 'throws');
        /* HH_IGNORE_ERROR[4110]: This will only give TypedTag */
        $this->returns = $tags->filter($a ==> $a instanceof TypedTag && $a->getName() === 'return')->firstValue();
    }

    /**
     * {@inheritDoc}
     */
    public function getFilename(): string
    {
        return $this->function->getFileName();
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
        return $this->function->getName();
    }

    /**
     * {@inheritDoc}
     */
    public function getToken(): ScannedFunction
    {
        return $this->function;
    }

    /**
     * {@inheritDoc}
     */
    public function getParameterTags(): \ConstMap<string,ParameterTag>
    {
        return $this->tags;
    }

    /**
     * {@inheritDoc}
     */
    public function getThrowsTags(): \ConstVector<TypedTag>
    {
        return $this->throws;
    }

    /**
     * {@inheritDoc}
     */
    public function getReturnsTag(): ?TypedTag
    {
        return $this->returns;
    }
}
