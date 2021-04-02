// Eugene Skepner <eu.skepner at skepner.eu>
// Inspired by
// https://gist.github.com/rmhsilva/61cc45587ed34707da34818a76476e11
//
#include <iostream>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

// ======================================================================

struct MTPoint
{
    float x;
    float y;
};

struct MTVector
{
    MTPoint position;
    MTPoint velocity;
};

enum class MTTouchState : uint32_t {
    NotTracking = 0,
    StartInRange = 1,
    HoverInRange = 2,
    MakeTouch = 3,
    Touching = 4,
    BreakTouch = 5,
    LingerInRange = 6,
    OutOfRange = 7
};

struct MTTouch
{
    int32_t frame;
    int32_t padding;
    double timestamp;
    int32_t pathIndex;
    MTTouchState state;
    int32_t fingerID;
    int32_t handID;
    MTVector normalizedVector;
    // float zTotal;
    // int32_t field9;
    // float angle;
    // float majorAxis;
    // float minorAxis;
    // MTVector absoluteVector;
    // int32_t field14;
    // int32_t field15;
    // float zDensity;
};

using MTDeviceRef = void*;
using MTContactCallbackFunction = int (*)(int, MTTouch*, int, double, int);

extern "C" {
MTDeviceRef MTDeviceCreateDefault();
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int); // thanks comex
}

static bool middle_touch = false;

// ======================================================================

static int touch_callback(int device, MTTouch* touches, int num_touches, double timestamp, int frame);
static CGEventRef handle_cg_event(CGEventTapProxy proxy, CGEventType type, CGEventRef eventRef, void *refcon);

// ======================================================================

int main(int /*argc*/, const char* const /*argv*/[])
{
    MTDeviceRef dev = MTDeviceCreateDefault();
    MTRegisterContactFrameCallback(dev, touch_callback);
    MTDeviceStart(dev, 0);

    const CGEventMask event_mask = CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp);
    CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, event_mask, handle_cg_event, nullptr);
    if (eventTap) {
        CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
        CGEventTapEnable(eventTap, true);

        CFRunLoopRun();
        return 0;
    }
    else {
        std::cerr << "WARNING: CGEventTapCreate returned null\n";
        return 1;
    }
}

// ======================================================================

int touch_callback(int /*device*/, MTTouch* touches, int num_touches, double /*timestamp*/, int /*frame*/)
{
    switch (num_touches) {
      case 1:
      case 2:
          middle_touch = (touches[0].normalizedVector.position.x > 0.4f && touches[0].normalizedVector.position.x < 0.6f);
          break;
      default:
          middle_touch = false;
          break;
    }
    return 0;

} // touch_callback

// ----------------------------------------------------------------------

CGEventRef handle_cg_event(CGEventTapProxy /*proxy*/, CGEventType type, CGEventRef eventRef, void* /*refcon*/)
{
    if (middle_touch) {
        // std::cout << "handle_cg_event middle_touch type:" << type << std::endl;
        switch (type) {
            case kCGEventLeftMouseDown:
            case kCGEventRightMouseDown:
                // std::cout << "left down\n";
                // break;
                return CGEventCreateMouseEvent(CGEventCreateSourceFromEvent(eventRef), kCGEventOtherMouseDown, CGEventGetLocation(eventRef), kCGMouseButtonCenter);
            case kCGEventLeftMouseUp:
            case kCGEventRightMouseUp:
                // std::cout << "left up\n";
                return CGEventCreateMouseEvent(CGEventCreateSourceFromEvent(eventRef), kCGEventOtherMouseUp, CGEventGetLocation(eventRef), kCGMouseButtonCenter);
                // std::cout << "right down\n";
                // break;
                // std::cout << "right up\n";
                // break;

            case kCGEventNull:
            case kCGEventMouseMoved:
            case kCGEventLeftMouseDragged:
            case kCGEventRightMouseDragged:
            case kCGEventKeyDown:
            case kCGEventKeyUp:
            case kCGEventFlagsChanged:
            case kCGEventScrollWheel:
            case kCGEventTabletPointer:
            case kCGEventTabletProximity:
            case kCGEventOtherMouseDown:
            case kCGEventOtherMouseUp:
            case kCGEventOtherMouseDragged:
            case kCGEventTapDisabledByTimeout:
            case kCGEventTapDisabledByUserInput:
                break;
        }
    }

    return eventRef;

} // handle_cg_event

// ----------------------------------------------------------------------
