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

use FredEmmott\DefinitionFinder\ScannedConstant;

/**
 * Renders a constant.
 */
class :hphpdoc:constant extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :xhp:html-element,
        ScannedConstant constant @required;

    protected function render(): XHPRoot
    {
        $m = $this->:constant;
        $sc = $this->getContext('scannedClass');
        invariant($sc instanceof \FredEmmott\DefinitionFinder\ScannedBase, "scannedClass context value must be a ScannedBase");
        $parser = $this->getContext('docParser');
        if (!($parser instanceof Hphpdoc\Doc\Parser)) {
            $parser = new Hphpdoc\Doc\Parser();
        }
        $phpdoc = $parser->parse($m);
        $summary = trim($phpdoc->getSummary());
        $rt = Vector{$m->getTypehint()};
        foreach ($phpdoc->getTags() as $t) {
            if ($t->getName() === 'var' && $t instanceof Hphpdoc\Doc\TypedTag) {
                if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                    $rt = $t->getTypes();
                }
                if (strlen($summary) === 0) {
                    $summary = $t->getDescription();
                }
                break;
            }
        }
        return <section id={"constant_" . $m->getName()} class="constant-section">
            <header>
                <h1>{$m->getName()}</h1>
            </header>
            <div class="constant-signature">
                <code class="constant-type">
                    <hphpdoc:typehints tokens={$rt}/>
                </code>
                {" "}
                <code class="constant-name">{$m->getName()}</code>
                <code class="separator-constant">{" = "}</code>
                <code class="constant-value">{$m->getValue()}</code>
            </div>
            <div class="constant-details">
                <p class="constant-summary">{$summary}</p>
                <div class="constant-description">
                    <axe:paragraphs text={$phpdoc->getDescription()}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
            </div>
        </section>;
    }
}
