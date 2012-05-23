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

#include <algorithm>
#include <boost/typeof/typeof.hpp>

#include "DebugDefine.h"
#include "SceneObject.h"
#include "SceneManager.h"
#include "SceneNode.h"

namespace Engine
{
	static const SceneNodePtr NULL_SCENE_NODE;
	//---------------------------------------------------------------------
	SceneNode::SceneNode(const Util::Wstring & name, Util::u_int nodeType) :
	    mName(name),
		mPosition(0.0f, 0.0f, 0.0f),
		mRelativePosition(0.0f, 0.0f, 0.0f),
		mNodeType(nodeType)
	{
		XMStoreFloat4(&mOrientation, XMQuaternionIdentity());
		XMStoreFloat4(&mRelativeOrientation, XMQuaternionIdentity());
	}
	//---------------------------------------------------------------------
	SceneNode::~SceneNode()
	{
		dettachAllSceneObject();
		removeAllChildNode();
	}
	//---------------------------------------------------------------------
	void SceneNode::attachSceneObject(SceneObjectPtr & sceneObj)
	{
		WHISPERWIND_ASSERT(std::find(mSceneObjectVec.begin(), mSceneObjectVec.end(), sceneObj) == mSceneObjectVec.end());

		if (sceneObj->getAttachedSceneNode())
			sceneObj->getAttachedSceneNode()->dettachSceneObject(sceneObj);

		mSceneObjectVec.push_back(sceneObj);

		sceneObj->setAttachedSceneNode(this->shared_from_this());
	}
	//---------------------------------------------------------------------
	void SceneNode::dettachSceneObject(SceneObjectPtr & sceneObj)
	{
		WHISPERWIND_ASSERT(std::find(mSceneObjectVec.begin(), mSceneObjectVec.end(), sceneObj) != mSceneObjectVec.end());

		sceneObj->setAttachedSceneNode(NULL_SCENE_NODE);

		mSceneObjectVec.erase(std::find(mSceneObjectVec.begin(), mSceneObjectVec.end(), sceneObj));
	}
	//---------------------------------------------------------------------
	void SceneNode::dettachAllSceneObject()
	{
		while (!mSceneObjectVec.empty())
		{
			dettachSceneObject(*(mSceneObjectVec.begin()));
		}

		mSceneObjectVec.clear();
	}
	//---------------------------------------------------------------------
	SceneNodePtr & SceneNode::createChildNode(const Util::Wstring & name)
	{
		return createChildNode_impl(name);
	}
	//---------------------------------------------------------------------
	void SceneNode::addChildNode(const SceneNodePtr & childNode)
	{
		WHISPERWIND_ASSERT(std::find(mChildSceneNodeVec.begin(), mChildSceneNodeVec.end(), childNode) == mChildSceneNodeVec.end());

		mChildSceneNodeVec.push_back(childNode);
	}
	//---------------------------------------------------------------------
	void SceneNode::setParentNode(const SceneNodePtr & parentNode)
	{
		if (mParentNode)
		{
			SceneNodePtr me = this->shared_from_this();
			mParentNode->removeChildNode(me);
		}

		mParentNode = parentNode;
		mParentNode->addChildNode(this->shared_from_this());
	}
	//---------------------------------------------------------------------
	void SceneNode::removeChildNode(SceneNodePtr & childNode)
	{
		BOOST_AUTO(childIt, std::find(mChildSceneNodeVec.begin(), mChildSceneNodeVec.end(), childNode));
		if (childIt != mChildSceneNodeVec.end())
		{
			childNode->setParentNode(NULL_SCENE_NODE);
			mChildSceneNodeVec.erase(childIt);
		}
	}
	//---------------------------------------------------------------------
	void SceneNode::removeAllChildNode()
	{
		while (!mChildSceneNodeVec.empty())
		{
			removeChildNode(*(mChildSceneNodeVec.begin()));
		}
		mChildSceneNodeVec.clear();
	}
	//---------------------------------------------------------------------
	bool SceneNode::getChildNode(const Util::Wstring & name, SceneNodePtr & outChildNode) const
	{
		for (SceneNodeVector::const_iterator childIt; childIt != mChildSceneNodeVec.end(); ++ childIt)
		{
			if (name == (*childIt)->getName())
			{
				outChildNode = *childIt;
				return true;
			}
		}

		return false;
	}
	//---------------------------------------------------------------------
	bool SceneNode::getParentNode(SceneNodePtr & outParentNode) const
	{
		if (mParentNode)
		{
			outParentNode = mParentNode;
			return true;
		}

		return false;
	}
	//---------------------------------------------------------------------
	void SceneNode::addToRenderQueue()
	{
		BOOST_AUTO(it, mSceneObjectVec.begin());
		for (it; it != mSceneObjectVec.end(); ++it)
		{
			(*it)->addToRenderQueue();
		}
	}
	//---------------------------------------------------------------------
	XMVECTOR SceneNode::getPosition() const
	{
		if (mParentNode)
			return mParentNode->getPosition() + XMLoadFloat3(&mRelativePosition);
		else
			return XMLoadFloat3(&mPosition);
	}
	//---------------------------------------------------------------------
	void SceneNode::setPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);

		if (mParentNode)
			XMStoreFloat3(&mRelativePosition, position - mParentNode->getPosition());

		mNeedUpdateChilds = true;
	}
	//---------------------------------------------------------------------
	XMVECTOR SceneNode::getRelativePosition() const
	{
		WHISPERWIND_ASSERT(mParentNode != NULL);

		return XMLoadFloat3(&mRelativePosition);
	}
	//---------------------------------------------------------------------
	void SceneNode::setRelativePosition(FXMVECTOR relPosition)
	{
		WHISPERWIND_ASSERT(mParentNode != NULL);

		XMStoreFloat3(&mRelativePosition, relPosition);

		if (mParentNode)
			XMStoreFloat3(&mPosition, relPosition + mParentNode->getPosition());

		mNeedUpdateChilds = true;
	}
	//---------------------------------------------------------------------
	XMVECTOR SceneNode::getOrientation() const
	{
		if (mParentNode)
			return XMQuaternionMultiply(XMLoadFloat4(&mRelativeOrientation), mParentNode->getOrientation());
		else
			return XMLoadFloat4(&mOrientation);
	}
	//---------------------------------------------------------------------
	void SceneNode::setOrientation(FXMVECTOR orientation)
	{
		XMStoreFloat4(&mOrientation, orientation);

		if (mParentNode)
			XMStoreFloat4(&mRelativeOrientation, XMQuaternionMultiply(XMQuaternionInverse(mParentNode->getOrientation()), orientation));

		mNeedUpdateChilds = true;
	}
	//---------------------------------------------------------------------
	XMVECTOR SceneNode::getRelativeOrientation() const
	{
		WHISPERWIND_ASSERT(mParentNode != NULL);

		return XMLoadFloat4(&mRelativeOrientation);
	}
	//---------------------------------------------------------------------
	void SceneNode::setRelativeOrientation(FXMVECTOR relOrientation)
	{
		WHISPERWIND_ASSERT(mParentNode != NULL);

		XMStoreFloat4(&mRelativeOrientation, relOrientation);

		if (mParentNode)
			XMStoreFloat4(&mOrientation, XMQuaternionMultiply(mParentNode->getOrientation(), relOrientation));

		mNeedUpdateChilds = true;
	}
	//---------------------------------------------------------------------
	void SceneNode::update()
	{
		if (mParentNode)
		{
			XMStoreFloat3(&mPosition, mParentNode->getPosition() + XMLoadFloat3(&mRelativePosition));
			XMStoreFloat4(&mOrientation, XMQuaternionMultiply(mParentNode->getOrientation(), XMLoadFloat4(&mRelativeOrientation)));
		}
		
		if (mNeedUpdateChilds)
		{
			BOOST_AUTO(it, mChildSceneNodeVec.begin());
			for (it; it != mChildSceneNodeVec.end(); ++it)
			{
				(*it)->update();
			}
		}
	}

}
