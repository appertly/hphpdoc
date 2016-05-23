<?hh
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
namespace Hphpdoc;

use Axe\Page;
use FredEmmott\DefinitionFinder\ScannedBase;
use FredEmmott\DefinitionFinder\ScannedClass;
use Psr\Log\LoggerInterface;
use Psr\Log\LoggerAwareTrait;
use League\CommonMark\DocParser;
use League\CommonMark\Environment;

/**
 * Publishes Stuff
 */
class Publisher
{
    use LoggerAwareTrait;
    use Producer;

    /**
     * The file writer
     */
    protected Io\Writer $writer;

    /**
     * The comment parser
     */
    protected Doc\Parser $commentParser;

    /**
     * The markdown parser
     */
    protected DocParser $mdParser;

    /**
     * Create a new Publisher.
     *
     * @param $logger - The logger
     */
    public function __construct(LoggerInterface $logger)
    {
        $this->logger = $logger;
        $this->writer = new Io\Writer($logger);
        $this->commentParser = new Doc\Parser();
        $this->mdParser = new DocParser(Environment::createCommonMarkEnvironment());
    }

    /**
     * Publishes our documentation
     */
    public function publish(string $destination, \ConstVector<ScannedBase> $tokens): void
    {
        $this->writer->mkdir($destination);
        $destination = realpath($destination);
        $this->logger?->info("Exporting HTML to $destination");
        $awaits = Vector{};
        $namespaces = Map{};
        foreach ($tokens as $token) {
            $ns = $token->getNamespaceName();
            if (!$namespaces->containsKey($ns)) {
                $namespaces[$ns] = Vector{};
            }
            $namespaces[$ns][] = $token;
        }
        ksort($namespaces);
        $job = new Job($destination, $tokens);
        $this->writer->copy(
            dirname(__DIR__) . DIRECTORY_SEPARATOR . 'css' . DIRECTORY_SEPARATOR . 'styles.css',
            "$destination/styles.css"
        );
        \HH\Asio\join(\HH\Asio\v($this->doNamespaces($job, $namespaces)));
    }

    /**
     * Exports namespace pages.
     *
     * @param $job - The job details
     * @param $namespaces - The tokens sorted by namespace name
     */
    protected function doNamespaces(Job $job, \ConstMap<string,Vector<ScannedBase>> $namespaces): \ConstVector<Awaitable<void>>
    {
        $page = $this->getPage("Namespaces");
        $awaits = Vector{};
        foreach ($namespaces as $n => $tokens) {
            $awaits->addAll($this->doNamespace($job, $n, $tokens));
        }
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <main>
                <header>
                    <h1>Namespaces</h1>
                </header>
                <hphpdoc:namespaces-list namespaces={$namespaces->keys()}/>
            </main>
            <hphpdoc:footer/>
        </axe:layout>;
        $destination = $job->getDestination();
        $awaits[] = $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . "index.html", $xhp);
        return $awaits;
    }

    /**
     * Exports a single namespace.
     *
     * @param $job - The file path to contain exported pages
     * @param $namespace - The namespace name
     * @param $tokens - The tokens in this namespace
     */
    protected function doNamespace(Job $job, string $namespace, Vector<ScannedBase> $tokens): \ConstVector<Awaitable<void>>
    {
        $page = $this->getPage("Namespace $namespace");
        $awaits = Vector{};
        foreach ($tokens as $t) {
            if ($t instanceof ScannedClass) {
                $block = $this->commentParser->parse($t);
                $awaits[] = $this->doClass($job, $t, $block);
            }
        }
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:namespace namespace={$namespace} tokens={$tokens} tokensByName={$job->getTokensByName()} />
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('docParser', $this->commentParser);
        $xhp->setContext('mdParser', $this->mdParser);
        $awaits[] = $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . 'namespace-' . str_replace("\\", '_', $namespace) .  ".html", $xhp);
        return $awaits;
    }

    /**
     * Exports the page for a class, interface, or trait.
     *
     * @param $job - The file path to contain exported pages
     * @param $c - The scanned token
     * @param $block - Optional. The parsed doc block
     */
    protected function doClass(Job $job, ScannedClass $c, ?Doc\Block $block = null): Awaitable<void>
    {
        $block = $block === null ? $this->commentParser->parse($c) : $block;
        $page = $this->getPage('Class ' . $c->getName());
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:classlike block={$block}/>
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('scannedClass', $c);
        $xhp->setContext('docParser', $this->commentParser);
        $xhp->setContext('mdParser', $this->mdParser);
        return $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . $this->getFilename($c), $xhp);
    }
}
