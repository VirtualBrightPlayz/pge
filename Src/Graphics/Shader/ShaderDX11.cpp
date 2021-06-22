#include <PGE/Graphics/Shader.h>
#include <PGE/Graphics/Graphics.h>
#include "ShaderDX11.h"
#include <PGE/Exception/Exception.h>
#include "../GraphicsDX11.h"

using namespace PGE;

ShaderDX11::ShaderDX11(Graphics* gfx,const FilePath& path) : resourceManager(3) {
    graphics = gfx;

    filepath = path;

    BinaryReader reader(path + "reflection.dxri");

    readConstantBuffers(reader, "cbVertex", vertexConstantBuffers);

    u32 propertyCount = reader.readUInt();
    vertexInputElems.resize(propertyCount);
    // We have to keep the names in memory.
    std::vector<String> semanticNames(propertyCount);
    std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc(propertyCount);
    for (u32 i = 0; i < propertyCount; ++i) {
        vertexInputElems[i] = reader.readNullTerminatedString();

        semanticNames[i] = reader.readNullTerminatedString();
        byte semanticIndex = reader.readByte();
        DXGI_FORMAT format = (DXGI_FORMAT)reader.readByte();

        D3D11_INPUT_ELEMENT_DESC vertexInputElemDesc;
        vertexInputElemDesc.SemanticName = semanticNames[i].cstr();
        vertexInputElemDesc.SemanticIndex = semanticIndex;
        vertexInputElemDesc.Format = format;
        vertexInputElemDesc.InputSlot = 0;
        vertexInputElemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        vertexInputElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        vertexInputElemDesc.InstanceDataStepRate = 0;

        dxVertexInputElemDesc[i] = vertexInputElemDesc;
    }

    readConstantBuffers(reader, "cbPixel", pixelConstantBuffers);

    u32 samplerCount = reader.readUInt();

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.MaxAnisotropy = 8;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MipLODBias = -0.1f;

    ID3D11Device* dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    resourceManager.increaseSize(samplerCount);
    dxSamplerState = ResourceViewVector<ID3D11SamplerState*>::withSize(samplerCount);
    for (int i = 0; i < samplerCount; i++) {
        dxSamplerState[i] = resourceManager.addNewResource<D3D11SamplerState>(dxDevice, samplerDesc);
    }

    std::vector<byte> vertexShaderBytecode;
    (path + "vertex.dxbc").readBytes(vertexShaderBytecode);
    PGE_ASSERT(vertexShaderBytecode.size() > 0, "Vertex shader is empty (filename: " + path.str() + ")");

    std::vector<byte> pixelShaderBytecode;
    (path + "pixel.dxbc").readBytes(pixelShaderBytecode);
    PGE_ASSERT(pixelShaderBytecode.size() > 0, "pixel shader is empty (filename: " + path.str() + ")");

    dxVertexShader = resourceManager.addNewResource<D3D11VertexShader>(dxDevice, vertexShaderBytecode);
    dxPixelShader = resourceManager.addNewResource<D3D11PixelShader>(dxDevice, pixelShaderBytecode);
    dxVertexInputLayout = resourceManager.addNewResource<D3D11InputLayout>(dxDevice, dxVertexInputElemDesc, vertexShaderBytecode);
}

void ShaderDX11::readConstantBuffers(BinaryReader& reader, const String& bufferName, ResourceViewVector<CBufferInfo*>& constantBuffers) {
    u32 cBufferSize = reader.readUInt();
    if (cBufferSize == 0) {
        return;
    }

    resourceManager.increaseSize(2);

    CBufferInfoView constantBuffer = resourceManager.addNewResource<CBufferInfoOwner>(graphics, bufferName, cBufferSize, &resourceManager);
    constantBuffers.add(constantBuffer);

    String varName;
    u32 varCount = reader.readUInt();
    for (u32 i = 0; i < varCount; i++) {
        varName = reader.readNullTerminatedString();
        u32 varOffset = reader.readUInt();
        u32 varSize = reader.readUInt();
        constantBuffer->addConstant(varName, ConstantDX11(constantBuffer, varOffset, varSize));
    }
}

Shader::Constant* ShaderDX11::getVertexShaderConstant(const String& name) {
    for (auto cBuffer : vertexConstantBuffers) {
        auto map = cBuffer->getConstants();
        auto it = map->find(name);
        if (it != map->end()) {
            return &it->second;
        }
    }
    return nullptr;
}

Shader::Constant* ShaderDX11::getFragmentShaderConstant(const String& name) {
    for (auto cBuffer : pixelConstantBuffers) {
        auto map = cBuffer->getConstants();
        auto it = map->find(name);
        if (it != map->end()) {
            return &it->second;
        }
    }
    return nullptr;
}

