#include <Shader/Shader.h>
#include <Graphics/Graphics.h>
#include "ShaderDX11.h"
#include <Exception/Exception.h>
#include <fstream>
#include <Misc/FileUtil.h>
#include "../Graphics/GraphicsDX11.h"

using namespace PGE;

ShaderDX11::ShaderDX11(Graphics* gfx,const FilePath& path) {
    graphics = gfx;

    filepath = path;

    std::ifstream reflectionInfo; reflectionInfo.open(String(path.str(),"reflection.dxri").cstr(), std::ios_base::in | std::ios_base::binary);
    
    vertexConstantBuffers = destructor.getReferenceArray<CBufferInfo>(0);
    readConstantBuffers(reflectionInfo,vertexConstantBuffers);

    int inputParamCount = 0; reflectionInfo.read((char*)(void*)&inputParamCount,1);
    dxVertexInputElemDesc = destructor.getReference<std::vector<D3D11_INPUT_ELEMENT_DESC>>([](const std::vector<D3D11_INPUT_ELEMENT_DESC>& v) { for (auto& e : v) { delete[] e.SemanticName; }}, inputParamCount);
    for (int i=0;i<inputParamCount;i++) {
        String propertyName = "";
        char chr; reflectionInfo.read(&chr,1);
        while (chr!=0) {
            propertyName = String(propertyName,chr);
            reflectionInfo.read(&chr, 1);
        }
        vertexInputElems.push_back(propertyName);

        String semanticName = "";
        reflectionInfo.read(&chr,1);
        while (chr!=0) {
            semanticName = String(semanticName,chr);
            reflectionInfo.read(&chr, 1);
        }
        char* nameBuf = new char[semanticName.byteLength() + (size_t)1];
        memcpy(nameBuf,semanticName.cstr(),(semanticName.byteLength()+(size_t)1)*sizeof(char));
        int index = 0;
        reflectionInfo.read((char*)(void*)&index,1);
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        reflectionInfo.read((char*)(void*)&format,1);

        D3D11_INPUT_ELEMENT_DESC vertexInputElemDesc;

        vertexInputElemDesc.SemanticName = nameBuf;
        vertexInputElemDesc.SemanticIndex = index;
        vertexInputElemDesc.Format = format;
        vertexInputElemDesc.InputSlot = 0;
        vertexInputElemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        vertexInputElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        vertexInputElemDesc.InstanceDataStepRate = 0;

        dxVertexInputElemDesc()[i] = vertexInputElemDesc;
    }

    fragmentConstantBuffers = destructor.getReferenceArray<CBufferInfo>(0);
    readConstantBuffers(reflectionInfo, fragmentConstantBuffers);

    int samplerCount = 0; reflectionInfo.read((char*)(void*)&samplerCount, 1);

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
    dxSamplerState = destructor.getReferenceDifferentDestructor<std::vector<ID3D11SamplerState*>>(GraphicsDX11::destroyChildren, samplerCount);
    for (int i = 0; i < samplerCount; i++) {
        ID3D11SamplerState* samplerState = NULL;
        dxDevice->CreateSamplerState(&samplerDesc, &samplerState);
        dxSamplerState()[i] = samplerState;
    }

    reflectionInfo.close();

    vertexShaderBytecode = FileUtil::readBytes(path + "vertex.dxbc");
    if (vertexShaderBytecode.size() <= 0) {
        throw Exception("ShaderDX11","Vertex shader is empty (filename: "+path.str() +")");
    }

    fragmentShaderBytecode = FileUtil::readBytes(path + "fragment.dxbc");
    if (fragmentShaderBytecode.size() <= 0) {
        throw Exception("ShaderDX11","Fragment shader is empty (filename: "+path.str()+")");
    }

    HRESULT hResult = 0;

    dxVertexShader = destructor.getReferenceDifferentDestructor<ID3D11VertexShader*>(GraphicsDX11::destroyChild, nullptr);
    hResult = dxDevice->CreateVertexShader(vertexShaderBytecode.data(),sizeof(uint8_t)*vertexShaderBytecode.size(),NULL,&dxVertexShader);
    if (FAILED(hResult)) {
        throw Exception("ShaderDX11","Failed to create vertex shader (filename: "+path.str()+"; HRESULT "+String::fromInt(hResult)+")");
    }

    dxFragmentShader = destructor.getReferenceDifferentDestructor<ID3D11PixelShader*>(GraphicsDX11::destroyChild, nullptr);
    hResult = dxDevice->CreatePixelShader(fragmentShaderBytecode.data(),sizeof(uint8_t)*fragmentShaderBytecode.size(),NULL,&dxFragmentShader);
    if (FAILED(hResult)) {
        throw Exception("ShaderDX11", "Failed to create fragment shader (filename: "+path.str()+"; HRESULT "+String::fromInt(hResult)+")");
    }

    dxVertexInputLayout = destructor.getReferenceDifferentDestructor<ID3D11InputLayout*>(GraphicsDX11::destroyChild, nullptr);
    hResult = dxDevice->CreateInputLayout(dxVertexInputElemDesc().data(), (UINT)dxVertexInputElemDesc().size(), getDxVsCode(), getDxVsCodeLen() * sizeof(uint8_t), &dxVertexInputLayout);
    if (FAILED(hResult)) {
        throw Exception("ShaderDX11", "Failed to create input layout (filename: "+path.str()+"; HRESULT "+String::fromInt(hResult)+")");
    }
}

