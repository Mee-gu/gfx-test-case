#pragma once
#include "Core.h"
#include "cocos2d.h"

//#define USE_METAL
//#define USE_VULKAN
//#define USE_GLES2
namespace cc {

    typedef struct WindowInfo {
        intptr_t windowHandle;
        gfx::GFXRect screen;
        int physicalWidth;
        int physicalHeight;
    } WindowInfo;

    struct Framebuffer {
        gfx::GFXRenderPass *renderPass = nullptr;
        gfx::GFXTexture *colorTex = nullptr;
        gfx::GFXTexture *depthStencilTex = nullptr;
        gfx::GFXFramebuffer *framebuffer = nullptr;

        ~Framebuffer() {
            CC_SAFE_DESTROY(framebuffer);
            CC_SAFE_DESTROY(depthStencilTex);
            CC_SAFE_DESTROY(colorTex);
            CC_SAFE_DESTROY(renderPass);
        }
    };

#define DEFINE_CREATE_METHOD(className)                \
    static TestBaseI *create(const WindowInfo &info) { \
        TestBaseI *test = CC_NEW(className(info));     \
        if (test->initialize())                        \
            return test;                               \
        else {                                         \
            CC_SAFE_DESTROY(test);                     \
            return nullptr;                            \
        }                                              \
    }

    class TestBaseI : public gfx::Object {
    public:
        TestBaseI(const WindowInfo &info);
        virtual ~TestBaseI() = default;

        virtual bool initialize() { return true; }
        virtual void destroy() {}
        virtual void tick(float dt) = 0;
        virtual void resize(uint width, uint height) { _device->resize(width, height); }

        static gfx::GFXDevice *getDevice() { return _device; }
        static void destroyGlobal();

        static unsigned char *RGB2RGBA(Image *img);
        static void modifyProjectionBasedOnDevice(Mat4 &projection);
        static float getViewportTopBasedOnDevice(float top, float height);
        static uint getMipmapLevelCounts(uint width, uint height);

    protected:
        static gfx::GFXDevice *_device;
        static gfx::GFXFramebuffer* _fbo;
        static std::vector<gfx::GFXCommandBuffer *> _commandBuffers;

        static gfx::GFXRenderPass *_renderPass;
    };

} // namespace cc
