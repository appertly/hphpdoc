<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class DescribedTagTest
{
    <<Test>>
    public async function testGetDescription(Assert $assert): Awaitable<void>
    {
        $description = "Hey this is cool";
        $tag = new DescribedTag("test", $description);
        $assert->string($tag->getDescription())->is($description);
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $description = "Hey this is cool";
        $tag = new DescribedTag("test", $description);
        $assert->string((string)$tag)->is("@test Hey this is cool");
    }
}
