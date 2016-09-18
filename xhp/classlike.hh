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

use FredEmmott\DefinitionFinder\ScannedConstant;
use Hphpdoc\Source\ClassyDeclaration;
use Hphpdoc\Source\ConstantDeclaration;
use Hphpdoc\Source\MethodDeclaration;
use Hphpdoc\Source\PropertyDeclaration;
use Hphpdoc\Job;

/**
 * Renders a class
 */
class :hphpdoc:classlike extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;
    use MarkdownHelper;

    category %flow;
    children empty;
    attribute :main;

    protected function render(): XHPRoot
    {
        $job = $this->getContext('job');
        $tbn = $job instanceof Job ? $job->getTokensByName() : ImmMap{};
        $class = $this->getContext('classyDeclaration');
        invariant($class instanceof ClassyDeclaration, "classyDeclaration context value must be a ClassyDeclaration");
        $sc = $class->getToken();

        $parent = $sc->getParentClassInfo();
        $extends = $parent !== null ?
            <code class="class-extends">{" extends "}<hphpdoc:typehint token={$parent}/></code> :
            null;

        $finality = $sc->isFinal() ? <code class="class-finality">{"final "}</code> : null;
        $abstractness = $sc->isAbstract() ? <code class="class-abstractness">{"abstract "}</code> : null;

        $interfaces = $sc->getInterfaceInfo();
        $implements = count($interfaces) > 0 ?
            <code class="class-implements">
                {$sc->isInterface() ? ' extends' : ' implements'}
                {" "}
            </code>:
            <x:frag />;
        foreach ($interfaces as $i => $v) {
            if ($i > 0) {
                $implements->appendChild(", ");
            }
            $implements->appendChild(<hphpdoc:typehint token={$v}/>);
        }

        $block = $class->getDocBlock();
        $mdParser = $this->getMarkdownParser();
        $main = <main role="main">
            <header>
                <ul class="breadcrumb">
                    <li>In Namespace <hphpdoc:namespace-link namespace={$sc->getNamespaceName()}/></li>
                </ul>
                <h1>
                    <span class="class-name">{$sc->getShortName()}</span>
                    <hphpdoc:generics generics={$sc->getGenericTypes()}/>
                </h1>
                <div class="class-signature">
                    {$finality}
                    {$abstractness}
                    <code class="class-type">{$sc->isTrait() ? "trait " : ($sc->isInterface() ? "interface " : "class ")}</code>
                    <code class="class-name">{$sc->getShortName()}</code>
                    <hphpdoc:generics generics={$sc->getGenericTypes()}/>
                    {$extends}
                    {$implements}
                </div>
                <div class="class-summary"><axe:markdown text={$block->getSummary()} docParser={$mdParser}/></div>
            </header>
            <div class="class-description">
                <axe:markdown text={$block->getDescription()} docParser={$mdParser}/>
            </div>
            <hphpdoc:authorship block={$block}/>
            <hphpdoc:versions block={$block}/>
            <hphpdoc:links block={$block}/>
        </main>;

        $methods = $class->getMethods();
        $methodsv = $methods->values();
        $properties = $class->getProperties();
        $propertiesv = $properties->values();
        $constants = $class->getConstants();
        $main->appendChild(<hphpdoc:constants-table constants={$constants->values()} title="Constants" />);
        $main->appendChild(<hphpdoc:properties-table title="Instance Properties" members={$propertiesv->filter($a ==> !$a->getToken()->isStatic())}/>);
        $main->appendChild(<hphpdoc:properties-table title="Static Properties" members={$propertiesv->filter($a ==> $a->getToken()->isStatic())}/>);
        $main->appendChild(<hphpdoc:methods-table title="Constructors / Destructors" methods={$methodsv->filter($a ==> !!preg_match('/^__(con|de)struct$/', $a->getName()))} />);
        $main->appendChild(<hphpdoc:methods-table title="Instance Methods" methods={$methodsv->filter($a ==> !$a->getToken()->isStatic() && !preg_match('/^__(con|de)struct$/', $a->getName()))} />);
        $main->appendChild(<hphpdoc:methods-table title="Static Methods" methods={$methodsv->filter($a ==> $a->getToken()->isStatic())} />);
        $this->appendConstants($main, $constants);
        $this->appendProperties($main, $properties);
        $this->appendMethods($main, $methods);
        return $main;
    }

    protected function appendConstants(:main $main, \ConstMap<string,ConstantDeclaration> $constants): void
    {
        if (count($constants) > 0) {
            $constants = new Map($constants);
            ksort($constants);
            $section = <section class="constant-details">
                <header>
                    <h1>Constant Details</h1>
                </header>
            </section>;
            foreach ($constants as $m) {
                $section->appendChild(<hphpdoc:constant constant={$m}/>);
            }
            $main->appendChild($section);
        }
    }

    protected function appendProperties(:main $main, \ConstMap<string,PropertyDeclaration> $membersByName): void
    {
        $membersByName = $membersByName->filter($a ==> !$a->getToken()->isPrivate());
        if (count($membersByName) > 0) {
            $members = new Map($membersByName);
            ksort($members);
            $section = <section class="property-details">
                <header>
                    <h1>Property Details</h1>
                </header>
            </section>;
            foreach ($members as $m) {
                $section->appendChild(<hphpdoc:property property={$m}/>);
            }
            $main->appendChild($section);
        }
    }

    protected function appendMethods(:main $main, \ConstMap<string,MethodDeclaration> $membersByName): void
    {
        $membersByName = $membersByName->filter($a ==> !$a->getToken()->isPrivate());
        if (count($membersByName) > 0) {
            $members = new Map($membersByName);
            ksort($members);
            $section = <section class="method-details">
                <header>
                    <h1>Method Details</h1>
                </header>
            </section>;
            foreach ($members as $m) {
                $section->appendChild(<hphpdoc:method method={$m}/>);
            }
            $main->appendChild($section);
        }
    }
}
