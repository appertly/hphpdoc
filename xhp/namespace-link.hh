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
 * Renders a link to a namespace
 */
class :hphpdoc:namespace-link extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase, %interactive;
    children empty;
    attribute :xhp:html-element,
        string namespace @required;

    protected function render(): XHPRoot
    {
        $n = $this->:namespace;
        $this->addClass('namespace');
        if ($n === '') {
            return <a href="namespace-.html">\</a>;
        } else {
            $a = <span/>;
            $last = '';
            foreach (explode("\\", $n) as $i => $v) {
                $last .= $last === '' ? $v : "\\$v";
                if ($i > 0) {
                    $a->appendChild("\\");
                }
                $a->appendChild(<a href={"namespace-" . str_replace('\\', '_', $last) . '.html'} title={"Namespace $last"}>{$v}</a>);
            }
            return $a;
        }
    }
}
