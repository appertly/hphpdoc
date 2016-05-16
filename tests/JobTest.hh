<?hh

namespace Hphpdoc;

use HackPack\HackUnit\Contract\Assert;

class JobTest
{
    <<Test>>
    public async function testGetDestination(Assert $assert): Awaitable<void>
    {
        $destination = "/tmp/foo";
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => 'test.php'), 'TESTING', "/** Hi */", '', null);
        $job = new Job($destination, Vector{$token});
        $assert->string($job->getDestination())->is($destination);
    }

    <<Test>>
    public async function testGetTokensByName(Assert $assert): Awaitable<void>
    {
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => 'test.php'), 'TESTING', "/** Hi */", '', null);
        $job = new Job("/tmp/bar", Vector{$token});
        $tokens = Map{'TESTING' => $token};
        $assert->container($job->getTokensByName())->containsOnly($tokens);
    }

    <<Test>>
    public async function testGetTokens(Assert $assert): Awaitable<void>
    {
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => 'test.php'), 'TESTING', "/** Hi */", '', null);
        $tokens = Vector{$token};
        $job = new Job("/tmp/bar", $tokens);
        $atokens = $job->getTokens();
        $assert->container($atokens)->containsOnly($tokens);
        $assert->mixed($job->getTokens())->identicalTo($atokens);
    }
}
