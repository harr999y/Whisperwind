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

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace Engine
{
	class EngineManager;
	class RenderSystem;
	class EngineConfig;
	class PluginConfig;
	class Renderable;
	struct BufferData;
	struct VertexElement;
	struct RenderTextureMapping;
	struct RenderTargetMapping;
	struct RenderableMapping;
	class RenderTexture;
	class RenderTarget;
	class SceneManager;
	class SceneNode;
	class SceneObject;
	class SceneComponent;
	class Resource;
	class ResourceManager;
	class ResourceConfig;

	/** For the unordered_maps nowadays.
	@note:
	    For below's unordered_maps I DONOT need to write my own hash function,
		because now I just use std::wstring and boost::shared_ptr/weak_ptr,and these all
		have their own implementations,I think I cannot write a better one.
	*/
	typedef boost::shared_ptr<RenderSystem> RenderSystemPtr;
	typedef boost::shared_ptr<EngineConfig> EngineConfigPtr;
	typedef boost::shared_ptr<PluginConfig> PluginConfigPtr;
	typedef boost::shared_ptr<ResourceConfig> ResourceConfigPtr;
	typedef boost::shared_ptr<Renderable> RenderablePtr;
	typedef boost::weak_ptr<Renderable> RenderableWeakPtr;
	typedef boost::shared_ptr<RenderableMapping> RenderableMappingPtr;
	typedef boost::unordered_map<RenderableMappingPtr, RenderableWeakPtr>RenderableMappingMap;
	typedef boost::shared_ptr<RenderTextureMapping> RenderTextureMappingPtr;
	typedef boost::shared_ptr<RenderTargetMapping> RenderTargetMappingPtr;
	typedef boost::shared_ptr<RenderTexture>RenderTexturePtr;
	typedef boost::weak_ptr<RenderTexture> RenderTextureWeakPtr;
	typedef boost::unordered_map<Util::Wstring, RenderTextureWeakPtr>RenderTextureFileMap;
	typedef boost::unordered_map<RenderTextureMappingPtr, RenderTextureWeakPtr>RenderTextureMappingMap;
	typedef boost::shared_ptr<RenderTarget> RenderTargetPtr;
	typedef boost::weak_ptr<RenderTarget> RenderTargetWeakPtr;
	typedef boost::unordered_map<RenderTargetMappingPtr, RenderTargetWeakPtr>RenderTargetMappingMap;
	typedef boost::shared_ptr<SceneManager> SceneManagerPtr;
	typedef boost::shared_ptr<SceneNode> SceneNodePtr;
	typedef boost::unordered_map<Util::Wstring, SceneNodePtr> SceneNodeMap;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::unordered_map<Util::Wstring, SceneObjectPtr> SceneObjectMap;
	typedef boost::shared_ptr<SceneComponent> SceneComponentPtr;
	typedef boost::shared_ptr<Resource> ResourcePtr;
	typedef boost::shared_ptr<ResourceManager> ResourceManagerPtr;
}

#endif