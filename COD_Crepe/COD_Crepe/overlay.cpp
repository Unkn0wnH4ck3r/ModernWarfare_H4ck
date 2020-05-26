#include <stdio.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <Windows.h>
#include <ShlObj.h>
#include <string>

#include "engine.h"
#include "utils.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

#include "d3d12downlevel.h"
#include "obfuscation.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#define LOG(f_, ...) printf((f_), __VA_ARGS__); printf("\n");

static bool							s_initialized = false;
static IDXGISwapChain*				s_swapChain = nullptr;
static IDXGISwapChain3*				s_swapChain3 = nullptr;
static ID3D12Device*				s_dx12Device = nullptr;

static ID3D12DescriptorHeap*		g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap*		g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue*			g_pd3dCommandQueue = nullptr;
static ID3D12CommandQueueDownlevel* g_pd3dCommandQueueDownlevel = nullptr;
static ID3D12GraphicsCommandList*	g_pd3dCommandList = nullptr;
static ID3D12Fence*					g_fence = nullptr;
static HANDLE                       g_fenceEvent = nullptr;
static UINT64                       g_fenceLastSignaledValue = 1;
static D3D12_CPU_DESCRIPTOR_HANDLE* g_mainRenderTargetDescriptor = nullptr;

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

static FrameContext*				g_frameContexts = nullptr;
static ID3D12Resource**				g_backBuffers = nullptr;
static UINT							g_backBufferCount = 0;

WNDPROC								g_origWndProc = 0;

DWORD D3D12_SWAP_CHAIN_OFFSET = 0x1201D618;
DWORD D3D12_CommandQueue_OFFSET = 0x120182A8;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return TRUE;

	return CallWindowProc(g_origWndProc, hWnd, msg, wParam, lParam);
}

void StyleColorsCustom()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.99f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.98f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 0.75f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.08f, 0.12f, 0.6f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	colors[ImGuiCol_Tab] = ImVec4(0.95f, 0.08f, 0.12f, 0.5f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.95f, 0.08f, 0.12f, 0.8f);
	colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}
