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
use FredEmmott\DefinitionFinder\ScannedConstant;
use FredEmmott\DefinitionFinder\ScannedFunction;
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
        $mapper = new Source\Mapper($tokens, $this->commentParser);
        $job = new Job($destination, $mapper);
        $this->logger?->info("Exporting HTML to $destination");
        $this->writer->copy(
            dirname(__DIR__) . DIRECTORY_SEPARATOR . 'css' . DIRECTORY_SEPARATOR . 'styles.css',
            "$destination/styles.css"
        );
        \HH\Asio\join(\HH\Asio\v($this->doNamespaces($job)));
    }

    /**
     * Exports namespace pages.
     *
     * @param $job - The job details
     * @param $mapper - The mapper
     * @return - The awaitables for each namespace
     */
    protected function doNamespaces(Job $job): \ConstVector<Awaitable<void>>
    {
        $page = $this->getPage("Namespaces");
        $awaits = Vector{};
        $namespaces = $job->getMapper()->getTokensByNamespace();
        $ns = new Map($namespaces);
        foreach ($namespaces->keys() as $k) {
            $split = explode('\\', $k);
            while(array_pop($split)) {
                $n = implode('\\', $split);
                if ($n && !$ns->containsKey($n)) {
                    $ns[$n] = ImmVector{};
                }
            }
        }
        ksort($ns);
        foreach ($ns as $n => $tokens) {
            $awaits->addAll($this->doNamespace($job, $n, $tokens));
        }
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <main>
                <header>
                    <h1>Namespaces</h1>
                </header>
                <hphpdoc:namespaces-list namespaces={$ns->keys()}/>
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
     * @param $mapper - The mapper
     * @return - The awaitables for each member of the namespace
     */
    protected function doNamespace(Job $job, string $namespace, \ConstVector<ScannedBase> $tokens): \ConstVector<Awaitable<void>>
    {
        $mapper = $job->getMapper();
        $page = $this->getPage("Namespace $namespace");
        $awaits = Vector{};
        $declarations = Vector{};
        foreach ($tokens as $t) {
            if ($t instanceof ScannedClass) {
                $cd = $mapper->getClass($t->getName());
                if ($cd !== null) {
                    $declarations[] = $cd;
                    $awaits[] = $this->doClass($job, $cd);
                }
            } elseif ($t instanceof ScannedFunction) {
                $fd = $mapper->getFunction($t->getName());
                if ($fd !== null) {
                    $declarations[] = $fd;
                }
            } elseif ($t instanceof ScannedConstant) {
                $cd = $mapper->getConstant($t->getName());
                if ($cd !== null) {
                    $declarations[] = $cd;
                }
            }
        }
        /* HH_IGNORE_ERROR[4110] */
        $awaits[] = $this->doConstants($job, $namespace, $declarations->filter($a ==> $a instanceof Source\ConstantDeclaration));
        /* HH_IGNORE_ERROR[4110] */
        $awaits[] = $this->doFunctions($job, $namespace, $declarations->filter($a ==> $a instanceof Source\FunctionDeclaration));
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:namespace namespace={$namespace} declarations={$declarations} namespaces={$this->getSubNamespaces($job, $namespace)}/>
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('mdParser', $this->mdParser);
        $awaits[] = $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . 'namespace-' . str_replace("\\", '_', $namespace) .  ".html", $xhp);
        return $awaits;
    }

    /**
     * Gets any sub-namespaces for a given namespace.
     *
     * @param $job - The job
     * @param $namespace - The namespace
     */
    protected function getSubNamespaces(Job $job, string $namespace): \ConstSet<string>
    {
        $namespaces = $job->getTokensByName()->toMap();
        $namespaces = $namespaces->filterWithKey(function ($k, $v) use ($namespace) {
            $ns = substr($k, 0, strrpos($k, "\\"));
            return substr($ns, 0, strlen($namespace) + 1) === "$namespace\\";
        });
        ksort($namespaces);
        return $namespaces->keys()->map($k ==> substr($k, 0, strrpos($k, "\\")))->toSet();
    }

    /**
     * Exports the page for a class, interface, or trait.
     *
     * @param $job - The export job
     * @param $c - The scanned token
     * @return - Awaitable for writing the class
     */
    protected function doClass(Job $job, Source\ClassyDeclaration $c): Awaitable<void>
    {
        $page = $this->getPage('Class ' . $c->getName());
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:classlike/>
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('classyDeclaration', $c);
        $xhp->setContext('mdParser', $this->mdParser);
        return $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . $this->getFilename($c->getToken()), $xhp);
    }

    /**
     * Exports the page for functions.
     *
     * @param $job - The export job
     * @param $namespace - The namespace to scan
     * @param $functions - The list of functions
     * @return - Awaitable for writing the class
     */
    public function doFunctions(Job $job, string $namespace, Vector<Source\FunctionDeclaration> $functions): Awaitable<void>
    {
        if ($functions->isEmpty()) {
            return \HH\Asio\usleep(0);
        }
        /* HH_FIXME[1002]: Bug in the typechecker */
        usort($functions, ($a, $b) ==> $a->getName() <=> $b->getName());
        $page = $this->getPage("Functions in $namespace");
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:functions namespace={$namespace} functions={$functions}/>
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('mdParser', $this->mdParser);
        $ns = str_replace("\\" , '_', $namespace);
        return $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . "functions-$ns.html", $xhp);
    }

    /**
     * Exports the page for functions.
     *
     * @param $job - The export job
     * @param $namespace - The namespace to scan
     * @param $functions - The list of functions
     * @return - Awaitable for writing the class
     */
    public function doConstants(Job $job, string $namespace, Vector<Source\ConstantDeclaration> $constants): Awaitable<void>
    {
        if ($constants->isEmpty()) {
            return \HH\Asio\usleep(0);
        }
        /* HH_FIXME[1002]: Bug in the typechecker */
        usort($constants, ($a, $b) ==> $a->getName() <=> $b->getName());
        $page = $this->getPage("Constants in $namespace");
        $xhp = <axe:layout page={$page}>
            <hphpdoc:banner/>
            <hphpdoc:constants namespace={$namespace} constants={$constants}/>
            <hphpdoc:footer/>
        </axe:layout>;
        $xhp->setContext('job', $job);
        $xhp->setContext('mdParser', $this->mdParser);
        $ns = str_replace("\\" , '_', $namespace);
        return $this->writer->write($job->getDestination() . DIRECTORY_SEPARATOR . "constants-$ns.html", $xhp);
    }
}
