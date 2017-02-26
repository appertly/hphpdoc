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

use Facebook\DefinitionFinder\ScannedConstant;
use Hphpdoc\Source\ClassyDeclaration;
use Hphpdoc\Source\MethodDeclaration;
use Hphpdoc\Source\PropertyDeclaration;
use Hphpdoc\Job;

/**
 * Renders a class
 */
class :hphpdoc:constants extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow;
    children empty;
    attribute :main,
        string namespace @required,
        ConstVector<Hphpdoc\Source\ConstantDeclaration> constants @required;

    protected function render(): XHPRoot
    {
        $section = <section class="symbol-details constant-details">
            <header>
                <h1>Constant Details</h1>
            </header>
        </section>;
        foreach ($this->:constants as $m) {
            $section->appendChild(<hphpdoc:constant constant={$m}/>);
        }
        return <main role="main">
            <header>
                <ul class="breadcrumb">
                    <li>In Namespace <hphpdoc:namespace-link namespace={$this->:namespace}/></li>
                </ul>
                <h1><span class="token-type">Constants</span></h1>
            </header>
            <hphpdoc:constants-table constants={$this->:constants}/>
            {$section}
        </main>;
    }
}
