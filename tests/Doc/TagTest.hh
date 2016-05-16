<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class TagTest
{
    <<Test>>
    public async function testGetName(Assert $assert): Awaitable<void>
    {
        $tag = new Tag("test");
        $assert->string($tag->getName())->is("test");
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $tag = new Tag("test");
        $assert->string((string)$tag)->is("@test");
    }
}
