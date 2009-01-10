#pragma once

#include "common.h"

class Face3
{
public:
	int ids[3];
};

class GeodesicSphere
{
	std::vector<Vector3> m_nodes;
	std::vector<Face3> m_faces;

public:
	GeodesicSphere() { }

	void	create(int subdivisions, double radius = 1.0);
	int		getNodesNum() const		{ return (int)m_nodes.size(); }
	Vector3 getNode(int i) const	{ return m_nodes[i]; };
	void	renderGL();

	std::vector<Vector3> getNodes() const	{ return m_nodes; }
	std::vector<Face3>	getFaces() const	{ return m_faces; }
};