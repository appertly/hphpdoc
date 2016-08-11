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
class :hphpdoc:versions extends :x:element implements HasXHPHelpers
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
            if ($tag instanceof Hphpdoc\Doc\VersionTag) {
                switch ($tag->getName()) {
                    case "version":
                        $dl->appendChild(<dt>Version</dt>);
                        $dl->appendChild(<dd>{$tag->getVersion()} {$tag->getDescription()}</dd>);
                        break;
                    case "since":
                        $dl->appendChild(<dt>Since</dt>);
                        $dl->appendChild(<dd>{$tag->getVersion()} {$tag->getDescription()}</dd>);
                        break;
                    case "deprecated":
                        $dl->appendChild(<dt>Deprecated</dt>);
                        $v = $tag->getVersion();
                        list($vd, $vr) = array_pad(explode(':', $v, 2), 2, '');
                        $dd = <dd/>;
                        $description = $tag->getDescription();
                        if ($vd) {
                            $dd->appendChild("Since $vd");
                            if (strlen($description) > 0) {
                                $dd->appendChild(". ");
                            }
                        }
                        if (strlen($description) > 0) {
                            $dd->appendChild($description);
                        }
                        $dl->appendChild($dd);
                        if ($vr) {
                            $dl->appendChild(<dd>Scheduled for removal in {$vr}</dd>);
                        }
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
