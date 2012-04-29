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
#ifndef _XML_READER_H_
#define _XML_READER_H_

#include "rapidxml/rapidxml.hpp"
#include "boost/shared_ptr.hpp"
#include "Util.h"

/// disable the bad_cast warning.
#pragma warning(push)
#pragma warning(disable : 4244)
#include "rapidxml/rapidxml_utils.hpp"
#pragma warning(pop)

namespace Util
{
	/** Class XmlReader.
	@note:
	    Use type char to parse.Then you can convert it into any format.
	*/
	class WHISPERWIND_API XmlReader
	{
	public:
		explicit XmlReader(const Util::String &fileName);

	public:
		bool advanceFirstChildNode(const Util::String & nodeName);
		bool advanceNextSiblingNode(const Util::String & nodeName);
		const Util::String getAttribute(const Util::String & attributeName);

	private:
		DISALLOW_COPY_AND_ASSIGN(XmlReader);

		void init(const Util::String & fileName);

	private:
		typedef Util::String::value_type CharType;
		typedef rapidxml::xml_document<CharType> Doc;
		typedef boost::shared_ptr<Doc> DocPtr;
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