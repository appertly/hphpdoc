<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class TypedTagTest
{
    <<Test>>
    public async function testGetType(Assert $assert): Awaitable<void>
    {
        $types = Vector{
            new \FredEmmott\DefinitionFinder\ScannedTypehint('array', Vector{}, false),
            new \FredEmmott\DefinitionFinder\ScannedTypehint('string', Vector{}, false)
        };
        $tag = new TypedTag("test", $types, '');
        $assert->container($tag->getTypes())->containsOnly($types);
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $types = Vector{
            new \FredEmmott\DefinitionFinder\ScannedTypehint('array', Vector{}, false),
            new \FredEmmott\DefinitionFinder\ScannedTypehint('string', Vector{}, false)
        };
        $tag = new TypedTag("test", $types, 'Hello');
        $assert->string((string)$tag)->is("@test array|string Hello");
    }
}
