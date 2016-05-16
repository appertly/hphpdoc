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

use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedBasicClass;
use FredEmmott\DefinitionFinder\ScannedTrait;
use FredEmmott\DefinitionFinder\ScannedInterface;

/**
 * Renders a namespace page
 */
class :hphpdoc:namespace extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :xhp:html-element,
        string namespace @required,
        Traversable<ScannedBase> tokens @required,
        ConstMap<string,Vector<ScannedBase>> tokensByName @required;

    protected function render(): XHPRoot
    {
        $namespace = $this->:namespace;
        $tokens = new Vector($this->:tokens);
        $main = <main role="main">
            <header>
                <h1>Namespace {$namespace}</h1>
            </header>
        </main>;
        $namespaces = new Map($this->:tokensByName);
        $namespaces = $namespaces->filterWithKey(function ($k, $v) use ($namespace) {
            $ns = substr($k, 0, strrpos($k, "\\"));
            return substr($ns, 0, strlen($namespace) + 1) === "$namespace\\";
        });
        ksort($namespaces);
        $namespaces = $namespaces->keys()->map($k ==> substr($k, 0, strrpos($k, "\\")))->toSet();
        if (count($namespaces) > 0) {
            $main->appendChild(<section>
                <header>
                    <h1>Sub-namespaces</h1>
                </header>
                <hphpdoc:namespaces-list namespaces={$namespaces}/>
            </section>);
        }
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Interfaces" tokens={$tokens->filter($a ==> $a instanceof ScannedInterface)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Traits" tokens={$tokens->filter($a ==> $a instanceof ScannedTrait)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Classes" tokens={$tokens->filter($a ==> $a instanceof ScannedBasicClass)}/>);
        return $main;
    }
}
