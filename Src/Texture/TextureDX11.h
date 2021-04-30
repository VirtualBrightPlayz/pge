#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <Texture/Texture.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../ResourceManagement/DX11.h"

namespace PGE {

class TextureDX11 : public Texture {
    public:
        // Render target.
        TextureDX11(Graphics* gfx, int w, int h, FORMAT fmt);
        // Loaded texture.
        TextureDX11(Graphics* gfx, int w, int h, uint8_t* buffer, FORMAT fmt);

        void useTexture(int index);

        Texture* copy() const override;

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        void* getNative() const override;

    private:
        D3D11Texture2D::Ref dxTexture;
        D3D11ShaderResourceView::Ref dxShaderResourceView;

        D3D11RenderTargetView::Ref dxRtv;
        D3D11Texture2D::Ref dxZBufferTexture;
        D3D11DepthStencilView::Ref dxZBufferView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
