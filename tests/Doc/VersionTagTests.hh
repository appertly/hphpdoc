<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class VersionTagTest
{
    <<Test>>
    public async function testGetType(Assert $assert): Awaitable<void>
    {
        $version = "0.1.0";
        $tag = new VersionTag("test", $version, '');
        $assert->string($tag->getVersion())->is($version);
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $version = "0.1.0";
        $tag = new VersionTag("test", $version, 'Testing');
        $assert->string((string)$tag)->is('@test 0.1.0 Testing');
    }

    <<Test>>
    public async function testApplies(Assert $assert): Awaitable<void>
    {
        foreach (['version', 'since', 'deprecated'] as $tagName) {
            $assert->bool(VersionTag::applies($tagName))->is(true);
        }
        $assert->bool(VersionTag::applies('foobar'))->is(false);
    }
}
