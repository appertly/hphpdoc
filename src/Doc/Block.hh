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
namespace Hphpdoc\Doc;

/**
 * A parsed PHPDoc.
 */
class Block
{
    private string $summary;
    private string $description;
    private ImmVector<Tag> $tags;

    /**
     * Creates a new Block.
     *
     * @param $summary - The summary
     * @param $description - The description
     * @param $tags - The tags
     */
    public function __construct(string $summary, string $description, Traversable<Tag> $tags)
    {
        $this->summary = trim($summary);
        $this->description = trim($description);
        $this->tags = new ImmVector($tags);
    }

    /**
     * Gets the doc block summary
     *
     * @return The summary
     */
    public function getSummary(): string
    {
        return $this->summary;
    }

    /**
     * Gets the doc block description.
     *
     * @return The description
     */
    public function getDescription(): string
    {
        return $this->description;
    }

    /**
     * Gets the doc block tags.
     *
     * @return The tags
     */
    public function getTags(): ImmVector<Tag>
    {
        return $this->tags;
    }
}
