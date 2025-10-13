# 游戏引擎事件系统设计文档

## 概述

这是一个基于观察者模式和事件队列的事件管理系统，用于游戏引擎中的松耦合组件通信。

## 架构设计

### 核心组件

```
┌─────────────────────────────────────────────┐
│           EventManager (单例)                │
│  ┌────────────────┐  ┌──────────────────┐  │
│  │  Event Queue   │  │    Listeners     │  │
│  │  (待处理事件)   │  │  (事件监听器映射)  │  │
│  └────────────────┘  └──────────────────┘  │
└─────────────────────────────────────────────┘
            ▲                    ▲
            │                    │
    ┌───────┴────────┐   ┌──────┴────────┐
    │  PostEvent()   │   │  Subscribe()  │
    │ (发布事件)      │   │ (注册监听器)   │
    └────────────────┘   └───────────────┘
```

### 1. Event (基类)

事件的基础接口，所有具体事件都需要继承此类。

**关键方法：**

- `GetEventType()`: 获取事件类型
- `GetStaticType()`: 静态方法，用于类型匹配
- `SetHandled()`: 标记事件是否已处理

**设计要点：**

```cpp
class Event {
    EventType type_;
    bool handled_ = false;
};
```

### 2. EventListener (监听器基类)

监听器接口，定义事件处理的统一接口。

```cpp
class EventListener {
    virtual bool OnEvent(Event& event) = 0;
};
```

**作用：** 类型擦除，允许不同类型的监听器存储在同一个容器中。

### 3. FunctionEventListener (函数式监听器)

模板类，将具体事件类型的回调函数包装成监听器。

```cpp
template<typename T>
class FunctionEventListener : public EventListener {
    std::function<bool(T&)> function_;
    
    bool OnEvent(Event& event) override {
        if (event.GetEventType() == T::GetStaticType()) {
            return function_(static_cast<T&>(event));
        }
        return false;
    }
};
```

**工作流程：**

1. 检查事件类型是否匹配
2. 如果匹配，强制转换为具体类型并调用回调
3. 返回事件是否被处理

### 4. EventDispatcher (单事件分发器)

用于在事件处理函数中按类型分发单个事件。

```cpp
class EventDispatcher {
    Event& event_;
    
    template<typename T, typename F>
    bool Dispatch(const F& func) {
        if (event_.GetEventType() == T::GetStaticType()) {
            event_.SetHandled(func(static_cast<T&>(event_)));
            return true;
        }
        return false;
    }
};
```

**使用场景：**

```cpp
void OnEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
        // 处理按键事件
        return true;
    });
    dispatcher.Dispatch<MouseMovedEvent>([](MouseMovedEvent& e) {
        // 处理鼠标事件
        return true;
    });
}
```

### 5. EventManager (核心管理器)

事件系统的核心，负责事件的发布、分发和监听器管理。

**核心数据结构：**

```cpp
class EventManager {
    // 事件队列 - 延迟处理
    std::queue<std::unique_ptr<Event>> eventQueue_;
    
    // 监听器映射 - 按事件类型分类
    std::unordered_map<
        std::type_index,
        std::vector<std::shared_ptr<EventListener>>
    > listeners_;
    
    bool loggingEnabled_ = false;
};
```

**关键方法：**

#### 发布事件

```cpp
void PostEvent(std::unique_ptr<Event> event);
```

- 将事件加入队列
- 延迟处理，不会立即触发监听器

#### 立即分发

```cpp
void DispatchEvent(Event& event);
```

- 立即调用所有匹配的监听器
- 不加入队列

#### 处理队列

```cpp
void ProcessEvents();
```

- 处理队列中的所有事件
- 通常在每帧调用一次

#### 注册监听器

```cpp
template<typename EventType>
void Subscribe(std::function<bool(EventType&)> callback);

template<typename EventType, typename Func>
void On(Func&& func);  // 简化版本
```

## 工作流程

### 1. 发布-处理流程

```
[组件A] --PostEvent()--> [EventQueue] --ProcessEvents()--> [Listeners]
                                                               |
                         [组件B] <--OnEvent()------------------+
                         [组件C] <--OnEvent()------------------+
                         [组件D] <--OnEvent()------------------+
```

### 2. 时序图

```
发布者           EventManager          监听者A    监听者B
  |                   |                  |         |
  |--PostEvent()----->|                  |         |
  |                   |                  |         |
  |              [加入队列]               |         |
  |                   |                  |         |
[下一帧]               |                  |         |
  |                   |                  |         |
  |--ProcessEvents()->|                  |         |
  |                   |--OnEvent()------>|         |
  |                   |                  |         |
  |                   |--OnEvent()-------|-------->|
  |                   |                  |         |
```

### 3. 事件处理详细流程

```cpp
void EventManager::ProcessEvents() {
    while (!eventQueue_.empty()) {
        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        
        // 1. 记录日志
        if (loggingEnabled_) {
            LogEvent(*event);
        }
        
        // 2. 分发给监听器
        DispatchToListeners(*event);
    }
}

void EventManager::DispatchToListeners(Event& event) {
    auto type = std::type_index(typeid(*&event));
    
    // 查找该类型的所有监听器
    auto it = listeners_.find(type);
    if (it != listeners_.end()) {
        for (auto& listener : it->second) {
            if (listener->OnEvent(event)) {
                // 监听器返回true表示事件已处理
                if (event.IsHandled()) {
                    break;  // 停止传播
                }
            }
        }
    }
}
```

