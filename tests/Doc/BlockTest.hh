<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class BlockTest
{
    <<Test>>
    public async function testConstruct(Assert $assert): Awaitable<void>
    {
        $summary = "Foo";
        $description = "Bar";
        $tags = [new Tag("test")];
        $block = new Block($summary, $description, $tags);
        $assert->string($block->getSummary())->is($summary);
        $assert->string($block->getDescription())->is($description);
        $assert->container($block->getTags())->containsOnly($tags);
    }
}
