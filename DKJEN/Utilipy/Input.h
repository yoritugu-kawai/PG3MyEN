#pragma once
#include <Windows.h>
#include <wrl.h>
#include <vector>
#include <array>
#include <Xinput.h>
#include <cassert>

#pragma comment(lib,"xinput.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "../Base/WinApp.h"
#include"../Base/Log.h"

class Input
{
public:


	static Input* GetInstance();

	static void Initialize();

	static void BeginFlame();

	bool PushKey(uint8_t keyNum);

	bool PushKeyPressed(uint32_t keyNum);

	static void NoneJoyState(XINPUT_STATE& state);

	bool GetJoystickState(XINPUT_STATE& state);

private:

	static void CreateKeybordDevice();

	ComPtr<IDirectInput8> directInput = nullptr;
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	BYTE keys[256] = {};
	BYTE preKeys[256] = {};
	XINPUT_STATE state_{};

	bool isInitialize = false;



	//0x80=押している状態
	//0x00=押してない状態
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;
};

