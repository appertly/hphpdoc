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
namespace Hphpdoc\Source;

use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedClass;
use FredEmmott\DefinitionFinder\ScannedMethod;
use FredEmmott\DefinitionFinder\ScannedProperty;
use Hphpdoc\Doc\Block;
use Hphpdoc\Doc\Parser;

/**
 * A declared interface, trait, or class.
 */
class ClassyDeclaration implements TokenDeclaration<ScannedClass>
{
    private ImmMap<string,PropertyDeclaration> $properties;
    private ImmMap<string,MethodDeclaration> $methods;
    private ImmMap<string,ConstantDeclaration> $constants;
    private ImmMap<string,TypeConstantDeclaration> $typeConstants;

    /**
     * Creates a new ClassDeclaration.
     *
     * @param $class - The interface, trait, or class
     * @param $parser - The doc parser
     */
    public function __construct(
        private ScannedClass $class,
        private Block $block,
        \ConstMap<string,PropertyDeclaration> $properties,
        \ConstMap<string,MethodDeclaration> $methods,
        \ConstMap<string,ConstantDeclaration> $constants,
        \ConstMap<string,TypeConstantDeclaration> $typeConstants,
    ) {
        $this->properties = $properties->toImmMap();
        $this->methods = $methods->toImmMap();
        $this->constants = $constants->toImmMap();
        $this->typeConstants = $typeConstants->toImmMap();
    }

    /**
     * Convenience method to create a new ClassyDeclaration.
     *
     * @param $token - The class to read
     * @param $parser - The doc comment parser
     * @param $mapper - The mapper
     * @return - The created ClassyDeclaration
     */
    public static function factory(ScannedClass $token, Parser $parser, Mapper $mapper): ClassyDeclaration
    {
        $block = $parser->parse($token);
        $parents = Vector{};
        if ($token->isInterface()) {
            $parents = $token->getInterfaceNames();
        } else {
            $pcn = $token->getParentClassName();
            if ($pcn !== null) {
                $parents[] = $pcn;
            }
        }
        foreach ($parents as $pi) {
            $p = $mapper->getClass($pi);
            if ($p instanceof ClassyDeclaration) {
                $block = $block->inherit($p->getDocBlock());
            }
        }
        $fileBlock = $mapper->getFile($token->getFileName())?->getDocBlock();
        $fblock = new Block('', '', $fileBlock?->getTags()?->filter($a ==> in_array($a->getName(), ['version', 'author', 'copyright', 'license'])) ?? []);
        $block = $block->inherit($fblock);
        return new ClassyDeclaration($token, $block,
            self::mergeProperties($token, $parser, $mapper),
            self::mergeMethods($token, $parser, $mapper),
            self::mergeConstants($token, $parser, $mapper),
            self::mergeTypeConstants($token, $parser, $mapper),
        );
    }

    /**
     * {@inheritDoc}
     */
    public function getFilename(): string
    {
        return $this->class->getFileName();
    }

    /**
     * {@inheritDoc}
     */
    public function getDocBlock(): Block
    {
        return $this->block;
    }

    /**
     * {@inheritDoc}
     */
    public function getName(): string
    {
        return $this->class->getName();
    }

    /**
     * {@inheritDoc}
     */
    public function getToken(): ScannedClass
    {
        return $this->class;
    }

    /**
     * Whether this class is an interface
     *
     * @return - `true` if this class is an interface
     */
    public function isInterface(): bool
    {
        return $this->class->isInterface();
    }

    /**
     * Whether this class is a trait
     *
     * @return - `true` if this class is a trait
     */
    public function isTrait(): bool
    {
        return $this->class->isTrait();
    }

    /**
     * Gets a method by name
     *
     * @param $name - The method name
     * @return - The method found or `null`
     */
    public function getMethod(string $name): ?MethodDeclaration
    {
        return $this->methods->get($name);
    }

    /**
     * Gets all methods in this class by name
     *
     * @return - All methods in this class
     */
    public function getMethods(): \ConstMap<string,MethodDeclaration>
    {
        return $this->methods;
    }

    /**
     * Gets all properties in this class by name
     *
     * @return - All properties in this class
     */
    public function getProperties(): \ConstMap<string,PropertyDeclaration>
    {
        return $this->properties;
    }

    /**
     * Gets a property by name
     *
     * @param $name - The property name
     * @return - The property found or `null`
     */
    public function getProperty(string $name): ?PropertyDeclaration
    {
        return $this->properties->get($name);
    }

    /**
     * Gets all constants in this class by name
     *
     * @return - All constants in this class
     */
    public function getConstants(): \ConstMap<string,ConstantDeclaration>
    {
        return $this->constants;
    }

    /**
     * Gets a constant by name
     *
     * @param $name - The constant name
     * @return - The constant found or `null`
     */
    public function getConstant(string $name): ?ConstantDeclaration
    {
        return $this->constants->get($name);
    }

