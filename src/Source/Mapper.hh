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
use FredEmmott\DefinitionFinder\ScannedConstant;
use FredEmmott\DefinitionFinder\ScannedFunction;
use Hphpdoc\Doc\Parser;

/**
 * Takes the ScannedBase tokens and resolves PHPDoc inheritance.
 */
class Mapper
{
    private ImmVector<ScannedBase> $tokens;
    private ImmMap<string,ScannedBase> $byId;
    private ImmMap<string,ImmVector<ScannedBase>> $byNamespace;
    private ImmMap<string,FileDeclaration> $files;
    private ImmMap<string,ImmVector<ScannedBase>> $byFile;
    private Map<string,ClassyDeclaration> $classes = Map{};
    private Map<string,FunctionDeclaration> $functions = Map{};
    private Map<string,ConstantDeclaration> $constants = Map{};

    /**
     * Creates a new Mapper.
     *
     * @param $tokens - All scanned tokens
     */
    public function __construct(\ConstVector<ScannedBase> $tokens, private Parser $parser)
    {
        $this->tokens = $tokens->toImmVector();
        $files = Map{};
        $byId = Map{};
        $namespaces = Map{};
        $byFile = Map{};
        foreach ($tokens as $token) {
            $ns = $token->getNamespaceName();
            if (!$namespaces->containsKey($ns)) {
                $namespaces[$ns] = Vector{};
            }
            $byId[$token->getName()] = $token;
            $namespaces[$ns][] = $token;
            $filename = $token->getFileName();
            $files[$filename] = $filename;
            if (!$byFile->containsKey($filename)) {
                $byFile[$filename] = Vector{};
            }
            $byFile[$filename][] = $token;
        }
        ksort($byFile);
        $this->byFile = $byFile->map($a ==> $a->toImmVector())->immutable();
        ksort($files);
        $this->files = $this->mapFiles($files);
        ksort($byId);
        $this->byId = $byId->immutable();
        ksort($namespaces);
        $this->byNamespace = $namespaces->map($a ==> $a->immutable())->immutable();
    }

    private function mapFiles(\ConstMap<string,string> $filenames): ImmMap<string,FileDeclaration>
    {
        $map = Map{};
        $breakOn = Set{T_CLASS, T_INTERFACE, T_TRAIT, T_REQUIRE, T_REQUIRE_ONCE,
            T_INCLUDE, T_INCLUDE_ONCE, T_FUNCTION, T_ENUM, T_TYPE, T_NEWTYPE,
            T_ASYNC, T_VAR};
        foreach ($filenames as $_ => $k) {
            $comment = null;
            $peek = false;
            foreach (token_get_all(file_get_contents($k)) as $t) {
                if ($peek) {
                    // make sure the doc block we found doesn't belong to a
                    // structural element
                    if (is_array($t) && $breakOn->contains($t[0])) {
                        $comment = null;
                    }
                    break;
                } elseif (is_array($t) && $t[0] === T_DOC_COMMENT) {
                    $comment = $t[1];
                    $peek = true;
                }
            }
            $block = $this->parser->parseString($comment);
            $map[$k] = new FileDeclaration($k, $block);
        }
        return $map->immutable();
    }

    /**
     * Gets the file definitions.
     *
     * @return - The file definitions
     */
    public function getFiles(): \ConstMap<string,FileDeclaration>
    {
        return $this->files;
    }

    /**
     * Gets a file definition.
     *
     * @param $name - The file name
     * @return - The file declaration or `null`
     */
    public function getFile(string $name): ?FileDeclaration
    {
        return $this->files->get($name);
    }

    /**
     * Gets a list of all tokens
     */
    public function getTokens(): \ConstVector<ScannedBase>
    {
        return $this->tokens;
    }

    /**
     * Gets the namespaces of the tokens.
     *
     * @return - The token namespaces
     */
    public function getNamespaces(): \ConstSet<string>
    {
        return $this->byNamespace->keys()->toImmSet();
    }

    /**
     * Gets all tokens by their name.
     *
     * @return - The tokens by name
     */
    public function getTokensByName(): ImmMap<string,ScannedBase>
    {
        return $this->byId;
    }

    /**
     * Gets all tokens by filename.
     *
     * @return - The tokens by filename
     */
    public function getTokensByFilename(): ImmMap<string,ImmVector<ScannedBase>>
    {
        return $this->byFile;
    }

    /**
     * Gets all tokens by namespace name.
     *
     * @return - The tokens by namespace name
     */
    public function getTokensByNamespace(): ImmMap<string,ImmVector<ScannedBase>>
    {
        return $this->byNamespace;
    }

    /**
     * Gets all class declarations.
     *
     * @return - The class declarations
     */
    public function getClasses(): \ConstMap<string,ClassyDeclaration>
    {
        foreach ($this->byId as $k => $v) {
            $this->getClass($k);
        }
        return $this->classes->toImmMap();
    }

    /**
     * Gets a class declaration
     *
     * @param $name - The class name
     * @return - The class declaration or `null`
     */
    public function getClass(string $name): ?ClassyDeclaration
    {
        if (!$this->classes->containsKey($name) && $this->byId->containsKey($name)) {
            $token = $this->byId[$name];
            if ($token instanceof ScannedClass) {
                $this->classes[$name] = ClassyDeclaration::factory($token, $this->parser, $this);
            }
        }
        return $this->classes->get($name);
    }

    /**
     * Gets a function declaration.
     *
     * @param $name - The function name
     * @return - The function declaration or `null`
     */
    public function getFunction(string $name): ?FunctionDeclaration
    {
        if (!$this->functions->containsKey($name) && $this->byId->containsKey($name)) {
            $token = $this->byId[$name];
            if ($token instanceof ScannedFunction) {
                $this->functions[$name] = new FunctionDeclaration($token, $this->parser->parse($token));
            }
        }
        return $this->functions->get($name);
    }

    /**
     * Gets a constant declaration.
     *
     * @param $constant - The constant name
     * @return - The constant declaration or `null`
     */
    public function getConstant(string $name): ?ConstantDeclaration
    {
        if (!$this->constants->containsKey($name) && $this->byId->containsKey($name)) {
            $token = $this->byId[$name];
            if ($token instanceof ScannedConstant) {
                $this->constants[$name] = new ConstantDeclaration($token, $this->parser->parse($token));
            }
        }
        return $this->constants->get($name);
    }
}
