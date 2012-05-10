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

#include "ExceptionDefine.h"
#include "XmlReader.h"
#include "ResourceConfig.h"

namespace Engine
{
	static const Util::String FOLDER("Folder");
	static const Util::String SEVEN_Z("7Z");
	static const Util::String ATTRIBUTE_NAME("path");
	//---------------------------------------------------------------------
	void ResourceConfig::parse_impl()
	{
		if (mXmlReader->advanceFirstChildNode(FOLDER))
		{
			try
			{
				do
				{
					Util::String str(mXmlReader->getAttribute(ATTRIBUTE_NAME));
					Util::Wstring wstr;
					Util::StringToWstring(str, wstr);

					mFolderVec.push_back(wstr);
				} while (mXmlReader->advanceNextSiblingNode(FOLDER));
			}
			catch (std::exception & e)
			{
				WHISPERWIND_EXCEPTION(Util::String("Resource config parse failed : ") + e.what());
			}
		}

		if (mXmlReader->advanceFirstChildNode(SEVEN_Z))
		{
			try
			{
				do
				{
					Util::String str(mXmlReader->getAttribute(ATTRIBUTE_NAME));
					Util::Wstring wstr;
					Util::StringToWstring(str, wstr);

					mFolderVec.push_back(wstr);
				} while (mXmlReader->advanceNextSiblingNode(SEVEN_Z));
			}
			catch (std::exception & e)
			{
				WHISPERWIND_EXCEPTION(Util::String("Resource config parse failed : ") + e.what());
			}
		}
	}

}