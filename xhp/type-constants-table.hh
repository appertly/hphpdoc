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

/**
 * Renders a summary table of type constants.
 */
class :hphpdoc:type-constants-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        ConstVector<Hphpdoc\Source\TypeConstantDeclaration> typeConstants @required;

    protected function render(): XHPRoot
    {
        $typeConstants = new Vector($this->:typeConstants);
        if (count($typeConstants) === 0) {
            return <x:frag/>;
        }
        $mdParser = $this->getMarkdownParser();
        /* HH_FIXME[1002]: Bug in the typechecker */
        usort($typeConstants, ($a, $b) ==> $a->getName() <=> $b->getName());
        $tbody = <tbody/>;
        foreach ($typeConstants as $m) {
            $phpdoc = $m->getDocBlock();
            $summary = $m->getSummary();
            $name = $m->getToken()->getShortName();
            $tbody->appendChild(
                <tr>
                    <th scope="row"><code class="constant-name"><a href={"#type_constant_$name"}>{$name}</a></code></th>
                    <td><div class="constant-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div></td>
                </tr>
            );
        }
        $title = $this->:title;
        $this->removeAttribute('title');
        $header = <x:frag/>;
        if ($title) {
            $header = <header><h1>{$title}</h1></header>;
        }
        return <section class="symbol-index type-constants-index">
            {$header}
            <table>
                <caption>Type Constants</caption>
                <thead>
                    <tr>
                        <th scope="col">Name</th>
                        <th scope="col">Summary</th>
                    </tr>
                </thead>
                {$tbody}
            </table>
        </section>;
    }
}
