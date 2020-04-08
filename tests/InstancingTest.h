#pragma once

#include "TestBase.h"
#include "math/Mat4.h"
#include "math/Vec2.h"

NS_CC_BEGIN

class Instancing: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(Instancing)
    Instancing(const WindowInfo& info) : TestBaseI(info) {};
    ~Instancing() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    GFXShader* _shader = nullptr;
    GFXBuffer* _vertexBuffer = nullptr;
    GFXBuffer* _instancedBuffer = nullptr;
    GFXPipelineState* _pipelineState = nullptr;
    GFXInputAssembler* _inputAssembler = nullptr;
    GFXBindingLayout* _bindingLayout = nullptr;

    cocos2d::Vec2 _translations[100];
};

NS_CC_END
