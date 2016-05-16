<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class ParameterTagTest
{
    <<Test>>
    public async function testGetType(Assert $assert): Awaitable<void>
    {
        $types = Vector{
            new \FredEmmott\DefinitionFinder\ScannedTypehint('array', Vector{}, false),
            new \FredEmmott\DefinitionFinder\ScannedTypehint('string', Vector{}, false)
        };
        $tag = new ParameterTag("test", $types, '$foobar', '');
        $assert->string($tag->getVariable())->is('foobar');
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $types = Vector{
            new \FredEmmott\DefinitionFinder\ScannedTypehint('array', Vector{}, false),
            new \FredEmmott\DefinitionFinder\ScannedTypehint('string', Vector{}, false)
        };
        $tag = new ParameterTag("test", $types, '$foobar', 'Hello');
        $assert->string((string)$tag)->is('@test array|string $foobar Hello');
    }

    <<Test>>
    public async function testApplies(Assert $assert): Awaitable<void>
    {
        foreach (['var', 'param', 'property'] as $tagName) {
            $assert->bool(ParameterTag::applies($tagName))->is(true);
        }
        $assert->bool(ParameterTag::applies('foobar'))->is(false);
    }
}
