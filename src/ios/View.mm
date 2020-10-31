//
//  View.m
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//
#import "View.h"
@implementation View {
}
@synthesize preventTouch;

- (id)initWithFrame:(CGRect)frame {
#ifdef CC_USE_METAL
    // if (self = [super initWithFrame:frame device:MTLCreateSystemDefaultDevice()]) {
    //     self.framebufferOnly = YES;
    //     self.delegate = self;
    //     self.preventTouch = FALSE;
    // }
#else
    if (self = [super initWithFrame:frame]) {
        self.preventTouch = FALSE;
    }
#endif

    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.preventTouch)
        return;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.preventTouch)
        return;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.preventTouch)
        return;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.preventTouch)
        return;
}

- (void)setPreventTouchEvent:(BOOL)flag {
    self.preventTouch = flag;
}

 #ifdef CC_USE_METAL
 - (void)drawInMTKView:(nonnull MTKView *)view {
 }

 - (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
     //TODO
 }
 #endif

@end
