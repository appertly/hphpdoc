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
namespace Hphpdoc\Doc;

use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedConstant;
use FredEmmott\DefinitionFinder\ScannedFunctionAbstract;
use FredEmmott\DefinitionFinder\ScannedMethod;
use FredEmmott\DefinitionFinder\ScannedProperty;
use FredEmmott\DefinitionFinder\ScannedTypehint;

/**
 * Creates tags.
 */
class TagFactory
{
    protected static ImmSet<string> $psrKeywords = ImmSet{'string', 'int',
        'bool', 'float', 'object', 'array', 'mixed', 'resource', 'void', 'null',
        'true', 'false', 'callable'};
    protected static ImmSet<string> $thisy = ImmSet{'self', '$this', 'this',
        'static'};
    protected static ImmMap<string,string> $replaceTypes = ImmMap{
        'integer' => 'int',
        'boolean' => 'bool',
        'real' => 'float',
        'double' => 'float'
    };
    protected static ImmSet<string> $notTypes = ImmSet{'null', 'false', 'true'};
    protected static ImmSet<string> $constants = ImmSet{'string', 'int', 'bool',
        'float', 'array'};

    /**
     * Creates a bunch of tags
     *
     * @param $token - The scanned token whose doc comment has these tags
     * @param $tags - The tags to convert
     */
    public function convert(?ScannedBase $token, \ConstVector<string> $tags): \ConstVector<Tag>
    {
        $out = Vector{};
        foreach ($tags as $tag) {
            $this->doTag($out, $token, $tag);
        }
        return $out;
    }

    protected function doTag(Vector<Tag> $out, ?ScannedBase $token, string $tag): void
    {
        $matches = [];
        if (preg_match('/^@([a-z\\][a-z0-9_\\-]*(?::[a-z0-9-]*)?)\s*(.*)$/i', $tag, $matches)) {
            if (TypedTag::applies($matches[1])) {
                $out[] = $this->doTypedTag($token, $matches);
            } elseif (VersionTag::applies($matches[1])) {
                $out[] = $this->doVersionTag($token, $matches);
            } elseif (UriTag::applies($matches[1])) {
                list($uri, $description) = array_pad(preg_split('/\s+/', $matches[2], 2), 2, '');
                $out[] = new UriTag($matches[1], $uri, $description);
            } elseif (ParameterTag::applies($matches[1])) {
                $out[] = $this->doParameterTag($token, $matches);
            } elseif (strlen($matches[2]) > 0) {
                $out[] = new DescribedTag($matches[1], $matches[2]);
            } else {
                $out[] = new Tag($matches[1]);
            }
        }
    }

    protected function doTypedTag(?ScannedBase $token, array<string> $matches): TypedTag
    {
        list($type, $description) = array_pad(preg_split('/\s+/', $matches[2], 2), 2, '');
        $types = Vector{};
        if ($matches[1] === 'throws') {
            $th = $this->stringToTypehint($type, $token);
            if ($th !== null) {
                $types[] = $th;
            }
        } elseif ($matches[1] === 'return' && $token instanceof ScannedFunctionAbstract) {
            $rt = $token->getReturnType();
            if ($type === '-' && $rt !== null) {
                $types[] = $rt;
            } elseif (self::$thisy->contains($type) && ($rt === null || self::$thisy->contains($rt->getTypeName()))) {
                $th = $this->stringToTypehint(
                    $token instanceof ScannedMethod && $token->isStatic() ? 'static' : '$this',
                    $token,
                );
                if ($th !== null) {
                    $types[] = $th;
                }
            } else {
                $types->addAll($this->parseTypes($type, $token));
            }
        }
        return new TypedTag($matches[1], $types, $description);
    }

    protected function doVersionTag(?ScannedBase $token, array<string> $matches): VersionTag
    {
        list($version, $description) = array_pad(preg_split('/\s+/', $matches[2], 2), 2, '');
        if ($matches[1] === 'deprecated' && !preg_match('/^[:]?[0-9]+(\.[0-9]+)+/', $version)) {
            $description = "$version $description";
            $version = '';
        }
        return new VersionTag($matches[1], $version, $description);
    }

