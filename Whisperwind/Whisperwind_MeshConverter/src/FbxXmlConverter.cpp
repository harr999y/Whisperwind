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

#include "UtilWarningDisable.h"

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "ExceptionDefine.h"
#include "FbxXmlConverter.h"

namespace Tool
{
	static const Util::Wstring XML_SUFFIX(TO_UNICODE(".wmesh.xml"));
	//---------------------------------------------------------------------
	FbxXmlConverter::FbxXmlConverter(const Util::String & filePath)  :
        mPath(filePath),
		mMeshNode(NULL),
		mUVInverse(false),
		mScaleFactor(1.0f)
	{
		mFbxManager = FbxManager::Create();
		IF_NULL_EXCEPTION(mFbxManager, (filePath + " FbxManager create failed!").c_str());

		FbxIOSettings * fbxIos = FbxIOSettings::Create(mFbxManager, IOSROOT);
		IF_NULL_EXCEPTION(fbxIos, (filePath + " FbxIOSettings create failed!").c_str());
		mFbxManager->SetIOSettings(fbxIos);

		FbxString path = FbxGetApplicationDirectory();
		mFbxManager->LoadPluginsDirectory(path.Buffer());

		mFbxScene = FbxScene::Create(mFbxManager, "");
		IF_NULL_EXCEPTION(mFbxScene, (filePath + " FbxScene create failed!").c_str());

		FbxImporter * sceneImporter = FbxImporter::Create(mFbxManager, "");
		IF_NULL_EXCEPTION(sceneImporter, (filePath + " FbxImporter create failed!").c_str());

		Util::Wstring wpath = Util::StringToWstring(filePath);
		char * utf8Path;
		FbxWCToUTF8(wpath.c_str(), utf8Path);
		IF_FALSE_EXCEPTION(sceneImporter->Initialize(utf8Path, -1, fbxIos), (filePath + " parse error!").c_str());
		WHISPERWIND_DELETE(utf8Path);

		sceneImporter->Import(mFbxScene);
		sceneImporter->Destroy();
	}
	//---------------------------------------------------------------------
	FbxXmlConverter::~FbxXmlConverter()
	{
		if (mFbxManager)
			mFbxManager->Destroy();
	}
	//---------------------------------------------------------------------
	Util::Wstring FbxXmlConverter::convertToXml()
	{
		FbxGeometryConverter converter(mFbxManager);
		converter.SplitMeshesPerMaterial(mFbxScene);

		/// save to xml
		mXmlWriter = boost::make_shared<Util::XmlWriter>();
		mXmlWriter->setConvertUtf8ToAnsi(true);

		mMeshNode = mXmlWriter->appendNode(mXmlWriter->getRootNode(), "mesh");
		IF_NULL_EXCEPTION(mMeshNode, (mPath + " append mesh node failed!").c_str());

		FbxNode * rootNode = mFbxScene->GetRootNode();
		IF_NULL_EXCEPTION(rootNode, (mPath + " Get FBX root node failed!").c_str());
		for (Util::s_int it = 0; it < rootNode->GetChildCount(); ++it)
		{
			doWalk(rootNode->GetChild(it));
		}

		Util::Wstring wstrPath = Util::StringToWstring(mPath);
		boost::erase_last(wstrPath, ".FBX");
		boost::erase_last(wstrPath, ".fbx");
		wstrPath += XML_SUFFIX;
		mXmlWriter->writeToFile(wstrPath);

		return wstrPath;
	}
	//---------------------------------------------------------------------
	void FbxXmlConverter::doWalk(FbxNode * fbxNode)
	{
		IF_NULL_RETURN(fbxNode);

		FbxNodeAttribute * nodeAtribute = fbxNode->GetNodeAttribute();
		if (nodeAtribute)
		{
			switch (nodeAtribute->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
				{
					processMesh(fbxNode);
					break;
				}
			}
		}
		
		for (Util::s_int it = 0; it < fbxNode->GetChildCount(); ++it)
		{
			doWalk(fbxNode->GetChild(it));
		}
	}
	//---------------------------------------------------------------------
	void FbxXmlConverter::processMesh(FbxNode * fbxNode)
	{
		IF_NULL_RETURN(fbxNode);

		FbxMesh * fbxMesh = static_cast<FbxMesh *>(fbxNode->GetNodeAttribute());

		if (!fbxMesh->IsTriangleMesh())
		{
			FbxGeometryConverter converter(mFbxManager);
			converter.TriangulateInPlace(fbxNode);
			fbxMesh = static_cast<FbxMesh *>(fbxNode->GetNodeAttribute());
		}

		/// save
		{
			Util::XmlNode * submeshNode = mXmlWriter->appendNode(mMeshNode, "submesh");
			IF_NULL_EXCEPTION(submeshNode, mPath + " append submesh node failed!");

			Util::String name(fbxNode->GetName());
			if (name.empty())
			{
				static Util::u_int NameGeneratorNum = 0;
				++NameGeneratorNum;
				name = boost::lexical_cast<Util::String>(NameGeneratorNum);
			}

			mXmlWriter->appendAttribute(submeshNode, "name", name.c_str());

			/// material
			{
				FbxNode * node = fbxMesh->GetNode();
				Util::u_int materialCount = node->GetMaterialCount();

				Util::XmlNode * materialNode = mXmlWriter->appendNode(submeshNode, "material");
				if (materialCount > 0)
				{ 
					FbxSurfaceMaterial * fbxMaterial = node->GetMaterial(0);
					if (fbxMaterial)
					{
						const FbxImplementation * implementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
						if (implementation)
						{
							const FbxBindingTable * rootTable = implementation->GetRootTable();
							if (rootTable)
							{
								FbxString effectFile = rootTable->DescAbsoluteURL.Get();
								Util::StringVector strVec;
								Util::String effectName(effectFile.Buffer());
								boost::algorithm::split(strVec, effectName, boost::is_any_of("\\"));
								mXmlWriter->appendAttribute(materialNode, "effect", strVec.rbegin()->c_str());

								FbxString techniqueName = rootTable->DescTAG.Get();
								mXmlWriter->appendAttribute(materialNode, "technique", techniqueName.Buffer());

								Util::String nodeName;
								Util::u_int entryNum = rootTable->GetEntryCount();
								for (Util::u_int entryIt = 0; entryIt < entryNum; ++entryIt)
								{
									const FbxBindingTableEntry & entry = rootTable->GetEntry(entryIt);
									const char * entrySrcType = entry.GetEntryType(true);
									FbxProperty fbxProperty;

									if (strcmp(FbxPropertyEntryView::sEntryType, entrySrcType) == 0)
									{
										fbxProperty = fbxMaterial->FindPropertyHierarchical(entry.GetSource());
										if (!fbxProperty.IsValid())
										{
											fbxProperty = fbxMaterial->RootProperty.FindHierarchical(entry.GetSource());
										}
									}
									else if (strcmp(FbxConstantEntryView::sEntryType, entrySrcType) == 0)
									{
										fbxProperty = implementation->GetConstants().FindHierarchical(entry.GetSource());
									}
									else if (strcmp(FbxSemanticEntryView::sEntryType, entrySrcType) == 0)
									{
										fbxProperty = implementation->GetConstants().FindHierarchical(entry.GetSource());
									}

									if (fbxProperty.IsValid())
									{										
										Util::StringVector valueVec;

										if (fbxProperty.GetSrcObjectCount(FBX_TYPE(FbxTexture)) > 0)
										{
											nodeName = "texture";

											for (Util::s_int texIt = 0; texIt < fbxProperty.GetSrcObjectCount(FBX_TYPE(FbxFileTexture)); ++texIt)
											{
												FbxFileTexture * texture = fbxProperty.GetSrcObject(FBX_TYPE(FbxFileTexture), texIt);
												IF_NULL_CONTINUE(texture);

												valueVec.push_back(texture->GetMediaName().Buffer());
											}
										}
										else
										{
											nodeName = "param";

											FbxDataType fbxDataType = fbxProperty.GetPropertyDataType();
											if (FbxBoolDT == fbxDataType)
											{
												valueVec.push_back(boost::lexical_cast<Util::String>(fbxProperty.Get<FbxBool>()));
											}
											else if ((FbxIntDT == fbxDataType) || (FbxEnumDT == fbxDataType))
											{
												valueVec.push_back(boost::lexical_cast<Util::String>(fbxProperty.Get<FbxInt>()));
											}
											else if (FbxFloatDT == fbxDataType)
											{
												valueVec.push_back(boost::lexical_cast<Util::String>(fbxProperty.Get<FbxFloat>()));
											}
											else if (FbxDoubleDT == fbxDataType)
											{
												valueVec.push_back(boost::lexical_cast<Util::String>(fbxProperty.Get<FbxDouble>()));
											}
											else if (FbxStringDT == fbxDataType)
											{
												valueVec.push_back(fbxProperty.Get<FbxString>().Buffer());
											}
											else if (FbxDouble2DT == fbxDataType)
											{
												FbxDouble2 vec2 = fbxProperty.Get<FbxDouble2>();
												valueVec.push_back(boost::lexical_cast<Util::String>(vec2[0]) + "," + 
													boost::lexical_cast<Util::String>(vec2[1]));
											}
											else if ((FbxDouble3DT == fbxDataType) || (FbxColor3DT == fbxDataType))
											{
												FbxDouble3 vec3 = fbxProperty.Get<FbxDouble3>();
												valueVec.push_back(boost::lexical_cast<Util::String>(vec3[0]) + "," + 
													boost::lexical_cast<Util::String>(vec3[1]) + "," + 
													boost::lexical_cast<Util::String>(vec3[2]));
											}
											else if ((FbxDouble4DT == fbxDataType) || (FbxColor4DT == fbxDataType))
											{
												FbxDouble4 vec4 = fbxProperty.Get<FbxDouble4>();
												valueVec.push_back(boost::lexical_cast<Util::String>(vec4[0]) + "," + 
													boost::lexical_cast<Util::String>(vec4[1]) + "," + 
													boost::lexical_cast<Util::String>(vec4[2]) + "," + 
													boost::lexical_cast<Util::String>(vec4[3]));
											}
											else if (FbxDouble4x4DT == fbxDataType)
											{
												FbxDouble4x4 vec4x4 = fbxProperty.Get<FbxDouble4x4>();

												Util::String value;
												for (Util::size_t row = 0; row < 4; ++row)
												{
													for (Util::size_t column = 0; column < 4; ++column)
													{
														value += boost::lexical_cast<Util::String>(vec4x4[row][column]) + ",";
													}
												}
												value.erase(value.end() - 1);

												valueVec.push_back(value);
											}
										}

										if (!valueVec.empty())
										{
											BOOST_FOREACH(const Util::String & value, valueVec)
											{
												Util::XmlNode * textureNode = mXmlWriter->appendNode(materialNode, nodeName.c_str());
												IF_NULL_EXCEPTION(textureNode, mPath + " append node" + nodeName + " failed!");

												Util::StringVector strVec;
												Util::String str(entry.GetSource());
												boost::algorithm::split(strVec, str, boost::is_any_of("|"));
												const Util::XmlCharType * name = strVec.rbegin()->c_str();

												mXmlWriter->appendAttribute(textureNode, "name", name);
												mXmlWriter->appendAttribute(textureNode, "value", value.c_str());
											}

											valueVec.clear();
										}
									}
								}
							}
						}
						else if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
						{
							fillParams<FbxSurfacePhong>(fbxMaterial, materialNode);

							Util::XmlNode * specularNode = mXmlWriter->appendNode(materialNode, "param");

							Util::String value;
							FbxDouble3 vec3;
							vec3 = static_cast<FbxSurfacePhong *>(fbxMaterial)->Specular;
							value = boost::lexical_cast<Util::String>(vec3[0]) + ",";
							value += boost::lexical_cast<Util::String>(vec3[1]) + ",";
							value += boost::lexical_cast<Util::String>(vec3[2]);
							mXmlWriter->appendAttribute(specularNode, "name", "specular");
							mXmlWriter->appendAttribute(specularNode, "value", value.c_str());
						}
						else if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
						{
							fillParams<FbxSurfaceLambert>(fbxMaterial, materialNode);
						}
					}
				}
				else
				{
					mXmlWriter->appendAttribute(materialNode, "effect", "default.fx"); 
					mXmlWriter->appendAttribute(materialNode, "technique", "basic"); 
                }
			}

			Util::u_int vertexCount = fbxMesh->GetControlPointsCount();
			Util::XmlNode * vbNode = mXmlWriter->appendNode(submeshNode, "vertexbuffer");
			mXmlWriter->appendAttribute(vbNode, "vertexcount", (boost::lexical_cast<Util::String>(vertexCount)).c_str());

			if (vertexCount > 0)
			{
				std::vector<Util::XmlNode *> vertexNodeVec(vertexCount, NULL);

				FbxVector4 * vertexBuf = fbxMesh->GetControlPoints();
				IF_NULL_EXCEPTION(vertexBuf, mPath + " get vertex buffer failed!");

				for (Util::size_t vertexIt = 0; vertexIt < vertexCount; ++vertexIt)
				{
					Util::XmlNode * vertexNode = mXmlWriter->appendNode(vbNode, "vertex");
					vertexNodeVec[vertexIt] = vertexNode;

					/// Position
					Util::XmlNode * positionNode = mXmlWriter->appendNode(vertexNode, "position");
					/// NOTE:Here convert to left-hand axis.
					mXmlWriter->appendAttribute(positionNode, "x", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][0] * mScaleFactor).c_str());
					mXmlWriter->appendAttribute(positionNode, "y", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][2] * mScaleFactor).c_str());
					mXmlWriter->appendAttribute(positionNode, "z", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][1] * mScaleFactor).c_str());
				}

				/// UV and Normal
				/// NOTE:Now only surpport one texcoord and one normal!
				Util::u_int polygonCount = fbxMesh->GetPolygonCount();
				for (Util::u_int polygonIt = 0; polygonIt < polygonCount; ++polygonIt)
				{
					Util::s_int polygonSize = fbxMesh->GetPolygonSize(polygonIt);
					if (-1 == polygonSize)
						continue;

					for (Util::s_int polygonIndex = 0; polygonIndex < polygonSize; ++polygonIndex)
					{
						Util::s_int vertexIt = fbxMesh->GetPolygonVertex(polygonIt, polygonIndex);
						if (-1 == vertexIt)
							continue;

						/// UV
						FbxGeometryElementUV * fbxUVs = fbxMesh->GetElementUV(0);
						if (fbxUVs)
						{		
							FbxVector2 uv;
							switch (fbxUVs->GetMappingMode())
							{
							case FbxGeometryElement::eByControlPoint:
								{
									switch (fbxUVs->GetReferenceMode())
									{
									case FbxGeometryElement::eDirect:
										{
											uv = fbxUVs->GetDirectArray().GetAt(vertexIt);
											break;
										}
									case FbxGeometryElement::eIndexToDirect:
										{
											Util::u_int id = fbxUVs->GetIndexArray().GetAt(vertexIt);
											uv = fbxUVs->GetDirectArray().GetAt(id);
											break;
										}
									}
									break;
								}
							case FbxGeometryElement::eByPolygonVertex:
								{
									Util::s_int texUVIndex = fbxMesh->GetTextureUVIndex(polygonIt, polygonIndex);
									if (-1 == texUVIndex)
										continue;

									switch (fbxUVs->GetReferenceMode())
									{
									case FbxGeometryElement::eDirect:
									case FbxGeometryElement::eIndexToDirect:
										{
											uv = fbxUVs->GetDirectArray().GetAt(texUVIndex);
											break;
										}
									}

									break;
								}
							} // switch

							if (!vertexNodeVec[vertexIt]->first_node("texcoord"))
							{
								Util::XmlNode * uvNode = mXmlWriter->appendNode(vertexNodeVec[vertexIt], "texcoord");
								mXmlWriter->appendAttribute(uvNode, "u", boost::lexical_cast<Util::String>(uv[0]).c_str());

								if (!mUVInverse)
									mXmlWriter->appendAttribute(uvNode, "v", boost::lexical_cast<Util::String>(1 - uv[1]).c_str());
								else
									mXmlWriter->appendAttribute(uvNode, "v", boost::lexical_cast<Util::String>(uv[1]).c_str());
							}
						}

						/// Normal
						FbxGeometryElementNormal * fbxNormals = fbxMesh->GetElementNormal(0);
						if (fbxNormals)
						{
							FbxVector4 normal;
							FbxLayerElement::EMappingMode emMode = fbxNormals->GetMappingMode();
							FbxLayerElement::EReferenceMode erMode = fbxNormals->GetReferenceMode();
							if (emMode == FbxGeometryElement::eByControlPoint)
							{
								if (erMode == FbxLayerElement::eDirect)
								{
									normal = fbxNormals->GetDirectArray().GetAt(vertexIt);
								}
								else if (erMode == FbxLayerElement::eIndexToDirect)
								{
									Util::u_int id = fbxNormals->GetIndexArray().GetAt(vertexIt);
									normal = fbxNormals->GetDirectArray().GetAt(id);
								}
							}
							else if (emMode == FbxGeometryElement::eByPolygonVertex)
							{
								Util::s_int normalIndex = fbxMesh->GetPolygonVertex(polygonIt, polygonIndex);
								if (-1 == normalIndex)
									continue;

								normal = fbxNormals->GetDirectArray().GetAt(normalIndex);
							}

							if (!vertexNodeVec[vertexIt]->first_node("normal"))
							{
								Util::XmlNode * normalNode = mXmlWriter->appendNode(vertexNodeVec[vertexIt], "normal");
								
								mXmlWriter->appendAttribute(normalNode, "x", boost::lexical_cast<Util::String>(normal[0]).c_str());
								mXmlWriter->appendAttribute(normalNode, "y", boost::lexical_cast<Util::String>(normal[2]).c_str());
								mXmlWriter->appendAttribute(normalNode, "z", boost::lexical_cast<Util::String>(normal[1]).c_str());
							}
						}
					}
				}

			}
			
			Util::u_int trianglesCount = fbxMesh->GetPolygonCount();
			Util::XmlNode * trianglesNode = mXmlWriter->appendNode(submeshNode, "triangles");
			mXmlWriter->appendAttribute(trianglesNode, "trianglescount", boost::lexical_cast<Util::String>(trianglesCount).c_str());

			if (trianglesCount > 0)
			{
				Util::s_int * indexBuf = fbxMesh->GetPolygonVertices();
				IF_NULL_EXCEPTION(indexBuf, mPath + " get index buffer failed!");

				Util::s_int startIndex = 0;
				for (Util::u_int it = 0; it < trianglesCount; ++it)
				{
					startIndex = fbxMesh->GetPolygonVertexIndex(it);
					if (-1 == startIndex)
						continue;

					Util::XmlNode * triangleNode = mXmlWriter->appendNode(trianglesNode, "triangle");
					mXmlWriter->appendAttribute(triangleNode, "v1", boost::lexical_cast<Util::String>(indexBuf[startIndex + 2]).c_str());
					mXmlWriter->appendAttribute(triangleNode, "v2", boost::lexical_cast<Util::String>(indexBuf[startIndex + 1]).c_str());
					mXmlWriter->appendAttribute(triangleNode, "v3", boost::lexical_cast<Util::String>(indexBuf[startIndex]).c_str());
				}
			}
		}
	}

}