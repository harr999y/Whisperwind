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

/// disable the unreferenced formal parameter warning.
#pragma warning(push)
#pragma warning(disable : 4100)
#include <rapidxml/rapidxml_print.hpp>
#pragma warning(pop)

#include "DebugDefine.h"
#include "ExceptionDefine.h"
#include "XmlManipulator.h"

namespace Util
{
	//---------------------------------------------------------------------
	XmlWriter::XmlWriter()
	{
		mDoc = boost::make_shared<Doc>();

		XmlCharType * nodeName = mDoc->allocate_string("root");
		mRootNode = mDoc->allocate_node(rapidxml::node_element, nodeName);
		mDoc->append_node(mRootNode);
	}
	//---------------------------------------------------------------------
	XmlNode * XmlWriter::appendNode(XmlNode * parentNode, const XmlCharType * name)
	{
		WHISPERWIND_ASSERT(parentNode);

		XmlCharType * nodeName = mDoc->allocate_string(name);
		XmlNode * node = mDoc->allocate_node(rapidxml::node_element, nodeName);
		parentNode->append_node(node);

		return node;
	}
	//---------------------------------------------------------------------
	void XmlWriter::appendAttribute(XmlNode * node, const XmlCharType * attribute, const XmlCharType * value)
	{
		WHISPERWIND_ASSERT(node);

		XmlCharType * attributeName = mDoc->allocate_string(attribute);
		XmlCharType * valueStr = mDoc->allocate_string(value);

		node->append_attribute(mDoc->allocate_attribute(attributeName, valueStr));
	}
	//---------------------------------------------------------------------
	void XmlWriter::writeToFile(const Util::Wstring & filePath)
	{
		mXmlStream.open(filePath, std::ios::out | std::ios::trunc);
		if (!mXmlStream.is_open())
		{
			WHISPERWIND_EXCEPTION((Util::WstringToString(filePath) + " open failed!").c_str());
		}

		mXmlStream << *mDoc;

		mXmlStream.close();
	}

}