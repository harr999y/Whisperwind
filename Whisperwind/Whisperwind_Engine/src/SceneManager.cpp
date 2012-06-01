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

#include "DebugDefine.h"
#include "ExceptionDefine.h"
#include "LogManager.h"
#include "StringConverter.h"
#include "CheckedCast.h"
#include "MathDefine.h"
#include "EngineManager.h"
#include "EngineConfig.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "DebugResource.h"
#include "SceneNode.h"
#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "Light.h"
#include "SceneManager.h"

namespace Engine
{
	//---------------------------------------------------------------------
	SceneManager::~SceneManager()
	{
		mSceneNodeMap.clear();
	}
	//---------------------------------------------------------------------
	void SceneManager::init()
	{
		init_impl();
	}
	//---------------------------------------------------------------------
	SceneNodePtr & SceneManager::createSceneNode(const Util::Wstring & name, Util::u_int nodeType)
	{
		WHISPERWIND_ASSERT(mSceneNodeMap.find(name) == mSceneNodeMap.end());

		SceneNodePtr sceneNode = createSceneNode_impl(name, nodeType);
		IF_NULL_EXCEPTION(sceneNode, Util::WstringToString(name) + " node create failed!" );

		mSceneNodeMap.insert(SceneNodeMap::value_type(name, sceneNode));

		return mSceneNodeMap[name];
	}
	//---------------------------------------------------------------------
	SceneNodePtr & SceneManager::getSceneNode(const Util::Wstring & name)
	{
		WHISPERWIND_ASSERT(mSceneNodeMap.find(name) != mSceneNodeMap.end());

		return mSceneNodeMap[name];
	}
	//---------------------------------------------------------------------
	void SceneManager::destroySceneNode(const Util::Wstring & name)
	{
		WHISPERWIND_ASSERT(mSceneNodeMap.find(name) != mSceneNodeMap.end());

		SceneNodePtr & node = mSceneNodeMap[name];
		node->removeAllChildNode();
		node->dettachAllSceneObject();

		SceneNodePtr parentNode;
		if (node->getParentNode(parentNode))
			parentNode->removeChildNode(node);

		destroySceneNode_impl(name);

		mSceneNodeMap.erase(name);
	}
	//---------------------------------------------------------------------
	void SceneManager::destroyAllSceneNode()
	{
		while (!mSceneNodeMap.empty())
		{
			destroySceneNode((mSceneNodeMap.begin())->first);
		}

		destroyAllSceneNode_impl();

		mSceneNodeMap.clear();
	}
	//---------------------------------------------------------------------
	void SceneManager::preUpdate(Util::time elapsedTime)
	{
		if (mPreCallback)
			mPreCallback(elapsedTime);

		preUpdate_impl(elapsedTime);
	}
	//---------------------------------------------------------------------
	void SceneManager::postUpdate(Util::time elapsedTime)
	{
		if (mPostCallback)
			mPostCallback(elapsedTime);

		postUpdate_impl(elapsedTime);
	}
	//---------------------------------------------------------------------
	void SceneManager::regSceneObjectFactory(const SceneObjectFactoryPtr & factory)
	{
		WHISPERWIND_ASSERT(mSceneObjectFactoryMap.find(factory->getName()) == mSceneObjectFactoryMap.end());

		mSceneObjectFactoryMap.insert(SceneObjectFactoryMap::value_type(factory->getName(), factory));
	}
	//---------------------------------------------------------------------
	SceneObjectPtr & SceneManager::createSceneObject(const Util::Wstring & type, const Util::Wstring & name, const Util::Wstring & resourceName)
	{
		WHISPERWIND_ASSERT(mSceneObjectFactoryMap.find(type) != mSceneObjectFactoryMap.end());
		WHISPERWIND_ASSERT(mSceneObjectMap.find(name) == mSceneObjectMap.end());

		const ResourceManagerPtr & rm = EngineManager::getSingleton().getResourceManager(); 
		ResourcePtr resource = rm->loadResource(resourceName);

		const SceneObjectPtr & obj = mSceneObjectFactoryMap[type]->create(name, resource);
		IF_NULL_EXCEPTION(obj, Util::WstringToString(name) + " object create failed!" );

		mSceneObjectMap.insert(SceneObjectMap::value_type(name, obj));

		return mSceneObjectMap[name];
	}
	//---------------------------------------------------------------------
	SceneObjectPtr & SceneManager::getSceneObject(const Util::Wstring & name)
	{
		WHISPERWIND_ASSERT(mSceneObjectMap.find(name) != mSceneObjectMap.end());

		return mSceneObjectMap[name];
	}
	//---------------------------------------------------------------------
	void SceneManager::destroySceneObject(const Util::Wstring & name)
	{
		WHISPERWIND_ASSERT(mSceneObjectMap.find(name) != mSceneObjectMap.end());

		SceneNodePtr & parentNode = mSceneObjectMap[name]->getAttachedSceneNode();
		if (parentNode)
		    parentNode->dettachSceneObject(mSceneObjectMap[name]);

		mSceneObjectMap.erase(name);
	}
	//---------------------------------------------------------------------
	void SceneManager::destroyAllSceneObject()
	{
		while (!mSceneObjectMap.empty())
		{
			destroySceneObject((mSceneObjectMap.begin())->second->getName());
		}

		mSceneObjectMap.clear();
	}
	//---------------------------------------------------------------------
	SceneObjectPtr & SceneManager::createLight(const Util::Wstring & name, const LightInfo & lightInfo)
	{
		WHISPERWIND_ASSERT(mSceneObjectMap.find(name) == mSceneObjectMap.end());

		LightPtr light;
		switch (lightInfo.Type)
		{
		case LT_DIRECTIONAL:
			{
				DirectionalLightPtr dirLight = boost::make_shared<DirectionalLight>(name);
				dirLight->setDirection(lightInfo.Direction);

				light = dirLight;

				break;
			}
		case LT_POINT:
			{
				PointLightPtr pointLight = boost::make_shared<PointLight>(name);
				pointLight->setEffectDistance(lightInfo.EffectDistance);

				light = pointLight;

				break;
			}
		default:
			{
				WHISPERWIND_ASSERT(false && "Donnot have this light type!");
				break;
			}
		}

		light->setColor(lightInfo.Color);

		mSceneObjectMap.insert(SceneObjectMap::value_type(name, light));

		mLightNamesVec.push_back(name);

		return mSceneObjectMap[name]	;
	}
	//---------------------------------------------------------------------
	LightVector SceneManager::getAffectedLights(const Util::AABBPtr & /*aabb*/)
	{
		LightVector lightVec;
		LightPtr light;

		BOOST_AUTO(it, mLightNamesVec.begin());
		for (it; it != mLightNamesVec.end();)
		{
			if (mSceneObjectMap.find(*it) == mSceneObjectMap.end())
			{
				it = mLightNamesVec.erase(it);

				continue;
			}

			const SceneObjectPtr & obj = mSceneObjectMap[*it];
			light = Util::checkedPtrCast<Light>(obj);

			if (light->isAffected(obj->getAABB()))
				lightVec.push_back(light);

			++it;
		}

		return lightVec;
	}
	//---------------------------------------------------------------------
	void SceneManager::loadScene(const Util::Wstring & scene)
	{
		const ResourceManagerPtr & rm = EngineManager::getSingleton().getResourceManager();
		rm->loadResource(scene);

		WHISPERWIND_LOG(scene + TO_UNICODE(" scene load done!"));

		/// TODO:Now put it here.
		if (EngineManager::getSingleton().getEngineConfig()->getDebugRendering())
			createDebugRendering();
	}
	//---------------------------------------------------------------------
	void SceneManager::createDebugRendering()
	{
		BOOST_AUTO(nodePair, mSceneNodeMap.begin());
		for (nodePair; nodePair != mSceneNodeMap.end(); ++nodePair)
		{
			const Util::Wstring & name = nodePair->second->getName();

			SceneObjectPtr & obj = this->createSceneObject(DebugResource::getDebugCreateType(), 
				name + TO_UNICODE("_debugAABB"), name + DebugResource::getDebugNodeSuffix());

			nodePair->second->attachSceneObject(obj);
		}
	}

}

