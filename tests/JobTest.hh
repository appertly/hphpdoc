<?hh

namespace Hphpdoc;

use HackPack\HackUnit\Contract\Assert;

class JobTest
{
    <<Test>>
    public async function testGetDestination(Assert $assert): Awaitable<void>
    {
        $destination = "/tmp/foo";
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => __FILE__), 'TESTING', "/** Hi */", '', null);
        $parser = new Doc\Parser();
        $job = new Job($destination, new Source\Mapper(Vector{$token}, $parser));
        $assert->string($job->getDestination())->is($destination);
    }

    <<Test>>
    public async function testGetTokensByName(Assert $assert): Awaitable<void>
    {
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => __FILE__), 'TESTING', "/** Hi */", '', null);
        $parser = new Doc\Parser();
        $job = new Job("/tmp/bar", new Source\Mapper(Vector{$token}, $parser));
        $tokens = Map{'TESTING' => $token};
        $assert->container($job->getTokensByName())->containsOnly($tokens);
    }

    <<Test>>
    public async function testGetTokens(Assert $assert): Awaitable<void>
    {
        $token = new \FredEmmott\DefinitionFinder\ScannedConstant(shape('filename' => __FILE__), 'TESTING', "/** Hi */", '', null);
        $parser = new Doc\Parser();
        $tokens = Vector{$token};
        $job = new Job("/tmp/bar", new Source\Mapper($tokens, $parser));
        $atokens = $job->getTokens();
        $assert->container($atokens)->containsOnly($tokens);
        $assert->mixed($job->getTokens())->identicalTo($atokens);
    }
}
