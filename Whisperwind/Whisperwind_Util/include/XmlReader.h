/*-------------------------------------------------------------------------
This source file is a part of Whisperwind(GameEngine + GamePlay + GameTools)
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
#ifndef _XML_READER_H_
#define _XML_READER_H_

#include "rapidxml/rapidxml.hpp"
#include "boost/shared_ptr.hpp"
#include "Util.h"

/// disable the bad_cast warning.
#pragma warning(disable : 4244)
#include "rapidxml/rapidxml_utils.hpp"
#pragma warning(default : 4244)

namespace Util
{
	class XmlReader
	{
	public:
		XmlReader(const Util::string &fileName) :
		    mCurrentNode(NULL),
		    mRootNode(NULL)
		{
			init(fileName);
		}

	public:
		bool advanceFirstChildNode(const Util::wstring & nodeName);
		bool advanceNextSiblingNode(const Util::wstring & nodeName);
		const Util::wstring getAttribute(const Util::wstring & attributeName);

	private:
		DISALLOW_COPY_AND_ASSIGN(XmlReader);

		void init(const Util::string & fileName);

	private:
		typedef Util::wstring::value_type CharType;
		typedef boost::shared_ptr<rapidxml::xml_document<CharType>> DocPtr;
		DocPtr mDoc;

		typedef rapidxml::xml_node<CharType> Node;
		Node * mRootNode;
		Node * mCurrentNode;

		typedef rapidxml::file<CharType> File;
		typedef boost::shared_ptr<File> FilePtr;
		FilePtr mFileStream;

		typedef rapidxml::xml_attribute<CharType> Attribute;
	};
}

#endif