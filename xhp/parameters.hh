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
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */

/**
 * Renders method parameters.
 */
class :hphpdoc:parameters extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase;
    children empty;
    attribute :xhp:html-element,
        ConstVector<FredEmmott\DefinitionFinder\ScannedParameter> params @required;

    protected function render(): XHPRoot
    {
        $tag = <span class="method-parameters"/>;
        $tag->appendChild(<span class="separator-parameter">{"("}</span>);
        foreach ($this->:params as $k => $param) {
            if ($k > 0) {
                $tag->appendChild(<span class="separator-parameter">{", "}</span>);
            }
            $p = <code class="parameter"/>;
            $p->appendChild(<hphpdoc:typehint token={$param->getTypehint()}/>);
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
