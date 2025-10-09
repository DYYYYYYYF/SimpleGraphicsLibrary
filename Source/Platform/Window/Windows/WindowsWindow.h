#pragma once
#ifdef _WIN32

#include "Window/WindowImpl.h"
#include "Core/Event.h"     

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

class WindowsWindowImpl : public WindowImpl {
public:
	WindowsWindowImpl(const std::string& title, uint32_t width, uint32_t height);
	~WindowsWindowImpl() override;

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

private:
	// Windows API���
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool RegisterWindowClass();
	void UnregisterWindowClass();
	std::wstring StringToWString(const std::string& str);
	std::string WStringToString(const std::wstring& wstr);

	// �¼�ת����������
	KeyCode VirtualKeyToKeyCode(WPARAM vkCode);
	MouseButton VirtualButtonToMouseButton(UINT message, WPARAM wParam);
	ModifierKeys GetCurrentModifiers();
	void DispatchEvent(Event& event);
	std::unique_ptr<Event> CreateEventCopy(const Event& event);  // �����¼�����

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

	// Windows�ض���Ա
	HWND hwnd_;
	HDC hdc_;
	HINSTANCE hInstance_;

	// �¼�ϵͳ
	EventCallbackFn eventCallback_;
	bool useGlobalEventQueue_;
	bool useDirectCallback_;

	// ��̬��Ա
	static const wchar_t* WINDOW_CLASS_NAME;
	static bool classRegistered_;
	static std::unordered_map<HWND, WindowsWindowImpl*> windowMap_;
};

#endif