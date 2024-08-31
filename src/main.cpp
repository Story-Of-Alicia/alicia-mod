#include "mod.hpp"

#include <format>
#include <thread>

#include <windows.h>

namespace mod
{

  // const uint64_t module = reinterpret_cast<uint64_t>(
  // GetModuleHandleW(nullptr));
  constexpr std::size_t address_of_function = 0x3d5b60;
  constexpr std::size_t ConsoleHandleAddress = 0x01eb6ed0 - 0x400000;

  constexpr std::size_t WindowHandleAddress = 0x01870d08 - 0x400000;
  constexpr std::size_t D3DDeviceAddress = 0x01874844 - 0x400000;

  void main()
  {
    ui::init();
    ui::loop();
    ui::term();

    // HWND consoleWindow = *reinterpret_cast<HWND*>(module + ConsoleHandleAddress);
    // ShowWindow(consoleWindow, SW_SHOW);
  }

}


extern "C"
__declspec(dllexport)
BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,
  DWORD fdwReason,
  LPVOID lpReserved)
{
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
