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

use FredEmmott\DefinitionFinder\ScannedMethod;

/**
 * Renders a method.
 */
class :hphpdoc:method extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        ScannedMethod method @required;

    protected function render(): XHPRoot
    {
        $m = $this->:method;
        $sc = $this->getContext('scannedClass');
        invariant($sc instanceof \FredEmmott\DefinitionFinder\ScannedBase, "scannedClass context value must be a ScannedBase");
        $parser = $this->getContext('docParser');
        if (!($parser instanceof Hphpdoc\Doc\Parser)) {
            $parser = new Hphpdoc\Doc\Parser();
        }
        $phpdoc = $parser->parse($m);
        // TODO show method being final
        // TODO show method being async
        // TODO show method being abstract
        $labels = <p class="method-labels"/>;
        if ($m->getPosition()['filename'] !== $sc->getPosition()['filename']) {
            $labels->appendChild(<span class="label">Inherited</span>);
        }
        return <section id={"method_" . $m->getName()} class="method-section">
            <header>
                <h1>
                    {$m->getName()}
                </h1>
                {$labels}
            </header>
            <div class="method-signature">
                {$m->isStatic() ? 'static ' : ''}
                {$m->isPublic() ? 'public ' : ''}
                {$m->isProtected() ? 'protected ' : ''}
                {"function "}
                <code class="method-name">{$m->getName()}</code>
                <hphpdoc:generics generics={$m->getGenericTypes()}/>
                <hphpdoc:parameters params={$m->getParameters()}/>
            </div>
            <div class="method-details">
                <p class="method-summary">{$phpdoc->getSummary()}</p>
                <div class="method-description">
                    <axe:paragraphs text={$phpdoc->getDescription()}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
                {$this->getParameters($m, $phpdoc)}
                {$this->getReturns($m, $phpdoc)}
                {$this->getThrows($m, $phpdoc)}
            </div>
        </section>;
    }

    protected function getReturns(ScannedMethod $method, Hphpdoc\Doc\Block $block): XHPRoot
    {
        if ($this->isVoid($method->getReturnType())) {
            return <x:frag />;
        }
        $rt = Vector{$method->getReturnType()};
        $description = '';
        foreach ($block->getTags() as $v) {
            if ($v->getName() === 'return' && $v instanceof Hphpdoc\Doc\TypedTag) {
                $description = $v->getDescription();
                if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                    $rt = $v->getTypes();
                }
                break;
            }
        }
        return <table class="tags-return">
            <caption>Returns</caption>
            <thead>
                <tr>
                    <th scope="col">Type</th>
                    <th scope="col">Description</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td><hphpdoc:typehints tokens={$rt}/></td>
                    <td>{$description}</td>
                </tr>
            </tbody>
        </table>;
    }

    protected function getParameters(ScannedMethod $method, Hphpdoc\Doc\Block $block): XHPRoot
    {
        if (count($method->getParameters()) === 0) {
            return <x:frag />;
        }
        $tbody = <tbody/>;
        foreach ($method->getParameters() as $p) {
            $description = '';
            $rt = Vector{$p->getTypehint()};
            foreach ($block->getTags() as $t) {
                if ($t->getName() === 'param' && $t instanceof Hphpdoc\Doc\ParameterTag && $t->getVariable() == $p->getName()) {
                    $description = $t->getDescription();
                    if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                        $rt = $t->getTypes();
                    }
                }
            }
            $tbody->appendChild(
                <tr>
                    <th scope="row"><code class="parameter-name"><var>{"$"}{$p->getName()}</var></code></th>
                    <td><hphpdoc:typehints tokens={$rt}/></td>
                    <td>{$description}</td>
                </tr>
            );
        }
        return <table class="tags-parameters">
            <caption>Parameters</caption>
            <thead>
                <tr>
                    <th scope="col">Name</th>
                    <th scope="col">Type</th>
                    <th scope="col">Description</th>
                </tr>
            </thead>
            {$tbody}
        </table>;
    }

    protected function getThrows(ScannedMethod $method, Hphpdoc\Doc\Block $block): XHPRoot
    {
        $tbody = <tbody/>;
        foreach ($block->getTags() as $tag) {
            if ($tag->getName() === 'throws' && $tag instanceof Hphpdoc\Doc\TypedTag) {
                $tbody->appendChild(
                    <tr>
                        <th scope="row">
                            <hphpdoc:typehints tokens={$tag->getTypes()}/>
                        </th>
                        <td>
                            {$tag->getDescription()}
                        </td>
                    </tr>
                );
            }
        }
        if (count($tbody->getChildren()) === 0) {
            return <x:frag/>;
        }
        return <table class="tags-throws">
            <caption>Throws</caption>
            <thead>
                <tr>
                    <th scope="col">Type</th>
                    <th scope="col">Conditions</th>
                </tr>
            </thead>
            {$tbody}
        </table>;
    }
}
