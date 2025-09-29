#ifdef __APPLE__

#include "../../Core/WindowImpl.h"
#include "Core/Event.h"
#include "Core/EventManager.h"
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

// ǰ������
@class AppleWindowDelegate;
@class AppleWindowView;

class AppleWindowImpl : public WindowImpl {
public:
    AppleWindowImpl(const std::string& title, uint32_t width, uint32_t height);
    ~AppleWindowImpl() override;

    // WindowImpl�ӿ�ʵ��
    bool Create() override;
    void Destroy() override;
    void Show() override;
    void Hide() override;

    void SetTitle(const std::string& title) override;
    void SetSize(uint32_t width, uint32_t height) override;
    void SetPosition(int x, int y) override;
    void SetResizable(bool resizable) override;

    void ProcessMessages() override;
    bool ShouldClose() const override;

    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;
    int GetX() const override;
    int GetY() const override;
    const std::string& GetTitle() const override;
    bool IsVisible() const override;
    bool IsResizable() const override;

    void* GetNativeHandle() const override;

    // �¼�ϵͳ����
    void SetEventCallback(const EventCallbackFn& callback) override;
    void SetEventDispatchMode(bool useGlobalQueue, bool useDirectCallback = true) override;

    // �¼�����������Objective-Cί�е��ã�
    void OnWindowClose();
    void OnWindowResize(uint32_t width, uint32_t height);
    void OnWindowMove(int x, int y);
    void OnWindowFocus();
    void OnWindowLostFocus();
    void OnKeyPressed(KeyCode keyCode, ModifierKeys modifiers, bool isRepeat);
    void OnKeyReleased(KeyCode keyCode, ModifierKeys modifiers);
    void OnCharInput(uint32_t character);
    void OnMouseButtonPressed(MouseButton button, ModifierKeys modifiers);
    void OnMouseButtonReleased(MouseButton button, ModifierKeys modifiers);
    void OnMouseMoved(float x, float y);
    void OnMouseScrolled(float xOffset, float yOffset);

    // �¼�ת����������
    KeyCode CocoaKeyToKeyCode(unsigned short keyCode);
    MouseButton CocoaButtonToMouseButton(NSInteger buttonNumber);
    ModifierKeys CocoaModifiersToModifierKeys(NSEventModifierFlags flags);
    void DispatchEvent(Event& event);
    std::unique_ptr<Event> CreateEventCopy(const Event& event);

    // NSStringת����������
    NSString* StringToNSString(const std::string& str);
    std::string NSStringToString(NSString* nsStr);

private:
    // ��������
    std::string title_;
    uint32_t width_;
    uint32_t height_;
    int x_;
    int y_;
    bool shouldClose_;
    bool isVisible_;
    bool isResizable_;

    // Cocoa����
    NSWindow* nsWindow_;
    AppleWindowDelegate* windowDelegate_;
    AppleWindowView* windowView_;

    // �¼�ϵͳ
    EventCallbackFn eventCallback_;
    bool useGlobalEventQueue_;
    bool useDirectCallback_;

    // ��̬��Ա
    static std::unordered_map<NSWindow*, AppleWindowImpl*> windowMap_;
    static bool applicationInitialized_;
};

// Objective-C����ί����
@interface AppleWindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) AppleWindowImpl* windowImpl;
@end

@implementation AppleWindowDelegate

- (instancetype)initWithWindowImpl:(AppleWindowImpl*)impl {
    self = [super init];
    if (self) {
        self.windowImpl = impl;
    }
    return self;
}

- (BOOL)windowShouldClose:(NSWindow*)window {
    if (self.windowImpl) {
        self.windowImpl->OnWindowClose();
    }
    return NO; // ��C++������ƴ��ڹر�
}

- (void)windowDidResize:(NSNotification*)notification {
    if (self.windowImpl) {
        NSWindow* window = notification.object;
        NSRect contentRect = [window.contentView frame];
        self.windowImpl->OnWindowResize(
            static_cast<uint32_t>(contentRect.size.width),
            static_cast<uint32_t>(contentRect.size.height)
        );
    }
}

