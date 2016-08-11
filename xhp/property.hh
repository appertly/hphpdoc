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

use Hphpdoc\Source\PropertyDeclaration;

/**
 * Renders a property.
 */
class :hphpdoc:property extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        PropertyDeclaration property @required;

    protected function render(): XHPRoot
    {
        $m = $this->:property;
        $token = $m->getToken();
        $name = $token->getName();
        $cd = $this->getContext('classyDeclaration');
        invariant($cd instanceof Hphpdoc\Source\ClassyDeclaration, "classyDeclaration context value must be a ClassyDeclaration");
        $mdParser = $this->getMarkdownParser();
        $phpdoc = $m->getDocBlock();
        $summary = $m->getSummary();
        $rt = $m->getTypes();
        $labels = <p class="method-labels"/>;
        if ($cd->getName() !== $m->getClass()->getName()) {
            $labels->appendChild(
                <span class="label">Inherited from {$this->abbrClass($m->getClass()->getName())}</span>
            );
        }
        return <section id={"property_$name"} class="property-section">
            <header>
                <h1>${$name}</h1>
                {$labels}
            </header>
            <div class="property-signature">
                {$token->isStatic() ? 'static ' : ''}
                {$token->isPublic() ? 'public ' : ''}
                {$token->isProtected() ? 'protected ' : ''}
                <code class="property-type">
                    <hphpdoc:typehints tokens={$rt}/>
                </code>
                {" "}
                <code class="property-name"><var>${$name}</var></code>
            </div>
            <div class="property-details">
                <div class="property-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div>
                <div class="property-description">
                    <axe:markdown text={$phpdoc->getDescription()} docParser={$mdParser}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
            </div>
        </section>;
    }

    private function abbrClass(string $name): XHPChild
    {
        return strpos($name, '\\') !== false ?
            <abbr title={$name}>{substr(strrchr($name, '\\'), 1)}</abbr> : $name;
    }
}
