#include "polygonMesh.h"
#include "rayIntersection.h"
#include "triangle.h"
#include "vector2d.h"
#include <fstream>

// file-scoped variables
static constexpr char factoryName[] = "PolygonMesh";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::geometric;
using namespace math::linear_algebra::vector;
using namespace utilities;

namespace math
{

namespace geometric
{

namespace shapes
{

// register factories...
static FactoryRegistrar<Shape> factory(factoryName, &PolygonMesh::create);

/**
 * Constructor
 */
PolygonMesh::PolygonMesh(void)
{

}

/**
 * Copy constructor
 */
PolygonMesh::PolygonMesh(const PolygonMesh &polygonMesh)
{

}

/**
 * Move constructor
 */
PolygonMesh::PolygonMesh(PolygonMesh &&polygonMesh)
{

}

/**
 * Destructor
 */
PolygonMesh::~PolygonMesh(void)
{

}

/**
 * Copy assignment operator
 */
PolygonMesh &PolygonMesh::operator = (const PolygonMesh &polygonMesh)
{
    if (&polygonMesh != this)
    {
        Shape3d::operator = (polygonMesh);

        m_polygons = polygonMesh.m_polygons;
    }

    return *this;
}

/**
 * Move assignment operator
 */
PolygonMesh &PolygonMesh::operator = (PolygonMesh &&polygonMesh)
{
    if (&polygonMesh != this)
    {
        Shape3d::operator = (std::move(polygonMesh));

        m_polygons = std::move(polygonMesh.m_polygons);
    }

    return *this;
}

/**
 * Subscript operator
 */
Polygon &PolygonMesh::operator [] (int index)
{
    return m_polygons[index];
}

/**
 * Subscript operator
 */
Polygon PolygonMesh::operator [] (int index) const
{
    return m_polygons[index];
}

/**
 * begin() overload
 */
PolygonMesh::iterator PolygonMesh::begin(void)
{
    return m_polygons.begin();
}

/**
 * Calculate the area of this shape
 */
double PolygonMesh::calcArea(void) const
{
    // TODO: implement this

    return 0.0;
}

/**
 * Calculate the volume of this shape
 */
double PolygonMesh::calcVolume(void) const
{
    // TODO: implement this

    return 0.0;
}

/**
 * cbegin() overload
 */
PolygonMesh::const_iterator PolygonMesh::cbegin(void) const
{
    return m_polygons.cbegin();
}

/**
 * cend() overload
 */
PolygonMesh::const_iterator PolygonMesh::cend(void) const
{
    return m_polygons.cend();
}

/**
 * Clear this polygon mesh of its entries
 */
void PolygonMesh::clear(void)
{
    m_polygons.clear();
}

/**
 * clone() function
 */
PolygonMesh *PolygonMesh::clone(void) const
{
    return new PolygonMesh(*this);
}

/**
 * crbegin() overload
 */
PolygonMesh::const_reverse_iterator PolygonMesh::crbegin(void) const
{
    return m_polygons.crbegin();
}

/**
 * create() function
 */
PolygonMesh *PolygonMesh::create(void)
{
    return new PolygonMesh();
}

/**
 * crend() overload
 */
PolygonMesh::const_reverse_iterator PolygonMesh::crend(void) const
{
    return m_polygons.crend();
}

/**
 * Test for empty variable registry
 */
bool PolygonMesh::empty(void) const
{
    return m_polygons.empty();
}

/**
 * end() overload
 */
PolygonMesh::iterator PolygonMesh::end(void)
{
    return m_polygons.end();
}

/**
 * Erase the entry in this polygon mesh associated with the input iterator
 */
PolygonMesh::iterator PolygonMesh::erase(iterator it)
{
    return m_polygons.erase(it);
}

/**
 * Get the name of this class
 */
std::string PolygonMesh::getClassName(void) const
{
    return "PolygonMesh";
}

/**
 * Get the factory name of this constructible
 */
std::string PolygonMesh::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this object's polygons
 */
std::vector<Polygon> &PolygonMesh::getPolygons(void)
{
    return m_polygons;
}

/**
 * Calculate the location(s) at which a ray intersects this shape; returns true if at least one intersection
 * occurs
 */
bool PolygonMesh::intersect(const Ray &ray,
                            RayIntersection &intersection)
{
    intersection.clear();

    auto &&itPolygon = m_polygons.begin();
    while (itPolygon != m_polygons.end())
    {
        // TODO: implement this
    }

    return true;
}

/**
 * rbegin() overload
 */
PolygonMesh::reverse_iterator PolygonMesh::rbegin(void)
{
    return m_polygons.rbegin();
}

/**
 * Function to read a standard obj file and populate this object's polygons
 */
bool PolygonMesh::readObjFile(const std::string &filename)
{
    std::string line, lineHeader;
    std::ifstream stream;
    std::istringstream iss;
    bool bSuccess = (bool)stream;
    if (bSuccess)
    {
        std::size_t numTriangles = 0;
        std::vector<std::size_t> normalIndices, textureIndices, vertexIndices;
        std::vector<Vector3d> normals, textures, vertices;
        while (getline(stream, line))
        {
            iss.str(line);
            iss >> lineHeader;
            if (std::strcmp(lineHeader.c_str(), "v") == 0)
            {
                Vector3d vertex;
                iss >> vertex[0];
                iss >> vertex[1];
                iss >> vertex[2];
                vertices.emplace_back(vertex);
            }
            else if (std::strcmp(lineHeader.c_str(), "vt") == 0)
            {
                Vector2d vertex;
                iss >> vertex[0];
                iss >> vertex[1];
                textures.emplace_back(vertex);
            }
            else if (std::strcmp(lineHeader.c_str(), "vn") == 0)
            {
                Vector3d normal;
                iss >> normal[0];
                iss >> normal[1];
                iss >> normal[2];
                normals.emplace_back(normal);
            }
            else if (std::strcmp(lineHeader.c_str(), "f") == 0)
            {
                std::size_t indices[9] = { 0 };
                bool bReadNormalIndices = false;
                bool bReadTextureIndices = false;
                bool bReadVertexIndices = false;
                if (std::sscanf(line.c_str() + 1, "%lu/%lu/%lu %lu/%lu/%lu %lu/%lu/%lu\n",
                                &indices[0],
                                &indices[1],
                                &indices[2],
                                &indices[3],
                                &indices[4],
                                &indices[5],
                                &indices[6],
                                &indices[7],
                                &indices[8]))
                {
                    bReadNormalIndices = true;
                    bReadTextureIndices = true;
                    bReadVertexIndices = true;
                }
                else if (std::sscanf(line.c_str() + 1, "%lu/%lu %lu/%lu %lu/%lu\n",
                                     &indices[0],
                                     &indices[1],
                                     &indices[2],
                                     &indices[3],
                                     &indices[4],
                                     &indices[5]))
                {
                    bReadVertexIndices = true;
                    bReadTextureIndices = true;
                }
                else if (std::sscanf(line.c_str() + 1, "%lu//%lu %lu//%lu %lu//%lu\n",
                                     &indices[0],
                                     &indices[1],
                                     &indices[2],
                                     &indices[6],
                                     &indices[7],
                                     &indices[8]))
                {
                    bReadNormalIndices = true;
                    bReadVertexIndices = true;
                }
                else if (std::sscanf(line.c_str() + 1, "%lu %lu %lu\\n",
                                     &indices[0],
                                     &indices[1],
                                     &indices[2]))
                {
                    bReadVertexIndices = true;
                }
                else
                {
                    logMsg(std::cout, LoggingLevel::Enum::Error,
                           "Failed to parse 3-d object file \"" + filename + "\"!\n",
                           getQualifiedMethodName(__func__));

                    return false;
                }

                if (bReadNormalIndices)
                {
                    // store normal indices
                    normalIndices.push_back(indices[2]);
                    normalIndices.push_back(indices[5]);
                    normalIndices.push_back(indices[8]);
                }

                if (bReadTextureIndices)
                {
                    // store texture indices
                    textureIndices.push_back(indices[1]);
                    textureIndices.push_back(indices[4]);
                    textureIndices.push_back(indices[7]);
                }

                if (bReadVertexIndices)
                {
                    // store vertex indices
                    vertexIndices.push_back(indices[0]);
                    vertexIndices.push_back(indices[3]);
                    vertexIndices.push_back(indices[6]);
                }

                ++numTriangles;
            }

            iss.clear();
        }

        Triangle triangle;
        m_polygons.resize(numTriangles);
        std::size_t polygonIndex = 0;
        while (polygonIndex < numTriangles)
        {
            auto i = 3 * polygonIndex + 0;
            auto j = 3 * polygonIndex + 1;
            auto k = 3 * polygonIndex + 2;

            auto &&polygon = m_polygons[polygonIndex];
            triangle.from3dVertices(vertices[vertexIndices[i]],
                                    vertices[vertexIndices[j]],
                                    vertices[vertexIndices[k]]);

            auto &&plane = polygon.getPlane();
            if (k < normals.size())
            {
                if (normalIndices[i] == normalIndices[j] &&
                    normalIndices[i] == normalIndices[k])
                {
                    plane.setNormal(normals[normalIndices[i]]);
                }
                else
                {
                    logMsg(std::cout, LoggingLevel::Enum::Error,
                           "Normal vector indices do not agree at face index " +
                           std::to_string(polygonIndex) + " in file \"" + filename + "\"!\n",
                           getQualifiedMethodName(__func__));

                    return false;
                }
            }
            else
            {
                plane.setNormal(triangle.getPlane().getNormal());
            }

            polygon.add(triangle.getVertexOne());
            polygon.add(triangle.getVertexTwo());
            polygon.add(triangle.getVertexThree());

            ++polygonIndex;
        }
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to open 3-d object file \"" + filename + "\"!\n",
               getQualifiedMethodName(__func__));
    }

    stream.close();

    return bSuccess;
}

/**
 * rend() overload
 */
PolygonMesh::reverse_iterator PolygonMesh::rend(void)
{
    return m_polygons.rend();
}

/**
 * Re-size this state vector
 */
void PolygonMesh::resize(std::size_t size)
{
    m_polygons.resize(size);
}

/**
 * Set this object's polygons
 */
void PolygonMesh::setPolygons(const std::vector<Polygon> &polygons)
{
    m_polygons = polygons;
}

/**
 * Return the size of this state vector
 */
std::size_t PolygonMesh::size(void) const
{
    return m_polygons.size();
}

}

}

}
