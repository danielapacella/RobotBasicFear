/* 
 * Evorobot* - mesh.h
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



#ifndef wMESH_H
#define wMESH_H

#include <QString>


class wMesh
{

public:
	wMesh();
	virtual ~wMesh();

	bool loadMS3DModel(QString filename);

	//structures
	/*!	Mesh struct */
	struct Mesh {
		int m_materialIndex;
		int m_numTriangles;
		int *m_pTriangleIndices;
	};
	/*!	Material properties */
	struct Material {
		float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
		float m_shininess;
		QString m_pTextureFilename;
	};
	/*!	Triangle struct */
	struct Triangle {
		float m_vertexNormals[3][3];
		float m_s[3], m_t[3];
		int m_vertexIndices[3];
	};
	/*!	Vertex struct */
	struct Vertex {
		char m_boneID;
		float m_location[3];
	};

	/*! Return Meshes */
	Mesh* meshes() {
		return m_pMeshes;
	};
	/*! Number of meshes */
	int meshesCount() {
		return m_numMeshes;
	};
	/*! Return Materials (for now are ignored during rendering) */
	Material* materials() {
		return m_pMaterials;
	};
	/*! Number of materials */
	int materialsCount() {
		return m_numMaterials;
	};
	/*! Return Triangles composing the mesh */
	Triangle* triangles() {
		return m_pTriangles;
	};
	/*! Number of triangles */
	int trianglesCount() {
		return m_numTriangles;
	};
	/*! Return Vertices composing the triangles of the mesh */
	Vertex* vertices() {
		return m_pVertices;
	};
	/*! Number of vertices */
	int verticesCount() {
		return m_numVertices;
	};

	private:
	//--- Data representing the mesh
	int m_numMeshes;
	Mesh *m_pMeshes;
	int m_numMaterials;
	Material *m_pMaterials;
	int m_numTriangles;
	Triangle *m_pTriangles;
	int m_numVertices;
	Vertex *m_pVertices;





};//end class

#endif
