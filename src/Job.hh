<?hh // strict
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

use Facebook\DefinitionFinder\ScannedBase;

/**
 * Contains details of a job run.
 */
class Job
{
    /**
     * Creates a new Job.
     *
     * @param $destination - The file path to contain exported pages
     * @param $mapper - The mapper
     */
    public function __construct(private string $destination, private Source\Mapper $mapper)
    {
    }

    /**
     * Gets the export file path.
     *
     * @return - The file path to contain exported pages
     */
    public function getDestination(): string
    {
        return $this->destination;
    }

    /**
     * Gets the Mapper
     *
     * @return - The mapper
     */
    public function getMapper(): Source\Mapper
    {
        return $this->mapper;
    }

    /**
     * Gets the tokens, keyed by name
     *
     * @return - The scanned tokens, keyed by name
     */
    public function getTokensByName(): \ConstMap<string,ScannedBase>
    {
        return $this->mapper->getTokensByName();
    }

    /**
     * Gets the namespaces of the tokens.
     *
     * @return - The token namespaces
     */
    public function getNamespaces(): \ConstSet<string>
    {
        return $this->mapper->getNamespaces();
    }

    /**
     * Gets the tokens.
     *
     * @return - The scanned tokens
     */
    public function getTokens(): \ConstVector<ScannedBase>
    {
        return $this->mapper->getTokens();
    }
}
