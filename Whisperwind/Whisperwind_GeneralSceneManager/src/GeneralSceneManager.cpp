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

#include "SceneNode.h"
#include "SceneObject.h"
#include "GeneralSceneNode.h"
#include "GeneralSceneManager.h"

namespace Engine
{
	static const Util::Wstring ROOT_SCENE_NODE_NAME(TO_UNICODE("SceneGraphRoot"));
	//---------------------------------------------------------------------
	GeneralSceneManager::~GeneralSceneManager()
	{
		destroyAllSceneNode();
		destroyAllSceneObject();
	}
	//---------------------------------------------------------------------
	SceneNodePtr GeneralSceneManager::createSceneNode_impl(const Util::Wstring & name, Util::u_int nodeType)
	{
		GeneralSceneNodePtr sceneNode = boost::make_shared<GeneralSceneNode>(name, nodeType);

		if ((NT_DYNAMIC & nodeType) != 0)
		{
			if (0 == (NT_AS_CHILD & nodeType))
				mSceneGraphVec.push_back(sceneNode);

			mDynamicSpatialPartitionVec.push_back(sceneNode);
		}
		else if ((NT_STATIC & nodeType) != 0)
		{
			mStaticSpatialPartitionVec.push_back(sceneNode);
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

			BOOST_AUTO(it, mStaticSpatialPartitionVec.begin());
			for ( ; it != mStaticSpatialPartitionVec.end(); ++ it)
			{
				if (name == (*it)->getName())
				{
					mStaticSpatialPartitionVec.erase(it);
					break;
				}
			}
		}
		else if ((nodeType & NT_STATIC) != 0)
		{
			BOOST_AUTO(it, mDynamicSpatialPartitionVec.begin());
			for ( ; it != mDynamicSpatialPartitionVec.end(); ++ it)
			{
				if (name == (*it)->getName())
				{
					mDynamicSpatialPartitionVec.erase(it);
					break;
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void GeneralSceneManager::destroyAllSceneNode_impl()
	{
		mSceneGraphVec.clear();
		mStaticSpatialPartitionVec.clear();
		mDynamicSpatialPartitionVec.clear();
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
		
		/// TODO:Find visibilty
		{
			/// StaticSpatialPartitionVec
			{
				BOOST_AUTO(it, mStaticSpatialPartitionVec.begin());
				for (it; it != mStaticSpatialPartitionVec.end(); ++it)
				{
					(*it)->addToRenderQueue();
				}
			}

			/// DynamicSpatialPartitionVec
			{
				BOOST_AUTO(it, mDynamicSpatialPartitionVec.begin());
				for (it; it != mDynamicSpatialPartitionVec.end(); ++it)
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
			(*it)->update(false);
		}
	}

}