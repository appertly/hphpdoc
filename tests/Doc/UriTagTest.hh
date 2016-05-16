<?hh

namespace Hphpdoc\Doc;

use HackPack\HackUnit\Contract\Assert;

class UriTagTest
{
    <<Test>>
    public async function testGetType(Assert $assert): Awaitable<void>
    {
        $uri = "http://example.com";
        $tag = new UriTag("test", $uri, '');
        $assert->string($tag->getUri())->is($uri);
    }

    <<Test>>
    public async function testToString(Assert $assert): Awaitable<void>
    {
        $uri = "http://example.com";
        $tag = new UriTag("test", $uri, 'Testing');
        $assert->string((string)$tag)->is('@test http://example.com Testing');
    }

    <<Test>>
    public async function testApplies(Assert $assert): Awaitable<void>
    {
        foreach (['see', 'example', 'license'] as $tagName) {
            $assert->bool(UriTag::applies($tagName))->is(true);
        }
        $assert->bool(UriTag::applies('foobar'))->is(false);
    }
}
