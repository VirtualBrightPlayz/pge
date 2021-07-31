#include <PGE/Graphics/Mesh.h>

using namespace PGE;

Mesh* Mesh::clone() {
    Mesh* newMesh = create(graphics);
    newMesh->material = material;
    newMesh->primitiveType = primitiveType;
    newMesh->vertices = vertices;
    newMesh->indices = indices;
    newMesh->mustReuploadInternalData = true;
    return newMesh;
}

void Mesh::setGeometry(const StructuredData& verts, const std::vector<Line>& lines) {
    PGE_ASSERT(material != nullptr, "Material must be set before geometry can be set");
    //TODO: check shader layout

    vertices = verts;
    indices.resize(lines.size() * 2);
    for (int i = 0; i < lines.size(); i++) {
        indices[(i * 2) + 0] = lines[i].indices[0];
        indices[(i * 2) + 1] = lines[i].indices[1];
    }
    primitiveType = PrimitiveType::LINE;

    mustReuploadInternalData = true;
}

void Mesh::setGeometry(const StructuredData& verts, const std::vector<Triangle>& triangles) {
    PGE_ASSERT(material != nullptr, "Material must be set before geometry can be set");
    //TODO: check shader layout

    vertices = verts;
    indices.resize(triangles.size() * 3);
    for (int i = 0; i < triangles.size(); i++) {
        indices[(i * 3) + 0] = triangles[i].indices[0];
        indices[(i * 3) + 1] = triangles[i].indices[1];
        indices[(i * 3) + 2] = triangles[i].indices[2];
    }
    primitiveType = PrimitiveType::TRIANGLE;

    mustReuploadInternalData = true;
}

void Mesh::clearGeometry() {
    indices.clear();
    primitiveType = PrimitiveType::NONE;
    mustReuploadInternalData = true;
}

void Mesh::clearGeometryAndSetMaterial(Material* m) {
    clearGeometry();
    material = m;
}

void Mesh::preserveGeometryAndSetMaterial(Material* m) {
    //TODO: check shader layout
    material = m;
}

bool Mesh::isOpaque() const {
    return material->isOpaque();
}

Mesh::Line::Line(int a, int b) {
    indices[0] = a; indices[1] = b;
}

Mesh::Triangle::Triangle(int a, int b, int c) {
    indices[0] = a; indices[1] = b; indices[2] = c;
}
