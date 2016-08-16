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

use FredEmmott\DefinitionFinder\ScannedFunctionAbstract;

/**
 * Renders method parameters.
 */
class :hphpdoc:parameters extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase;
    children empty;
    attribute :xhp:html-element,
        Hphpdoc\Source\FunctionyDeclaration member @required;

    protected function render(): XHPRoot
    {
        $function = $this->:member;
        $tag = <span class="method-parameters"/>;
        $tag->appendChild(<span class="separator-parameter">{"("}</span>);
        $tags = $function->getParameterTags();
        $params = $function->getToken()->getParameters();
        foreach ($params as $i => $param) {
            $th = $param->getTypehint();
            $rt = Vector{$th};
            $t = $tags[$param->getName()];
            $description = $t->getDescription();
            $tht = $th?->getTypeText();
            if ($th === null || $th->getTypeName() === 'mixed' || $tht === 'array') {
                $rt = $t->getTypes();
            }
            if ($i > 0) {
                $tag->appendChild(<span class="separator-parameter">{", "}</span>);
            }
            $p = <code class="parameter"/>;
            $nullable = $param->isOptional() && $param->getDefaultString() === 'null' ? true : null;
            $typehint = count($rt) > 1 ? $param->getTypehint() : ($rt[0] ?? null);
            $p->appendChild(<hphpdoc:typehint token={$typehint} nullable={$nullable}/>);
            if ($param->isVariadic()) {
                $p->appendChild(<span class="variadic">...</span>);
            }
            $p->appendChild(" ");
            if ($param->isPassedByReference()) {
                $p->appendChild(<span class="parameter-reference">{"&"}"</span>);
            }
            $p->appendChild(<var class="parameter-name">{'$'}{$param->getName()}</var>);
            if ($param->isOptional()) {
                $p->appendChild(<kbd class="parameter-default">{" = "}{$param->getDefaultString()}</kbd>);
            }
            $tag->appendChild($p);
        }
        $tag->appendChild(<span class="separator-parameter">{")"}</span>);
        return $tag;
    }
}