void ShaderDX11::readConstantBuffers(std::ifstream& reflectionInfo, SmartRef<std::vector<CBufferInfo*>>& constantBuffers) {
    int cBufferCount = 0; reflectionInfo.read((char*)(void*)&cBufferCount, 1);
    for (int i = 0; i < cBufferCount; i++) {
        String cBufferName = "";
        char chr; reflectionInfo.read(&chr, 1);
        while (chr != 0) {
            cBufferName = String(cBufferName, chr);
            reflectionInfo.read(&chr, 1);
        }
        int cBufferSize = 0; reflectionInfo.read((char*)(void*)&cBufferSize, 1);
        
        CBufferInfo* constantBuffer = new CBufferInfo(graphics,cBufferName,cBufferSize);

        int varCount = 0; reflectionInfo.read((char*)(void*)&varCount, 1);
        for (int j = 0; j < varCount; j++) {
            String varName = "";
            reflectionInfo.read(&chr, 1);
            while (chr != 0) {
                varName = String(varName, chr);
                reflectionInfo.read(&chr, 1);
            }
            int varOffset = 0; reflectionInfo.read((char*)(void*)&varOffset, 1);
            int varSize = 0; reflectionInfo.read((char*)(void*)&varSize, 1);
            constantBuffer->addConstant(ConstantDX11(constantBuffer,varName,varOffset,varSize));
        }
        constantBuffers().push_back(constantBuffer);
    }
}

Shader::Constant* ShaderDX11::getVertexShaderConstant(String name) {
    for (int i=0;i<vertexConstantBuffers().size();i++) {
        std::vector<ConstantDX11>& vars = vertexConstantBuffers()[i]->getConstants(); 
        for (int j=0;j<vars.size();j++) {
            if (name.equals(vars[j].getName())) {
                return &vars[j];
            }
        }
    }
    return nullptr;
}

Shader::Constant* ShaderDX11::getFragmentShaderConstant(String name) {
    for (int i=0;i<fragmentConstantBuffers().size();i++) {
        std::vector<ConstantDX11>& vars = fragmentConstantBuffers()[i]->getConstants(); 
        for (int j=0;j<vars.size();j++) {
            if (name.equals(vars[j].getName())) {
                return &vars[j];
            }
        }
    }
    return nullptr;
}

uint8_t* ShaderDX11::getDxVsCode() {
    return vertexShaderBytecode.data();
}

int ShaderDX11::getDxVsCodeLen() const {
    return (int)vertexShaderBytecode.size();
}

uint8_t* ShaderDX11::getDxFsCode() {
    return fragmentShaderBytecode.data();
}

int ShaderDX11::getDxFsCodeLen() const {
    return (int)fragmentShaderBytecode.size();
}

