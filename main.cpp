// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "commons.h"

#include "stockman.h"

#include <d3d11.h>
#include <tchar.h>

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static HWND g_hwnd;
static WNDCLASSEX g_wcex;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();


bool InitGraphics();
void CloseGraphics();
bool RenderGUI(ImVec4 clear_color);


result validateLogin(loginToken token, userType type);

//Modal helper functions
void showLoginNeeded(bool isopen = false,enum userType requiredType = guest);
void showAbout(bool isopen = false);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

stockMan StockMan;

using namespace ImGui;
// Main code
int main(int, char**)
{
    // Initialize window and graphics
    if (!InitGraphics())return 1;

    // window states

    bool logged_in = false;



    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();





#pragma region UILogic



        static bool isloggedin = false;
        static bool showloginwindow = true;
        static bool showregisterwindow = false;

        static bool showInStockWindow = false;

        static bool showAddCategory = false;
        static bool showAddAttr = false;
        static bool showAddItem = false;
        static bool showEditItem = false;
        static bool showUserList = false;
        static bool showLoginWindow = false;
        static bool showYealystat = false;
        static bool showMonthlystat = false;
        static bool showWeeklystat = false;
        static bool showUsermanage = false;
        static bool showLogwindow = false;
        static bool showDatabaseSelection = false;

        static ImVec2 windowsize_login = ImVec2(800, 600);
        static ImVec2 windowpos_login = ImVec2(250, 50);
        static ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        static loginToken currentLoginToken;

        
        if (BeginMainMenuBar()) {

            if (BeginMenu("About")) {
                if (MenuItem("About this program...."))
                    showAbout(true);
                ImGui::EndMenu();
            }
            if (BeginMenu("In/Out")) {
                if (MenuItem("In Stock.")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                        showInStockWindow = true;
                    else
                        showLoginNeeded(true,worker);

                }
                if (MenuItem("Out Stock")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                        showInStockWindow = true;
                    else
                        showLoginNeeded(true,worker);

                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Edit")) {
                if (ImGui::MenuItem("Add/Remove Category")) {
                    if (validateLogin(currentLoginToken, worker) == success)showAddCategory = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Attribute")) {
                    if (validateLogin(currentLoginToken, worker)==success)showAddCategory = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Items")) {
                    if (validateLogin(currentLoginToken, worker) == success)showAddItem = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Edit Item Properties")) {
                    if (validateLogin(currentLoginToken, worker) == success)showEditItem = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Statistics")) {
                if (MenuItem("Yearly")) {
                    if (validateLogin(currentLoginToken, guest) == success)showYealystat = true;
                    else
                    {
                        showLoginNeeded(true, guest);
                    }

                }
                if (MenuItem("Monthly")) {
                    if (validateLogin(currentLoginToken, guest) == success)showMonthlystat = true;
                    else
                    {
                        showLoginNeeded(true, guest);
                    }
                }
                if (MenuItem("Weekly")) {
                    if (validateLogin(currentLoginToken, guest) == success)showWeeklystat = true;
                    else
                    {
                        showLoginNeeded(true, guest);
                    }
                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Management")) {
                if (MenuItem("User management")) {
                    if(validateLogin(currentLoginToken,admin) == success)
                    showUsermanage = true;
                    else {
                        showLoginNeeded(true, admin);
                    }
                }
                if (MenuItem("Open logs")) {
                    if (validateLogin(currentLoginToken, admin) == success)
                        showLogwindow = true;
                    else {
                        showLoginNeeded(true, admin);
                    }
                }
                if (MenuItem("Select database")) {
                    if (validateLogin(currentLoginToken, admin) == success)
                        showDatabaseSelection = true;
                    else {
                        showLoginNeeded(true,admin);
                    }
                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Save & Exit")) {
                if (MenuItem("Save And Exit")) {

                }
                if (MenuItem("Exit without saving")) {

                }
                ImGui::EndMenu();
            }
            EndMainMenuBar();
        }
        showLoginNeeded();
        showAbout();
        
        if (!isloggedin) {
            SetNextWindowSize(windowsize_login);
            SetNextWindowPos(windowpos_login);

            Begin("Login");//Login window
            {

            }
            End();

        }

#pragma endregion

        // Render the GUI and draw it onto the window
        if (!RenderGUI(clear_color))return 1;
    }

    CloseGraphics();
    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


bool InitGraphics() {
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    g_wcex = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&g_wcex);
    g_hwnd = ::CreateWindow(g_wcex.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME), 100, 100, 1280, 800, NULL, NULL, g_wcex.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(g_hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(g_wcex.lpszClassName, g_wcex.hInstance);
        return false;
    }

    // Show the window
    ::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    return true;

}
void CloseGraphics() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(g_hwnd);
    ::UnregisterClass(g_wcex.lpszClassName, g_wcex.hInstance);

}
bool RenderGUI(ImVec4 clear_color) {

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync
    return true;
}

void showLoginNeeded(bool isopen, userType requiredType) {
    static bool opened = false;
    static userType usrType = requiredType;
    static  map<userType, const char*> titles = { {guest,"Login Needed!"},{worker,"Privellege Required!"},{admin,"Privellege Required!"} };
    static  map<userType, const char*> contents = { {guest,"You need to at least login as guest to perform thisn operation."},{worker,"You need the privellege of worker or above \nto perform this operation."},{admin,"You need administration privellege to \nperform this operation."} };
    if(isopen)usrType = requiredType;
    if (isopen)opened = true;
    if (opened) {
        OpenPopup(titles[usrType]);
    }
    if (BeginPopupModal(titles[usrType], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text(contents[usrType]);
        Separator();
        PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
        if (Button("Got it")) {
            CloseCurrentPopup();
            opened = false;
        }
        PopStyleVar();
        EndPopup();
    }
}
void showAbout(bool isopen) {
    static bool opened = false;
    if (isopen)opened = true;
    if (opened) {
        OpenPopup("About Me");
    }
    if (BeginPopupModal("About Me", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text("This is just an about_me.\nContenet to be added.\n");
        Separator();
        PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
        if (Button("Got it")) {
            CloseCurrentPopup();
            opened = false;
        }
        PopStyleVar();
        EndPopup();
    }
}
result validateLogin(loginToken token,userType type) {
    result res = success;
    if (!token.valid)return unknown_error;
    if (token.login_time > (unsigned)time(NULL))return unknown_error;
    if (token.usertype != type)return bad_privilege;
    if ((res = StockMan.findUser(token.userid)) != success)return res;
    return res;
}
