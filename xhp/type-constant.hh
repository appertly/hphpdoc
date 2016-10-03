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

use Hphpdoc\Source\TypeConstantDeclaration;

/**
 * Renders a constant.
 */
class :hphpdoc:type-constant extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow, %sectioning;
    children empty;
    attribute :xhp:html-element,
        TypeConstantDeclaration constant @required;

    protected function render(): XHPRoot
    {
        $m = $this->:constant;
        $mdParser = $this->getMarkdownParser();
        $phpdoc = $m->getDocBlock();
        $summary = $m->getSummary();
        $rt = $m->getType();
        $inherit = null;
        $cd = $this->getContext('classyDeclaration');
        $labels = <p class="constant-labels"/>;
        $mclass = $m->getClass();
        if ($mclass !== null && $cd instanceof Hphpdoc\Source\ClassyDeclaration
                && $mclass->getName() !== $cd->getName()) {
            $th = new FredEmmott\DefinitionFinder\ScannedTypehint($mclass->getName(), Vector{}, false);
            $labels->appendChild(
                <span class="label">Inherited from <hphpdoc:typehint token={$th}/></span>
            );
        }
        return <section id={"type_constant_" . $m->getToken()->getShortName()} class="constant-section">
            <header>
                <h1>{$m->getToken()->getShortName()}</h1>
                {$labels}
            </header>
            <div class="signature constant-signature">
                {$m->getToken()->isAbstract() ? 'abstract ' : ''}
                {"const type "}
                <code class="constant-name">{$m->getToken()->getShortName()}</code>
                <code class="separator-constant">{" = "}</code>
                <code class="constant-type">
                    <hphpdoc:typehint token={$rt}/>
                </code>
            </div>
            <div class="constant-details">
                <div class="constant-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div>
                <div class="constant-description">
                    <axe:markdown text={$phpdoc->getDescription()} docParser={$mdParser}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
            </div>
        </section>;
    }
}
