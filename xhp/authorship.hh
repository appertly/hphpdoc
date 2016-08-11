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
 * Renders a summary list of version info
 */
class :hphpdoc:authorship extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :table,
        Hphpdoc\Doc\Block block @required;

    protected function render(): XHPRoot
    {
        $dl = <dl />;
        $authors = Vector{};
        $copyright = Vector{};
        foreach ($this->:block->getTags() as $tag) {
            switch ($tag->getName()) {
                case "author":
                    invariant($tag instanceof Hphpdoc\Doc\DescribedTag, "@author is not a DescribedTag");
                    $a = $tag->getDescription();
                    $matches = [];
                    if (preg_match("/^([\S\s]+)\s+<([^>]+)>$/", $a, $matches)) {
                        $a = <x:frag>
                            <axe:gravatar email={$matches[2]} size={16}/>
                            {" "}
                            <a href={"mailto:{$matches[2]}"}>{$matches[1]}</a>
                        </x:frag>;
                    }
                    $authors[] = $a;
                    break;
                case "copyright":
                    invariant($tag instanceof Hphpdoc\Doc\DescribedTag, "@copyright is not a DescribedTag");
                    $copyright[] = $tag->getDescription();
                    break;
            }
        }
        if (count($authors) > 0) {
            $dl->appendChild(<dt>Authors</dt>);
            foreach ($authors as $c) {
                $dl->appendChild(<dd>{$c}</dd>);
            }
        }
        if (count($copyright) > 0) {
            $dl->appendChild(<dt>Copyright</dt>);
            foreach ($copyright as $c) {
                $dl->appendChild(<dd>{$c}</dd>);
            }
        }
        if (count($dl->getChildren()) === 0) {
            return <x:frag/>;
        }
        return $dl;
    }
}
