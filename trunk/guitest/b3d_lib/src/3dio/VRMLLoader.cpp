#include "VRMLLoader.h"

VRMLLoader::VRMLLoader()
{
}

VRMLLoader::~VRMLLoader()
{
}

bool VRMLLoader::loadFile(const std::string &filename)
{
	std::string str;

	// open the file
	if (!m_file.loadFile(filename))
		throw std::exception(strcat("failed to open file: ", filename.c_str()));

	try {
		m_file.addWordBreakChar('{');
		m_file.addWordBreakChar('}');
		m_file.addWordBreakChar(',');

		// check the VRML header
		m_file.skipComments(false);
		str = m_file.getline();
		if (str != "#VRML V2.0 utf8")
			throw std::exception("Wrong VRML header");

		// skip comments
		m_file.skipComments(true);
		m_file.addLineCommentDef("#");

		// search for Shape nodes
		while (!m_file.eof())
		{
			std::string nodeName, nodeClass;
			nextNode(nodeName, nodeClass);

			// read the block opening
			m_file >> str;
			if (str.length() == 0 && m_file.eof())
				break;
			if (str != "{")
				throw std::exception(("("+nodeName+":"+nodeClass+") : '{' expected, '" + str + "' found").c_str());
			
			// read the node contents
			if (nodeClass == "Shape") {
				readShapeNode();
			}
			else
				skipNode();
		}
	}
	catch (std::exception e) {
		m_file.close();
		char msg[1024];
		sprintf(msg, "error - line %d: %s", m_file.getCurLine(), e.what());
		throw std::exception(msg);
	}

	// close the file
	m_file.close();

	return true;
}

void VRMLLoader::nextNode(std::string &nodeName, std::string &nodeClass)
{
	std::string str;
	m_file >> str;
	if (str == "DEF") {
		// read node name
		m_file >> nodeName;
		m_file >> nodeClass;
	}
	else {
		nodeName = "";
		nodeClass = str;
	}
}

void VRMLLoader::readShapeNode()
{
	std::string nodeName, nodeClass, str;
	while (!m_file.eof())
	{
		std::string fld;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "appearance") {
			nextNode(nodeName, nodeClass);
			if (nodeClass != "Appearance")
				throw std::exception("Appearance node expected");

			m_file >> str;
			if (str != "{")
				throw std::exception(("'{' expected, '" + str + "' found").c_str());

			readAppearanceNode();
		}
		else if (fld == "geometry") {
			nextNode(nodeName, nodeClass);
			
			m_file >> str;
			if (str != "{")
				throw std::exception(("'{' expected, '" + str + "' found").c_str());
			
			if (nodeClass != "IndexedFaceSet")
				skipNode();

			readIndexedFaceSetNode();
		}
	}
}

void VRMLLoader::skipNode()
{
	int depth = 1;
	while (!m_file.eof() && depth > 0) {
		std::string str;
		m_file >> str;
		if (str == "{")
			depth++;
		else if (str == "}")
			depth--;
	}
}

void VRMLLoader::readAppearanceNode()
{
	std::string nodeName, nodeClass, str;
	while (!m_file.eof())
	{
		std::string fld;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "texture") {
			nextNode(nodeName, nodeClass);
			if (nodeClass != "ImageTexture")
				throw std::exception("ImageTexture node expected");

			m_file >> str;
			if (str != "{")
				throw std::exception(("'{' expected, '" + str + "' found").c_str());

			readTextureNode();
		}
	}
}

void VRMLLoader::readIndexedFaceSetNode()
{
	std::vector<Vector3> coords;
	std::vector<Vector2> texCoords;
	std::vector<int> coordIndices, texCoordIndices;

	std::string nodeName, nodeClass, str;
	while (!m_file.eof())
	{
		std::string fld;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "coord")
		{
			// Read a new Coordinate node
			nextNode(nodeName, nodeClass);
			if (nodeClass != "Coordinate")
				throw std::exception("Coordinate node expected");

			m_file >> str;
			if (str != "{")
				throw std::exception(("'{' expected, '" + str + "' found").c_str());

			readCoordinateNode(coords);
		}
		else if (fld == "coordIndex")
		{
			// read an array of indices
			m_file >> str;
			if (str != "[")
				throw std::exception("'[' expected");

			while (!m_file.eof()) {
				int id;
				m_file >> str;
				if (str == "]")
					break;
				id = atoi(str.c_str());
				if (id == -1) {
					m_file >> str;
					if (str == "]") break;
					if (str != ",") throw std::exception("',' expected");
				}
				else
					coordIndices.push_back(id);
			}
		}
		else if (fld == "texCoord")
		{
			// Read a new Coordinate node
			nextNode(nodeName, nodeClass);
			if (nodeClass != "TextureCoordinate")
				throw std::exception("Coordinate node expected");

			m_file >> str;
			if (str != "{")
				throw std::exception(("'{' expected, '" + str + "' found").c_str());

			readTextureCoordinateNode(texCoords);
		}
		else if (fld == "texCoordIndex")
		{
			// read an array of indices
			m_file >> str;
			if (str != "[")
				throw std::exception("'[' expected");

			while (!m_file.eof()) {
				int id;
				m_file >> str;
				if (str == "]")
					break;
				id = atoi(str.c_str());
				if (id == -1) {
					m_file >> str;
					if (str == "]") break;
					if (str != ",") throw std::exception("',' expected");
				}
				else
					texCoordIndices.push_back(id);
			}
		}
	}

	Console::print("loaded %d vertices, %d indices, %d tex coords, %d tex indices\n", 
		coords.size(), coordIndices.size(), texCoords.size(), texCoordIndices.size());
}

void VRMLLoader::readCoordinateNode(std::vector<Vector3> &coords)
{
	while (!m_file.eof())
	{
		std::string fld, str;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "point") {
			m_file >> str;
			if (str != "[")
				throw std::exception("'[' expected");

			while (!m_file.eof()) {
				Vector3 pt;
				m_file >> str;
				if (str == "]")
					break;
				else
					pt.x = atof(str.c_str());
				m_file >> pt.y;
				m_file >> pt.z;
				m_file >> str;

				coords.push_back(pt);

				if (str == "]") break;
				if (str != ",")
					throw std::exception(("',' expected, '" + str + "' found").c_str());
			}
		}
	}
}

void VRMLLoader::readTextureCoordinateNode(std::vector<Vector2> &coords)
{
	while (!m_file.eof())
	{
		std::string fld, str;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "point") {
			m_file >> str;
			if (str != "[")
				throw std::exception("'[' expected");

			while (!m_file.eof()) {
				Vector2 pt;
				m_file >> str;
				if (str == "]")
					break;
				else
					pt.x = atof(str.c_str());
				m_file >> pt.y;
				m_file >> str;

				coords.push_back(pt);

				if (str == "]") break;
				if (str != ",")
					throw std::exception(("',' expected, '" + str + "' found").c_str());
			}
		}
	}
}

void VRMLLoader::readTextureNode()
{
	while (!m_file.eof())
	{
		std::string fld, str;
		m_file >> fld;

		if (fld == "}")
			break;
		else if (fld == "url") {
			std::string texUrl;
			char c;
			do {
				c = fgetc(m_file.getFP());
			} while (!m_file.eof() && c!='"');
			while (!m_file.eof()) {
				c = fgetc(m_file.getFP());
				if (c == '"')
					break;
				texUrl += c;
			}

			Console::print("found texture url : " + texUrl + "\n");
		}
	}
}