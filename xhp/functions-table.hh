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
class :hphpdoc:functions-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        Stringish title = '',
        ConstVector<Hphpdoc\Source\FunctionDeclaration> functions @required;

    protected function render(): XHPRoot
    {
        $functions = $this->:functions;
        if (count($functions) === 0) {
            return <x:frag/>;
        }
        $mdParser = $this->getMarkdownParser();
        $title = $this->:title;
        $this->removeAttribute('title');
        $tbody = <tbody/>;
        foreach ($functions as $m) {
            $phpdoc = $m->getDocBlock();
            $rt = Vector{$m->getToken()->getReturnType()};
            if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                foreach ($phpdoc->getTags() as $t) {
                    if ($t->getName() === 'return' && $t instanceof Hphpdoc\Doc\TypedTag) {
                        $rt = $t->getTypes();
                    }
                }
            }
            $name = $m->getToken()->getShortName();
            $ns = str_replace("\\", '_', $m->getToken()->getNamespaceName());
            $tbody->appendChild(
                <tr>
                    <td><hphpdoc:typehints tokens={$rt}/></td>
                    <td>
                        <div class="method-signature">
                        <code class="method-name"><a href={"functions-$ns.html#function_" . $name}>{$name}</a></code>
                        <hphpdoc:generics generics={$m->getToken()->getGenericTypes()}/>
                        <hphpdoc:parameters member={$m}/>
                        </div>
                        <div class="method-summary"><axe:markdown text={$phpdoc->getSummary()} docParser={$mdParser}/></div>
                    </td>
                </tr>
            );
        }
        $header = <x:frag/>;
        if ($title) {
            $header = <header><h1>{$title}</h1></header>;
        }
        return <section class="functions-index">
            {$header}
            <table>
                <thead>
                    <tr>
                        <th scope="col">Returns</th>
                        <th scope="col">Signature and Description</th>
                    </tr>
                </thead>
                {$tbody}
            </table>
        </section>;
    }

    protected function getMarkdownParser(): League\CommonMark\DocParser
    {
        $mdParser = $this->getContext('markdownParser');
        if (!($mdParser instanceof League\CommonMark\DocParser)) {
            $mdParser = new League\CommonMark\DocParser(
                League\CommonMark\Environment::createCommonMarkEnvironment()
            );
        }
        return $mdParser;
    }
}
