#include <PGE/Graphics/Material.h>

#include <PGE/Graphics/Graphics.h>

using namespace PGE;

Material::Material(Graphics& gfx, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o) {
    shader = &sh;
    textures = texs;
    opaque = o;
}

bool Material::isOpaque() const {
    return opaque == Opaque::YES;
}

Shader& Material::getShader() const {
    return *shader;
}

int Material::getTextureCount() const {
    return (int)textures.size();
}

Texture& Material::getTexture(int index) const {
    PGE_ASSERT(index >= 0 && index < getTextureCount(), "Texture index out of bounds");
    return textures.at(index);
}
