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
 * Renders a summary table of methods.
 */
class :hphpdoc:methods-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        Stringish title @required,
        ConstVector<FredEmmott\DefinitionFinder\ScannedMethod> methods @required;

    protected function render(): XHPRoot
    {
        $methods = $this->:methods;
        if (count($methods) === 0) {
            return <x:frag/>;
        }
        $parser = $this->getContext('docParser');
        if (!($parser instanceof Hphpdoc\Doc\Parser)) {
            $parser = new Hphpdoc\Doc\Parser();
        }
        $membersByVisibility = Map{'Public' => Vector{}, 'Protected' => Vector{}};
        foreach ($methods as $m) {
            if ($m->isPublic()) {
                $membersByVisibility['Public'][] = $m;
            } elseif ($m->isProtected()) {
                $membersByVisibility['Protected'][] = $m;
            }
        }
        if (count($membersByVisibility['Public']) === 0 &&
            count($membersByVisibility['Protected']) === 0) {
            return <x:frag/>;
        }
        $methodSection = <section class="methods-index">
            <header>
                <h1>{$this->:title}</h1>
            </header>
        </section>;
        foreach ($membersByVisibility as $vis => $methods) {
            if (count($methods) > 0) {
                /* HH_FIXME[1002]: Bug in the typechecker */
                usort($methods, ($a, $b) ==> $a->getName() <=> $b->getName());
                $tbody = <tbody/>;
                foreach ($methods as $m) {
                    $phpdoc = $parser->parse($m);
                    $rt = Vector{$m->getReturnType()};
                    if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                        foreach ($phpdoc->getTags() as $t) {
                            if ($t->getName() === 'return' && $t instanceof Hphpdoc\Doc\TypedTag) {
                                $rt = $t->getTypes();
                            }
                        }
                    }
                    $tbody->appendChild(
                        <tr>
                            <td><hphpdoc:typehints tokens={$rt}/></td>
                            <td>
                                <div class="method-signature">
                                <code class="method-name"><a href={"#method_" . $m->getName()}>{$m->getName()}</a></code>
                                <hphpdoc:generics generics={$m->getGenericTypes()}/>
                                <hphpdoc:parameters params={$m->getParameters()}/>
                                </div>
                                <div class="method-summary">{$phpdoc->getSummary()}</div>
                            </td>
                        </tr>
                    );
                }
                $methodSection->appendChild(
                    <table>
                        <caption>{$vis}</caption>
                        <thead>
                            <tr>
                                <th scope="col">Returns</th>
                                <th scope="col">Signature and Description</th>
                            </tr>
                        </thead>
                        {$tbody}
                    </table>
                );
            }
        }
        return $methodSection;
    }
}
