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
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

#include "CheckedCast.h"
#include "MathDefine.h"
#include "AABB.h"
#include "Frustum.h"
#include "OctreeSceneNode.h"
#include "LooseOctree.h"

/// For debug rendering
#include <boost/lexical_cast.hpp>
#include "EngineManager.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "DebugResource.h"

namespace Engine
{
	static Util::real WORLD_SIZE = 20000.0f;
	static const Util::u_int MAX_OCTREE_DEEP = 8;
	static const Util::real LOOSE_K = 2.0f;
	static const LooseOctreeZonePtr NULL_ZONE;
	//---------------------------------------------------------------------
	// LooseOctreeZone
	//---------------------------------------------------------------------
	LooseOctreeZone::LooseOctreeZone(FXMVECTOR minPoint, FXMVECTOR maxPoint, Util::u_int depth) :
	    mDepth(depth),
		mTotalNodes(0)
	{
		mAABB = boost::make_shared<Util::AABB>(minPoint, maxPoint);
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::addOctreeNode(const OctreeSceneNodePtr & node, Util::u_int depth)
	{
		const Util::AABBPtr & aabb = node->getAABB();

		if ((depth < MAX_OCTREE_DEEP) && (isFitInChildZone(aabb)))
		{
			LooseOctreeZonePtr & zone = getFitChildZone(aabb);

			zone->addOctreeNode(node, ++depth);
		}
		else
		{
			mSceneNodeVec.push_back(node);

			node->setZone(this->shared_from_this());

			increaseTotalNodesCount();
		}
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::removeOctreeNode(const SceneNodePtr & node)
	{
		BOOST_AUTO(nodeIt, std::find(mSceneNodeVec.begin(), mSceneNodeVec.end(), node));
		if (nodeIt != mSceneNodeVec.end())
		{
			OctreeSceneNodePtr octreeNode = Util::checkedPtrCast<OctreeSceneNode>(node);
			octreeNode->setZone(NULL_ZONE);

			mSceneNodeVec.erase(nodeIt);

			decreaseTotalNodesCount();
		}
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::removeAllOctreeNode()
	{
		while (!mSceneNodeVec.empty())
		{
			removeOctreeNode(*(mSceneNodeVec.begin()));
		}
		mSceneNodeVec.clear();
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::clear()
	{
		removeAllOctreeNode();

		for (Util::u_int x = 0; x < 2; ++x)
		{
			for (Util::u_int y = 0; y < 2; ++y)
			{
				for (Util::u_int z = 0; z < 2; ++z)
				{
					if (mChildZones[x][y][z])
					{
						mChildZones[x][y][z]->setParentZone(NULL_ZONE);
						mChildZones[x][y][z]->clear();
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::increaseTotalNodesCount()
	{
		++mTotalNodes;

		if (mParentZone)
			mParentZone->increaseTotalNodesCount();
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::decreaseTotalNodesCount()
	{
		--mTotalNodes;

		if (mParentZone)
			mParentZone->decreaseTotalNodesCount();
	}
	//---------------------------------------------------------------------
	bool LooseOctreeZone::isInside(const Util::AABBPtr & aabb)
	{
		XMVECTOR size = XMVectorReplicate((LOOSE_K * WORLD_SIZE / (2 << mDepth)));

		XMVECTOR minPoint = mAABB->getCenterPoint() - size;
		XMVECTOR maxPoint = mAABB->getCenterPoint() + size;

		return XMVector3LessOrEqual(minPoint, aabb->getMinPoint()) && 
			XMVector3LessOrEqual(aabb->getMaxPoint(), maxPoint);
	}
	//---------------------------------------------------------------------
	bool LooseOctreeZone::isFitInChildZone(const Util::AABBPtr & aabb)
	{
		XMVECTOR halfSideSize = XMVectorReplicate((LOOSE_K * WORLD_SIZE / (2 << mDepth)) * 0.5f);

		XMVECTOR nodeSize = aabb->getSize();

		/// "&& true" to kill the warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning).
		return XMVector3LessOrEqual(nodeSize, halfSideSize) && true;
	}
	//---------------------------------------------------------------------
	LooseOctreeZonePtr & LooseOctreeZone::getFitChildZone(const Util::AABBPtr & aabb)
	{
		XMVECTOR nodeCenter = aabb->getCenterPoint();
		XMVECTOR myCenter = mAABB->getCenterPoint();

		Util::u_int x = (XMVectorGetX(nodeCenter) > XMVectorGetX(myCenter)) ? 1 : 0;
		Util::u_int y = (XMVectorGetY(nodeCenter) > XMVectorGetY(myCenter)) ? 1 : 0;
		Util::u_int z = (XMVectorGetZ(nodeCenter) > XMVectorGetZ(myCenter)) ? 1 : 0;

		if (NULL == mChildZones[x][y][z])
		{
			XMVECTOR minPoint = mAABB->getMinPoint();
			XMVECTOR maxPoint = mAABB->getMaxPoint();
			XMFLOAT3 newMin;
			XMFLOAT3 newMax;

			if (0 == x)
			{
				newMin.x = XMVectorGetX(minPoint);
				newMax.x = XMVectorGetX(myCenter);
			}
			else
			{
				newMin.x = XMVectorGetX(myCenter);
				newMax.x = XMVectorGetX(maxPoint);
			}

			if (0 == y)
			{
				newMin.y = XMVectorGetY(minPoint);
				newMax.y = XMVectorGetY(myCenter);
			}
			else
			{
				newMin.y = XMVectorGetY(myCenter);
				newMax.y = XMVectorGetY(maxPoint);
			}

			if (0 == z)
			{
				newMin.z = XMVectorGetZ(minPoint);
				newMax.z = XMVectorGetZ(myCenter);
			}
			else
			{
				newMin.z = XMVectorGetZ(myCenter);
				newMax.z = XMVectorGetZ(maxPoint);
			}

			mChildZones[x][y][z] = boost::make_shared<LooseOctreeZone>(XMLoadFloat3(&newMin), XMLoadFloat3(&newMax), mDepth + 1);
			mChildZones[x][y][z]->setParentZone(this->shared_from_this()); 
		}

		return mChildZones[x][y][z];
	}
	//---------------------------------------------------------------------
	const Util::AABBPtr & LooseOctreeZone::getCullAABB()
	{
		XMVECTOR size = XMVectorReplicate((LOOSE_K * WORLD_SIZE / (2 << mDepth)));

		/// static for perfomance.
		static Util::AABBPtr aabb = boost::make_shared<Util::AABB>();
		aabb->reset();

		aabb->addPoint(mAABB->getCenterPoint() - size);
		aabb->addPoint(mAABB->getCenterPoint() + size);

		return aabb;
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::walkOctree(const FrustumPtr & frustum, SceneNodeVector & outNodeVec)
	{
		IF_FALSE_RETURN(!isEmpty());

		VisibilityType vt = frustum->isVisible(getCullAABB());

		if (VT_NONE == vt)
		{
			return;
		}
		else if (VT_FULL == vt)
		{
			BOOST_FOREACH(const SceneNodePtr & node, mSceneNodeVec)
			{
				outNodeVec.push_back(node);
			}
		}
		else if (VT_PARTIAL == vt)
		{
			BOOST_FOREACH(const SceneNodePtr & node, mSceneNodeVec)
			{
				VisibilityType visType = frustum->isVisible(node->getAABB());
				if (visType != VT_NONE)
				{
					/// TODO:If VT_PARTIAL,then cull the scene objects.
					outNodeVec.push_back(node);
				}
			}
		}

		for (Util::u_int x = 0; x < 2; ++x)
		{
			for (Util::u_int y = 0; y < 2; ++y)
			{
				for (Util::u_int z = 0; z < 2; ++z)
				{
					if (mChildZones[x][y][z])
						mChildZones[x][y][z]->walkOctree(frustum, outNodeVec);
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void LooseOctreeZone::createDebugRendering()
	{
		SceneManagerPtr & sceneMgr = EngineManager::getSingleton().getSceneManager();

		static Util::u_int nameGenerator = 0;
		++nameGenerator;

		/// static for performance.
		static Util::Wstring name;
		static Util::Wstring nodeName;
		static Util::Wstring objectName;
		static Util::Wstring objectResName;

		name = boost::lexical_cast<Util::Wstring>(nameGenerator);
		nodeName = name + TO_UNICODE("_debugLooseOctreeNode");
		objectName = name + TO_UNICODE("_debugLooseOctreeObject");
		objectResName = nodeName + DebugResource::getDebugNodeSuffix();

		SceneNodePtr & node = sceneMgr->createSceneNode(nodeName, NT_STATIC);
		node->setAABB(mAABB);

		SceneObjectPtr & obj = sceneMgr->createSceneObject(DebugResource::getDebugCreateType(), 
			objectName, objectResName);

		node->attachSceneObject(obj);

		for (Util::u_int x = 0; x < 2; ++x)
		{
			for (Util::u_int y = 0; y < 2; ++y)
			{
				for (Util::u_int z = 0; z < 2; ++z)
				{
					if (mChildZones[x][y][z])
					{
						mChildZones[x][y][z]->createDebugRendering();
					}
				}
			}
		}
	}

	//---------------------------------------------------------------------
	// LooseOctree
	//---------------------------------------------------------------------
	LooseOctree::LooseOctree()
	{}
	//---------------------------------------------------------------------
	void LooseOctree::createRootZone()
	{
		/// Sorry for the globle variable.
		WORLD_SIZE = EngineManager::getSingleton().getSceneManager()->getWorldSize();

		Util::real size = WORLD_SIZE * 0.5f;
		mRootZone = boost::make_shared<LooseOctreeZone>(
			XMVectorSet(-size, -size, -size, 0.0f),
			XMVectorSet(size, size, size, 0.0f),
			0);
	}
	//---------------------------------------------------------------------
	void LooseOctree::addOctreeNode(const OctreeSceneNodePtr & node)
	{
		if (!mRootZone)
			createRootZone();

		const Util::AABBPtr & aabb = node->getAABB();
		
		/// TODO:Now donnot handle the node outside root zone.
		IF_FALSE_RETURN(isInside(aabb));

		mRootZone->addOctreeNode(node, 0);
	}
	//---------------------------------------------------------------------
	void LooseOctree::removeOctreeNode(const SceneNodePtr & node)
	{
		OctreeSceneNodePtr octreeNode = Util::checkedPtrCast<OctreeSceneNode>(node);
		LooseOctreeZonePtr & zone = octreeNode->getZone();
		if (zone)
			zone->removeOctreeNode(node);
	}
	//---------------------------------------------------------------------
	void LooseOctree::removeAllOctreeNode()
	{
		mRootZone->clear();
	}
	//---------------------------------------------------------------------
	bool LooseOctree::isInside(const Util::AABBPtr & aabb)
	{
		return XMVector3LessOrEqual(mRootZone->getAABB()->getMinPoint(), aabb->getMinPoint()) && 
			XMVector3LessOrEqual(aabb->getMaxPoint(), mRootZone->getAABB()->getMaxPoint());
	}
	//---------------------------------------------------------------------
	const SceneNodeVector & LooseOctree::findVisibles(const FrustumPtr & frustum)
	{
		mVisibleNodeVec.clear();

		mRootZone->walkOctree(frustum, mVisibleNodeVec);

		return mVisibleNodeVec;
	}
	//---------------------------------------------------------------------
	void LooseOctree::createDebugRendering()
	{
		mRootZone->createDebugRendering();
	}

}