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

#include "ExceptionDefine.h"
#include "StringConverter.h"
#include "MathDefine.h"
#include "Camera.h"
#include "EngineManager.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "SceneResource.h"

namespace Engine
{
	/// TODO:Need to remove when scene manager have a root node!
	static SceneNodePtr NULL_SCENE_NODE;
	//---------------------------------------------------------------------
	void SceneResource::load(const Util::Wstring & resourcePath)
	{
		Util::String strPath = Util::WstringToString(resourcePath);
		Util::XmlReader xmlReader(strPath);

		Util::XmlNode * rootNode = xmlReader.getRootNode();
		IF_NULL_EXCEPTION(rootNode, strPath + " donnot have root node!");

		Util::XmlNode * sceneNode = xmlReader.getFirstNode(rootNode, "scene");
		IF_NULL_EXCEPTION(sceneNode, strPath + " donnot have scene node!");

		Util::XmlNode * cameraNode = xmlReader.getFirstNode(sceneNode, "camera");
		IF_NULL_EXCEPTION(cameraNode, strPath + " donnot have camera node!");
		processCamera(xmlReader, cameraNode);

		Util::XmlNode * snNode = xmlReader.getFirstNode(sceneNode, "node");
		while (snNode)
		{
			processSceneNode(xmlReader, snNode, NULL_SCENE_NODE);

			snNode = xmlReader.getNextSiblingNode(snNode);
		}
	}
	//---------------------------------------------------------------------
	void SceneResource::processCamera(const Util::XmlReader & xmlReader, const Util::XmlNode * cameraNode)
	{
		CameraPtr & camera = EngineManager::getSingleton().getCamera();
		
		/// postion
		{
			Util::String posStr(xmlReader.getAttribute(cameraNode, "position"));
			camera->setPosition(Util::StringToVector(posStr, 3));
		}

		/// lookat
		{
			Util::String str(xmlReader.getAttribute(cameraNode, "lookat"));
			camera->lookAt(Util::StringToVector(str, 3));
		}
		
		/// movespeed
		{
			Util::String str(xmlReader.getAttribute(cameraNode, "move_speed"));
			camera->setMoveSpeed(boost::lexical_cast<Util::real>(str));
		}
	}
	//---------------------------------------------------------------------
	void SceneResource::processSceneNode(const Util::XmlReader & xmlReader, const Util::XmlNode * snNode, SceneNodePtr & parentSceneNode)
	{
		Util::Wstring name = Util::StringToWstring(xmlReader.getAttribute(snNode, "name"));

		SceneNodePtr sceneNode;
		if (!parentSceneNode)
		{
			NodeType nodeType = boost::algorithm::equals(xmlReader.getAttribute(snNode, "node_type"), "static") ? NT_STATIC : NT_DYNAMIC;

			sceneNode = EngineManager::getSingleton().getSceneManager()->createSceneNode(name, nodeType);
		}
		else
		{
			sceneNode = parentSceneNode->createChildNode(name);
		}

		/// node property
		{
			Util::String posStr(xmlReader.getAttribute(snNode, "position"));
			sceneNode->setPosition(Util::StringToVector(posStr, 3));

			Util::String orientStr(xmlReader.getAttribute(snNode, "orientation"));
			sceneNode->setOrientation(Util::StringToVector(orientStr, 4));
		}

		Util::XmlNode * soNode = xmlReader.getFirstNode(snNode, "object");
		while (soNode)
		{
			processSceneObject(xmlReader, soNode, sceneNode);

			soNode = xmlReader.getNextSiblingNode(soNode);
		}

		Util::XmlNode * snChildNode = xmlReader.getFirstNode(snNode, "node");
		while (snChildNode)
		{
			processSceneNode(xmlReader, snChildNode, sceneNode);

			snChildNode = xmlReader.getNextSiblingNode(snChildNode);
		}
	}
	//---------------------------------------------------------------------
	void SceneResource::processSceneObject(const Util::XmlReader & xmlReader, const Util::XmlNode * soNode, SceneNodePtr & parentSceneNode)
	{
		Util::Wstring name = Util::StringToWstring(xmlReader.getAttribute(soNode, "name"));
		Util::Wstring type = Util::StringToWstring(xmlReader.getAttribute(soNode, "type"));
		Util::Wstring resource = Util::StringToWstring(xmlReader.getAttribute(soNode, "resource"));

		SceneObjectPtr & object = EngineManager::getSingleton().getSceneManager()->createSceneObject(type, name, resource);
		parentSceneNode->attachSceneObject(object);
	}

}