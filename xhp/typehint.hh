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
 * Renders a typehint.
 */
class :hphpdoc:typehint extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %phrase;
    children empty;
    attribute :xhp:html-element,
        ?FredEmmott\DefinitionFinder\ScannedTypehint token @required;

    protected function render(): XHPRoot
    {
        $type = $this->:token;
        $job = $this->getContext('job');
        $tbn = $job instanceof Hphpdoc\Job ? $job->getTokensByName() : Map{};
        $sc = $this->getContext('scannedClass');

        $tag = <span class="typehint"/>;
        if ($this->isVoid($type)) {
            $tag->appendChild(<code class="void">void</code>);
            return $tag;
        }
        invariant($type !== null, "Typehint is null");
        $base = $type->getTypeName();
        if ($type->isNullable()) {
            $tag->appendChild(<code class="nullable" title="Nullable">?</code>);
        }

        if ($this->isPrimitive($base) || $base === 'tuple') {
            $tag->appendChild(<code class="primitive">{$base}</code>);
        } elseif ($this->isThisy($type)) {
            $tag->appendChild(<code class="ref"><a href="#">{$base}</a></code>);
        } elseif (substr($base, 0, 1) == '\\') {
            if ($tbn->containsKey(substr($base, 1))) {
                $tag->appendChild(<code class="ref"><a href={$this->getFilename($tbn[substr($base, 1)])}>{substr($base, 1)}</a></code>);
            } else {
                $tag->appendChild(<code class="ref">{substr($base, 1)}</code>);
            }
        } elseif ($sc instanceof FredEmmott\DefinitionFinder\ScannedClass) {
            $gens = $sc->getGenericTypes()->map($a ==> $a->getName());
            if ($tbn->containsKey($sc->getNamespaceName() . '\\' . $base)) {
                $tag->appendChild(<code class="ref"><a href={$this->getFilename($tbn[$sc->getNamespaceName() . '\\' . $base])}>{$base}</a></code>);
            } elseif ($gens->linearSearch($base) > -1) {
                $tag->appendChild(<code class="generic-ref">{$base}</code>);
            } else {
                $tag->appendChild(<code class="ref">{$base}</code>);
            }
        } else {
            $tag->appendChild(<code class="ref">{$base}</code>);
        }

        $generics = $type->getGenericTypes();
        if (count($generics) > 0) {
            if ($base === 'tuple') {
                $tag->appendChild(<span class="separator-generic">(</span>);
                foreach ($generics as $k => $v) {
                    if ($k > 0) {
                        $tag->appendChild(<span class="separator-generic">{","}</span>);
                    }
                    $tag->appendChild(<hphpdoc:typehint token={$v}/>);
                }
                $tag->appendChild(<span class="separator-generic">)</span>);
            } else {
                $tag->appendChild(<span class="separator-generic">{"<"}</span>);
                foreach ($generics as $k => $v) {
                    if ($k > 0) {
                        $tag->appendChild(<span class="separator-generic">{","}</span>);
                    }
                    $tag->appendChild(<hphpdoc:typehint token={$v}/>);
                }
                $tag->appendChild(<span class="separator-generic">{">"}</span>);
            }
        }

        return $tag;
    }
}