const std::vector<String>& ShaderDX11::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderDX11::useShader() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    for (int i=0;i<vertexConstantBuffers().size();i++) {
        vertexConstantBuffers()[i]->update();
        ID3D11Buffer* dxCBuffer = vertexConstantBuffers()[i]->getDxCBuffer();
        dxContext->VSSetConstantBuffers(i,1,&dxCBuffer);
    }

    for (int i=0;i<fragmentConstantBuffers().size();i++) {
        fragmentConstantBuffers()[i]->update();
        ID3D11Buffer* dxCBuffer = fragmentConstantBuffers()[i]->getDxCBuffer();
        dxContext->PSSetConstantBuffers(i,1,&dxCBuffer);
    }
    
    dxContext->VSSetShader(dxVertexShader(),NULL,0);
    dxContext->PSSetShader(dxFragmentShader(),NULL,0);
}

void ShaderDX11::useVertexInputLayout() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->IASetInputLayout(dxVertexInputLayout());
}

void ShaderDX11::useSamplers() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->PSSetSamplers(0, (UINT)dxSamplerState().size(), dxSamplerState().data());
}

ShaderDX11::CBufferInfo::CBufferInfo(Graphics* graphics,String nm,int sz) {
    name = nm;
    size = sz;
    data = new uint8_t[size];

    dxCBuffer = nullptr;

    D3D11_BUFFER_DESC cBufferDesc;
    D3D11_SUBRESOURCE_DATA cBufferSubresourceData;

    ZeroMemory( &cBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    cBufferDesc.ByteWidth = sizeof(FLOAT)*48;
    cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &cBufferSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA) );
    cBufferSubresourceData.pSysMem = data;

    HRESULT hResult = 0;

    hResult = ((GraphicsDX11*)graphics)->getDxDevice()->CreateBuffer(&cBufferDesc,&cBufferSubresourceData,&dxCBuffer);
    if (FAILED(hResult)) {
        throw Exception("ShaderDX11::ShaderDX11","Failed to create CBuffer (name: "+nm+", size: "+String::fromInt(sz)+")");
    }

    dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    dirty = true;
}

ShaderDX11::CBufferInfo::~CBufferInfo() {
    delete[] data;
    dxCBuffer->Release();
}

uint8_t* ShaderDX11::CBufferInfo::getData() {
    return data;
}

std::vector<ShaderDX11::ConstantDX11>& ShaderDX11::CBufferInfo::getConstants() {
    return constants;
}

void ShaderDX11::CBufferInfo::addConstant(ShaderDX11::ConstantDX11 constant) {
    constants.push_back(constant);
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

ID3D11Buffer* ShaderDX11::CBufferInfo::getDxCBuffer() {
    return dxCBuffer;
}

ShaderDX11::ConstantDX11::ConstantDX11(ShaderDX11::CBufferInfo* cBuffer, String nm, int offst, int sz) {
    constantBuffer = cBuffer;
    name = nm;
    offset = offst;
    size = sz;
}

void ShaderDX11::ConstantDX11::setValue(Matrix4x4f value) {
    memcpy(constantBuffer->getData()+offset,value.transpose().elements,16*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector2f value) {
    float arr[2]; arr[0] = value.x; arr[1] = value.y;
    memcpy(constantBuffer->getData() + offset, arr, 2 * sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector3f value) {
    float arr[3]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z;
    memcpy(constantBuffer->getData() + offset, arr, 3 * sizeof(float));
    if (size == 4 * sizeof(float)) {
        float one = 1.f;
        memcpy(constantBuffer->getData() + offset + (3 * sizeof(float)), &one, sizeof(float));
    }
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector4f value) {
    float arr[4]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z; arr[3] = value.w;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Color value) {
    float arr[4]; arr[0] = value.red; arr[1] = value.green; arr[2] = value.blue; arr[3] = value.alpha;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(float value) {
    memcpy(constantBuffer->getData()+offset,&value,sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(int value) {
    uint32_t valUi32 = value;
    memcpy(constantBuffer->getData()+offset,&valUi32,sizeof(uint32_t));
    constantBuffer->markAsDirty();
}

String ShaderDX11::ConstantDX11::getName() const {
    return name;
}

