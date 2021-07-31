#ifndef PGEINTERNAL_MESHOGL3_H_INCLUDED
#define PGEINTERNAL_MESHOGL3_H_INCLUDED

#include <PGE/Graphics/Mesh.h>

#include <vector>

#include <glad/gl.h>

#include "../../ResourceManagement/OGL3.h"
#include "../../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class GraphicsOGL3;

class MeshOGL3 : public Mesh {
    public:
        MeshOGL3(Graphics* gfx);

        void render() override;

    private:
        void uploadInternalData() override;

        GLBuffer::View glVertexBufferObject;
        GLBuffer::View glIndexBufferObject;

        GLVertexArray::View glVertexArrayObject;

        ResourceManagerOGL3 resourceManager;

        GraphicsOGL3* getGraphicsOgl3() const;
};

}

#endif // PGEINTERNAL_MESHOGL3_H_INCLUDED
