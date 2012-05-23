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

#include "ExceptionDefine.h"
#include "DebugDefine.h"
#include "XmlManipulator.h"

namespace Util
{
	static const String ROOT_NODE_NAME("root");
	//---------------------------------------------------------------------
	XmlReader::XmlReader(const Util::String &fileName) :
		mRootNode(NULL)
	{
		try
		{
			init(fileName);
		}
		catch (std::exception &)
		{
			/// NOTE:I can throw exception in constructor here because it's exception-safe!
			/// DONNOT throw if it's NOT EXCEPTION-SAFE!
			WHISPERWIND_EXCEPTION(fileName + " parse failed!");
		}
	}
	//---------------------------------------------------------------------
	void XmlReader::init(const Util::String &fileName)
	{
		mFileStream = boost::make_shared<File>(fileName.c_str());
		mDoc = boost::make_shared<Doc>();

		mDoc->parse<rapidxml::parse_no_data_nodes>(mFileStream->data());

		mRootNode = mDoc->first_node(ROOT_NODE_NAME.c_str());

		WHISPERWIND_ASSERT(mRootNode && "This xml doesnot have root node!");
	}
	//---------------------------------------------------------------------
	XmlNode * XmlReader::getRootNode() const
	{
		WHISPERWIND_ASSERT(mRootNode);

		return mRootNode;
	}
	//---------------------------------------------------------------------
	XmlNode * XmlReader::getFirstNode(const XmlNode * parentNode, const Util::String & nodeName) const
	{
		WHISPERWIND_ASSERT(parentNode);

		return parentNode->first_node(nodeName.c_str());
	}
	//---------------------------------------------------------------------
	XmlNode * XmlReader::getNextSiblingNode(const XmlNode * currentNode, bool regardlessName) const
	{
		WHISPERWIND_ASSERT(currentNode);

		if (regardlessName)
			return currentNode->next_sibling();

		return currentNode->next_sibling(currentNode->name());
	}
	//---------------------------------------------------------------------
	Util::String XmlReader::getAttribute(const XmlNode * node, const Util::String & attributeName) const
	{
		WHISPERWIND_ASSERT(node);

		Attribute * attribute = node->first_attribute(attributeName.c_str());
		WHISPERWIND_ASSERT(attribute && "Dont have this attribute.");

		return attribute->value();
	}

}