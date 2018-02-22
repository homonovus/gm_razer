#include <vector>
#include <map>
#include <string>
#include <Guiddef.h>
#include <windows.h>

#include "GarrysMod/Lua/Interface.h"

#include "Razer/RzErrors.h"
#include "Razer/RzChromaSDKTypes.h"
#include "Razer/RzChromaSDKDefines.h"

using namespace GarrysMod::Lua;

using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Keypad;
using namespace ChromaSDK::Mouse;
using namespace ChromaSDK::Mousepad;
using namespace ChromaSDK::Headset;

typedef RZRESULT (*INIT)(void);
typedef RZRESULT (*UNINIT)(void);
typedef RZRESULT (*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEHEADSETEFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEPADEFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYPADEFFECT)(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATECHROMALINKEFFECT)(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*SETEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*DELETEEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT (*UNREGISTEREVENTNOTIFICATION)(void);
typedef RZRESULT (*QUERYDEVICE)(RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);

INIT Init = NULL;
UNINIT UnInit = NULL;
CREATEEFFECT CreateEffect = NULL;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = NULL;
CREATEMOUSEEFFECT CreateMouseEffect = NULL;
CREATEHEADSETEFFECT CreateHeadsetEffect = NULL;
CREATEMOUSEPADEFFECT CreateMousematEffect = NULL;
CREATEKEYPADEFFECT CreateKeypadEffect = NULL;
CREATECHROMALINKEFFECT CreateChromaLinkEffect = NULL;
SETEFFECT SetEffect = NULL;
DELETEEFFECT DeleteEffect = NULL;

int rgbtohex( int r, int g, int b ) {
	return ((r & 0xff) << 16) + ((g & 0xff) << 8) + ((b & 0xff));
}

// todo: actually make a map/vector of effects
//std::vector <RZEFFECTID, ChromaSDK::EFFECT_TYPE> effectmap;

HMODULE razer;

ChromaSDK::Keyboard::CUSTOM_KEY_EFFECT_TYPE m_CustomKeyKeyboardEffect;
ChromaSDK::Keyboard::STATIC_EFFECT_TYPE m_StaticKeyboardEffect;

LUA_FUNCTION(createCustomKeyboardEffect) {
	if (CreateKeyboardEffect == NULL) return 0;

	LUA->CheckNumber(1);
	int type = (int)LUA->GetNumber(1);
	RZRESULT ret;

	if (type == ChromaSDK::Keyboard::CHROMA_CUSTOM || type == ChromaSDK::Keyboard::CHROMA_CUSTOM_KEY) {
		m_CustomKeyKeyboardEffect = {};
		ret = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &m_CustomKeyKeyboardEffect, nullptr);
	}
	else if (type == ChromaSDK::Keyboard::CHROMA_STATIC) {
		m_StaticKeyboardEffect = {};
		ret = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_STATIC, &m_StaticKeyboardEffect, nullptr);
	}

	LUA->PushNumber(ret);
	return 1;
}

LUA_FUNCTION(fillKeys) {
	if (CreateKeyboardEffect == NULL) return 0;

	LUA->CheckType(1, Type::TABLE);

	LUA->Push(1);
		LUA->GetField(-1, "b");
		int r = (int)LUA->GetNumber(-1);
		LUA->GetField(-2, "g");
		int g = (int)LUA->GetNumber(-1);
		LUA->GetField(-3, "r");
		int b = (int)LUA->GetNumber(-1);
	LUA->Pop(-4);

	int col = rgbtohex(r, g, b);

	for (int x = 1; x < ChromaSDK::Keyboard::MAX_COLUMN-1; x++) {
		for (int y = 1; y < ChromaSDK::Keyboard::MAX_ROW; y++) {
			// m_currentEffect[x][y] = col
		}
	}
}

// C Function to clear effects on all devices.
// No arguments.
LUA_FUNCTION(clear) {

	return 0;
}

