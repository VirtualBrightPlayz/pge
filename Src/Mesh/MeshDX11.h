#ifndef PGEINTERNAL_MESH_DX11_H_INCLUDED
#define PGEINTERNAL_MESH_DX11_H_INCLUDED

#include <vector>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <Windows.h>
#include <Mesh/Mesh.h>

#include "../ResourceManagement/DX11.h"

namespace PGE {

class MeshDX11 : public Mesh {
    public:
        MeshDX11(Graphics* gfx, Primitive::TYPE pt);

        virtual void updateInternalData();

        virtual void render();

    private:
        std::vector<uint8_t> dxVertexData;
        std::vector<WORD> dxIndexData;
        UINT stride = 0;

        D3D11_BUFFER_DESC dxVertexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxVertexBufferData;
        D3D11Buffer::Ref dxVertexBuffer;

        D3D11_BUFFER_DESC dxIndexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxIndexBufferData;
        D3D11Buffer::Ref dxIndexBuffer;

        ResourceManager resourceManager;

        virtual void uploadInternalData();
};

}

#endif // PGEINTERNAL_MESH_DX11_H_INCLUDED
