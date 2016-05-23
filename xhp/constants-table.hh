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
 * Renders a summary table of constants.
 */
class :hphpdoc:constants-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        ConstVector<FredEmmott\DefinitionFinder\ScannedConstant> constants @required;

    protected function render(): XHPRoot
    {
        $constants = $this->:constants;
        if (count($constants) === 0) {
            return <x:frag/>;
        }
        $parser = $this->getContext('docParser');
        if (!($parser instanceof Hphpdoc\Doc\Parser)) {
            $parser = new Hphpdoc\Doc\Parser();
        }
        $mdParser = $this->getContext('markdownParser');
        if (!($mdParser instanceof League\CommonMark\DocParser)) {
            $mdParser = new League\CommonMark\DocParser(
                League\CommonMark\Environment::createCommonMarkEnvironment()
            );
        }
        /* HH_FIXME[1002]: Bug in the typechecker */
        usort($constants, ($a, $b) ==> $a->getName() <=> $b->getName());
        $tbody = <tbody/>;
        foreach ($constants as $m) {
            $phpdoc = $parser->parse($m);
            $rt = Vector{$m->getTypehint()};
            $summary = trim($phpdoc->getSummary());
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
            $tbody->appendChild(
                <tr>
                    <th scope="row"><code class="constant-name"><a href={"#constant_" . $m->getName()}>{$m->getName()}</a></code></th>
                    <td><hphpdoc:typehints tokens={$rt}/></td>
                    <td><div class="constant-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div></td>
                </tr>
            );
        }
        return <section class="constants-index">
            <header>
                <h1>{$this->:title}</h1>
            </header>
            <table>
                <caption>Constants</caption>
                <thead>
                    <tr>
                        <th scope="col">Name</th>
                        <th scope="col">Type</th>
                        <th scope="col">Summary</th>
                    </tr>
                </thead>
                {$tbody}
            </table>
        </section>;
    }
}
