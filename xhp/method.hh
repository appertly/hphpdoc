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

use Hphpdoc\Source\MethodDeclaration;

/**
 * Renders a method.
 */
class :hphpdoc:method extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use MarkdownHelper;
    use FunctionHelper;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        MethodDeclaration method @required;

    protected function render(): XHPRoot
    {
        $m = $this->:method;
        $token = $m->getToken();
        $name = $token->getName();
        $cd = $this->getContext('classyDeclaration');
        invariant($cd instanceof Hphpdoc\Source\ClassyDeclaration, "classyDeclaration context value must be a ClassyDeclaration");
        $mdParser = $this->getMarkdownParser();
        $phpdoc = $m->getDocBlock();
        // TODO show method being final
        // TODO show method being abstract
        $labels = <p class="method-labels"/>;
        if ($cd->getName() !== $m->getClass()->getName()) {
            $labels->appendChild(
                <span class="label">Inherited from {$this->abbrClass($m->getClass()->getName())}</span>
            );
        }
        return <section id={"method_$name"} class="method-section">
            <header>
                <h1>{$name}</h1>
                {$labels}
            </header>
            <div class="method-signature">
                {$token->isAbstract() ? 'abstract ' : ''}
                {$token->isFinal() ? 'final ' : ''}
                {$token->isPublic() ? 'public ' : ''}
                {$token->isStatic() ? 'static ' : ''}
                {$token->isProtected() ? 'protected ' : ''}
                {"function "}
                <code class="method-name">{$name}</code>
                <hphpdoc:generics generics={$token->getGenericTypes()}/>
                <hphpdoc:parameters member={$m}/>
            </div>
            <div class="method-details">
                <div class="method-summary"><axe:markdown text={$phpdoc->getSummary()} docParser={$mdParser}/></div>
                <div class="method-description">
                    <axe:markdown text={$phpdoc->getDescription()} docParser={$mdParser}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
                {$this->getParameters($m, $mdParser)}
                {$name === '__construct' || $name === '__destruct' ? null : $this->getReturns($m, $mdParser)}
                {$this->getThrows($m, $mdParser)}
            </div>
        </section>;
    }

    private function abbrClass(string $name): XHPChild
    {
        return strpos($name, '\\') !== false ?
            <abbr title={$name}>{substr(strrchr($name, '\\'), 1)}</abbr> : $name;
    }
}
