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

use Facebook\DefinitionFinder\ScannedFunctionAbstract;
use Hphpdoc\Source\FunctionyDeclaration;
use League\CommonMark\DocParser;

/**
 * To be used by XHP that renders functions and methods
 */
trait FunctionHelper
{
    use Hphpdoc\Producer;

    protected function getReturns(FunctionyDeclaration $function, DocParser $mdParser): ?XHPRoot
    {
        $returnType = $function->getToken()->getReturnType();
        if ($returnType !== null && $this->isVoid($returnType)) {
            return null;
        }
        $tht = $returnType?->getTypeText();
        $rt = Vector{$returnType};
        $tag = $function->getReturnsTag();
        $description = $tag?->getDescription();
        if ($tag !== null && ($returnType === null || $returnType->getTypeName() === 'mixed' || $tht === 'array')) {
            $rt = $tag->getTypes();
        }
        return <table class="tags-return">
            <caption>Returns</caption>
            <thead>
                <tr>
                    <th scope="col">Type</th>
                    <th scope="col">Description</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td><hphpdoc:typehints tokens={$rt} returnType={true}/></td>
                    <td><axe:markdown text={$description} docParser={$mdParser}/></td>
                </tr>
            </tbody>
        </table>;
    }

    protected function getParameters(FunctionyDeclaration $function, DocParser $mdParser): ?XHPRoot
    {
        $params = $function->getToken()->getParameters();
        if ($params->isEmpty()) {
            return null;
        }
        $tbody = <tbody/>;
        $tags = $function->getParameterTags();
        foreach ($params as $p) {
            $th = $p->getTypehint();
            $rt = Vector{$th};
            $t = $tags[$p->getName()];
            $description = $t->getDescription();
            $tht = $th?->getTypeText();
            if ($th === null || $th->getTypeName() === 'mixed' || $tht === 'array') {
                $rt = $t->getTypes();
            }
            $tbody->appendChild(
                <tr>
                    <th scope="row"><code class="parameter-name"><var>{"$"}{$p->getName()}</var></code></th>
                    <td><hphpdoc:typehints tokens={$rt}/></td>
                    <td><axe:markdown text={$description} docParser={$mdParser}/></td>
                </tr>
            );
        }
        return <table class="tags-parameters">
            <caption>Parameters</caption>
            <thead>
                <tr>
                    <th scope="col">Name</th>
                    <th scope="col">Type</th>
                    <th scope="col">Description</th>
                </tr>
            </thead>
            {$tbody}
        </table>;
    }

    protected function getThrows(FunctionyDeclaration $function, DocParser $mdParser): ?XHPRoot
    {
        $tags = $function->getThrowsTags();
        if ($tags->isEmpty()) {
            return null;
        }
        $tbody = <tbody/>;
        foreach ($tags as $tag) {
            $tbody->appendChild(
                <tr>
                    <th scope="row">
                        <hphpdoc:typehints tokens={$tag->getTypes()}/>
                    </th>
                    <td>
                        <axe:markdown text={$tag->getDescription()} docParser={$mdParser}/>
                    </td>
                </tr>
            );
        }
        return <table class="tags-throws">
            <caption>Throws</caption>
            <thead>
                <tr>
                    <th scope="col">Type</th>
                    <th scope="col">Conditions</th>
                </tr>
            </thead>
            {$tbody}
        </table>;
    }
}
