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
#ifndef _FBX_XML_CONVERTER_H_
#define _FBX_XML_CONVERTER_H_

#include <fbxsdk.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "Util.h"
#include "XmlManipulator.h"

namespace Tool
{
	class FbxXmlConverter
	{
	public:
		explicit FbxXmlConverter(const Util::String & filePath);

		~FbxXmlConverter();

	public:
		Util::Wstring convertToXml();

	public:
		SET_VALUE(bool, UVInverse);

	private:
		void doWalk(FbxNode * fbxNode);
		void processMesh(FbxNode * fbxNode);

	private:
		FbxManager * mFbxManager;
		FbxScene * mFbxScene;

		Util::String mPath;
		Util::XmlWriterPtr mXmlWriter;
		Util::XmlNode * mMeshNode;

		bool mUVInverse;

	private:
		template<typename FbxSurfaceType>
		void fillParams(FbxSurfaceMaterial * fbxMaterial, Util::XmlNode * materialNode)
		{
			mXmlWriter->appendAttribute(materialNode, "effect", "default.fx"); 
			mXmlWriter->appendAttribute(materialNode, "technique", "basic"); 

			/// TODO:Now only for the diffuse map.
			FbxProperty fbxProperty = fbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[0]);
			if (fbxProperty.IsValid())
			{
				FbxTexture * fbxTexture = FbxCast<FbxTexture>(fbxProperty.GetSrcObject(FbxTexture::ClassId, 0));
				if (fbxTexture)
				{
					FbxFileTexture * fbxFileTexture = FbxCast<FbxFileTexture>(fbxTexture);
					if (fbxFileTexture)
					{
						Util::XmlNode * textureNode = mXmlWriter->appendNode(materialNode, "texture");
						mXmlWriter->appendAttribute(textureNode, "name", "diffuseTexture");

						Util::String str(fbxFileTexture->GetFileName());
						Util::StringVector strVec;
						boost::algorithm::split(strVec, str, boost::is_any_of("/\\"));
						mXmlWriter->appendAttribute(textureNode, "value", strVec.rbegin()->c_str());
					}
				}
			}

			Util::XmlNode * ambientNode = mXmlWriter->appendNode(materialNode, "param");
			Util::XmlNode * diffuseNode = mXmlWriter->appendNode(materialNode, "param");

			Util::String value;
			FbxDouble3 vec3;

			vec3 = static_cast<FbxSurfaceType *>(fbxMaterial)->Ambient;
			value = boost::lexical_cast<Util::String>(vec3[0]) + ",";
			value += boost::lexical_cast<Util::String>(vec3[1]) + ",";
			value += boost::lexical_cast<Util::String>(vec3[2]);
			mXmlWriter->appendAttribute(ambientNode, "name", "ambient");
			mXmlWriter->appendAttribute(ambientNode, "value", value.c_str());

			vec3 = static_cast<FbxSurfaceType *>(fbxMaterial)->Diffuse;
			value = boost::lexical_cast<Util::String>(vec3[0]) + ",";
			value += boost::lexical_cast<Util::String>(vec3[1]) + ",";
			value += boost::lexical_cast<Util::String>(vec3[2]);
			mXmlWriter->appendAttribute(diffuseNode, "name", "diffuse");
			mXmlWriter->appendAttribute(diffuseNode, "value", value.c_str());

			/// TODO:transparent.
		}

	private:
		DISALLOW_COPY_AND_ASSIGN(FbxXmlConverter);
	};

}

#endif