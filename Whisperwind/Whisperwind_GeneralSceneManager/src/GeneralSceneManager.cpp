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

#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

#include "SceneObject.h"
#include "Camera.h"
#include "Frustum.h"
#include "EngineManager.h"
#include "ABTreeSceneNode.h"
#include "ABTree.h"
#include "OctreeSceneNode.h"
#include "LooseOctree.h"
#include "GeneralSceneManager.h"

namespace Engine
{
	//---------------------------------------------------------------------
	GeneralSceneManager::GeneralSceneManager()
	{
		mLooseOctree = boost::make_shared<LooseOctree>();
		mABTree = boost::make_shared<ABTree>();
	}
	//---------------------------------------------------------------------
	GeneralSceneManager::~GeneralSceneManager()
	{
		destroyAllSceneNode();
		destroyAllSceneObject();
	}
	//---------------------------------------------------------------------
	SceneNodePtr GeneralSceneManager::createSceneNode_impl(const Util::Wstring & name, Util::u_int nodeType)
	{
		SceneNodePtr sceneNode;

		if ((NT_DYNAMIC & nodeType) != 0)
		{
			sceneNode = boost::make_shared<ABTreeSceneNode>(name, nodeType);

			if (0 == (NT_AS_CHILD & nodeType))
				mSceneGraphVec.push_back(sceneNode);
		}
		else if ((NT_STATIC & nodeType) != 0)
		{
			sceneNode = boost::make_shared<OctreeSceneNode>(name, nodeType);
		}

		return sceneNode;
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::init_impl()
	{}
	//---------------------------------------------------------------------
	void GeneralSceneManager::destroySceneNode_impl(const Util::Wstring & name)
	{
		SceneNodePtr & node = mSceneNodeMap[name];
		Util::u_int nodeType = node->getNodeType();

		if ((nodeType & NT_DYNAMIC) != 0)
		{
			if (0 == (nodeType & NT_AS_CHILD))
			{
				BOOST_AUTO(it, mSceneGraphVec.begin());
				for ( ; it != mSceneGraphVec.end(); ++ it)
				{
					if (name == (*it)->getName())
					{
						mSceneGraphVec.erase(it);
						break;
					}
				}
			}

			mABTree->removeSceneNode(node);
		}
		else if ((nodeType & NT_STATIC) != 0)
		{
			mLooseOctree->removeOctreeNode(node);
		}
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::destroyAllSceneNode_impl()
	{
		mSceneGraphVec.clear();

		mABTree->removeAllSceneNode();
		mLooseOctree->removeAllOctreeNode();
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::preUpdate_impl(Util::time elapsedTime)
	{
		updateSceneGraph();
		
		/// Scene object
		{
			BOOST_AUTO(it, mSceneObjectMap.begin());
			for (it; it != mSceneObjectMap.end(); ++it)
			{
				it->second->preUpdate(elapsedTime);
			}
		}
		
		{
			const FrustumPtr & frustum = EngineManager::getSingleton().getCamera()->getFrustum();

			/// TODO:ABTree
			{
				const SceneNodeVector & snVec = mABTree->findVisibles(frustum);
				BOOST_AUTO(it, snVec.begin());
				for (it; it != snVec.end(); ++it)
				{
					(*it)->addToRenderQueue();
				}
			}

			/// LooseOctree
			{
				const SceneNodeVector & snVec = mLooseOctree->findVisibles(frustum);
				BOOST_AUTO(it, snVec.begin());
				for (it; it != snVec.end(); ++it)
				{
					(*it)->addToRenderQueue();
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::postUpdate_impl(Util::time elapsedTime)
	{
		updateSceneGraph();

		/// Scene object
		{
			BOOST_AUTO(it, mSceneObjectMap.begin());
			for (it; it != mSceneObjectMap.end(); ++it)
			{
				it->second->postUpdate(elapsedTime);
			}
		}
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::updateSceneGraph()
	{
		BOOST_AUTO(it, mSceneGraphVec.begin());
		for (it; it != mSceneGraphVec.end(); ++it)
		{
			(*it)->update();
		}
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::createDebugRendering()
	{
		SceneManager::createDebugRendering();

		mLooseOctree->createDebugRendering();
	}

}