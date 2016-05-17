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

use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedClass;
use FredEmmott\DefinitionFinder\ScannedConstant;
use FredEmmott\DefinitionFinder\ScannedMethod;
use FredEmmott\DefinitionFinder\ScannedProperty;
use Hphpdoc\Doc\Block;
use Hphpdoc\Job;

/**
 * Renders a class
 */
class :hphpdoc:classlike extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow;
    children empty;
    attribute :xhp:html-element,
        Block block @required;

    protected function render(): XHPRoot
    {
        $block = $this->:block;
        $job = $this->getContext('job');
        $tbn = $job instanceof Job ? $job->getTokensByName() : ImmMap{};
        $sc = $this->getContext('scannedClass');
        invariant($sc instanceof ScannedClass, "scannedClass context value must be a ScannedClass");

        $parent = $sc->getParentClassInfo();
        $extends = $parent !== null ?
            <p class="class-extends">{"Extends "}<hphpdoc:typehint token={$parent}/></p> :
            <x:frag/>;

        $interfaces = $sc->getInterfaceInfo();
        $implements = count($interfaces) > 0 ?
            <p class="class-implements">
                {$sc->isInterface() ? 'Extends' : 'Implements'}
                {" "}
            </p>:
            <x:frag />;
        foreach ($interfaces as $i => $v) {
            if ($i > 0) {
                $implements->appendChild(", ");
            }
            $implements->appendChild(<hphpdoc:typehint token={$v}/>);
        }

        $main = <main role="main">
            <header>
                <ul class="breadcrumb">
                    <li>In Namespace <hphpdoc:namespace-link namespace={$sc->getNamespaceName()}/></li>
                </ul>
                <h1>
                    <span class="token-type">{$sc->isTrait() ? "Trait" : ($sc->isInterface() ? "Interface" : "Class")}</span>
                    {" "}
                    <span class="class-name">
                        {$sc->getShortName()}
                    </span>
                    <hphpdoc:generics generics={$sc->getGenericTypes()}/>
                </h1>
                {$extends}
                {$implements}
                <p class="class-summary">{$block->getSummary()}</p>
            </header>
            <div class="class-description">
                <axe:paragraphs text={$block->getDescription()}/>
            </div>
            <hphpdoc:authorship block={$block}/>
            <hphpdoc:versions block={$block}/>
            <hphpdoc:links block={$block}/>
        </main>;

        $methods = $this->getMethods($sc, $tbn);
        $methodsv = $methods->values();
        $properties = $this->getProperties($sc, $tbn);
        $propertiesv = $properties->values();
        $constants = $sc->getConstants();
        $main->appendChild(<hphpdoc:constants-table constants={$constants} />);
        $main->appendChild(<hphpdoc:properties-table title="Instance Properties" members={$propertiesv->filter($a ==> !$a->isStatic())}/>);
        $main->appendChild(<hphpdoc:properties-table title="Static Properties" members={$propertiesv->filter($a ==> $a->isStatic())}/>);
        $main->appendChild(<hphpdoc:methods-table title="Constructors / Destructors" methods={$methodsv->filter($a ==> !!preg_match('/^__(con|de)struct$/', $a->getName()))} />);
        $main->appendChild(<hphpdoc:methods-table title="Instance Methods" methods={$methodsv->filter($a ==> !$a->isStatic() && !preg_match('/^__(con|de)struct$/', $a->getName()))} />);
        $main->appendChild(<hphpdoc:methods-table title="Static Methods" methods={$methodsv->filter($a ==> $a->isStatic())} />);
        $this->appendConstants($main, $constants);
        $this->appendProperties($main, $properties);
        $this->appendMethods($main, $methods);
        return $main;
    }

    protected function appendConstants(:main $main, \ConstVector<ScannedConstant> $constants): void
    {
        if (count($constants) > 0) {
            /* HH_FIXME[1002]: Bug in the typechecker */
            usort($constants, ($a, $b) ==> $a->getName() <=> $b->getName());
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

    protected function appendProperties(:main $main, \ConstMap<string,ScannedProperty> $membersByName): void
    {
        $membersByName = $membersByName->filter($a ==> !$a->isPrivate());
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

    protected function appendMethods(:main $main, \ConstMap<string,ScannedMethod> $membersByName): void
    {
        $membersByName = $membersByName->filter($a ==> !$a->isPrivate());
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

    protected function getProperties(ScannedClass $c, ImmMap<string,ScannedBase> $tokensByName): \ConstMap<string,ScannedProperty>
    {
        $members = Map{};
        $pi = $c->getParentClassName();
        if ($pi !== null) {
            $p = $tokensByName->get($pi) ?? $tokensByName->get($c->getNamespaceName() . '\\' . $pi);
            if ($p instanceof ScannedClass) {
                $members->setAll($this->getProperties($p, $tokensByName));
            }
        }
        // TODO FredEmmott\DefinitionFinder has a bug with public properties
        foreach ($c->getProperties()->filter($a ==> !$a->isPublic()) as $m) {
            $members[$m->getName()] = $m;
        }
        return $members;
    }

    protected function getMethods(ScannedClass $c, ImmMap<string,ScannedBase> $tokensByName): \ConstMap<string,ScannedMethod>
    {
        $members = Map{};
        $parents = Vector{};
        if ($c->isInterface()) {
            $parents = $c->getInterfaceNames();
        } else {
            $pcn = $c->getParentClassName();
            if ($pcn !== null) {
                $parents[] = $pcn;
            }
        }
        foreach ($parents as $pi) {
            $p = $tokensByName->get($pi) ?? $tokensByName->get($c->getNamespaceName() . '\\' . $pi);
            if ($p instanceof ScannedClass) {
                $members->setAll($this->getMethods($p, $tokensByName));
            }
        }
        foreach ($c->getMethods() as $m) {
            $members[$m->getName()] = $m;
        }
        return $members;
    }
}
