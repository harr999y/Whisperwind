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
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "ExceptionDefine.h"
#include "DebugDefine.h"
#include "ResourceConfig.h"
#include "MeshResource.h"
#include "SceneResource.h"
#include "DebugResource.h"
#include "ResourceManager.h"

namespace
{
	void traverseFolderWithWin32API(const Engine::FolderPair & fp, Engine::FilePathMap & fileMap);
}

namespace Engine
{
#ifdef WHISPERWIND_DEBUG
	static const Util::String RESOURCE_CONFIG_PATH("../config/Resource_d.cfg");
#else
	static const Util::String RESOURCE_CONFIG_PATH("../config/Resource.cfg");
#endif
	static const Util::Wstring WMESH_SUFFIX(TO_UNICODE(".wmesh"));
	static const Util::Wstring WSCENE_SUFFIX(TO_UNICODE(".wscene"));
	//---------------------------------------------------------------------
	ResourceManager::ResourceManager()
	{
		mResourceConfig = boost::make_shared<ResourceConfig>(RESOURCE_CONFIG_PATH);

		parse();
	}
	//---------------------------------------------------------------------
	void ResourceManager::parse()
	{
		mResourceConfig->parse();

		/// Folder
		{
			const FolderPairVector & folderVec = mResourceConfig->getFolderVec();
			BOOST_FOREACH(const FolderPair & it, folderVec)
			{
				traverseFolderWithWin32API(it, mFilePathMap);
			}
		}

		/// TODO:7z
		{}

		mResourceConfig.reset();
	}
	//---------------------------------------------------------------------
	Util::Wstring ResourceManager::getResourcePath(const Util::Wstring & name)
	{
		if (mFilePathMap.find(name) == mFilePathMap.end())
		{
			WHISPERWIND_EXCEPTION(Util::WstringToString(name) + " cannot find!");
		}

		return mFilePathMap[name];
	}
	//---------------------------------------------------------------------
	ResourcePtr ResourceManager::loadResource(const Util::Wstring & resource)
	{
		/// TODO:Seperate impl bellow.
		if (boost::algorithm::find_first(resource, WMESH_SUFFIX))
		{
			ResourcePtr mesh = boost::make_shared<MeshResource>();
			mesh->load(this->getResourcePath(resource));

			WHISPERWIND_ASSERT(mesh);

			return mesh;
		}
		else if (boost::algorithm::find_first(resource, WSCENE_SUFFIX))
		{
			ResourcePtr scene = boost::make_shared<SceneResource>();
			scene->load(this->getResourcePath(resource));

			WHISPERWIND_ASSERT(scene);

			return scene;
		}
		else if (boost::algorithm::find_first(resource, DebugResource::getDebugNodeSuffix()) ||
			boost::algorithm::find_first(resource, DebugResource::getDebugObjectSuffix()))
		{
			ResourcePtr scene = boost::make_shared<DebugResource>();
			scene->load(resource);

			WHISPERWIND_ASSERT(scene);

			return scene;
		}

		WHISPERWIND_EXCEPTION(Util::WstringToString(resource) + " isn't a surpport format!");
	}

}

namespace
{
/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

	void traverseFolderWithWin32API(const Engine::FolderPair & fp, Engine::FilePathMap & fileMap)
	{
		WIN32_FIND_DATA fileData;
		HANDLE findResult;

		Util::Wstring findPattern = fp.first + TO_UNICODE("*.*");

		findResult = FindFirstFile(findPattern.c_str(), &fileData);
		do
		{
			Util::Wstring name(fileData.cFileName);
			if (boost::algorithm::equals(name, ".") || boost::algorithm::equals(name, ".."))
				continue;

			if (findResult != INVALID_HANDLE_VALUE)
			{
				if (!fp.second || (0 == (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
				{
					WHISPERWIND_ASSERT(fileMap.find(name) == fileMap.end());

					fileMap.insert(Engine::FilePathMap::value_type(name, fp.first + fileData.cFileName));
				}
				else if (fp.second && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					traverseFolderWithWin32API(std::make_pair(fp.first + name + TO_UNICODE("/"), fp.second), fileMap);
				}
			}
		} while (FindNextFile(findResult, &fileData));
	}

}