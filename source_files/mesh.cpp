/* 
 * Evorobot* - mesh.cpp
 * Copyright (C) 2009, Onofrio Gigliotta
 * LARAL, Institute of Cognitive Science and Technologies, CNR, Roma, Italy 
 * http://laral.istc.cnr.it

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
// Thanks to Brett Porter and Mete Ciragan for help with this MS3D model loading code
// Thanks to Ronny A. Reierstad, Vadim Tikhanoff and Gianluca Massera

#include "mesh.h"
#include "string.h"
#include <QString>
#include <QFile>
#include <QImage>


typedef unsigned char byte;
typedef unsigned short word;

//----------------------------------
//--- MS3D STRUCTURES --------------
// PACK_STRUCT : byte-align structures
#ifdef _MSC_VER
	#pragma pack( push, packing )
	#pragma pack( 1 )
	#define PACK_STRUCT
	#ifndef PATH_MAX
		#define PATH_MAX _MAX_PATH
	#endif
#elif defined( __GNUC__ )
	#define PACK_STRUCT	__attribute__((packed))
	#include <limits.h>
#else
	#error you must byte-align these structures with the appropriate compiler directives
#endif





// File Header
struct MS3DHeader {
	char m_ID[10];
	int m_version;
} PACK_STRUCT;

// Vertex info
struct MS3DVertex {
	byte m_flags;
	float m_vertex[3];
	char m_boneID;
	byte m_refCount;
} PACK_STRUCT;

// Triangle info
struct MS3DTriangle {
	word m_flags;
	word m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	byte m_smoothingGroup;
	byte m_groupIndex;
} PACK_STRUCT;
// Material info
struct MS3DMaterial {
	static unsigned int Texture[15];
	char m_name[32];
	float m_ambient[4];
	float m_diffuse[4];
	float m_specular[4];
	float m_emissive[4];
	float m_shininess;					// 0.0f - 128.0f
	float m_transparency;				// 0.0f - 1.0f
	byte m_mode;						// 0, 1, 2 (unused now)
	char m_texture[128];
	char m_alphamap[128];
} PACK_STRUCT;
// back to Default alignment
#ifdef _MSC_VER							
	#pragma pack( pop, packing )
#endif
#undef PACK_STRUCT
//--- MS3D STRUCTURES ENDS ---------
//----------------------------------


wMesh::wMesh()
	{
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
	};


wMesh::~wMesh() {
	int i;
	for ( i = 0; i < m_numMeshes; i++ ) {
		delete[] m_pMeshes[i].m_pTriangleIndices;
	}
	m_numMeshes = 0;
	if ( m_pMeshes != NULL ) {
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}
	m_numMaterials = 0;
	if ( m_pMaterials != NULL ) {
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}
	m_numTriangles = 0;
	if ( m_pTriangles != NULL ) {
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}
	m_numVertices = 0;
	if ( m_pVertices != NULL ) {
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
}


bool wMesh::loadMS3DModel( QString filename ) {
	QFile inputFile( filename );
	if ( !inputFile.open( QIODevice::ReadOnly ) ) {
		return false;
	}
	QByteArray bBuffer = inputFile.readAll();
	const char *pPtr = bBuffer.data();
	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );
	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 ) {
		 // "Not a valid Milkshape3D model file."
		return false;
	}
	if ( pHeader->m_version < 3 ) {
		 // "Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 is supported."
		return false;
	}
	int nVertices = *( word* )pPtr;
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof( word );
	int i;
	for ( i = 0; i < nVertices; i++ ) {
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i].m_boneID = pVertex->m_boneID;
		memcpy( m_pVertices[i].m_location, pVertex->m_vertex, sizeof( float )*3 );
		pPtr += sizeof( MS3DVertex );
	}
	int nTriangles = *( word* )pPtr;
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( word );
	for ( i = 0; i < nTriangles; i++ ) {
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
		int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
		float t[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };
		memcpy( m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
		memcpy( m_pTriangles[i].m_s, pTriangle->m_s, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_t, t, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof( int )*3 );
		pPtr += sizeof( MS3DTriangle );
	}
	int nGroups = *( word* )pPtr;
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( word );
	for ( i = 0; i < nGroups; i++ ) {
		pPtr += sizeof( byte );	// flags
		pPtr += 32;				// name
		word nTriangles = *( word* )pPtr;
		pPtr += sizeof( word );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ ) {
			pTriangleIndices[j] = *( word* )pPtr;
			pPtr += sizeof( word );
		}
		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );
		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}
	int nMaterials = *( word* )pPtr;
	m_numMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( word );
	for ( i = 0; i < nMaterials; i++ ) {
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		memcpy( m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof( float )*4 );
		m_pMaterials[i].m_shininess = pMaterial->m_shininess;
		m_pMaterials[i].m_pTextureFilename = QString( pMaterial->m_texture );
		pPtr += sizeof( MS3DMaterial );
	}
	return true;
}