## 使用示例

### 1. 定义事件类型

```cpp
class KeyPressedEvent : public Event {
public:
    KeyPressedEvent(int keyCode, int repeatCount)
        : keyCode_(keyCode), repeatCount_(repeatCount) {}
    
    static EventType GetStaticType() { 
        return EventType::KeyPressed; 
    }
    
    EventType GetEventType() const override { 
        return GetStaticType(); 
    }
    
    int GetKeyCode() const { return keyCode_; }
    int GetRepeatCount() const { return repeatCount_; }
    
private:
    int keyCode_;
    int repeatCount_;
};
```

### 2. 注册监听器

#### 方式1：直接使用EventManager

```cpp
EventManager::Instance().Subscribe<KeyPressedEvent>(
    [](KeyPressedEvent& e) {
        std::cout << "Key pressed: " << e.GetKeyCode() << std::endl;
        return true;  // 返回true表示事件已处理
    }
);
```

#### 方式2：使用On简化语法

```cpp
EventManager::Instance().On<KeyPressedEvent>([](KeyPressedEvent& e) {
    if (e.GetKeyCode() == KEY_ESCAPE) {
        // 处理ESC键
        return true;
    }
    return false;  // 允许其他监听器继续处理
});
```

#### 方式3：继承EventHandler

```cpp
class GameLayer : public EventHandler {
public:
    void Init() {
        // 使用EVENT_BIND_FN宏绑定成员函数
        On<KeyPressedEvent>(EVENT_BIND_FN(OnKeyPressed));
        On<MouseMovedEvent>(EVENT_BIND_FN(OnMouseMoved));
    }
    
private:
    bool OnKeyPressed(KeyPressedEvent& e) {
        // 处理按键
        return false;
    }
    
    bool OnMouseMoved(MouseMovedEvent& e) {
        // 处理鼠标移动
        return false;
    }
};
```

### 3. 发布事件

#### 延迟处理（推荐）

```cpp
EventManager::Instance().PostEvent(
    std::make_unique<KeyPressedEvent>(KEY_SPACE, 0)
);
```

#### 立即处理

```cpp
KeyPressedEvent event(KEY_SPACE, 0);
EventManager::Instance().DispatchEvent(event);
```

### 4. 每帧处理事件

```cpp
void Engine::Update(float deltaTime) {
    // 处理本帧所有事件
    EventManager::Instance().ProcessEvents();
    
    // 更新游戏逻辑
    UpdateGameLogic(deltaTime);
    
    // 渲染
    Render();
}
```

### 5. 使用EventDispatcher处理多类型事件

```cpp
class Layer {
public:
    virtual void OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        
        dispatcher.Dispatch<KeyPressedEvent>(
            [this](KeyPressedEvent& e) {
                return OnKeyPressed(e);
            }
        );
        
        dispatcher.Dispatch<MouseMovedEvent>(
            [this](MouseMovedEvent& e) {
                return OnMouseMoved(e);
            }
        );
    }
    
private:
    bool OnKeyPressed(KeyPressedEvent& e) { /* ... */ }
    bool OnMouseMoved(MouseMovedEvent& e) { /* ... */ }
};
```

## 设计模式分析

### 1. 单例模式

```cpp
static EventManager& Instance() {
    static EventManager instance;
    return instance;
}
```

**优点：** 全局唯一实例，方便访问
 **注意：** 单例的生命周期管理需要小心

### 2. 观察者模式

```cpp
Subscribe<EventType>(callback);  // 注册观察者
PostEvent(event);                // 通知观察者
```

**优点：** 发布者和订阅者解耦

### 3. 命令模式

```cpp
std::queue<std::unique_ptr<Event>> eventQueue_;
```

**优点：** 事件可以排队、延迟执行

### 4. 类型擦除

```cpp
std::vector<std::shared_ptr<EventListener>> listeners_;
```

**作用：** 存储不同类型的监听器在同一容器

## 优势分析

### 1. 松耦合

- 发布者不需要知道订阅者
- 订阅者不需要知道发布者
- 组件之间通过事件通信

### 2. 多订阅者支持

```cpp
// 多个组件可以监听同一事件
EventManager::Instance().On<KeyEvent>(uiCallback);
EventManager::Instance().On<KeyEvent>(gameCallback);
EventManager::Instance().On<KeyEvent>(debugCallback);
```

### 3. 事件队列

- 避免在事件处理中修改数据结构导致的问题
- 控制事件处理时机
- 支持事件优先级（可扩展）

### 4. 类型安全

```cpp
template<typename EventType>
void Subscribe(std::function<bool(EventType&)> callback);
```

- 编译期类型检查
- 避免类型转换错误

### 5. 灵活的API

```cpp
// 三种注册方式
Subscribe<EventType>(callback);
On<EventType>(callback);
EventHandler::On<EventType>(callback);
```

