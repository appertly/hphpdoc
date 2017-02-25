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

use Facebook\DefinitionFinder\ScannedBasicClass;
use Facebook\DefinitionFinder\ScannedBase;
use Facebook\DefinitionFinder\ScannedConstant;
use Facebook\DefinitionFinder\ScannedTrait;
use Facebook\DefinitionFinder\ScannedInterface;
use Facebook\DefinitionFinder\ScannedFunction;
use Hphpdoc\Source\TokenDeclaration;
use Hphpdoc\Job;

/**
 * Renders a namespace page
 */
class :hphpdoc:namespace extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :main,
        string namespace @required,
        Traversable<TokenDeclaration<ScannedBase>> declarations @required,
        ConstSet<string> namespaces @required;

    protected function render(): XHPRoot
    {
        $namespace = $this->:namespace;
        $namespaces = $this->:namespaces;
        $declarations = new Vector($this->:declarations);
        $breadcrumbs = <x:frag/>;
        if (strpos($namespace, '\\') !== false) {
            $breadcrumbs = <ul class="breadcrumb">
                <li>In Namespace <hphpdoc:namespace-link namespace={substr($namespace, 0, strrpos($namespace, "\\"))} /></li>
            </ul>;
        }
        $main = <main role="main">
            <header>
                {$breadcrumbs}
                <h1>Namespace {$namespace}</h1>
            </header>
        </main>;
        if (count($namespaces) > 0) {
            $main->appendChild(<section>
                <header>
                    <h1>Sub-namespaces</h1>
                </header>
                <hphpdoc:namespaces-list namespaces={$namespaces}/>
            </section>);
        }

        $job = $this->getContext('job');
        invariant($job instanceof Job, "job context value must be a Job");

        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Interfaces" tokens={$declarations->filter($a ==> $a->getToken() instanceof ScannedInterface)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Traits" tokens={$declarations->filter($a ==> $a->getToken() instanceof ScannedTrait)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:classlike-table title="Classes" tokens={$declarations->filter($a ==> $a->getToken() instanceof ScannedBasicClass)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:constants-table title="Constants" constants={$declarations->filter($a ==> $a->getToken() instanceof ScannedConstant)}/>);
        /* HH_IGNORE_ERROR[4110]: Generic type is fine */
        $main->appendChild(<hphpdoc:functions-table title="Functions" functions={$declarations->filter($a ==> $a->getToken() instanceof ScannedFunction)}/>);
        return $main;
    }
}
