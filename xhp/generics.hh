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
 * Renders generic clauses.
 */
class :hphpdoc:generics extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase;
    children empty;
    attribute :xhp:html-element,
        ConstVector<Facebook\DefinitionFinder\ScannedGeneric> generics @required;

    protected function render(): XHPRoot
    {
        $generics = $this->:generics;
        if (count($generics) === 0) {
            return <x:frag />;
        }
        $tag = <span class="generic-clause"/>;
        $tag->appendChild(<span class="separator-generic">{"<"}</span>);
        foreach ($generics as $i => $t) {
            if ($i > 0) {
                $tag->appendChild(<span class="separator-generic">{","}</span>);
            }
            if ($t->isContravariant()) {
                $tag->appendChild(<code class="generic-variance" title="Contravariant">-</code>);
            } elseif ($t->isCovariant()) {
                $tag->appendChild(<code class="generic-variance" title="Covariant">+</code>);
            }
            $tag->appendChild(<code class="generic-type"><var>{$t->getName()}</var></code>);
            if ($t->getConstraintRelationship() !== null) {
                $tag->appendChild(<code class="generic-constraint">{" " . $t->getConstraintRelationship() . " "}</code>);
                $tag->appendChild(<code class="generic-constrainttype">{$this->abbrClass((string) $t->getConstraintTypeName())}</code>);
            }
        }
        $tag->appendChild(<span class="separator-generic">{">"}</span>);
        return $tag;
    }

    private function abbrClass(string $name): XHPChild
    {
        return strpos($name, '\\') !== false ?
            <abbr title={$name}>{substr(strrchr($name, '\\'), 1)}</abbr> : $name;
    }
}
