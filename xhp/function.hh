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
use Hphpdoc\Source\FunctionDeclaration;
use Hphpdoc\Source\FunctionyDeclaration;

/**
 * Renders a function.
 */
class :hphpdoc:function extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use MarkdownHelper;
    use FunctionHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        FunctionDeclaration function @required;

    protected function render(): XHPRoot
    {
        $m = $this->:function;
        $token = $m->getToken();
        $name = $token->getShortName();
        $mdParser = $this->getMarkdownParser();
        $phpdoc = $m->getDocBlock();
        $labels = <p class="method-labels"/>;
        return <section id={"function_$name"} class="method-section">
            <header>
                <h1>{$name}</h1>
                {$labels}
            </header>
            <div class="signature function-signature">
                {"function "}
                <code class="method-name">{$name}</code>
                <hphpdoc:generics generics={$token->getGenericTypes()}/>
                <hphpdoc:parameters member={$m}/>
            </div>
            <div class="method-details">
                <div class="method-summary"><axe:markdown text={$phpdoc->getSummary()} docParser={$mdParser}/></div>
                <div class="method-description">
                    <axe:markdown text={$phpdoc->getDescription()} docParser={$mdParser}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
                {$this->getParameters($m, $mdParser)}
                {$this->getReturns($m, $mdParser)}
                {$this->getThrows($m, $mdParser)}
            </div>
        </section>;
    }
}