    protected function doParameterTag(?ScannedBase $token, array<string> $matches): ParameterTag
    {
        list($type, $variable, $description) = array_pad(preg_split('/\s+/', $matches[2], 3), 3, '');
        $types = Vector{};
        if ($token instanceof ScannedConstant) {
            if ($variable === $token->getName()) {
                // best case scenario
                $types->addAll($this->parseTypes($type, $token));
            } elseif ($type === $token->getName()) {
                if ($variable !== '-') {
                    $description = "$variable $description";
                }
                $variable = $type;
                $th = $token->getTypehint();
                if ($th !== null) {
                    $types[] = $th;
                }
            } elseif (self::$constants->contains($type)) {
                $description = "$variable $description";
                $th = $this->stringToTypehint($type, $token);
                if ($th !== null) {
                    $types[] = $th;
                }
            } else {
                $description = "$type $variable $description";
                $variable = $token->getName();
                $th = $token->getTypehint();
                if ($th !== null) {
                    $types[] = $th;
                }
            }
        } elseif ($token instanceof ScannedProperty) {
            if (substr($variable, 0, 1) === '$') {
                // best case scenario
                $types->addAll($this->parseTypes($type, $token));
            } elseif (substr($type, 0, 1) === '$') {
                if ($variable !== '-') {
                    $description = "$variable $description";
                }
                $variable = $type;
                $th = $token->getTypehint();
                if ($th !== null) {
                    $types[] = $th;
                }
            } elseif (self::$psrKeywords->contains($type) || preg_match('#[|\\\\<]#', $type) || (!$variable && !$description)) {
                $description = "$variable $description";
                $variable = $token->getName();
                $types->addAll($this->parseTypes($type, $token));
            } else {
                $th = $token->getTypehint();
                if ($th !== null && $th->getTypeName() !== 'mixed') {
                    if ($type === $th->getTypeText()) {
                        $description = "$variable $description";
                    } else {
                        $description = "$type $variable $description";
                    }
                    $types[] = $th;
                }
                $variable = $token->getName();
            }
        } elseif ($matches[1] === 'param') {
            $params = Map{};
            if ($token instanceof ScannedFunctionAbstract) {
                foreach ($token->getParameters() as $p) {
                    $params[$p->getName()] = $p;
                }
            }
            if (substr($type, 0, 1) === '$') {
                if ($variable !== '-') {
                    $description = "$variable $description";
                }
                $variable = $type;
                $pt = $params->get(substr($variable, 1))?->getTypehint();
                if ($pt !== null) {
                    $types[] = $pt;
                }
            } else {
                $types->addAll($this->parseTypes($type, $token));
            }
        }
        return new ParameterTag($matches[1], $types, $variable, $description);
    }

    /**
     * Tries to convert a string typehint into a Vector.
     *
     * ```
     * parseTypes('Foo|string|null');
     * ```
     *
     * @param $hints - The hints to parse
     * @return - The typehints contained
     */
    protected function parseTypes(string $hints, ?ScannedBase $token = null): \ConstVector<ScannedTypehint>
    {
        $types = new \SplDoublyLinkedList();
        foreach (explode("|", trim($hints)) as $t) {
            $typehint = $this->stringToTypehint($t, $token);
            if ($typehint !== null) {
                /* HH_IGNORE_ERROR[4006]: Bug in typechecker */
                $types[] = $typehint;
            }
        }
        return new ImmVector($types);
    }

    /**
     * Tries to convert a string into a Typehint
     *
     * @param $t - The string typehint
     * @return - The scanned typehint or null
     */
    protected function stringToTypehint(string $t, ?ScannedBase $token = null): ?ScannedTypehint
    {
        $typehint = null;
        if (self::$thisy->contains($t) || self::$notTypes->contains($t)) {
            return new ScannedTypehint($t, Vector{}, false);
        } elseif (self::$replaceTypes->containsKey($t)) {
            $t = self::$replaceTypes[$t];
        }
        if ($t === '-') {
            return $typehint;
        }
        if (strlen($t) > 0) {
            if (preg_match('/\[\]$/', $t)) {
                // tags like @param Foo[] $stuff The stuff
                // should be @param array<Foo> $stuff The stuff
                $t = 'array<' . substr($t, 0, -2) . '>';
            }
            $sourceCode = '<?hh function test(' . $t . ' $a) {}';
            $tq = new \FredEmmott\DefinitionFinder\TokenQueue($sourceCode);
            while ($tq->haveTokens()) {
                $tk = $tq->shift();
                if ($tk[0] === '(') {
                    break;
                }
            }
            try {
                $tc = new \FredEmmott\DefinitionFinder\TypehintConsumer($tq, shape(
                  'filename' => $token?->getFileName() ?? '',
                  'sourceType' => $token?->getSourceType() ?? \FredEmmott\DefinitionFinder\SourceType::HACK_PARTIAL,
                  'namespace' => $token?->getNamespaceName(),
                  'aliases' => ImmMap{}, // TODO can we get this?
                  'genericTypeNames' => ImmSet{}, // TODO can we get this?
                ));
                $typehint = $tc->getTypehint();
            } catch (\Exception $e) {
                throw $e;
            }
        }
        return $typehint;
    }
}
