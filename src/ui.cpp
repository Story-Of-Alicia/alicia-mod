//
// Created by rgnter on 31/08/2024.
//

#include "mod.hpp"

#include <cstdint>
#include <format>

#include <windows.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>
#include <d3d9.h>

namespace ui
{

namespace
{

//! A window handle.
HWND windowHandle = nullptr;
uint32_t resizeWidth = 0;
uint32_t resizeHeight = 0;

//! A flag indicating whether the UI should loop.
bool shouldLoop = true;

//! A Direct3D9 context.
struct D3D9Ctx
{
  IDirect3D9* _d3d { nullptr };
  IDirect3DDevice9* _d3dDevice { nullptr };
  D3DPRESENT_PARAMETERS presentParameters{};
} d3dCtx;

//! Initializes the Direct3D9 context.
bool initD3D9(D3D9Ctx& ctx)
{
  ctx._d3d = Direct3DCreate9(D3D_SDK_VERSION);
  if (ctx._d3d == nullptr)
  {
    OutputDebugStringA("Failed to create Direct3D");
    return false;
  }

  ZeroMemory(&ctx.presentParameters, sizeof(ctx.presentParameters));
  ctx.presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
  ctx.presentParameters.BackBufferCount = 1;
  ctx.presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
  ctx.presentParameters.hDeviceWindow = windowHandle;
  ctx.presentParameters.Windowed = true;
  ctx.presentParameters.EnableAutoDepthStencil = true;
  ctx.presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
  ctx.presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  HRESULT result = ctx._d3d->CreateDevice(
    D3DADAPTER_DEFAULT,
    D3DDEVTYPE_HAL,
    windowHandle,
    D3DCREATE_HARDWARE_VERTEXPROCESSING,
    &ctx.presentParameters,
    &ctx._d3dDevice);
  if (result != D3D_OK)
  {
    OutputDebugStringA(
      std::format(
        "Failed to create Direct3D device: HRESULT 0x{:x}",
        static_cast<uint64_t>(result)).c_str());
    return false;
  }

  return true;
}

//! Terminates the Direct3D9 context.
void termD3D9(D3D9Ctx ctx)
{
  if (ctx._d3dDevice)
  {
    ctx._d3dDevice->Release();
    ctx._d3dDevice = nullptr;
  }

  if (ctx._d3d)
  {
    ctx._d3d->Release();
    ctx._d3d = nullptr;
  }
}


//! Windows process function.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED)
      return 0;
    resizeWidth = (UINT)LOWORD(lParam); // Queue resize
    resizeHeight = (UINT)HIWORD(lParam);
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // anonymous namespace

void init()
{
  // Register the window class.
  const WNDCLASSEXW windowClass {
    .cbSize = sizeof(windowClass),
    .style = CS_CLASSDC,
    .lpfnWndProc = WndProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = GetModuleHandle(nullptr),
    .hIcon = nullptr,
    .hCursor = nullptr,
    .hbrBackground = nullptr,
    .lpszMenuName = nullptr,
    .lpszClassName = L"Alicia Mod",
    .hIconSm = nullptr
  };
  if (RegisterClassExW(&windowClass) == 0)
  {
    OutputDebugStringA("Failed to register the window class.");
    OutputDebugStringA(
      std::format("Last win32 error: 0x{:x}", GetLastError()).c_str());
    return;
  }

  // Create the Window.
  windowHandle = CreateWindowW(
    windowClass.lpszClassName,
    L"Alicia Mod",
    WS_OVERLAPPEDWINDOW,
    100, 100, 1280, 800,
    nullptr,
    nullptr,
    windowClass.hInstance,
    nullptr);
  if (windowHandle == nullptr)
  {
    OutputDebugStringA("Failed to create the window.");
    OutputDebugStringA(
      std::format("Last win32 error: 0x{:x}", GetLastError()).c_str());
    return;
  }

  // Initialize the D3D context.
  // if (!initD3D9(d3dCtx))
  // {
  //   OutputDebugStringA("Couldn't initialize D3D9 context.");
  //   return;
  // }

  constexpr std::size_t D3DDeviceAddress = 0x01874844 - 0x400000;
  d3dCtx._d3dDevice = *reinterpret_cast<IDirect3DDevice9**>(GetModuleHandle(nullptr) + D3DDeviceAddress);

  // Show the Window.
  ShowWindow(windowHandle, SW_SHOW);
  UpdateWindow(windowHandle);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(windowHandle);
  ImGui_ImplDX9_Init(d3dCtx._d3dDevice);

  OutputDebugStringA("Initialized the user-interface.");
}

void term()
{
  if (windowHandle != nullptr)
  {
    DestroyWindow(windowHandle);
  }

  OutputDebugStringA("Terminated the user-interface.");
}

void loop()
{
  while (shouldLoop)
  {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT)
        shouldLoop = false;

      ImGui_ImplDX9_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();

      {
        constexpr std::size_t ConsoleHandleAddress = 0x01eb6ed0;

        ImGui::Begin("Alicia Mod");
        static bool enableEngineConsole = false;
        if (ImGui::Checkbox("Enable engine console", &enableEngineConsole))
        {
          HWND consoleWindow = *reinterpret_cast<HWND*>(ConsoleHandleAddress);
          ShowWindow(consoleWindow, enableEngineConsole ? SW_SHOW : SW_HIDE);
        }

        ImGui::End();
      }

      ImGui::EndFrame();
      d3dCtx._d3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
      d3dCtx._d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      d3dCtx._d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
      D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(0, 0, 0, 0);
      d3dCtx._d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
      if (d3dCtx._d3dDevice->BeginScene() >= 0)
      {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        d3dCtx._d3dDevice->EndScene();
      }
      d3dCtx._d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    }
  }
}

} // namespace uiD