- (void)windowDidMove:(NSNotification*)notification {
    if (self.windowImpl) {
        NSWindow* window = notification.object;
        NSRect frame = [window frame];
        NSRect screenFrame = [[NSScreen mainScreen] frame];
        
        // ת������ϵ��Cocoaʹ�����½�Ϊԭ�㣬����ʹ�����Ͻǣ�
        int x = static_cast<int>(frame.origin.x);
        int y = static_cast<int>(screenFrame.size.height - frame.origin.y - frame.size.height);
        
        self.windowImpl->OnWindowMove(x, y);
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    if (self.windowImpl) {
        self.windowImpl->OnWindowFocus();
    }
}

- (void)windowDidResignKey:(NSNotification*)notification {
    if (self.windowImpl) {
        self.windowImpl->OnWindowLostFocus();
    }
}

@end

// Objective-C��ͼ�࣬���ڴ�����̺�����¼�
@interface AppleWindowView : NSView
@property (nonatomic, assign) AppleWindowImpl* windowImpl;
@end

@implementation AppleWindowView

- (instancetype)initWithWindowImpl:(AppleWindowImpl*)impl {
    self = [super init];
    if (self) {
        self.windowImpl = impl;
    }
    return self;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent*)event {
    if (self.windowImpl) {
        KeyCode keyCode = self.windowImpl->CocoaKeyToKeyCode([event keyCode]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        bool isRepeat = [event isARepeat];
        
        self.windowImpl->OnKeyPressed(keyCode, modifiers, isRepeat);
        
        // �����ַ�����
        NSString* characters = [event charactersIgnoringModifiers];
        if ([characters length] > 0) {
            unichar character = [characters characterAtIndex:0];
            if (character >= 32 && character != 127) { // ���˿����ַ�
                self.windowImpl->OnCharInput(static_cast<uint32_t>(character));
            }
        }
    }
}

- (void)keyUp:(NSEvent*)event {
    if (self.windowImpl) {
        KeyCode keyCode = self.windowImpl->CocoaKeyToKeyCode([event keyCode]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        
        self.windowImpl->OnKeyReleased(keyCode, modifiers);
    }
}

- (void)mouseDown:(NSEvent*)event {
    if (self.windowImpl) {
        MouseButton button = self.windowImpl->CocoaButtonToMouseButton([event buttonNumber]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        
        self.windowImpl->OnMouseButtonPressed(button, modifiers);
    }
}

- (void)mouseUp:(NSEvent*)event {
    if (self.windowImpl) {
        MouseButton button = self.windowImpl->CocoaButtonToMouseButton([event buttonNumber]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        
        self.windowImpl->OnMouseButtonReleased(button, modifiers);
    }
}

- (void)rightMouseDown:(NSEvent*)event {
    if (self.windowImpl) {
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        self.windowImpl->OnMouseButtonPressed(MouseButton::Right, modifiers);
    }
}

- (void)rightMouseUp:(NSEvent*)event {
    if (self.windowImpl) {
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        self.windowImpl->OnMouseButtonReleased(MouseButton::Right, modifiers);
    }
}

- (void)otherMouseDown:(NSEvent*)event {
    if (self.windowImpl) {
        MouseButton button = self.windowImpl->CocoaButtonToMouseButton([event buttonNumber]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        
        self.windowImpl->OnMouseButtonPressed(button, modifiers);
    }
}

- (void)otherMouseUp:(NSEvent*)event {
    if (self.windowImpl) {
        MouseButton button = self.windowImpl->CocoaButtonToMouseButton([event buttonNumber]);
        ModifierKeys modifiers = self.windowImpl->CocoaModifiersToModifierKeys([event modifierFlags]);
        
        self.windowImpl->OnMouseButtonReleased(button, modifiers);
    }
}

- (void)mouseMoved:(NSEvent*)event {
    [self handleMouseMove:event];
}

- (void)mouseDragged:(NSEvent*)event {
    [self handleMouseMove:event];
}

- (void)rightMouseDragged:(NSEvent*)event {
    [self handleMouseMove:event];
}

- (void)otherMouseDragged:(NSEvent*)event {
    [self handleMouseMove:event];
}

- (void)handleMouseMove:(NSEvent*)event {
    if (self.windowImpl) {
        NSPoint locationInWindow = [event locationInWindow];
        NSRect contentRect = [self frame];
        
        // ת������ϵ��Cocoaʹ�����½�Ϊԭ�㣬����ʹ�����Ͻǣ�
        float x = locationInWindow.x;
        float y = contentRect.size.height - locationInWindow.y;
        
        self.windowImpl->OnMouseMoved(x, y);
    }
}

- (void)scrollWheel:(NSEvent*)event {
    if (self.windowImpl) {
        float deltaX = static_cast<float>([event scrollingDeltaX]);
        float deltaY = static_cast<float>([event scrollingDeltaY]);
        
        // ����ȷ�������й���
        if ([event hasPreciseScrollingDeltas]) {
            deltaX *= 0.1f; // ������ȷ���������ж�
            deltaY *= 0.1f;
        }
        
        self.windowImpl->OnMouseScrolled(deltaX, deltaY);
    }
}

@end

// ��̬��Ա����
std::unordered_map<NSWindow*, AppleWindowImpl*> AppleWindowImpl::windowMap_;
bool AppleWindowImpl::applicationInitialized_ = false;

// ���캯��
AppleWindowImpl::AppleWindowImpl(const std::string& title, uint32_t width, uint32_t height)
    : title_(title), width_(width), height_(height), x_(100), y_(100)
    , shouldClose_(false), isVisible_(false), isResizable_(true)
    , nsWindow_(nil), windowDelegate_(nil), windowView_(nil)
    , useGlobalEventQueue_(true), useDirectCallback_(true) {
    
    // ȷ��NSApplication�ѳ�ʼ��
    if (!applicationInitialized_) {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        applicationInitialized_ = true;
    }
}

// ��������
AppleWindowImpl::~AppleWindowImpl() {
    Destroy();
}

// ��������
bool AppleWindowImpl::Create() {
    if (nsWindow_) {
        std::cout << "Window already created" << std::endl;
        return true;
    }

    @autoreleasepool {
        // ����������ʽ
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
        if (isResizable_) {
            styleMask |= NSWindowStyleMaskResizable;
        }

        // �������ھ���
        NSRect windowRect = NSMakeRect(x_, y_, width_, height_);

        // ����NSWindow
        nsWindow_ = [[NSWindow alloc] initWithContentRect:windowRect
                                                styleMask:styleMask
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];

        if (!nsWindow_) {
            std::cerr << "Failed to create NSWindow" << std::endl;
            return false;
        }

        // ���ô��ڱ���
        [nsWindow_ setTitle:StringToNSString(title_)];

        // ��������ί��
        windowDelegate_ = [[AppleWindowDelegate alloc] initWithWindowImpl:this];
        [nsWindow_ setDelegate:windowDelegate_];

        // �����Զ�����ͼ
        windowView_ = [[AppleWindowView alloc] initWithWindowImpl:this];
        [nsWindow_ setContentView:windowView_];

        // ���ô��ڽ�������ƶ��¼�
        [nsWindow_ setAcceptsMouseMovedEvents:YES];

        // ��������ӵ�ӳ��
        windowMap_[nsWindow_] = this;

        std::cout << "Window created successfully: " << title_ << " (" << width_ << "x" << height_ << ")" << std::endl;
        return true;
    }
}

// ���ٴ���
void AppleWindowImpl::Destroy() {
    if (nsWindow_) {
        @autoreleasepool {
            // ��ӳ�����Ƴ�
            windowMap_.erase(nsWindow_);

            // ����ί�к���ͼ
            [nsWindow_ setDelegate:nil];
            [nsWindow_ setContentView:nil];

            if (windowDelegate_) {
                [windowDelegate_ release];
                windowDelegate_ = nil;
            }

            if (windowView_) {
                [windowView_ release];
                windowView_ = nil;
            }

            // �رմ���
            [nsWindow_ close];
            [nsWindow_ release];
            nsWindow_ = nil;

            std::cout << "Window destroyed: " << title_ << std::endl;
        }
    }
}

// ��ʾ����
void AppleWindowImpl::Show() {
    if (nsWindow_) {
        @autoreleasepool {
            [nsWindow_ makeKeyAndOrderFront:nil];
            [NSApp activateIgnoringOtherApps:YES];
            isVisible_ = true;
            std::cout << "Window shown: " << title_ << std::endl;
        }
    }
}

// ���ش���
void AppleWindowImpl::Hide() {
    if (nsWindow_) {
        @autoreleasepool {
            [nsWindow_ orderOut:nil];
            isVisible_ = false;
            std::cout << "Window hidden: " << title_ << std::endl;
        }
    }
}

// ���ô��ڱ���
void AppleWindowImpl::SetTitle(const std::string& title) {
    title_ = title;
    if (nsWindow_) {
        @autoreleasepool {
            [nsWindow_ setTitle:StringToNSString(title)];
        }
    }
}

// ���ô��ڴ�С
void AppleWindowImpl::SetSize(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;

    if (nsWindow_) {
        @autoreleasepool {
            NSRect frame = [nsWindow_ frame];
            NSSize newContentSize = NSMakeSize(width, height);
            NSRect newFrame = [nsWindow_ frameRectForContentRect:NSMakeRect(0, 0, newContentSize.width, newContentSize.height)];
            
            // ���ִ������Ͻ�λ�ò���
            newFrame.origin.x = frame.origin.x;
            newFrame.origin.y = frame.origin.y + (frame.size.height - newFrame.size.height);
            
            [nsWindow_ setFrame:newFrame display:YES];
        }
    }
}

// ���ô���λ��
void AppleWindowImpl::SetPosition(int x, int y) {
    x_ = x;
    y_ = y;

    if (nsWindow_) {
        @autoreleasepool {
            NSRect screenFrame = [[NSScreen mainScreen] frame];
            NSRect windowFrame = [nsWindow_ frame];
            
            // ת������ϵ
            NSPoint newOrigin = NSMakePoint(x, screenFrame.size.height - y - windowFrame.size.height);
            [nsWindow_ setFrameOrigin:newOrigin];
        }
    }
}

// �����Ƿ�ɵ�����С
void AppleWindowImpl::SetResizable(bool resizable) {
    isResizable_ = resizable;

    if (nsWindow_) {
        @autoreleasepool {
            NSWindowStyleMask currentStyleMask = [nsWindow_ styleMask];
            
            if (resizable) {
                currentStyleMask |= NSWindowStyleMaskResizable;
            } else {
                currentStyleMask &= ~NSWindowStyleMaskResizable;
            }
            
            [nsWindow_ setStyleMask:currentStyleMask];
        }
    }
}

// �¼�ϵͳ����
void AppleWindowImpl::SetEventCallback(const EventCallbackFn& callback) {
    eventCallback_ = callback;
}

void AppleWindowImpl::SetEventDispatchMode(bool useGlobalQueue, bool useDirectCallback) {
    useGlobalEventQueue_ = useGlobalQueue;
    useDirectCallback_ = useDirectCallback;
}

// ������Ϣ
void AppleWindowImpl::ProcessMessages() {
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            [NSApp sendEvent:event];
        }
    }
}

// ���Ի�ȡ����
bool AppleWindowImpl::ShouldClose() const {
    return shouldClose_;
}

uint32_t AppleWindowImpl::GetWidth() const {
    if (nsWindow_) {
        @autoreleasepool {
            NSRect contentRect = [[nsWindow_ contentView] frame];
            return static_cast<uint32_t>(contentRect.size.width);
        }
    }
    return width_;
}

uint32_t AppleWindowImpl::GetHeight() const {
    if (nsWindow_) {
        @autoreleasepool {
            NSRect contentRect = [[nsWindow_ contentView] frame];
            return static_cast<uint32_t>(contentRect.size.height);
        }
    }
    return height_;
}

int AppleWindowImpl::GetX() const {
    if (nsWindow_) {
        @autoreleasepool {
            NSRect frame = [nsWindow_ frame];
            return static_cast<int>(frame.origin.x);
        }
    }
    return x_;
}

int AppleWindowImpl::GetY() const {
    if (nsWindow_) {
        @autoreleasepool {
            NSRect frame = [nsWindow_ frame];
            NSRect screenFrame = [[NSScreen mainScreen] frame];
            return static_cast<int>(screenFrame.size.height - frame.origin.y - frame.size.height);
        }
    }
    return y_;
}

const std::string& AppleWindowImpl::GetTitle() const {
    return title_;
}

bool AppleWindowImpl::IsVisible() const {
    if (nsWindow_) {
        @autoreleasepool {
            return [nsWindow_ isVisible];
        }
    }
    return isVisible_;
}

bool AppleWindowImpl::IsResizable() const {
    return isResizable_;
}

void* AppleWindowImpl::GetNativeHandle() const {
    return (__bridge void*)nsWindow_;
}

// �¼�������
void AppleWindowImpl::OnWindowClose() {
    shouldClose_ = true;
    WindowCloseEvent event;
    DispatchEvent(event);
}

void AppleWindowImpl::OnWindowResize(uint32_t width, uint32_t height) {
    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        WindowResizeEvent event(width_, height_);
        DispatchEvent(event);
    }
}

void AppleWindowImpl::OnWindowMove(int x, int y) {
    if (x != x_ || y != y_) {
        x_ = x;
        y_ = y;
        WindowMoveEvent event(x_, y_);
        DispatchEvent(event);
    }
}

void AppleWindowImpl::OnWindowFocus() {
    WindowFocusEvent event;
    DispatchEvent(event);
}

void AppleWindowImpl::OnWindowLostFocus() {
    WindowLostFocusEvent event;
    DispatchEvent(event);
}

void AppleWindowImpl::OnKeyPressed(KeyCode keyCode, ModifierKeys modifiers, bool isRepeat) {
    KeyPressedEvent event(keyCode, modifiers, isRepeat);
    DispatchEvent(event);
}

void AppleWindowImpl::OnKeyReleased(KeyCode keyCode, ModifierKeys modifiers) {
    KeyReleasedEvent event(keyCode, modifiers);
    DispatchEvent(event);
}

void AppleWindowImpl::OnCharInput(uint32_t character) {
    CharInputEvent event(character);
    DispatchEvent(event);
}

void AppleWindowImpl::OnMouseButtonPressed(MouseButton button, ModifierKeys modifiers) {
    MouseButtonPressedEvent event(button, modifiers);
    DispatchEvent(event);
}

void AppleWindowImpl::OnMouseButtonReleased(MouseButton button, ModifierKeys modifiers) {
    MouseButtonReleasedEvent event(button, modifiers);
    DispatchEvent(event);
}

void AppleWindowImpl::OnMouseMoved(float x, float y) {
    MouseMovedEvent event(x, y);
    DispatchEvent(event);
}

void AppleWindowImpl::OnMouseScrolled(float xOffset, float yOffset) {
    MouseScrolledEvent event(xOffset, yOffset);
    DispatchEvent(event);
}

// �¼��ַ���������
void AppleWindowImpl::DispatchEvent(Event& event) {
    // ��ʽ1: ֱ�ӻص� (��������)
    if (useDirectCallback_ && eventCallback_) {
        eventCallback_(event);
    }

    // ��ʽ2: ����ȫ���¼����� (�ӳٴ���)
    if (useGlobalEventQueue_) {
        std::unique_ptr<Event> eventCopy = CreateEventCopy(event);
        if (eventCopy) {
            EventManager::Instance().PostEvent(std::move(eventCopy));
        }
    }
}

// �����¼������ĸ�������
std::unique_ptr<Event> AppleWindowImpl::CreateEventCopy(const Event& event) {
    switch (event.GetEventType()) {
    case EventType::WindowClose:
        return std::make_unique<WindowCloseEvent>();

    case EventType::WindowResize: {
        const auto& e = static_cast<const WindowResizeEvent&>(event);
        return std::make_unique<WindowResizeEvent>(e.GetWidth(), e.GetHeight());
    }

    case EventType::WindowMove: {
        const auto& e = static_cast<const WindowMoveEvent&>(event);
        return std::make_unique<WindowMoveEvent>(e.GetX(), e.GetY());
    }

    case EventType::WindowFocus:
        return std::make_unique<WindowFocusEvent>();

    case EventType::WindowLostFocus:
        return std::make_unique<WindowLostFocusEvent>();

    case EventType::KeyPressed: {
        const auto& e = static_cast<const KeyPressedEvent&>(event);
        return std::make_unique<KeyPressedEvent>(e.GetKeyCode(), e.GetModifiers(), e.IsRepeat());
    }

    case EventType::KeyReleased: {
        const auto& e = static_cast<const KeyReleasedEvent&>(event);
        return std::make_unique<KeyReleasedEvent>(e.GetKeyCode(), e.GetModifiers());
    }

    case EventType::CharInput: {
        const auto& e = static_cast<const CharInputEvent&>(event);
        return std::make_unique<CharInputEvent>(e.GetCharacter());
    }

    case EventType::MouseButtonPressed: {
        const auto& e = static_cast<const MouseButtonPressedEvent&>(event);
        return std::make_unique<MouseButtonPressedEvent>(e.GetMouseButton(), e.GetModifiers());
    }

    case EventType::MouseButtonReleased: {
        const auto& e = static_cast<const MouseButtonReleasedEvent&>(event);
        return std::make_unique<MouseButtonReleasedEvent>(e.GetMouseButton(), e.GetModifiers());
    }

    case EventType::MouseMoved: {
        const auto& e = static_cast<const MouseMovedEvent&>(event);
        return std::make_unique<MouseMovedEvent>(e.GetX(), e.GetY());
    }

    case EventType::MouseScrolled: {
        const auto& e = static_cast<const MouseScrolledEvent&>(event);
        return std::make_unique<MouseScrolledEvent>(e.GetXOffset(), e.GetYOffset());
    }

    default:
        return nullptr;
    }
}

// Cocoa����ת��ΪKeyCode
KeyCode AppleWindowImpl::CocoaKeyToKeyCode(unsigned short keyCode) {
    switch (keyCode) {
        // ��ĸ��
        case 0x00: return KeyCode::A; case 0x0B: return KeyCode::B; case 0x08: return KeyCode::C;
        case 0x02: return KeyCode::D; case 0x0E: return KeyCode::E; case 0x03: return KeyCode::F;
        case 0x05: return KeyCode::G; case 0x04: return KeyCode::H; case 0x22: return KeyCode::I;
        case 0x26: return KeyCode::J; case 0x28: return KeyCode::K; case 0x25: return KeyCode::L;
        case 0x2E: return KeyCode::M; case 0x2D: return KeyCode::N; case 0x1F: return KeyCode::O;
        case 0x23: return KeyCode::P; case 0x0C: return KeyCode::Q; case 0x0F: return KeyCode::R;
        case 0x01: return KeyCode::S; case 0x11: return KeyCode::T; case 0x20: return KeyCode::U;
        case 0x09: return KeyCode::V; case 0x0D: return KeyCode::W; case 0x07: return KeyCode::X;
        case 0x10: return KeyCode::Y; case 0x06: return KeyCode::Z;

        // ���ּ�
        case 0x1D: return KeyCode::D0; case 0x12: return KeyCode::D1; case 0x13: return KeyCode::D2;
        case 0x14: return KeyCode::D3; case 0x15: return KeyCode::D4; case 0x17: return KeyCode::D5;
        case 0x16: return KeyCode::D6; case 0x1A: return KeyCode::D7; case 0x1C: return KeyCode::D8;
        case 0x19: return KeyCode::D9;

        // ���ܼ�
        case 0x7A: return KeyCode::F1;  case 0x78: return KeyCode::F2;
        case 0x63: return KeyCode::F3;  case 0x76: return KeyCode::F4;
        case 0x60: return KeyCode::F5;  case 0x61: return KeyCode::F6;
        case 0x62: return KeyCode::F7;  case 0x64: return KeyCode::F8;
        case 0x65: return KeyCode::F9;  case 0x6D: return KeyCode::F10;
        case 0x67: return KeyCode::F11; case 0x6F: return KeyCode::F12;

        // �����
        case 0x31: return KeyCode::Space;
        case 0x24: return KeyCode::Enter;
        case 0x35: return KeyCode::Escape;
        case 0x30: return KeyCode::Tab;
        case 0x33: return KeyCode::Backspace;
        case 0x75: return KeyCode::Delete;
        case 0x72: return KeyCode::Insert;
        case 0x73: return KeyCode::Home;
        case 0x77: return KeyCode::End;
        case 0x74: return KeyCode::PageUp;
        case 0x79: return KeyCode::PageDown;

        // ��ͷ��
        case 0x7B: return KeyCode::Left;
        case 0x7E: return KeyCode::Up;
        case 0x7C: return KeyCode::Right;
        case 0x7D: return KeyCode::Down;

        // С����
        case 0x52: return KeyCode::NumPad0; case 0x53: return KeyCode::NumPad1;
        case 0x54: return KeyCode::NumPad2; case 0x55: return KeyCode::NumPad3;
        case 0x56: return KeyCode::NumPad4; case 0x57: return KeyCode::NumPad5;
        case 0x58: return KeyCode::NumPad6; case 0x59: return KeyCode::NumPad7;
        case 0x5B: return KeyCode::NumPad8; case 0x5C: return KeyCode::NumPad9;

        // ������
        case 0x39: return KeyCode::CapsLock;
        case 0x47: return KeyCode::NumLock;
        case 0x6B: return KeyCode::ScrollLock;
        case 0x69: return KeyCode::PrintScreen;
        case 0x71: return KeyCode::Pause;

        default: return KeyCode::A; // Ĭ�Ϸ���A��
    }
}

// Cocoa��갴ťת��
MouseButton AppleWindowImpl::CocoaButtonToMouseButton(NSInteger buttonNumber) {
    switch (buttonNumber) {
        case 0: return MouseButton::Left;
        case 1: return MouseButton::Right;
        case 2: return MouseButton::Middle;
        case 3: return MouseButton::Button4;
        case 4: return MouseButton::Button5;
        default: return MouseButton::Left;
    }
}

// Cocoa���μ�ת��
ModifierKeys AppleWindowImpl::CocoaModifiersToModifierKeys(NSEventModifierFlags flags) {
    ModifierKeys modifiers;
    modifiers.shift = (flags & NSEventModifierFlagShift) != 0;
    modifiers.control = (flags & NSEventModifierFlagControl) != 0;
    modifiers.alt = (flags & NSEventModifierFlagOption) != 0;
    modifiers.super = (flags & NSEventModifierFlagCommand) != 0;
    return modifiers;
}

// �ַ���ת����������
NSString* AppleWindowImpl::StringToNSString(const std::string& str) {
    return [NSString stringWithUTF8String:str.c_str()];
}

std::string AppleWindowImpl::NSStringToString(NSString* nsStr) {
    if (!nsStr) return std::string();
    const char* cStr = [nsStr UTF8String];
    return cStr ? std::string(cStr) : std::string();
}

// ʵ��Macƽ̨�ض��Ĺ�������
std::unique_ptr<WindowImpl> CreateMacWindow(const std::string& title, uint32_t width, uint32_t height) {
    return std::make_unique<AppleWindowImpl>(title, width, height);
}

#endif // __APPLE__
