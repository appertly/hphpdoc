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
        Traversable<FredEmmott\DefinitionFinder\ScannedClass> tokens @required;

    protected function render(): XHPRoot
    {
        $tokens = $this->:tokens;
        if (count($tokens) === 0) {
            return <x:frag/>;
        }
        $tb = <tbody/>;
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
        foreach ($tokens as $c) {
            $block = $parser->parse($c);
            $tb->appendChild(
                <tr>
                    <th scope="row">
                        <a class="class-name" href={$this->getFilename($c)}>{$c->getShortName()}</a>
                        <hphpdoc:generics generics={$c->getGenericTypes()}/>
                    </th>
                    <td><axe:markdown text={$block->getSummary()} docParser={$mdParser}/></td>
                </tr>
            );
        }
        return <section>
            <header>
                <h1>{$this->:title}</h1>
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
