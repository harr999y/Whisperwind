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

#include "ExceptionDefine.h"
#include "XmlManipulator.h"
#include "ResourceConfig.h"

namespace Engine
{
	static const Util::String FOLDER("Folder");
	static const Util::String SEVEN_Z("7Z");
	static const Util::String ATTRIBUTE_PATH("path");
	static const Util::String ATTRIBUTE_FIND("find_subfolder");
	//---------------------------------------------------------------------
	void ResourceConfig::parse_impl()
	{
		Util::XmlNode * rootNode = mXmlReader->getRootNode();
		IF_NULL_EXCEPTION(rootNode, "Resource config donnot have root node!");

		Util::XmlNode * node = mXmlReader->getFirstNode(rootNode, FOLDER);
		while (node)
		{
			mFolderVec.push_back(std::make_pair(
				Util::StringToWstring(mXmlReader->getAttribute(node, ATTRIBUTE_PATH)),
				boost::lexical_cast<bool>(mXmlReader->getAttribute(node, ATTRIBUTE_FIND))));

			node = mXmlReader->getNextSiblingNode(node);
		}

		node = mXmlReader->getFirstNode(rootNode, SEVEN_Z);
		while (node)
		{
			mSevenZVec.push_back(std::make_pair(
				Util::StringToWstring(mXmlReader->getAttribute(node, ATTRIBUTE_PATH)),
				boost::lexical_cast<bool>(mXmlReader->getAttribute(node, ATTRIBUTE_FIND))));

			node = mXmlReader->getNextSiblingNode(node);
		}
	}

}