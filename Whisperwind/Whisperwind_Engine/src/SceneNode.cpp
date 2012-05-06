/*-------------------------------------------------------------------------
This source file is a part of Whisperwind.(GameEngine + GamePlay + GameTools)
For the latest info, see http://lisuyong.com

Copyright (c) 2012 Suyong Li (79481268@qq.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/

#include "SceneNode.h"
#include "SceneObject.h"
#include "DebugDefine.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void SceneNode::attachSceneObject(SceneObjectPtr & sceneObj)
	{
		WHISPERWIND_ASSERT(mSceneObjectMap.find(sceneObj->getName()) == mSceneObjectMap.end());

		if (sceneObj->getAttachedSceneNode())
			sceneObj->getAttachedSceneNode()->dettachSceneObject(sceneObj);

		mSceneObjectMap[sceneObj->getName()] = sceneObj;

		sceneObj->setAttachedSceneNode(this->shared_from_this());
	}
	//---------------------------------------------------------------------
	void SceneNode::dettachSceneObject(SceneObjectPtr & sceneObj)
	{
		WHISPERWIND_ASSERT(mSceneObjectMap.find(sceneObj->getName()) != mSceneObjectMap.end());

		mSceneObjectMap.erase(sceneObj->getName());

		sceneObj->setAttachedSceneNode(SceneNodePtr());
	}
	//---------------------------------------------------------------------
	void SceneNode::addChildNode(const SceneNodePtr & sceneNode)
	{
		WHISPERWIND_ASSERT(mChildSceneNodeMap.find(sceneNode->getName()) == mChildSceneNodeMap.end());

		mChildSceneNodeMap[sceneNode->getName()] = sceneNode;
	}
	//---------------------------------------------------------------------
	bool SceneNode::getChildNode(const Util::Wstring & name, SceneNodePtr & outSceneNode)
	{
		if (mChildSceneNodeMap.find(name) != mChildSceneNodeMap.end())
		{
			outSceneNode = mChildSceneNodeMap[name];
			return true;
		}

		return false;
	}
	//---------------------------------------------------------------------
	void SceneNode::update(Util::time elapsedTime)
	{
		update_impl(elapsedTime);
	}

}