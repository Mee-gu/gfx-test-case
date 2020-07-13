#include "BasicTriangleTest.h"

namespace cc {

    void BasicTriangle::destroy() {
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_uniformBuffer);
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_bindingLayout);
        CC_SAFE_DESTROY(_pipelineState);
        CC_SAFE_DESTROY(_indexBuffer);
        CC_SAFE_DESTROY(_indirectBuffer);
    }

    bool BasicTriangle::initialize() {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createPipeline();

        return true;
    }

    void BasicTriangle::createShader() {

        ShaderSources sources;
        sources.glsl4 = {
R"(
            layout(location = 0) in vec2 a_position;
            void main() {
                gl_Position = vec4(a_position, 0.0, 1.0);
            }
)", R"(
            layout(binding = 0) uniform Color {
                vec4 u_color;
            };
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = u_color;
            }
)"
        };

        sources.glsl3 = {
R"(
            in vec2 a_position;
            void main() {
                gl_Position = vec4(a_position, 0.0, 1.0);
            }
)", R"(
            uniform Color {
                vec4 u_color;
            };

            out vec4 o_color;
            void main() {
                o_color = vec4(1, 1, 0, 1); // u_color;
            }
)" 
        };

        sources.glsl1 = {
R"(
            attribute vec2 a_position;
            void main() {
                gl_Position = vec4(a_position, 0.0, 1.0);
            }
)", R"(
            uniform vec4 u_color;
            void main() {
                gl_FragColor = vec4(1, 1, 0, 1); // u_color;
            }
)"
        };

        ShaderSource &source = TestBaseI::getAppropriateShaderSource(_device, sources);

        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::ShaderType::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::UniformList uniformList = { { "u_color", gfx::Type::FLOAT4, 1 } };
        gfx::UniformBlockList uniformBlockList = { { gfx::ShaderType::FRAGMENT, 0, "Color", uniformList } };
        gfx::AttributeList attributeList = { { "a_position", gfx::Format::RG32F, false, 0, false, 0 } };

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "Basic Triangle";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        _shader = _device->createShader(shaderInfo);
    }

    void BasicTriangle::createVertexBuffer() {
        float ySign = _device->getScreenSpaceSignY();

        float vertexData[] = {
            -0.5f,  0.5f * ySign,
            -0.5f, -0.5f * ySign,
             0.5f, -0.5f * ySign,
             0.0f,  0.5f * ySign,
             0.5f,  0.5f * ySign,
        };

        gfx::BufferInfo vertexBufferInfo = {
              gfx::BufferUsage::VERTEX,
              gfx::MemoryUsage::DEVICE,
              2 * sizeof(float),
              sizeof(vertexData),
              gfx::BufferFlagBit::NONE,
        };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        gfx::BufferInfo uniformBufferInfo = {
               gfx::BufferUsage::UNIFORM,
               gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
               4 * sizeof(float),
               sizeof(gfx::Color),
               gfx::BufferFlagBit::NONE,
        };
        _uniformBuffer = _device->createBuffer(uniformBufferInfo);

        unsigned short indices[] = { 1,3,0,1,2,3,2,4,3 };
        gfx::BufferInfo indexBufferInfo = {
            gfx::BufferUsageBit::INDEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(unsigned short),
            sizeof(indices),
            gfx::BufferFlagBit::NONE
        };
        _indexBuffer = _device->createBuffer(indexBufferInfo);
        _indexBuffer->update(indices, 0, sizeof(indices));

        gfx::DrawInfo drawInfo;
        drawInfo.firstIndex = 3;
        drawInfo.indexCount = 3;

        gfx::BufferInfo indirectBufferInfo = {
            gfx::BufferUsageBit::INDIRECT,
            gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
            sizeof(gfx::DrawInfo),
            sizeof(gfx::DrawInfo),
            gfx::BufferFlagBit::NONE
        };
        _indirectBuffer = _device->createBuffer(indirectBufferInfo);
        _indirectBuffer->update(&drawInfo, 0, sizeof(gfx::DrawInfo));

    }

    void BasicTriangle::createInputAssembler() {
        gfx::Attribute position = { "a_position", gfx::Format::RG32F, false, 0, false };
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        inputAssemblerInfo.indexBuffer = _indexBuffer;
        inputAssemblerInfo.indirectBuffer = _indirectBuffer;
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void BasicTriangle::createPipeline() {
        gfx::BindingLayoutInfo bindingLayoutInfo = { _shader };
        _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = _shader;
        pipelineInfo.inputState = { _inputAssembler->getAttributes() };
        pipelineInfo.renderPass = _fbo->getRenderPass();

        _pipelineState = _device->createPipelineState(pipelineInfo);
    }

    void BasicTriangle::tick(float dt) {

        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        _time += dt;
        gfx::Color clear_color = { 1.0f, 0, 0, 1.0f };

        gfx::Color uniformColor;
        uniformColor.r = std::abs(std::sin(_time));
        uniformColor.g = 1.0f;
        uniformColor.b = 0.0f;
        uniformColor.a = 1.0f;

        _uniformBuffer->update(&uniformColor, 0, sizeof(uniformColor));
        _bindingLayout->bindBuffer(0, _uniformBuffer);
        _bindingLayout->update();

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, render_area, std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->draw(_inputAssembler);
        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
