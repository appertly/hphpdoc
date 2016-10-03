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
 * Renders a summary table of methods.
 */
class :hphpdoc:methods-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        Stringish title @required,
        ConstVector<Hphpdoc\Source\MethodDeclaration> methods @required;

    protected function render(): XHPRoot
    {
        $methods = $this->:methods;
        if (count($methods) === 0) {
            return <x:frag/>;
        }
        $mdParser = $this->getMarkdownParser();
        $membersByVisibility = Map{'Public' => Vector{}, 'Protected' => Vector{}};
        foreach ($methods as $m) {
            if ($m->getToken()->isPublic()) {
                $membersByVisibility['Public'][] = $m;
            } elseif ($m->getToken()->isProtected()) {
                $membersByVisibility['Protected'][] = $m;
            }
        }
        if (count($membersByVisibility['Public']) === 0 &&
            count($membersByVisibility['Protected']) === 0) {
            return <x:frag/>;
        }
        $title = $this->:title;
        $this->removeAttribute('title');
        $methodSection = <section class="symbol-index methods-index">
            <header>
                <h1>{$title}</h1>
            </header>
        </section>;
        foreach ($membersByVisibility as $vis => $methods) {
            if (count($methods) > 0) {
                /* HH_FIXME[1002]: Bug in the typechecker */
                usort($methods, ($a, $b) ==> $a->getName() <=> $b->getName());
                $tbody = <tbody/>;
                foreach ($methods as $m) {
                    $phpdoc = $m->getDocBlock();
                    $th = $m->getToken()->getReturnType();
                    $rt = Vector{$th};
                    $tht = $th?->getTypeText();
                    if ($th === null || $th->getTypeName() === 'mixed' || $tht === 'array') {
                        $ret = $m->getReturnsTag();
                        if ($ret !== null) {
                            $rt = $ret->getTypes();
                        }
                    }
                    /* HH_IGNORE_ERROR[4101]: Yup */
                    $tbody->appendChild(
                        <tr>
                            <td><hphpdoc:typehints tokens={$rt} returnType={true}/></td>
                            <td>
                                <div class="signature method-signature">
                                    <code class="method-name"><a href={"#method_" . $m->getName()}>{$m->getName()}</a></code>
                                    <hphpdoc:generics generics={$m->getToken()->getGenericTypes()}/>
                                    <hphpdoc:parameters member={$m}/>
                                </div>
                                <div class="method-summary"><axe:markdown text={$phpdoc->getSummary()} docParser={$mdParser}/></div>
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
