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
 * Renders a summary table of classes.
 */
class :hphpdoc:classlike-table extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        Stringish title @required,
        Traversable<Hphpdoc\Source\ClassyDeclaration> tokens @required;

    protected function render(): XHPRoot
    {
        $tokens = $this->:tokens;
        if (count($tokens) === 0) {
            return <x:frag/>;
        }
        $tb = <tbody/>;
        $mdParser = $this->getContext('markdownParser');
        if (!($mdParser instanceof League\CommonMark\DocParser)) {
            $mdParser = new League\CommonMark\DocParser(
                League\CommonMark\Environment::createCommonMarkEnvironment()
            );
        }
        foreach ($tokens as $c) {
            $t = $c->getToken();
            $tb->appendChild(
                <tr>
                    <th scope="row">
                        <a class="class-name" href={$this->getFilename($t)}>{$t->getShortName()}</a>
                        <hphpdoc:generics generics={$t->getGenericTypes()}/>
                    </th>
                    <td><axe:markdown text={$c->getDocBlock()->getSummary()} docParser={$mdParser}/></td>
                </tr>
            );
        }
        $title = $this->:title;
        $this->removeAttribute('title');
        return <section class="symbol-index">
            <header>
                <h1>{$title}</h1>
            </header>
            <table>
                <thead>
                    <tr>
                        <th scope="col">Name</th>
                        <th scope="col">Summary</th>
                    </tr>
                </thead>
                {$tb}
            </table>
        </section>;
    }
}
