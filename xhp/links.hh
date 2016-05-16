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
 * Renders a summary list of version info
 */
class :hphpdoc:links extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :table,
        Hphpdoc\Doc\Block block @required;

    protected function render(): XHPRoot
    {
        $dl = <dl />;
        foreach ($this->:block->getTags() as $tag) {
            if ($tag instanceof Hphpdoc\Doc\DescribedTag) {
                switch ($tag->getName()) {
                    case "example":
                    case "see":
                        $dl->appendChild(<dt>See</dt>);
                        invariant($tag instanceof Hphpdoc\Doc\UriTag, "@see not a UriTag");
                        $a = null;
                        $uri = $tag->getUri();
                        $description = $tag->getDescription();
                        if (strlen($description) === 0) {
                            $description = $uri;
                        }
                        if (substr($uri, 0, 4) === 'http') {
                            $a = <a href={$uri}>{$description}</a>;
                        }
                        // TODO link FQSEN references
                        if ($a === null) {
                            $a = $uri === $description ? $uri : "$uri $description";
                        }
                        $dl->appendChild(<dd>{$a}</dd>);
                        break;
                    case "license":
                        $dl->appendChild(<dt>License</dt>);
                        invariant($tag instanceof Hphpdoc\Doc\UriTag, "@license not a UriTag");
                        $uri = $tag->getUri();
                        $description = $tag->getDescription();
                        if (strlen($description) === 0) {
                            $description = $uri;
                        }
                        $a = substr($uri, 0, 4) !== 'http' ?
                            <a href={"http://spdx.org/licenses/$uri.html"}>{$description}</a> :
                            <a href={$uri}>{$description}</a>;
                        $dl->appendChild(<dd>{$a}</dd>);
                        break;
                }
            }
        }
        if (count($dl->getChildren()) === 0) {
            return <x:frag/>;
        }
        return $dl;
    }
}
