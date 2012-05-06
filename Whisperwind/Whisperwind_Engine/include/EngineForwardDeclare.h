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
#ifndef _ENGINE_FORWARD_DECLARE_H_
#define _ENGINE_FORWARD_DECLARE_H_

#include "boost/shared_ptr.hpp"
#include <vector>
#include "boost/unordered_map.hpp"

namespace Engine
{
	class EngineManager;
	class RenderSystem;
	class EngineConfig;
	class PluginConfig;
	class Renderable;
	struct BufferData;
	struct VertexElement;
	struct RenderableMapping;
	class SceneManager;
	class SceneNode;
	class SceneObject;
	class SceneComponent;

	typedef boost::shared_ptr<RenderSystem> RenderSystemPtr;
	typedef boost::shared_ptr<EngineConfig> EngineConfigPtr;
	typedef boost::shared_ptr<PluginConfig> PluginConfigPtr;
	typedef boost::shared_ptr<Renderable> RenderablePtr;
	typedef std::vector<RenderablePtr> RenderableVector;
	typedef boost::shared_ptr<RenderableMapping> RenderableMappingPtr;
	typedef boost::shared_ptr<SceneManager> SceneManagerPtr;
	typedef boost::shared_ptr<SceneNode> SceneNodePtr;
	typedef boost::unordered_map<Util::Wstring, SceneNodePtr> SceneNodeMap;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::unordered_map<Util::Wstring, SceneObjectPtr> SceneObjectMap;
	typedef boost::shared_ptr<SceneComponent> SceneComponentPtr;
	typedef std::vector<SceneComponentPtr> SceneComponentVector;
}

#endif