// C Function to highlight multiple keys at once.
// Takes two arguments;
// Table of razer.keys.KEY_ enums to highlight
// Optional GLua Color()
LUA_FUNCTION(lightkeys) {
	if (CreateKeyboardEffect == NULL) return 0;

	LUA->CheckType(1, Type::TABLE);

	int col = NULL;
	if (LUA->IsType(2, Type::TABLE)) {
		LUA->Push(2);
			LUA->GetField(-1, "b");
			int r = (int)LUA->GetNumber(-1);
			LUA->GetField(-2, "g");
			int g = (int)LUA->GetNumber(-1);
			LUA->GetField(-3, "r");
			int b = (int)LUA->GetNumber(-1);
		LUA->Pop(-4);

		col = rgbtohex(r, g, b);
	}

	ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE CustomEffect = {};
	
	LUA->Push(1);
	LUA->PushNil();
	int lastkey = NULL;

	while (LUA->Next(-2) != 0) {
		if (LUA->IsType(-1, Type::TABLE)) {
			LUA->Push(-1);
			if (lastkey != NULL)
				LUA->PushNumber(lastkey);
			else
				LUA->PushNil();

			LUA->Next(-2);

			int x = (int)LUA->GetNumber();
			lastkey = (int)LUA->GetNumber(-2);

			LUA->Pop();

			//LUA->Push(-1);
			//	LUA->PushNumber(lastkey);
			//	LUA->Next(-2);
			//
			//	int y = (int)LUA->GetNumber();
			//LUA->Pop();

			LUA->PushSpecial(SPECIAL_GLOB);
				LUA->GetField(-1, "print");
				LUA->PushNumber(x);
				LUA->PushNumber(lastkey);
				LUA->Call(2, 0);
			LUA->Pop();
		}

		LUA->Pop();
	}

	//CustomEffect.Color[2][3] = col != NULL ? col : 0xb00b1e;
	//CustomEffect.Color[3][3] = col != NULL ? col : 0xb00b1e;
	//CustomEffect.Color[3][2] = col != NULL ? col : 0xb00b1e;
	//CustomEffect.Color[3][4] = col != NULL ? col : 0xb00b1e;

	RZRESULT ret = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &CustomEffect, nullptr);

	LUA->PushNumber(ret);

	return 1;
}

// C Function to highlight a single key on a Razer device.
// Takes three arguments;
// x position on keyboard to highlight
// y position on keyboard to highlight
// Optional GLua Color()
LUA_FUNCTION (lightkey) {
	if (CreateKeyboardEffect == NULL) return 0;
	
	if (LUA->CheckNumber(1) && LUA->CheckNumber(2)) {
		int x = (int)LUA->GetNumber(1);
		int y = (int)LUA->GetNumber(2);

		int col = NULL;

		if (LUA->IsType(3, Type::TABLE)) {
			LUA->Push(3);
				LUA->GetField(-1, "b");
				int r = (int)LUA->GetNumber(-1);
				LUA->GetField(-2, "g");
				int g = (int)LUA->GetNumber(-1);
				LUA->GetField(-3, "r");
				int b = (int)LUA->GetNumber(-1);
			LUA->Pop(-4);

			col = rgbtohex(r, g, b);
		}

		if (x >= ChromaSDK::Keyboard::MAX_COLUMN || x == HUGE_VAL) x = ChromaSDK::Keyboard::MAX_COLUMN - 1;
		if (y >= ChromaSDK::Keyboard::MAX_ROW || y == HUGE_VAL) y = ChromaSDK::Keyboard::MAX_ROW - 1;

		ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE CustomEffect = {};
		CustomEffect.Color[y][x] = col != NULL ? col : 0xffffff;
		
		RZRESULT ret = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &CustomEffect, nullptr);

		LUA->PushNumber(ret);
		return 1;
	}
	return 0;
}

