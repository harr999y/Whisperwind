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

#include "ExceptionDefine.h"
#include "ResourceConfig.h"
#include "ResourceManager.h"

namespace
{
	void traverseFolderWithWin32API(const Util::Wstring & path, Util::WstringVector & allFiles);
}

namespace Engine
{
#ifdef WHISPERWIND_DEBUG
	static const Util::String RESOURCE_CONFIG_PATH("../config/Resource_d.cfg");
#else
	static const Util::String RESOURCE_CONFIG_PATH("../config/Resource.cfg");
#endif
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
			Util::WstringVector fileVec;
			const Util::WstringVector & folderVec = mResourceConfig->getFolderVec();
			BOOST_FOREACH(const Util::Wstring & path, folderVec)
			{
				traverseFolderWithWin32API(path, fileVec);

				BOOST_FOREACH(const Util::Wstring & name, fileVec)
				{
					mFilePathMap.insert(FilePathMap::value_type(name, path + name));
				}
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
			Util::String str;
			Util::WstringToString(name, str);
			WHISPERWIND_EXCEPTION(str + " cannot find!");
		}

		return mFilePathMap[name];
	}

}

namespace
{
/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

	void traverseFolderWithWin32API(const Util::Wstring & path, Util::WstringVector & allFiles)
	{
		WIN32_FIND_DATA fileData;
		HANDLE findResult;
		Util::Wstring findPattern = path + TO_UNICODE("*.*");

		findResult = FindFirstFile(findPattern.c_str(), &fileData);
		do
		{
			if ((findResult != INVALID_HANDLE_VALUE) &&
				NULL == (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				allFiles.push_back(fileData.cFileName);
			}
		} while (FindNextFile(findResult, &fileData));
	}

}