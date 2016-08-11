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
 * Renders a summary table of properties.
 */
class :hphpdoc:properties-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        ConstVector<Hphpdoc\Source\PropertyDeclaration> members @required;

    protected function render(): XHPRoot
    {
        $properties = $this->:members->toVector();
        if (count($properties) === 0) {
            return <x:frag/>;
        }
        $mdParser = $this->getMarkdownParser();
        /* HH_FIXME[1002]: Bug in the typechecker */
        usort($properties, ($a, $b) ==> $a->getName() <=> $b->getName());
        $tbody = <tbody/>;
        $membersByVisibility = Map{'Public' => Vector{}, 'Protected' => Vector{}};
        foreach ($properties as $p) {
            $m = $p->getToken();
            if ($m->isPublic()) {
                $membersByVisibility['Public'][] = $p;
            } elseif ($m->isProtected()) {
                $membersByVisibility['Protected'][] = $p;
            }
        }
        if (count($membersByVisibility['Public']) === 0 &&
            count($membersByVisibility['Protected']) === 0) {
            return <x:frag/>;
        }
        $title = $this->:title;
        $this->removeAttribute('title');
        $section = <section class="properties-index">
            <header>
                <h1>{$title}</h1>
            </header>
        </section>;
        foreach ($membersByVisibility as $vis => $props) {
            if (count($props) > 0) {
                /* HH_FIXME[1002]: Bug in the typechecker */
                usort($props, ($a, $b) ==> $a->getName() <=> $b->getName());
                $tbody = <tbody/>;
                foreach ($props as $m) {
                    $phpdoc = $m->getDocBlock();
                    $rt = $m->getTypes();
                    $summary = $m->getSummary();
                    $tbody->appendChild(
                        <tr>
                            <th scope="row"><code class="property-name"><var><a href={"#property_" . $m->getName()}>${$m->getName()}</a></var></code></th>
                            <td><hphpdoc:typehints tokens={$rt}/></td>
                            <td><div class="property-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div></td>
                        </tr>
                    );
                }
                $section->appendChild(
                    <table>
                        <caption>{$vis}</caption>
                        <thead>
                            <tr>
                                <th scope="col">Name</th>
                                <th scope="col">Type</th>
                                <th scope="col">Summary</th>
                            </tr>
                        </thead>
                        {$tbody}
                    </table>
                );
            }
        }
        return $section;
    }
}
