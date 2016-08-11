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
 * Renders the top header.
 */
class :hphpdoc:banner extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :header,
        Stringish title = '';

    protected function render(): XHPRoot
    {
        return <header role="banner">
            <h1><a href="index.html">{$this->:title} API Documentation</a></h1>
        </header>;
    }
}