static void initializeDx12()
{
	// This is a bit of a question mark, might be a different offset for different versions.
	// Could possibly log the vtable pointer for ID3D12CommandQueue in mainThread() and then simply check for valid, readable pointers in a sensible range of the swapchain.
	// Idea #3: Hook CommandList::ResourceBarrier, check which command list creates transitions for any of the backbuffers, then also hook CommandQueue::ExecuteCommandLists
	// and find out which command queue is used to exectue that command list.
	//size_t* pOffset = (size_t*)((__int64)s_swapChain + 0x160);
	//*(&g_pd3dCommandQueue) = reinterpret_cast<ID3D12CommandQueue*>(*pOffset);
	//printf("CommandQueue from SwapChain pointer at 0x%I64X\n", reinterpret_cast<__int64>(g_pd3dCommandQueue));

	s_swapChain->GetDevice(IID_PPV_ARGS(&s_dx12Device));
	if (!s_dx12Device) {
		//LOG("Could not get ID3D12Device from IDXGISwapChain");
		return;
	}

	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		s_swapChain->GetDesc(&swapChainDesc);

		//LOG("DXGI_SWAP_CHAIN_DESC BufferCount %d", swapChainDesc.BufferCount);
		g_backBufferCount = swapChainDesc.BufferCount;
		g_backBuffers = new ID3D12Resource *[g_backBufferCount];

		for (UINT i = 0; i < g_backBufferCount; ++i) {
			s_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_backBuffers[i]));
			//LOG("\tBackbuffer %d - 0x%I64X", i, reinterpret_cast<__int64>(g_backBuffers[i]));
		}
	}

	// Create descriptor heaps
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.NumDescriptors = g_backBufferCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NodeMask = 1;

		if (FAILED(s_dx12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)))) {
			//LOG("Failed to create DescriptorHeap for backbuffers");
			return;
		}

		g_mainRenderTargetDescriptor = new D3D12_CPU_DESCRIPTOR_HANDLE[g_backBufferCount];
		SIZE_T rtvDescriptorSize = s_dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < g_backBufferCount; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(s_dx12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)))) {
			//LOG("Failed to create SRV DescriptorHeap");
			return;
		}
	}

	// Query SwapChain3 which can be used to get the current backbuffer index
	{
		s_swapChain->QueryInterface(IID_PPV_ARGS(&s_swapChain3));
		if (s_swapChain3) {
			UINT currentIndex = s_swapChain3->GetCurrentBackBufferIndex();
			//LOG("Current backbuffer index: %d", currentIndex);
		}
		else {
			//LOG("Could not query SwapChain to SwapChain3");
			return;
		}
	}

	// Create frame contexts
	{
		g_frameContexts = new FrameContext[g_backBufferCount];
		for (UINT i = 0; i < g_backBufferCount; ++i) {
			if (FAILED(s_dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContexts[i].CommandAllocator)))) {
				//LOG("Failed to create CommandAllocator for FrameContext %d", i);
				return;
			}
		}
	}

	// Create command list
	{
		if (s_dx12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContexts[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
			g_pd3dCommandList->Close() != S_OK) {
			//LOG("Failed to create CommandList");
			return;
		}
	}

	// Create fence and fence event
	{
		if (s_dx12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK) {
			//LOG("Failed to create Fence");
			return;
		}

		g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (g_fenceEvent == NULL) {
			//LOG("Failed to create FenceEvent");
			return;
		}
	}

	// Create render target views
	{
		for (UINT i = 0; i < g_backBufferCount; ++i) {
			s_dx12Device->CreateRenderTargetView(g_backBuffers[i], NULL, g_mainRenderTargetDescriptor[i]);
		}
	}

	// Setup ImGui
	{
		HWND hwnd = FindWindowA("IW8", NULL);
		if (hwnd == NULL)
			return;

		//LOG("Game window handle 0x%X", reinterpret_cast<int>(hwnd));

		// Subclass the window with a new WndProc to catch messages
		g_origWndProc = (WNDPROC)SetWindowLongPtr(hwnd, -4, (LONG_PTR)&WndProc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		char szPath[MAX_PATH];
		SHGetSpecialFolderPathA(NULL, szPath, CSIDL_FONTS, FALSE);
		std::string FontName = szPath;
		FontName += XorString("\\msyh.ttc");

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		hFind = FindFirstFile(FontName.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			FontName = szPath;
			FontName += XorString("\\msyh.ttf");
			FindClose(hFind);
			hFind = FindFirstFile(FontName.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) {
				GetCurrentDirectoryA(MAX_PATH, szPath);
				FontName = szPath;
				FontName += XorString("\\msyh.ttf");
				FindClose(hFind);
				hFind = FindFirstFile(FontName.c_str(), &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) {
					MessageBoxA(NULL, XorString("字体错误!"), XorString("提示"), MB_OK);
					ImGui_ImplDX12_Shutdown();
					ImGui::DestroyContext();
				}
				FindClose(hFind);
			}
			FindClose(hFind);
		}
		FindClose(hFind);

		io.Fonts->AddFontFromFileTTF(((FontName.c_str())), 15.f, 0, io.Fonts->GetGlyphRangesChineseFull());

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		StyleColorsCustom();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(s_dx12Device, g_backBufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
			g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	s_initialized = true;
}

void imDrawTextA(int x, int y, int a, int r, int g, int b, const char *fmt, ...)
{
	char str[128];
	va_list va_alist;
	va_start(va_alist, fmt);
	_vsnprintf_s(str, sizeof(str), sizeof(str) - strlen(str), fmt, va_alist);
	va_end(va_alist);

	ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(15.0f, FLT_MAX, 0.0f, str);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - textSize.x / 2.0f + 1.0f, y + 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), str);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - textSize.x / 2.0f, y), ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), str);
}
void imDrawRect(int x, int y, int width, int height, int nL, int a, int r, int g, int b)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), 0, 0, nL);
}
void imDrawRectFilled(int x, int y, int width, int height, int a, int r, int g, int b)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), 0, 0);
}
void imDrawLine(int x1, int y1, int x2, int y2, int a, int r, int g, int b, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), thickness);
}
void imDrawCircle(int x, int y, float radius, int a, int r, int g, int b, int segments, int thickness)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), segments, thickness);
}
void imDrawCircleFilled (int x, int y, float radius, int a, int r, int g, int b, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0)), segments);
}

