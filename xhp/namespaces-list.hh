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
class :hphpdoc:namespaces-list extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :ul,
        Traversable<string> namespaces @required;

    protected function render(): XHPRoot
    {
        $namespaces = $this->:namespaces;
        if (count($namespaces) == 0) {
            return <x:frag />;
        }
        $this->addClass('namespaces');
        $ul = <ul/>;
        foreach ($namespaces as $ns) {
            $ul->appendChild(
                <li><hphpdoc:namespace-link namespace={$ns}/></li>
            );
        }
        return $ul;
    }
}
