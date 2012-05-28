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

#include "CheckedCast.h"
#include "AABB.h"
#include "EngineManager.h"
#include "SceneManager.h"
#include "GeneralSceneManager.h"
#include "LooseOctree.h"
#include "OctreeSceneNode.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void OctreeSceneNode::updatedAABB()
	{
		IF_FALSE_RETURN(!mAABB->getIsInvalid());

		GeneralSceneManagerPtr gsMgr = Util::checkedPtrCast<GeneralSceneManager>(EngineManager::getSingleton().getSceneManager());
		LooseOctreePtr & octree = gsMgr->getLooseOctree();

		if (mZone && !mZone->isInside(mAABB))
		{
			SceneNodePtr me = this->shared_from_this(); 

			mZone->removeOctreeNode(me);

			octree->addOctreeNode(Util::checkedPtrCast<OctreeSceneNode>(me));
		}
		else if (!mZone)
		{
			SceneNodePtr me = this->shared_from_this(); 
			octree->addOctreeNode(Util::checkedPtrCast<OctreeSceneNode>(me));
		}
	}

}