#include "InstancingTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

#include "cocos2d.h"

NS_CC_BEGIN

void Instancing::destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_instancedBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_bindingLayout);
}

bool Instancing::initialize()
{
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createTexture();
    createPipeline();
    return true;
}

void Instancing::createShader()
{
    GFXShaderStageList shaderStageList;
    GFXShaderStage vertexShaderStage;
    vertexShaderStage.type = GFXShaderType::VERTEX;
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
    vertexShaderStage.source = R"(

    )";
    
#else
    
#ifdef USE_GLES2
    vertexShaderStage.source = R"(
    
    )";
#else
    vertexShaderStage.source = R"(#version 300 es
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aOffset;

    out vec3 fColor;

    void main()
    {
        fColor = aColor;
        gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    }
    )";
#endif // USE_GLES2
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
    
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
    fragmentShaderStage.source = R"(
    
    )";
#else
    
#ifdef USE_GLES2
    fragmentShaderStage.source = R"(
    
    )";
#else
    fragmentShaderStage.source = R"(#version 300 es
    #ifdef GL_ES
    precision highp float;
    #endif
    out vec4 FragColor;

    in vec3 fColor;

    void main()
    {
        FragColor = vec4(fColor, 1.0);
    }

    )";
#endif // USE_GLES2
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Instancing";
    shaderInfo.stages = std::move(shaderStageList);
    _shader = _device->createShader(shaderInfo);
}

void Instancing::createVertexBuffer()
{
    // generate a list of 100 quad locations/translation-vectors
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            cocos2d::Vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            _translations[index++] = translation;
        }
    }

    GFXBufferInfo instancedBufferInfo = {
          GFXBufferUsage::VERTEX,
          GFXMemoryUsage::HOST,
          2 * sizeof(float),
          sizeof(_translations),
          GFXBufferFlagBit::NONE };

    // store instance data in an array buffer
    _instancedBuffer = _device->createBuffer(instancedBufferInfo);
    _instancedBuffer->update(&_translations[0], 0, sizeof(_translations));

   float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    GFXBufferInfo vertexBufferInfo = {
          GFXBufferUsage::VERTEX,
          GFXMemoryUsage::HOST,
          5 * sizeof(float),
          sizeof(quadVertices),
          GFXBufferFlagBit::NONE };

    // store instance data in an array buffer
    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(&quadVertices[0], 0, sizeof(quadVertices));
}

void Instancing::createInputAssembler()
{
    GFXAttribute position = {"aPos", GFXFormat::RG32F, false, 0, false};
    GFXAttribute color = {"aColor", GFXFormat::RGB32F, false, 0, false};
    GFXAttribute offset = {"aOffset", GFXFormat::RG32F, false, 1, true};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.attributes.emplace_back(std::move(color));
    inputAssemblerInfo.attributes.emplace_back(std::move(offset));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.vertexBuffers.emplace_back(_instancedBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    _inputAssembler->setInstanceCount(100);
}

void Instancing::createPipeline()
{
    GFXPipelineLayoutInfo pipelineLayoutInfo;
    auto pipelineLayout = _device->createPipelineLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.inputState = { _inputAssembler->getAttributes() };
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = _device->getMainWindow()->getRenderPass();

    _pipelineState = _device->createPipelineState(pipelineInfo);

    CC_SAFE_DESTROY(pipelineLayout);
}

void Instancing::createTexture()
{
}

void Instancing::tick(float dt) {

    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    GFXColor clear_color = {0, 0, 0, 1.0f};

    for(auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
//        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->draw(_inputAssembler);
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