GMOD_MODULE_OPEN() {
	razer = LoadLibrary("RzChromaSDK.dll");
	Init = (INIT)::GetProcAddress(razer, "Init");

	if (Init != NULL) {
		RZRESULT rzResult = Init();
		if (rzResult == RZRESULT_SUCCESS) {
			UnInit = (UNINIT)::GetProcAddress(razer, "UnInit");
			CreateEffect = (CREATEEFFECT)::GetProcAddress(razer, "CreateEffect");
			CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)::GetProcAddress(razer, "CreateKeyboardEffect");
			CreateMouseEffect = (CREATEMOUSEEFFECT)::GetProcAddress(razer, "CreateMouseEffect");
			CreateHeadsetEffect = (CREATEHEADSETEFFECT)::GetProcAddress(razer, "CreateHeadsetEffect");
			CreateMousematEffect = (CREATEMOUSEPADEFFECT)::GetProcAddress(razer, "CreateMousepadEffect");
			CreateKeypadEffect = (CREATEKEYPADEFFECT)::GetProcAddress(razer, "CreateKeypadEffect");
			CreateChromaLinkEffect = (CREATECHROMALINKEFFECT)::GetProcAddress(razer, "CreateChromaLinkEffect");
			SetEffect = (SETEFFECT)GetProcAddress(razer, "SetEffect");
			DeleteEffect = (DELETEEFFECT)GetProcAddress(razer, "DeleteEffect");
		} else {
			LUA->PushSpecial(SPECIAL_GLOB);
				LUA->GetField(-1, "MsgC");
				LUA->PushString("[RAZER] ");
				LUA->Call(1, 0);

				LUA->GetField(-1, "print");
				LUA->PushString("Razer Chroma SDK not found, exiting.");
				LUA->Call(1, 0);
			LUA->Pop();

			return 1;
		}
	}

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();

			LUA->PushCFunction(lightkey);
			LUA->SetField(-2, "lightKey");

			LUA->PushCFunction(createCustomKeyboardEffect);
			LUA->SetField(-2, "createCustomKeyboardEffect");

			//LUA->PushCFunction(lightkeys);
			//LUA->SetField(-2, "lightkeys");

			LUA->CreateTable();

				LUA->CreateTable();
					LUA->PushNumber(1);
					LUA->SetField(-2, "num");
				LUA->SetField(-2, "mouse");

				LUA->CreateTable();
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_NONE);
					LUA->SetField(-2, "CHROMA_NONE");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_BREATHING);
					LUA->SetField(-2, "CHROMA_BREATHING");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_CUSTOM);
					LUA->SetField(-2, "CHROMA_CUSTOM");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_REACTIVE);
					LUA->SetField(-2, "CHROMA_REACTIVE");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_STATIC);
					LUA->SetField(-2, "CHROMA_STATIC");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_SPECTRUMCYCLING);
					LUA->SetField(-2, "CHROMA_SPECTRUMCYCLING");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_WAVE);
					LUA->SetField(-2, "CHROMA_WAVE");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_RESERVED);
					LUA->SetField(-2, "CHROMA_RESERVED");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_CUSTOM_KEY);
					LUA->SetField(-2, "CHROMA_CUSTOM_KEY");
					LUA->PushNumber(ChromaSDK::Keyboard::CHROMA_INVALID);
					LUA->SetField(-2, "CHROMA_INVALID");
				LUA->SetField(-2, "keyboard");
			LUA->SetField(-2, "effects");

			LUA->CreateTable();
				LUA->PushNumber(RZRESULT_INVALID);
				LUA->SetField(-2, "RZRESULT_INVALID");
				LUA->PushNumber(RZRESULT_SUCCESS);
				LUA->SetField(-2, "RZRESULT_SUCCESS");
				LUA->PushNumber(RZRESULT_ACCESS_DENIED);
				LUA->SetField(-2, "RZRESULT_ACCESS_DENIED");
				LUA->PushNumber(RZRESULT_INVALID_HANDLE);
				LUA->SetField(-2, "RZRESULT_INVALID_HANDLE");
				LUA->PushNumber(RZRESULT_NOT_SUPPORTED);
				LUA->SetField(-2, "RZRESULT_NOT_SUPPORTED");
				LUA->PushNumber(RZRESULT_INVALID_PARAMETER);
				LUA->SetField(-2, "RZRESULT_INVALID_PARAMETER");
				LUA->PushNumber(RZRESULT_SERVICE_NOT_ACTIVE);
				LUA->SetField(-2, "RZRESULT_SERVICE_NOT_ACTIVE");
				LUA->PushNumber(RZRESULT_SINGLE_INSTANCE_APP);
				LUA->SetField(-2, "RZRESULT_SINGLE_INSTANCE_APP");
				LUA->PushNumber(RZRESULT_DEVICE_NOT_CONNECTED);
				LUA->SetField(-2, "RZRESULT_DEVICE_NOT_CONNECTED");
				LUA->PushNumber(RZRESULT_NOT_FOUND);
				LUA->SetField(-2, "RZRESULT_NOT_FOUND");
				LUA->PushNumber(RZRESULT_REQUEST_ABORTED);
				LUA->SetField(-2, "RZRESULT_REQUEST_ABORTED");
				LUA->PushNumber(RZRESULT_ALREADY_INITIALIZED);
				LUA->SetField(-2, "RZRESULT_ALREADY_INITIALIZED");
				LUA->PushNumber(RZRESULT_RESOURCE_DISABLED);
				LUA->SetField(-2, "RZRESULT_RESOURCE_DISABLED");
				LUA->PushNumber(RZRESULT_DEVICE_NOT_AVAILABLE);
				LUA->SetField(-2, "RZRESULT_DEVICE_NOT_AVAILABLE");
				LUA->PushNumber(RZRESULT_NOT_VALID_STATE);
				LUA->SetField(-2, "RZRESULT_NOT_VALID_STATE");
				LUA->PushNumber(RZRESULT_NO_MORE_ITEMS);
				LUA->SetField(-2, "RZRESULT_NO_MORE_ITEMS");
				LUA->PushNumber(RZRESULT_FAILED);
				LUA->SetField(-2, "RZRESULT_FAILED");
			LUA->SetField(-2, "result");

		LUA->SetField(-2,"razer");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	if (UnInit != NULL) {
		UnInit();
	
		FreeLibrary(razer);
		razer = NULL;
	}
	return 0;
}
