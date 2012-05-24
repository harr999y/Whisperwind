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
#include <boost/algorithm/string.hpp>

#include "Util.h"
#include "ExceptionDefine.h"
#include "DebugDefine.h"
#include "StringConverter.h"
#include "FbxXmlConverter.h"
#include "XmlWmeshConverter.h"

int main(int argc, char ** argv)
{
	try
	{
		if (boost::algorithm::find_first(argv[1], ".fbx") || boost::algorithm::find_first(argv[1], ".FBX"))
		{
			if (argc != 4)
			{
				WHISPERWIND_EXCEPTION("The command line is Whisperwind_MeshConverter sourcefile uv_inverse=bool scale=%f.");
			}

			Tool::FbxXmlConverter fbx2XmlConverter(argv[1]);
			{
				Util::String param(argv[2]);
				boost::algorithm::erase_first(param, "uv_inverse=");
				fbx2XmlConverter.setUVInverse(boost::lexical_cast<bool>(param));

				param = argv[3];
				boost::algorithm::erase_first(param, "scale=");
				fbx2XmlConverter.setScaleFactor(boost::lexical_cast<Util::real>(param));
			}
			Util::Wstring xmlPath = fbx2XmlConverter.convertToXml();

			Tool::XmlWmeshConverter xml2WmeshConverter(Util::WstringToString(xmlPath));
			xml2WmeshConverter.converteToWmesh();
		}
		else if (boost::algorithm::find_first(argv[1], ".wmesh.xml"))
		{
			if (argc != 2)
			{
				WHISPERWIND_EXCEPTION("The command line is Whisperwind_MeshConverter sourcefile.");
			}

			Tool::XmlWmeshConverter xml2WmeshConverter(argv[1]);
			xml2WmeshConverter.converteToWmesh();
		}
		else if (boost::algorithm::find_first(argv[1], ".wmesh"))
		{
			/// TODO! .wmesh converte to .wmesh.xml. Now I donnot need it.
		}
	}
	catch (boost::exception & e)
	{
#ifdef WHISPERWIND_DEBUG
		DEBUG_PRINT_RED(boost::diagnostic_information_what(e));
#else
		Util::Wstring error(Util::StringToWstring(boost::diagnostic_information_what(e)));
		WHISPERWIND_LOG(error);
		::MessageBox(NULL, error.c_str(), TO_UNICODE("Xml to wmesh failed!"), MB_OK);		
#endif
	}

	return 0;
}