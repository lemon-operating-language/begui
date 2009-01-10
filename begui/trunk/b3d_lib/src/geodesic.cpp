#include "geodesic.h"

void GeodesicSphere::create(int subdivisions, double radius)
{
	// create a icosahedron first

	double t1 = 2*PI/5;
	double t2 = PI/10;
	double t3 = -3*PI/10;
	double t4 = PI/5;

	//double R = (side/2)/sin(t4);
	//double side = 1.0;
	double R = radius;
	double side = 2*R*sin(t4);
	double S = side;

	double H = cos(t4)*R;
	double Cx = R * cos(t2);
	double Cy = R * sin(t2);

	double H1 = sqrt(side*side - R*R);
	double H2 = sqrt((H+R)*(H+R) - H*H);
	double Z2 = (H2 - H1)/2;
	double Z1 = Z2 + H1;

	std::vector<Vector3> pts(12);
	pts[0] = Vector3(   0,  0, Z1);
	pts[1] = Vector3(   0,  R, Z2);
	pts[2] = Vector3(  Cx, Cy, Z2);
	pts[3] = Vector3( S/2, -H, Z2);
	pts[4] = Vector3(-S/2, -H, Z2);
	pts[5] = Vector3( -Cx, Cy, Z2);
	pts[6] = Vector3(   0, -R,-Z2);
	pts[7] = Vector3( -Cx,-Cy,-Z2);
	pts[8] = Vector3(-S/2,  H,-Z2);
	pts[9] = Vector3( S/2,  H,-Z2);
	pts[10] = Vector3( Cx,-Cy,-Z2);
	pts[11] = Vector3(  0,  0,-Z1);

	// now create the faces
	std::vector<Face3> face(20);
	face[0].ids[0] = 0;
	face[0].ids[1] = 1;
	face[0].ids[2] = 2;
	face[1].ids[0] = 0;
	face[1].ids[1] = 2;
	face[1].ids[2] = 3;
	face[2].ids[0] = 0;
	face[2].ids[1] = 3;
	face[2].ids[2] = 4;
	face[3].ids[0] = 0;
	face[3].ids[1] = 4;
	face[3].ids[2] = 5;
	face[4].ids[0] = 0;
	face[4].ids[1] = 5;
	face[4].ids[2] = 1;
	face[5].ids[0] = 1;
	face[5].ids[1] = 9;
	face[5].ids[2] = 2;
	face[6].ids[0] = 2;
	face[6].ids[1] = 9;
	face[6].ids[2] = 10;
	face[7].ids[0] = 2;
	face[7].ids[1] = 10;
	face[7].ids[2] = 3;
	face[8].ids[0] = 3;
	face[8].ids[1] = 10;
	face[8].ids[2] = 6;
	face[9].ids[0] = 3;
	face[9].ids[1] = 6;
	face[9].ids[2] = 4;
	face[10].ids[0] = 4;
	face[10].ids[1] = 6;
	face[10].ids[2] = 7;
	face[11].ids[0] = 4;
	face[11].ids[1] = 7;
	face[11].ids[2] = 5;
	face[12].ids[0] = 5;
	face[12].ids[1] = 7;
	face[12].ids[2] = 8;
	face[13].ids[0] = 5;
	face[13].ids[1] = 8;
	face[13].ids[2] = 1;
	face[14].ids[0] = 1;
	face[14].ids[1] = 8;
	face[14].ids[2] = 9;
	
	face[15].ids[0] = 9;
	face[15].ids[1] = 11;
	face[15].ids[2] = 10;
	face[16].ids[0] = 10;
	face[16].ids[1] = 11;
	face[16].ids[2] = 6;
	face[17].ids[0] = 6;
	face[17].ids[1] = 11;
	face[17].ids[2] = 7;
	face[18].ids[0] = 7;
	face[18].ids[1] = 11;
	face[18].ids[2] = 8;
	face[19].ids[0] = 8;
	face[19].ids[1] = 11;
	face[19].ids[2] = 9;

	// now we have our icosahedron

	// let's extend it to a geodesic sphere
	// subdivide each face
	std::vector<Face3> facesGS = face;
	std::vector<Vector3> nodesGS = pts;
	for (int subdiv=1; subdiv<=subdivisions; ++subdiv)
	{
		std::vector<Face3> facesT = facesGS;	// temp face list (previous subdivision)
		std::vector<Vector3> nodesT = nodesGS;	// temp node list
		facesGS.clear();
		//nodesGS.clear();
		for (size_t i=0; i<facesT.size(); ++i)
		{
			Face3 &f = facesT[i];

			// create the new points
			Vector3 newV1 = (nodesT[f.ids[0]] + nodesT[f.ids[1]])/2.0;
			Vector3 newV2 = (nodesT[f.ids[1]] + nodesT[f.ids[2]])/2.0;
			Vector3 newV3 = (nodesT[f.ids[2]] + nodesT[f.ids[0]])/2.0;

			// push the new points out to the radius of the sphere
			/*double len = newV1.length();
			newV1.x = newV1.x * R / len;
			newV1.y = newV1.y * R / len;
			newV1.z = newV1.z * R / len;
			len = newV2.length();
			newV2.x = newV2.x * R / len;
			newV2.y = newV2.y * R / len;
			newV2.z = newV2.z * R / len;
			len = newV3.length();
			newV3.x = newV3.x * R / len;
			newV3.y = newV3.y * R / len;
			newV3.z = newV3.z * R / len;*/

			// create the 3 new faces
			int id0 = f.ids[0];
			int id1 = f.ids[1];
			int id2 = f.ids[2];
			int idN1 = nodesGS.size();
			for (size_t j=0; j<nodesGS.size(); ++j)
				if (nodesGS[j] == newV1) {
					idN1 = j;
					break;
				}
			if (idN1 == nodesGS.size())
				nodesGS.push_back(newV1);

			int idN2 = nodesGS.size();
			for (size_t j=0; j<nodesGS.size(); ++j)
				if (nodesGS[j] == newV2) {
					idN2 = j;
					break;
				}
			if (idN2 == nodesGS.size())
				nodesGS.push_back(newV2);

			int idN3 = nodesGS.size();
			for (size_t j=0; j<nodesGS.size(); ++j)
				if (nodesGS[j] == newV3) {
					idN3 = j;
					break;
				}
			if (idN3 == nodesGS.size())
				nodesGS.push_back(newV3);

			Face3 f1,f2,f3,f4;
			f1.ids[0] = id0;
			f1.ids[1] = idN1;
			f1.ids[2] = idN3;
			f2.ids[0] = idN1;
			f2.ids[1] = id1;
			f2.ids[2] = idN2;
			f3.ids[0] = idN2;
			f3.ids[1] = id2;
			f3.ids[2] = idN3;
			f4.ids[0] = idN1;
			f4.ids[1] = idN2;
			f4.ids[2] = idN3;

			facesGS.push_back(f1);
			facesGS.push_back(f2);
			facesGS.push_back(f3);
			facesGS.push_back(f4);
		}
	}

	for (size_t i=0; i<nodesGS.size(); ++i)
	{
		Vector3 &v = nodesGS[i];
		double len = v.length();
		v.x = v.x * R / len;
		v.y = v.y * R / len;
		v.z = v.z * R / len;
	}

	// store the result;
	m_faces = facesGS;
	m_nodes = nodesGS;
}

void GeodesicSphere::renderGL()
{
	// render the sphere using OpenGL
	glBegin(GL_TRIANGLES);
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		glVertex3f(m_nodes[m_faces[i].ids[0]].x, 
				m_nodes[m_faces[i].ids[0]].y, 
				m_nodes[m_faces[i].ids[0]].z);
		glVertex3f(m_nodes[m_faces[i].ids[1]].x, 
				m_nodes[m_faces[i].ids[1]].y, 
				m_nodes[m_faces[i].ids[1]].z);
		glVertex3f(m_nodes[m_faces[i].ids[2]].x, 
				m_nodes[m_faces[i].ids[2]].y, 
				m_nodes[m_faces[i].ids[2]].z);
	}
	glEnd();
}