const std::vector<String>& ShaderDX11::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderDX11::useShader() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    for (int i = 0; i < (int)vertexConstantBuffers.size(); i++) {
        vertexConstantBuffers[i]->update();
        dxContext->VSSetConstantBuffers(i,1,&vertexConstantBuffers[i]->getDxCBuffer());
    }

    for (int i = 0; i < (int)pixelConstantBuffers.size(); i++) {
        pixelConstantBuffers[i]->update();
        dxContext->PSSetConstantBuffers(i,1,&pixelConstantBuffers[i]->getDxCBuffer());
    }
    
    dxContext->VSSetShader(dxVertexShader,NULL,0);
    dxContext->PSSetShader(dxPixelShader,NULL,0);
}

void ShaderDX11::useVertexInputLayout() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->IASetInputLayout(dxVertexInputLayout);
}

void ShaderDX11::useSamplers() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->PSSetSamplers(0, (UINT)dxSamplerState.size(), dxSamplerState.data());
}

ShaderDX11::CBufferInfoOwner::CBufferInfoOwner(Graphics* gfx, const String& nm, int sz, ResourceManager* rm) {
    resource = new ShaderDX11::CBufferInfo(gfx, nm, sz, rm);
}

ShaderDX11::CBufferInfo::CBufferInfo(Graphics* graphics, const String& nm, int sz, ResourceManager* resourceManager) {
    name = nm;
    size = sz;
    int cBufferSize = size;
    //round up to a multiple of 16, see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    if ((cBufferSize % 16) != 0) { cBufferSize += 16 - (cBufferSize % 16); }
    data = new byte[cBufferSize];

    D3D11_BUFFER_DESC cBufferDesc;
    D3D11_SUBRESOURCE_DATA cBufferSubresourceData;

    ZeroMemory( &cBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    cBufferDesc.ByteWidth = cBufferSize;
    cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &cBufferSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA) );
    cBufferSubresourceData.pSysMem = data;

    HRESULT hResult = 0;

    dxCBuffer = resourceManager->addNewResource<D3D11Buffer>(((GraphicsDX11*)graphics)->getDxDevice(), cBufferDesc, cBufferSubresourceData);

    dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    dirty = true;
}

ShaderDX11::CBufferInfo::~CBufferInfo() {
    delete[] data;
}

byte* ShaderDX11::CBufferInfo::getData() {
    return data;
}

std::unordered_map<String::Key, ShaderDX11::ConstantDX11>* ShaderDX11::CBufferInfo::getConstants() {
    return &constants;
}

void ShaderDX11::CBufferInfo::addConstant(const String& name, const ShaderDX11::ConstantDX11& constant) {
    constants.emplace(name, constant);
}

bool ShaderDX11::CBufferInfo::isDirty() const {
    return dirty;
}

void ShaderDX11::CBufferInfo::markAsDirty() {
    dirty = true;
}

void ShaderDX11::CBufferInfo::update() {
    if (!dirty) { return; }
    dxContext->UpdateSubresource(dxCBuffer,0,NULL,data,0,0);
    dirty = false;
}

D3D11Buffer::View ShaderDX11::CBufferInfo::getDxCBuffer() {
    return dxCBuffer;
}

ShaderDX11::ConstantDX11::ConstantDX11(ShaderDX11::CBufferInfoView cBuffer, int offst, int sz) {
    constantBuffer = cBuffer;
    offset = offst;
    size = sz;
}

void ShaderDX11::ConstantDX11::setValue(const Matrix4x4f& value) {
    memcpy(constantBuffer->getData()+offset,value.transpose()[0],16*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector2f& value) {
    float arr[2]; arr[0] = value.x; arr[1] = value.y;
    memcpy(constantBuffer->getData() + offset, arr, 2 * sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector3f& value) {
    float arr[3]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z;
    memcpy(constantBuffer->getData() + offset, arr, 3 * sizeof(float));
    if (size == 4 * sizeof(float)) {
        float one = 1.f;
        memcpy(constantBuffer->getData() + offset + (3 * sizeof(float)), &one, sizeof(float));
    }
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector4f& value) {
    float arr[4]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z; arr[3] = value.w;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Color& value) {
    float arr[4]; arr[0] = value.red; arr[1] = value.green; arr[2] = value.blue; arr[3] = value.alpha;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(float value) {
    memcpy(constantBuffer->getData()+offset,&value,sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(int value) {
    u32 valUi32 = value;
    memcpy(constantBuffer->getData()+offset,&valUi32,sizeof(u32));
    constantBuffer->markAsDirty();
}

