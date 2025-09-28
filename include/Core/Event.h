#pragma once
#include <cstdint>
#include <string>
#include <functional>

// 事件类型枚举
enum class EventType {
	None = 0,

	// 窗口事件
	WindowClose,
	WindowResize,
	WindowMove,
	WindowFocus,
	WindowLostFocus,
	WindowMinimize,
	WindowRestore,

	// 键盘事件
	KeyPressed,
	KeyReleased,
	KeyRepeated,
	CharInput,

	// 鼠标事件
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled,
	MouseEntered,
	MouseLeft,

	// 应用程序事件
	AppTick,
	AppUpdate,
	AppRender
};

// 事件类别标志 (可以组合)
enum class EventCategory : uint32_t {
	None = 0,
	Application = 1 << 0,
	Input = 1 << 1,
	Keyboard = 1 << 2,
	Mouse = 1 << 3,
	MouseButton = 1 << 4,
	Window = 1 << 5
};

// EventCategory位运算符重载
inline EventCategory operator|(EventCategory lhs, EventCategory rhs) {
	return static_cast<EventCategory>(
		static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
		);
}

inline EventCategory operator&(EventCategory lhs, EventCategory rhs) {
	return static_cast<EventCategory>(
		static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
		);
}

inline EventCategory operator^(EventCategory lhs, EventCategory rhs) {
	return static_cast<EventCategory>(
		static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs)
		);
}

inline EventCategory operator~(EventCategory rhs) {
	return static_cast<EventCategory>(
		~static_cast<uint32_t>(rhs)
		);
}

inline EventCategory& operator|=(EventCategory& lhs, EventCategory rhs) {
	lhs = lhs | rhs;
	return lhs;
}

inline EventCategory& operator&=(EventCategory& lhs, EventCategory rhs) {
	lhs = lhs & rhs;
	return lhs;
}

inline EventCategory& operator^=(EventCategory& lhs, EventCategory rhs) {
	lhs = lhs ^ rhs;
	return lhs;
}

// 按键代码定义 (基于Virtual Key Codes)
enum class KeyCode : uint16_t {
	// 字母键
	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	// 数字键
	D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,

	// 功能键
	F1 = 112, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	// 特殊键
	Space = 32,
	Enter = 13,
	Escape = 27,
	Tab = 9,
	Backspace = 8,
	Delete = 46,
	Insert = 45,
	Home = 36,
	End = 35,
	PageUp = 33,
	PageDown = 34,

	// 箭头键
	Left = 37,
	Up = 38,
	Right = 39,
	Down = 40,

	// 修饰键
	LeftShift = 160,
	RightShift = 161,
	LeftControl = 162,
	RightControl = 163,
	LeftAlt = 164,
	RightAlt = 165,

	// 小键盘
	NumPad0 = 96, NumPad1, NumPad2, NumPad3, NumPad4,
	NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,

	// 其他常用键
	CapsLock = 20,
	NumLock = 144,
	ScrollLock = 145,
	PrintScreen = 44,
	Pause = 19
};

// 鼠标按钮代码
enum class MouseButton : uint8_t {
	Left = 0,
	Right = 1,
	Middle = 2,
	Button4 = 3,
	Button5 = 4
};

// 修饰键状态
struct ModifierKeys {
	bool shift = false;
	bool control = false;
	bool alt = false;
	bool super = false;  // Windows键或Cmd键

	ModifierKeys() = default;
	ModifierKeys(bool s, bool c, bool a, bool sup = false)
		: shift(s), control(c), alt(a), super(sup) {}
};

// 基础事件类
class Event {
public:
	virtual ~Event() = default;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual uint32_t GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category) const {
		return GetCategoryFlags() & static_cast<uint32_t>(category);
	}

	bool IsHandled() const { return handled_; }
	void SetHandled(bool handled = true) { handled_ = handled; }

protected:
	bool handled_ = false;
};

// 事件宏定义 - 简化事件类的创建
#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual uint32_t GetCategoryFlags() const override { return static_cast<uint32_t>(category); }

// 窗口事件类
class WindowCloseEvent : public Event {
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory::Window)
};

class WindowResizeEvent : public Event {
public:
	WindowResizeEvent(uint32_t width, uint32_t height)
		: width_(width), height_(height) {}

	uint32_t GetWidth() const { return width_; }
	uint32_t GetHeight() const { return height_; }

	std::string ToString() const override {
		return "WindowResizeEvent: " + std::to_string(width_) + ", " + std::to_string(height_);
	}

	EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
	uint32_t width_, height_;
};

class WindowMoveEvent : public Event {
public:
	WindowMoveEvent(int x, int y) : x_(x), y_(y) {}