    /**
     * Gets all type constants in this class by name
     *
     * @return - All type constants in this class
     */
    public function getTypeConstants(): \ConstMap<string,TypeConstantDeclaration>
    {
        return $this->typeConstants;
    }

    /**
     * Gets a type constant by name
     *
     * @param $name - The constant name
     * @return - The type constant found or `null`
     */
    public function getTypeConstant(string $name): ?TypeConstantDeclaration
    {
        return $this->typeConstants->get($name);
    }

    /**
     * Grab any inherited constants and make sure doc blocks are inherited.
     *
     * @param $c - The token
     * @param $parser - The doc block parser
     * @param $mapper - The mapper
     * @return - The constants by name
     */
    protected static function mergeConstants(ScannedClass $c, Parser $parser, Mapper $mapper): \ConstMap<string,ConstantDeclaration>
    {
        $members = Map{};
        $parents = Vector{};
        $parents->addAll($c->getInterfaceNames());
        $pcn = $c->getParentClassName();
        if ($pcn !== null) {
            $parents[] = $pcn;
        }
        $parents->addAll($c->getTraitNames());
        foreach ($parents as $pi) {
            $p = $mapper->getClass($pi);
            if ($p instanceof ClassyDeclaration) {
                $members->setAll($p->getConstants());
            }
        }
        foreach ($c->getConstants() as $m) {
            $name = $m->getName();
            $block = $parser->parse($m);
            if ($members->containsKey($name)) {
                $block = $block->inherit($members[$name]->getDocBlock());
            }
            $members[$name] = new ConstantDeclaration($m, $block, $c);
        }
        return $members;
    }

    /**
     * Grab any inherited constants and make sure doc blocks are inherited.
     *
     * @param $c - The token
     * @param $parser - The doc block parser
     * @param $mapper - The mapper
     * @return - The constants by name
     */
    protected static function mergeTypeConstants(ScannedClass $c, Parser $parser, Mapper $mapper): \ConstMap<string,TypeConstantDeclaration>
    {
        $members = Map{};
        $parents = Vector{};
        $parents->addAll($c->getInterfaceNames());
        $pcn = $c->getParentClassName();
        if ($pcn !== null) {
            $parents[] = $pcn;
        }
        foreach ($parents as $pi) {
            $p = $mapper->getClass($pi);
            if ($p instanceof ClassyDeclaration) {
                $members->setAll($p->getTypeConstants());
            }
        }
        foreach ($c->getTypeConstants() as $m) {
            $name = $m->getName();
            $block = $parser->parse($m);
            if ($members->containsKey($name)) {
                $block = $block->inherit($members[$name]->getDocBlock());
            }
            $members[$name] = new TypeConstantDeclaration($m, $block, $c);
        }
        return $members;
    }

    /**
     * Grab any inherited properties and make sure doc blocks are inherited.
     *
     * @param $c - The token
     * @param $parser - The doc block parser
     * @param $mapper - The mapper
     * @return - The properties by name
     */
    protected static function mergeProperties(ScannedClass $c, Parser $parser, Mapper $mapper): \ConstMap<string,PropertyDeclaration>
    {
        $members = Map{};
        $parents = Vector{};
        $pcn = $c->getParentClassName();
        if ($pcn !== null) {
            $parents[] = $pcn;
        }
        $parents->addAll($c->getTraitNames());
        foreach ($parents as $pi) {
            $p = $mapper->getClass($pi);
            if ($p instanceof ClassyDeclaration) {
                $members->setAll($p->getProperties());
            }
        }
        foreach ($c->getProperties() as $m) {
            $name = $m->getName();
            $block = $parser->parse($m);
            if ($members->containsKey($name)) {
                $block = $block->inherit($members[$name]->getDocBlock());
            }
            $members[$name] = new PropertyDeclaration($m, $c, $block);
        }
        return $members;
    }

    /**
     * Grab any inherited methods and make sure doc blocks are inherited.
     *
     * @param $c - The token
     * @param $parser - The doc block parser
     * @param $mapper - The mapper
     * @return - The methods by name
     */
    protected static function mergeMethods(ScannedClass $c, Parser $parser, Mapper $mapper): \ConstMap<string,MethodDeclaration>
    {
        $members = Map{};
        $parents = Vector{};
        $parents->addAll($c->getInterfaceNames());
        $pcn = $c->getParentClassName();
        if ($pcn !== null) {
            $parents[] = $pcn;
        }
        $parents->addAll($c->getTraitNames());
        foreach ($parents as $pi) {
            $p = $mapper->getClass($pi);
            if ($p instanceof ClassyDeclaration) {
                $members->setAll($p->getMethods());
            }
        }
        foreach ($c->getMethods() as $m) {
            $name = $m->getName();
            $block = $parser->parse($m);
            if ($members->containsKey($name)) {
                $block = $block->inherit($members[$name]->getDocBlock());
            }
            $members[$m->getName()] = new MethodDeclaration($m, $c, $block);
        }
        return $members;
    }
}
