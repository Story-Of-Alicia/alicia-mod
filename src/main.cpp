#include "mod.hpp"

#include <format>
#include <thread>

#include <windows.h>

namespace alicia
{
  //!
  void RegisterGameEventListenerSignature(const void* listener, const void** event);

  //!
  const void* sMsgDoRenderPostOverlay = reinterpret_cast<void*>(0x018b60fc);
  //!
  const void* sMsgDoRendered = reinterpret_cast<void*>(0x018b6144);

  //!
  auto* RegisterGameEventListener = reinterpret_cast<
    decltype(RegisterGameEventListenerSignature)*>(0x00a3b4e0);

} // namespace alicia

namespace mod
{

const uint64_t module = reinterpret_cast<uint64_t>(
  GetModuleHandleW(nullptr));

constexpr std::size_t ConsoleHandleAddress = 0x01eb6ed0 - 0x400000;

class GameEventListener
{
  void* func0() = delete;
  void* func1() = delete;
  void* func2() = delete;
  void* HandleEvent(void* event)
  {
    OutputDebugStringW(L"Event!\n");
  }
} _listener{};

constexpr std::size_t WindowHandleAddress = 0x01870d08 - 0x400000;
constexpr std::size_t D3DDeviceAddress = 0x01874844 - 0x400000;

void main()
{
  HWND consoleWindow = *reinterpret_cast<HWND*>(module + ConsoleHandleAddress);
  OutputDebugStringW(L"Test");
  ShowWindow(consoleWindow, SW_SHOW);

  alicia::RegisterGameEventListener(&_listener, &alicia::sMsgDoRendered);
}

} // namespace mod

extern "C"
__declspec(dllexport)
BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,
  DWORD fdwReason,
  LPVOID lpReserved)
{
  mod::main();

  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
      mod::main();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}
