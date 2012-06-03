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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>

#include "ExceptionDefine.h"
#include "DebugDefine.h"
#include "StringConverter.h"
#include "MathDefine.h"
#include "EngineManager.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "SceneNode.h"
#include "Light.h"
#include "Camera.h"
#include "SceneResource.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void SceneResource::load(const Util::Wstring & resourcePath)
	{
		Util::String strPath = Util::WstringToString(resourcePath);
		mXmlReader = boost::make_shared<Util::XmlReader>(strPath);

		const Util::XmlNode * rootNode = mXmlReader->getRootNode();
		IF_NULL_EXCEPTION(rootNode, strPath + " donnot have root node!");

		const Util::XmlNode * sceneNode = mXmlReader->getFirstNode(rootNode, "scene");
		IF_NULL_EXCEPTION(sceneNode, strPath + " donnot have scene node!");

		processScene(sceneNode);
	}
	//---------------------------------------------------------------------
	void SceneResource::processScene( const Util::XmlNode * sceneNode) const
	{
		Util::String worldSizeStr(mXmlReader->getAttribute(sceneNode, "world_size"));
		IF_FALSE_EXCEPTION(!worldSizeStr.empty(), "Current scene donnot have world_size attribute!");

		Util::real worldSize = boost::lexical_cast<Util::real>(worldSizeStr);
		EngineManager::getSingleton().getSceneManager()->setWorldSize(worldSize);

		const Util::XmlNode * cameraNode = mXmlReader->getFirstNode(sceneNode, "camera");
		IF_NULL_EXCEPTION(cameraNode, "Current scene donnot have camera node!");

		processCamera(cameraNode);

		const Util::XmlNode * snNode = mXmlReader->getFirstNode(sceneNode, "node");
		while (snNode)
		{
			processSceneNode(snNode);

			snNode = mXmlReader->getNextSiblingNode(snNode);
		}
	}
	//---------------------------------------------------------------------
	void SceneResource::processCamera(const Util::XmlNode * cameraNode) const
	{
		CameraPtr camera;

		/// params
		{
			Util::String nearClipStr(mXmlReader->getAttribute(cameraNode, "near_clip"));
			Util::real nearClip = boost::lexical_cast<Util::real>(nearClipStr);

			Util::String farClipStr(mXmlReader->getAttribute(cameraNode, "far_clip"));
			Util::real farClip = boost::lexical_cast<Util::real>(farClipStr);

			Util::String viewportRectStr(mXmlReader->getAttribute(cameraNode, "viewport_rect"));
			XMVECTOR viewportRect = Util::StringToVector(viewportRectStr, 4);

			camera = boost::make_shared<Camera>(nearClip, farClip, 
				Util::UintPair(XMVectorGetIntX(viewportRect), XMVectorGetIntY(viewportRect)),
				Util::UintPair(XMVectorGetIntZ(viewportRect), XMVectorGetIntW(viewportRect)));

			IF_NULL_EXCEPTION(camera, "Camera creat failed!");
		}

		/// postion
		{
			Util::String posStr(mXmlReader->getAttribute(cameraNode, "position"));
			camera->setPosition(Util::StringToVector(posStr, 3));
		}

		/// lookat
		{
			Util::String str(mXmlReader->getAttribute(cameraNode, "lookat"));
			camera->lookAt(Util::StringToVector(str, 3));
		}
		
		/// movespeed
		{
			Util::String str(mXmlReader->getAttribute(cameraNode, "move_speed"));
			camera->setMoveSpeed(boost::lexical_cast<Util::real>(str));
		}

		EngineManager::getSingleton().setCamera(camera);
	}
	//---------------------------------------------------------------------
	void SceneResource::processSceneNode(const Util::XmlNode * snNode) const
	{
		WHISPERWIND_ASSERT(snNode);

		Util::Wstring name = Util::StringToWstring(mXmlReader->getAttribute(snNode, "name"));
		NodeType nodeType = boost::algorithm::equals(mXmlReader->getAttribute(snNode, "node_type"), "static") ? NT_STATIC : NT_DYNAMIC;

		SceneNodePtr & sceneNode = EngineManager::getSingleton().getSceneManager()->createSceneNode(name, nodeType);

		processSceneNodeContent(snNode, sceneNode, false);
	}
	//---------------------------------------------------------------------
	void SceneResource::processSceneNodeContent(const Util::XmlNode * snNode, SceneNodePtr & sceneNode, bool isChild) const
	{
		WHISPERWIND_ASSERT(snNode);

		/// node property
		{
			Util::String posStr(mXmlReader->getAttribute(snNode, "position"));
			XMVECTOR posVec = Util::StringToVector(posStr, 3);

			Util::String orientStr(mXmlReader->getAttribute(snNode, "orientation"));
			XMVECTOR orientVec = Util::StringToVector(orientStr, 4);

			if (!isChild)
			{
				sceneNode->setPosition(posVec);
				sceneNode->setOrientation(orientVec);
			}
			else
			{
				sceneNode->setRelativePosition(posVec);
				sceneNode->setRelativeOrientation(orientVec);
			}
		}

		const Util::XmlNode * snChildNode = mXmlReader->getFirstNode(snNode, "child_node");
		while (snChildNode)
		{
			processChildSceneNode(snChildNode, sceneNode);

			snChildNode = mXmlReader->getNextSiblingNode(snChildNode);
		}

		const Util::XmlNode * soNode = mXmlReader->getFirstNode(snNode, "object");
		while (soNode)
		{
			processSceneObject(soNode, sceneNode);

			soNode = mXmlReader->getNextSiblingNode(soNode);
		}

		const Util::XmlNode * lightNode = mXmlReader->getFirstNode(snNode, "light");
		while (lightNode)
		{
			processLight(lightNode, sceneNode);

			lightNode = mXmlReader->getNextSiblingNode(lightNode);
		}

		const Util::XmlNode * trackNode = mXmlReader->getFirstNode(snNode, "node_track");
		while (trackNode)
		{
			processNodeTrack(trackNode, sceneNode);

			trackNode = mXmlReader->getNextSiblingNode(trackNode);
		}
	}
	//---------------------------------------------------------------------
	void SceneResource::processChildSceneNode(const Util::XmlNode * snNode, SceneNodePtr & parentSceneNode) const
	{
		WHISPERWIND_ASSERT(snNode);

		Util::Wstring name = Util::StringToWstring(mXmlReader->getAttribute(snNode, "name"));
		SceneNodePtr & sceneNode = parentSceneNode->createChildNode(name);

		processSceneNodeContent(snNode, sceneNode, true);
	}
	//---------------------------------------------------------------------
	void SceneResource::processSceneObject(const Util::XmlNode * soNode, SceneNodePtr & parentSceneNode) const
	{
		WHISPERWIND_ASSERT(soNode);

		Util::Wstring name = Util::StringToWstring(mXmlReader->getAttribute(soNode, "name"));
		Util::Wstring type = Util::StringToWstring(mXmlReader->getAttribute(soNode, "type"));
		Util::Wstring resource = Util::StringToWstring(mXmlReader->getAttribute(soNode, "resource"));

		SceneObjectPtr & object = EngineManager::getSingleton().getSceneManager()->createSceneObject(type, name, resource);
		parentSceneNode->attachSceneObject(object);
	}
	//---------------------------------------------------------------------
	void SceneResource::processLight(const Util::XmlNode * lightNode, SceneNodePtr & parentSceneNode) const
	{
		Util::String typeStr(mXmlReader->getAttribute(lightNode, "type"));
		LightInfo lightInfo;
		
		Util::String str;
		if (boost::algorithm::equals("directional_light", typeStr))
		{
			lightInfo.Type = LT_DIRECTIONAL;

			str = mXmlReader->getAttribute(lightNode, "direction");
			XMStoreFloat3(&lightInfo.Direction, Util::StringToVector(str, 3));
		}
		else if (boost::algorithm::equals("point_light", typeStr))
		{
			lightInfo.Type = LT_POINT;

			str = mXmlReader->getAttribute(lightNode, "effect_distance");
			lightInfo.EffectDistance = boost::lexical_cast<Util::real>(str);
		}

		str = mXmlReader->getAttribute(lightNode, "color");
		XMStoreFloat3(&lightInfo.Color, Util::StringToVector(str, 3));

		Util::Wstring name = Util::StringToWstring(mXmlReader->getAttribute(lightNode, "name"));

		SceneObjectPtr light = EngineManager::getSingleton().getSceneManager()->createLight(name, lightInfo);

		parentSceneNode->attachSceneObject(light);
	}
	//---------------------------------------------------------------------
	void SceneResource::processNodeTrack(const Util::XmlNode * trackNode, SceneNodePtr & parentSceneNode) const
	{
		NodeTrack & nodeTrack = parentSceneNode->getNodeTrack();

		Util::String trackModeStr(mXmlReader->getAttribute(trackNode, "track_mode"));
		if (boost::algorithm::equals("as_world", trackModeStr))
			nodeTrack.TrackMode = NTM_AS_WORLD;
		else if (boost::algorithm::equals("as_parent", trackModeStr))
			nodeTrack.TrackMode = NTM_AS_PARENT;

		nodeTrack.MoveSpeed = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(trackNode, "move_speed"));
		nodeTrack.RotateSpeed = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(trackNode, "rotate_speed"));

		const Util::XmlNode * controllPointNode = mXmlReader->getFirstNode(trackNode, "controll_point");
		while (controllPointNode)
		{
			NodeControllPoint controllPoint;
			XMStoreFloat3(&controllPoint.Position, Util::StringToVector(mXmlReader->getAttribute(controllPointNode, "position"), 3));
			XMStoreFloat4(&controllPoint.Orientation, Util::StringToVector(mXmlReader->getAttribute(controllPointNode, "orientation"), 4));

			parentSceneNode->addTrackPoint(controllPoint);

			controllPointNode = mXmlReader->getNextSiblingNode(controllPointNode);
		}
	}

}