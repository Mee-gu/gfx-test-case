//
//  View.h
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//

#pragma once

#ifdef CC_USE_METAL
    #import <MetalKit/MetalKit.h>
@interface View : MTKView <MTKViewDelegate>
#else
    #import <UIKit/UIView.h>
@interface View : UIView
#endif
@property (nonatomic, assign) BOOL preventTouch;

- (void)setPreventTouchEvent:(BOOL)flag;
@end