	int GetX() const { return x_; }
	int GetY() const { return y_; }

	std::string ToString() const override {
		return "WindowMoveEvent: " + std::to_string(x_) + ", " + std::to_string(y_);
	}

	EVENT_CLASS_TYPE(WindowMove)
	EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
	int x_, y_;
};

class WindowFocusEvent : public Event {
public:
	WindowFocusEvent() = default;

	EVENT_CLASS_TYPE(WindowFocus)
	EVENT_CLASS_CATEGORY(EventCategory::Window)
};

class WindowLostFocusEvent : public Event {
public:
	WindowLostFocusEvent() = default;

	EVENT_CLASS_TYPE(WindowLostFocus)
	EVENT_CLASS_CATEGORY(EventCategory::Window)
};

// 键盘事件基类
class KeyEvent : public Event {
public:
	KeyCode GetKeyCode() const { return keyCode_; }
	const ModifierKeys& GetModifiers() const { return modifiers_; }

	EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)

protected:
	KeyEvent(KeyCode keyCode, const ModifierKeys& modifiers)
		: keyCode_(keyCode), modifiers_(modifiers) {}

	KeyCode keyCode_;
	ModifierKeys modifiers_;
};

class KeyPressedEvent : public KeyEvent {
public:
	KeyPressedEvent(KeyCode keyCode, const ModifierKeys& modifiers, bool isRepeat = false)
		: KeyEvent(keyCode, modifiers), isRepeat_(isRepeat) {}

	bool IsRepeat() const { return isRepeat_; }

	std::string ToString() const override {
		return "KeyPressedEvent: " + std::to_string(static_cast<int>(keyCode_)) +
			(isRepeat_ ? " (repeat)" : "");
	}

	EVENT_CLASS_TYPE(KeyPressed)

private:
	bool isRepeat_;
};

class KeyReleasedEvent : public KeyEvent {
public:
	KeyReleasedEvent(KeyCode keyCode, const ModifierKeys& modifiers)
		: KeyEvent(keyCode, modifiers) {}

	std::string ToString() const override {
		return "KeyReleasedEvent: " + std::to_string(static_cast<int>(keyCode_));
	}

	EVENT_CLASS_TYPE(KeyReleased)
};

class CharInputEvent : public Event {
public:
	CharInputEvent(uint32_t character) : character_(character) {}

	uint32_t GetCharacter() const { return character_; }

	std::string ToString() const override {
		return "CharInputEvent: " + std::to_string(character_);
	}

	EVENT_CLASS_TYPE(CharInput)
	EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)

private:
	uint32_t character_;
};

// 鼠标事件基类
class MouseEvent : public Event {
public:
	EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
};

class MouseButtonEvent : public MouseEvent {
public:
	MouseButton GetMouseButton() const { return button_; }
	const ModifierKeys& GetModifiers() const { return modifiers_; }

	EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::MouseButton | EventCategory::Input)

protected:
	MouseButtonEvent(MouseButton button, const ModifierKeys& modifiers)
		: button_(button), modifiers_(modifiers) {}

	MouseButton button_;
	ModifierKeys modifiers_;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
	MouseButtonPressedEvent(MouseButton button, const ModifierKeys& modifiers)
		: MouseButtonEvent(button, modifiers) {}

	std::string ToString() const override {
		return "MouseButtonPressedEvent: " + std::to_string(static_cast<int>(button_));
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
	MouseButtonReleasedEvent(MouseButton button, const ModifierKeys& modifiers)
		: MouseButtonEvent(button, modifiers) {}

	std::string ToString() const override {
		return "MouseButtonReleasedEvent: " + std::to_string(static_cast<int>(button_));
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)
};

class MouseMovedEvent : public MouseEvent {
public:
	MouseMovedEvent(float x, float y) : x_(x), y_(y) {}

	float GetX() const { return x_; }
	float GetY() const { return y_; }

	std::string ToString() const override {
		return "MouseMovedEvent: " + std::to_string(x_) + ", " + std::to_string(y_);
	}

	EVENT_CLASS_TYPE(MouseMoved)

private:
	float x_, y_;
};

class MouseScrolledEvent : public MouseEvent {
public:
	MouseScrolledEvent(float xOffset, float yOffset)
		: xOffset_(xOffset), yOffset_(yOffset) {}

	float GetXOffset() const { return xOffset_; }
	float GetYOffset() const { return yOffset_; }

	std::string ToString() const override {
		return "MouseScrolledEvent: " + std::to_string(xOffset_) + ", " + std::to_string(yOffset_);
	}

	EVENT_CLASS_TYPE(MouseScrolled)

private:
	float xOffset_, yOffset_;
};

// 事件回调函数类型
using EventCallbackFn = std::function<void(Event&)>;