static void drawImgui()
{
	UINT backBufferIndex = 0;
	backBufferIndex = s_swapChain3->GetCurrentBackBufferIndex();

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	DrawEsp();
	ImGui::SetNextWindowSize(ImVec2(260, 290), ImGuiCond_Always);
	{
		ImGui::Begin(XorString("ModernWarfare"), NULL, ImGuiWindowFlags_NoResize);
		if (ImGui::BeginTabBar(XorString("TabBar"))) {
			if (ImGui::BeginTabItem("Basic")) {
				ImGui::Checkbox(u8"人物方框", &bDrawBox);
				ImGui::SameLine(150);
				ImGui::Checkbox(u8"人物名称", &bDrawName);
				ImGui::Checkbox(u8"人物距离", &bDrawDistance);
				ImGui::SameLine(150);
				ImGui::Checkbox(u8"人物骨骼", &bDrawSkeleton);
				ImGui::Checkbox(u8"人物雷达", &bPlayerRadar);
				ImGui::SameLine(150);
				ImGui::Checkbox(u8"人物准线", &bDrawAimLine);
				ImGui::SliderInt(u8"透视距离", &fMaxDrawDistance, 200, 1000,"%d[M]");
				ImGui::Separator();
				ImGui::Checkbox(u8"显示载具", &bDrawVehicle);
				ImGui::SameLine(150);
				ImGui::Checkbox(u8"显示空投", &bDrawAirdrop);
				ImGui::Checkbox(u8"自瞄范围", &bDrawAimsize);
				ImGui::SameLine(150);
				ImGui::Checkbox(u8"屏幕准星", &bCrosshair);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Aimbot")) {
				ImGui::Checkbox(u8"自动瞄准", &bAimbot);
				const char* mouseitems[] = { u8"鼠标左键", u8"鼠标中键", u8"鼠标右键"};
				ImGui::Combo(u8"鼠标热键", &iAimKey, mouseitems, IM_ARRAYSIZE(mouseitems));
				const char* aimpositionitems[] = { u8"头部", u8"颈部", u8"胸部" };
				ImGui::Combo(u8"瞄准位置", &iAimPos, aimpositionitems, IM_ARRAYSIZE(aimpositionitems));
				ImGui::SliderInt(u8"平滑速度", &fAimSpeed, 5, 100, "%d[Uint]");
				ImGui::SliderInt(u8"瞄准范围", &fMaxAimDistance, 50, 400, "%d[Uint]");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		//ImGui::Text(XorString("Application average %.3f ms/frame (%.1f FPS)"), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	if (bCrosshair) {
		imDrawLine((int)ImGui::GetIO().DisplaySize.x / 2 - 10, (int)ImGui::GetIO().DisplaySize.y / 2, (int)ImGui::GetIO().DisplaySize.x / 2 + 10, (int)ImGui::GetIO().DisplaySize.y / 2, 255, 0, 255, 0, 2.f);
		imDrawLine((int)ImGui::GetIO().DisplaySize.x / 2, (int)ImGui::GetIO().DisplaySize.y / 2 - 10, (int)ImGui::GetIO().DisplaySize.x / 2, (int)ImGui::GetIO().DisplaySize.y / 2 + 10, 255, 0, 255, 0, 2.f);
	}
	if (bDrawAimsize) {
		imDrawCircle((int)ImGui::GetIO().DisplaySize.x / 2, (int)ImGui::GetIO().DisplaySize.y / 2,fMaxAimDistance, 255, 255, 255, 255, 100, 1);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	renderTargetViewHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = s_dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (backBufferIndex == 1)
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;

	// Rendering
	FrameContext* frameCtxt = &g_frameContexts[0];
	frameCtxt->CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = g_backBuffers[backBufferIndex];

	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	g_pd3dCommandList->Reset(frameCtxt->CommandAllocator, NULL);
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, NULL);
	g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->Close();
	g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList * const*)& g_pd3dCommandList);

}

static void waitForFence()
{
	// Signal and increment the fence value.
	UINT64 fenceToWaitFor = g_fenceLastSignaledValue;
	g_pd3dCommandQueue->Signal(g_fence, fenceToWaitFor);
	g_fenceLastSignaledValue++;
	// Wait until the GPU is done rendering.
	if (g_fence->GetCompletedValue() < fenceToWaitFor)
	{
		g_fence->SetEventOnCompletion(fenceToWaitFor, g_fenceEvent);
		WaitForSingleObject(g_fenceEvent, INFINITE);
	}
}

typedef DWORD(WINAPI* tIDXGISwapChain_Present)(IDXGISwapChain*, UINT, UINT);
tIDXGISwapChain_Present oSwapChain_Present = nullptr;
DWORD WINAPI hSwapChain_Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool s_presentCalled = false;
	if (!s_presentCalled) {
		s_presentCalled = true;
		s_swapChain = pSwapChain;
		initializeDx12();
	}
	if (s_initialized) {
		drawImgui();
		DWORD result = oSwapChain_Present(pSwapChain, SyncInterval, Flags);
		waitForFence();
		return result;
	}
	return oSwapChain_Present(pSwapChain, SyncInterval, Flags);
}

typedef HRESULT(WINAPI* tIDXGISwapChain_ResizeBuffers)(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
tIDXGISwapChain_ResizeBuffers otIDXGISwapChain_ResizeBuffers = nullptr;
HRESULT hSwapChain_ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	ImGui_ImplDX12_InvalidateDeviceObjects();
	waitForFence();
	for (int i = 0; i < g_backBufferCount; i++)
		if (g_backBuffers[i]) { g_backBuffers[i]->Release(); g_backBuffers[i] = NULL; }
	HRESULT ret = otIDXGISwapChain_ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
	for (int i = 0; i < g_backBufferCount; i++) {
		ID3D12Resource* pBackBuffer = NULL;
		s_swapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		s_dx12Device->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_backBuffers[i] = pBackBuffer;
	}
	ImGui_ImplDX12_CreateDeviceObjects();
	return ret;
}

LRESULT WINAPI TmpWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void InitializeDx12Hook()
{
	if (*(uintptr_t*)((uintptr_t)GetModuleHandle(NULL) + D3D12_SWAP_CHAIN_OFFSET)) {
		__int64* pSwapChainVTable = *(__int64**)(*(uintptr_t*)((uintptr_t)GetModuleHandle(NULL) + D3D12_SWAP_CHAIN_OFFSET));

		*(&g_pd3dCommandQueue) = reinterpret_cast<ID3D12CommandQueue*>(*(uintptr_t*)((uintptr_t)GetModuleHandle(NULL) + D3D12_CommandQueue_OFFSET));

		CreateVTableHook(XorString("IDXGISwapChain::Present"), (PDWORD64*)&pSwapChainVTable, hSwapChain_Present, 8, &oSwapChain_Present);
		CreateVTableHook(XorString("IDXGISwapChain::ResizeBuffers"), (PDWORD64*)&pSwapChainVTable, hSwapChain_ResizeBuffers, 13, &otIDXGISwapChain_ResizeBuffers);
	}
	else {
		MessageBoxA(NULL, XorString("Win7系统暂时不支持!"), XorString("提示"), MB_OK);
	}
	return;
}