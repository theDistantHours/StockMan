// Dear ImGui: standalone example application for DirectX 11

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "commons.h"

#include "stockman.h"

#include <d3d11.h>
#include <tchar.h>

using namespace ImGui;

enum fontStyle { default, normal, literal };
enum fontSize { text, caption, header };
enum fontWeight { regular, medium, bold };
enum statrange { week, month, year };

static map <fontStyle, map<fontSize, map<fontWeight, ImFont*>>> fontmap;

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static HWND                     g_hwnd;
static WNDCLASSEX               g_wcex;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();


bool InitGraphics();
void CloseGraphics();


bool RenderGUI(ImVec4 clear_color);
void TextCenter(string str) {
    float fontsize = GetFontSize() * str.size() / 2;
    SetCursorPosX(GetWindowContentRegionWidth() / 2 - fontsize / 2);
    Text(str.c_str());
}
bool ButtonCenter(string str) {
    float fontsize = GetFontSize() * str.size() / 2;
    SetCursorPosX(GetWindowContentRegionWidth() / 2 - fontsize / 2);
    return Button(str.c_str());
}

void ShowLogWindow(bool opt);

void ShowEditUser(bool opt);
void ShowStat(bool opt, statrange range = year);
void ShowStockManage(bool opt);
void ShowOverView(bool opt);

result validateLogin(loginToken token, userType type);



//Modal helper functions
void showLoginNeeded(bool isopen = false, userType requiredType = guest);
void showAbout(bool isopen = false);
void ShowInStockWindow(bool opt = false);
void ShowItemDetail(uid dest = 0, bool isopen = 0);

void showAddCategory(bool isopen = false);
void showAddItem(bool isopen = false, vector<uid> attrs = vector<uid>());
void showAddAttr(bool isopen = false);
void showAddUser(bool isopen = false);

void showRemoveCategory(uid id = 0, bool isopen = false);
void showRemoveItem(uid id = 0, bool isopen = false);
void showRemoveAttr(uid id = 0, bool isopen = false);

void showEditItem(uid id = 0, bool isopen = false);
void showEditCategory(uid id = 0, bool isopen = false);

//



LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

stockMan StockMan;
static ImVec2 vec_content = ImVec2(920, 720);
static ImGuiWindowFlags wndflg_content = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
static bool isloggedin = false;
static bool showloginwindow = true;
static bool showregisterwindow = false;


//

static bool showEditUser = false;
static bool showLoginWindow = false;
static bool showStat = false;
static bool showLogwindow = false;
static bool showStockManage = false;

static bool showOverview = true;

static bool refresh_required = true;

void clearContent(void) {
    showEditUser = false;
    showLoginWindow = false;
    showStat = false;
    showLogwindow = false;
    showStockManage = false;

    showOverview = false;
}
static loginToken currentLoginToken;


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



        static ImVec2 windowsize_login = ImVec2(700, 300);
        static ImVec2 windowpos_login = ImVec2(300, 150);
        static ImVec2 center = ImGui::GetMainViewport()->GetCenter();

        //
        currentLoginToken.valid = true;


        PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        PushFont(fontmap[normal][text][regular]);

        if (BeginMainMenuBar()) {

            if (BeginMenu("About")) {
                if (MenuItem("About this program...."))
                    showAbout(true);
                ImGui::EndMenu();
            }
            if (BeginMenu("In/Out")) {
                if (MenuItem("In Stock")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                    {
                        ShowInStockWindow(true);
                    }
                    else
                        showLoginNeeded(true, worker);
                }
                if (MenuItem("Out Stock")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                    {
                        ShowInStockWindow(true);
                    }
                    else
                        showLoginNeeded(true, worker);

                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Edit")) {
                if (ImGui::MenuItem("Add/Remove Category")) {
                    if (validateLogin(currentLoginToken, worker) == success) {
                        clearContent();
                        showStockManage = true;
                    }
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Attribute")) {
                    if (validateLogin(currentLoginToken, worker) == success) {
                        clearContent();
                        showStockManage = true;
                    }
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Items")) {
                    if (validateLogin(currentLoginToken, worker) == success) {
                        clearContent();
                        showStockManage = true;
                    }
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Edit Item Properties")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                    {
                        clearContent();
                        showStockManage = true;
                    }
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Statistics")) {
                if (MenuItem("Show statistics..")) {
                    if (validateLogin(currentLoginToken, guest) == success) {
                        clearContent();
                        showStat = true;
                    }
                    else
                    {
                        showLoginNeeded(true, guest);
                    }

                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Management")) {
                if (MenuItem("User management")) {
                    if (validateLogin(currentLoginToken, admin) == success) {
                        clearContent();
                        showEditUser = true;

                    }

                    else {
                        showLoginNeeded(true, admin);
                    }
                }
                if (MenuItem("Open logs")) {
                    if (validateLogin(currentLoginToken, admin) == success)
                    {
                        clearContent();
                        showLogwindow = true;
                    }
                    else {
                        showLoginNeeded(true, admin);
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


        if (!isloggedin) {
            SetNextWindowSize(windowsize_login);
            SetNextWindowPos(windowpos_login);

            Begin("Login");//Show Login window
            {
                static char username[32] = "";
                static char password[32] = "";
                PushFont(fontmap[normal][header][regular]);
                BeginChild("Title", ImVec2(windowsize_login.x, windowsize_login.y * 0.2f));
                {
                    TextCenter("Stock Manager Program");
                }
                EndChild();
                Separator();

                BeginChild("LoginWindowContent", ImVec2(GetWindowContentRegionWidth() * 0.6f, windowsize_login.y * 0.63f), true);
                {
                    SetCursorPosY(30);
                    PushFont(fontmap[normal][text][regular]);
                    SetCursorPosX(50);
                    Text("Enter username:");
                    SetCursorPosX(50);
                    InputText(" ", username, 32);
                    SetCursorPosX(50);
                    Text("Enter password:");
                    SetCursorPosX(50);
                    InputText("  ", password, 32, ImGuiInputTextFlags_Password);
                    PopFont();
                }
                EndChild();
                SameLine();
                BeginChild("Caption", ImVec2(GetWindowContentRegionWidth() * 0.39f, windowsize_login.y * 0.63f), true);
                {
                    SetCursorPos(ImVec2(80, 40));
                    PushFont(fontmap[normal][caption][regular]);
                    if (Button("Login", ImVec2(100, 40))) {
                        isloggedin = true;
                    }
                    SetCursorPos(ImVec2(80, 85));
                    if (Button("Register", ImVec2(100, 40))) {

                    }
                    PopFont();
                }
                EndChild();
                PopFont();
            }
            End();

        }
        else {
            ImGuiWindowFlags wndFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
            SetNextWindowPos(ImVec2(0, 25));
            SetNextWindowSize(ImVec2(1280, 775));
            Begin("BackgroundWindow", NULL, wndFlags);
            {
                //Background
                BeginChild("bg_left", ImVec2(320, 720), true);
                {
                    PushFont(fontmap[normal][header][regular]);
                    if (Button("In/Out Stock", ImVec2(300, 137))) {
                        if (validateLogin(currentLoginToken, worker) != success)showLoginNeeded(1, worker);
                        else {
                            ShowInStockWindow(true);
                        }
                    }
                    if (Button("View Log", ImVec2(300, 137))) {
                        if (validateLogin(currentLoginToken, admin) != success)
                            showLoginNeeded(1, admin);

                        else {
                            clearContent();
                            showLogwindow = true;
                        }
                    }
                    if (Button("Manage Stock \n       Info", ImVec2(300, 137))) {
                        if (validateLogin(currentLoginToken, worker) != success)showLoginNeeded(1, worker);
                        else {
                            clearContent();
                            showStockManage = true;
                        }
                    }
                    if (Button("Manage Users", ImVec2(300, 137))) {
                        if (validateLogin(currentLoginToken, admin) != success)showLoginNeeded(1, admin);
                        else {
                            clearContent();
                            showEditUser = true;
                        }
                    }
                    if (Button("Statistics", ImVec2(300, 137))) {
                        if (validateLogin(currentLoginToken, admin) != success)showLoginNeeded(1, guest);
                        else {
                            clearContent();
                            showStat = true;
                        }
                    }
                    PopFont();
                }
                EndChild();
                SameLine();
                ShowOverView(showOverview);
                ShowLogWindow(showLogwindow);
                ShowEditUser(showEditUser);
                ShowStat(showStat);
                ShowStockManage(showStockManage);

                showAddItem();
                showAddAttr();
                showAddUser();
                ShowInStockWindow();
                ShowItemDetail();
                showAddCategory();
                showRemoveCategory();
                showEditCategory();
            }
            End();
        }

        showLoginNeeded();
        showAbout();

#pragma endregion
        PopFont();
        ImGui::PopStyleVar();


        // Render the GUI and draw it onto the window
        if (!RenderGUI(clear_color))return 1;
        static bool tmp = false;
        if (tmp) {
            refresh_required = false;
            tmp = false;
        }
        if (refresh_required)tmp = true;
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

    ImFontConfig fontconfig;
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    fontmap[default][text][regular] = io.Fonts->AddFontDefault();

    fontmap[normal][text][regular] = io.Fonts->AddFontFromFileTTF("./misc/fonts/refsan.ttf", 18.0f);
    fontmap[normal][caption][regular] = io.Fonts->AddFontFromFileTTF("./misc/fonts/refsan.ttf", 22.0f);
    fontmap[normal][header][regular] = io.Fonts->AddFontFromFileTTF("./misc/fonts/refsan.ttf", 40.0f);



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
    static  map<userType, const char*> contents = { {guest,"You need to at least login as guest to perform this operation.\n"},{worker,"You need the privellege of worker or above \nto perform this operation.\n"},{admin,"You need administration privellege to \nperform this operation.\n"} };
    if (isopen) {
        usrType = requiredType;
        opened = true;
    }
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
void showAddCategory(bool isopen)
{
    static bool opened = false;
    static result res = success;
    static string hint;
    if (isopen)opened = true;
    if (opened) {
        SetNextWindowSize(ImVec2(600,170));
        SetNextWindowPos(ImVec2(350, 180));
        OpenPopup("add category");
        BeginPopupModal("add category", NULL, wndflg_content);
        {
            static stockCategory dest;
            static char catename[32];
            static char catedesc[64];
            static char com[64];
            static int dur = 0;
            InputText("Category name", catename, 32);
            InputText("Category description", catedesc, 64);
            InputText("Comment", com, 64);
            Separator();
            if (res != success) {
                Text(hint.c_str());
                SameLine();
                if (Button("Got it")) {
                    res = success;
                    memset(catename, 0, 32 * sizeof(char));
                    memset(catedesc, 0, 64 * sizeof(char));
                    memset(com, 0, 64 * sizeof(char));
                }
                Separator();
            }
            else {
                if (Button("Confirm")) {
                    dest.name = string(catename);
                    dest.desc = string(catedesc);

                    static result res = StockMan.addCategory(dest, string(com));
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                    else {
                        switch (res)
                        {
                        case item_already_exist:
                            hint = "This category already exists.Change a name!";
                            break;
                        default:
                            hint = "Unknown error occured.Retry or restart program.";
                            break;
                        }
                    }

                }
                SameLine();
                if (Button("Cancle")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }

        }
        EndPopup();
    }
}
void showAddItem(bool isopen, vector<uid> attrs)
{
    static bool opened = false;
    static result res = success;
    static string hint;
    static int szattr;
    static vector<stockAttr> attr;
    static int idxattr;
    if (isopen) {
        opened = true;
        attr = StockMan.getAttrs();
        szattr = attr.size();
    }
    if (opened) {
        SetNextWindowSize(ImVec2(600, 200));
        SetNextWindowPos(ImVec2(350, 180));
        OpenPopup("add item");
        BeginPopupModal("add item", NULL, wndflg_content);
        {
            static stockItem dest;
            static char catename[32];
            static char catedesc[64];
            static char com[64];
            static int dur = 0;


            InputText("Item name", catename, 32);
            InputText("Item description", catedesc, 64);

            if (BeginCombo("Item category", idxattr < szattr ? attr[idxattr].name.c_str() : "")) {
                for (int i = 0; i < szattr; i++) {
                    const bool selected = (i == idxattr);
                    if (Selectable(attr[i].name.c_str())) {
                        idxattr = i;
                    }
                    if(selected)SetItemDefaultFocus();
                }
                EndCombo();
            }
            InputText("Comment", com, 64);
            Separator();
            if (res != success) {
                Text(hint.c_str());
                SameLine();
                if (Button("Got it")) {
                    res = success;
                    memset(catename, 0, 32 * sizeof(char));
                    memset(catedesc, 0, 64 * sizeof(char));
                    memset(com, 0, 64 * sizeof(char));
                }
                Separator();
            }
            else {
                if (Button("Confirm")) {
                    dest.name = string(catename);
                    dest.desc = string(catedesc);

                    static result res = StockMan.addItem(dest, string(com));
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                    else {
                        switch (res)
                        {
                        case item_already_exist:
                            hint = "This itemname already exists.Change a name!";
                            break;
                        default:
                            hint = "Unknown error occured.Retry or restart program.";
                            break;
                        }
                    }

                }
                SameLine();
                if (Button("Cancle")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }

        }
        EndPopup();
    }
}
void showAddAttr(bool isopen)
{
    static bool opened = false;
    static result res = success;
    static string hint;
    if (isopen)opened = true;
    if (opened) {
        SetNextWindowSize(ImVec2(600, 170));
        SetNextWindowPos(ImVec2(350, 180));
        OpenPopup("add attribute");
        BeginPopupModal("add attribute", NULL, wndflg_content);
        {
            static stockAttr dest;
            static char catename[32];
            static char catedesc[64];
            static char com[64];
            static int dur = 0;
            InputText("Attribute name", catename, 32);
            InputText("Attribute description", catedesc, 64);
            InputText("Comment", com, 64);
            Separator();
            if (res != success) {
                Text(hint.c_str());
                SameLine();
                if (Button("Got it")) {
                    res = success;
                    memset(catename, 0, 32 * sizeof(char));
                    memset(catedesc, 0, 64 * sizeof(char));
                    memset(com, 0, 64 * sizeof(char));
                }
                Separator();
            }
            else {
                if (Button("Confirm")) {
                    dest.name = string(catename);
                    dest.desc = string(catedesc);

                    static result res = StockMan.addAttr(dest, string(com));
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                    else {
                        switch (res)
                        {
                        case item_already_exist:
                            hint = "This attribute already exists.Change a name!";
                            break;
                        default:
                            hint = "Unknown error occured.Retry or restart program.";
                            break;
                        }
                    }

                }
                SameLine();
                if (Button("Cancle")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }

        }
        EndPopup();
    }
}
void showRemoveCategory(uid id, bool isopen)
{
    static uid dest_id;
    static bool opened = false;;
    if (isopen) {
        opened = true;
        dest_id = id;
    }
    if (opened) {
        OpenPopup("remove category");
        BeginPopupModal("remove category", NULL, wndflg_content);
        {
            Text("Are you sure to remove category %s?\nAll items inside it will be deleted.\nThis operation cannot be undone.", StockMan.getCategory(dest_id).name.c_str());
            static char comment[32];
            static string strcomment;
            InputText("(Enter your comment here)", comment, 32);
            if (Button("Yes")) {
                refresh_required = true;
                strcomment = string(comment);
                if (strcomment.size() == 0)strcomment = "No comment";
                StockMan.removeCategory(dest_id, string(comment));
                opened = false;
                CloseCurrentPopup();
            }
            SameLine();
            if (Button("No")) {
                opened = false;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }

}

void showRemoveItem(uid id, bool isopen)
{
    static uid dest_id;
    static bool opened = false;;
    if (isopen) {
        opened = true;
        dest_id = id;
    }
    if (opened) {
        OpenPopup("remove item");
        BeginPopupModal("remove item", NULL, wndflg_content);
        {
            Text("Are you sure to remove item %s?\nThis operation cannot be undone.", StockMan.getItem(dest_id).name.c_str());
            static char comment[32];
            static string strcomment;
            InputText("(Enter your comment here)", comment, 32);
            if (Button("Yes")) {
                refresh_required = true;
                strcomment = string(comment);
                if (strcomment.size() == 0)strcomment = "No comment";
                StockMan.removeItem(dest_id, string(comment));
                opened = false;
                CloseCurrentPopup();
            }
            SameLine();
            if (Button("No")) {
                opened = false;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }

}


void showEditItem(uid id, bool isopen)
{
    static bool opened = false;
    static result res = success;
    static string hint;
    static int szattr;
    static vector<stockAttr> attr;
    static uid itemid;
    static stockItem item;
    static int idxattr;
    if (isopen) {
        opened = true;
        itemid = id;
        attr = StockMan.getAttrs();
        item = StockMan.getItem(itemid);
        szattr = attr.size();
    }
    if (opened) {
        SetNextWindowSize(ImVec2(600, 200));
        SetNextWindowPos(ImVec2(350, 180));
        OpenPopup("edit item");
        BeginPopupModal("edit item", NULL, wndflg_content);
        {
            static stockItem dest;
            static char itemname[32];
            strcpy_s(itemname, item.name.c_str());
            static char itemdesc[64];
            strcpy_s(itemdesc, item.desc.c_str());
            static char com[64];
            static int dur = 0;


            InputText("Item name", itemname, 32);
            InputText("Item description", itemdesc, 64);

            if (BeginCombo("Item category", idxattr < szattr ? attr[idxattr].name.c_str() : "")) {
                for (int i = 0; i < szattr; i++) {
                    const bool selected = (i == idxattr);
                    if (Selectable(attr[i].name.c_str())) {
                        idxattr = i;
                    }
                    if (selected)SetItemDefaultFocus();
                }
                EndCombo();
            }
            InputText("Comment", com, 64);
            Separator();
            if (res != success) {
                Text(hint.c_str());
                SameLine();
                if (Button("Got it")) {
                    res = success;
                    memset(itemname, 0, 32 * sizeof(char));
                    memset(itemdesc, 0, 64 * sizeof(char));
                    memset(com, 0, 64 * sizeof(char));
                }
                Separator();
            }
            else {
                if (Button("Confirm")) {
                    dest.name = string(itemname);
                    dest.desc = string(itemdesc);

                    static result res = StockMan.editItem(itemid,string(itemname),string(itemdesc), string(com));
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                    else {
                        switch (res)
                        {
                        case item_already_exist:
                            hint = "This itemname already exists.Change a name!";
                            break;
                        default:
                            hint = "Unknown error occured.Retry or restart program.";
                            break;
                        }
                    }

                }
                SameLine();
                if (Button("Cancle")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }

        }
        EndPopup();
    }
}

void showRemoveAttr(uid id, bool isopen)
{
    static uid dest_id;
    static bool opened = false;;
    if (isopen) {
        opened = true;
        dest_id = id;
    }
    if (opened) {
        OpenPopup("remove attr");
        BeginPopupModal("remove attr", NULL, wndflg_content);
        {
            Text("Are you sure to remove attribute %s?\nThis operation cannot be undone.", StockMan.getAttr(dest_id).name.c_str());
            static char comment[32];
            static string strcomment;
            InputText("(Enter your comment here)", comment, 32);
            if (Button("Yes")) {
                refresh_required = true;
                strcomment = string(comment);
                if (strcomment.size() == 0)strcomment = "No comment";
                StockMan.removeItem(dest_id, string(comment));
                opened = false;
                CloseCurrentPopup();
            }
            SameLine();
            if (Button("No")) {
                opened = false;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
}

void showEditCategory(uid id, bool isopen)
{
    static bool opened = false;
    static result res = success;
    static string hint = "no hint";
    static uid dest = 0;
    if (isopen) {
        dest = id;
        opened = true;
    }
    if (opened) {
        OpenPopup("rename category");
        BeginPopupModal("rename category");
        {
            static char name[32];
            static char desc[64];
            SameLine();
            InputText("Enter new name", name, 32);
            InputText("Enter new description", desc, 64);
            if (res == success) {
                if (Button("Confirm")) {
                    res = StockMan.editCategory(dest, string(name), string(desc));
                    if (res != success) {
                        if (res == item_already_exist) {
                            hint = "Category name already exist.Change one.";
                        }
                        else hint = "Unknown error";
                    }
                }
                SameLine();
                if (Button("Cancel")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }
            else {
                Text(hint.c_str());
                if (Button("Got it")) {
                    res = success;
                    hint = "";
                }
            }

        }
        EndPopup();
    }
}
result validateLogin(loginToken token, userType type) {

    return success;
    result res = success;
    if (!token.valid)return unknown_error;
    if (token.login_time > (unsigned)time(NULL))return unknown_error;
    switch (type)
    {
    case guest:
        break;
    case worker:
        if (token.usertype == guest)return bad_privilege;
        break;
    case admin:
        if (token.usertype != admin)return bad_privilege;
        break;
    default:
        break;
    }
    if ((res = StockMan.findUser(token.userid)) != success)return res;
    return res;
}


void ShowLogWindow(bool opt) {
    if (opt) {
        BeginChild("Recent logs", vec_content, true, wndflg_content);
        //have a look how to create charts
        Text("This part is currently unavaliable.\nFurther updates may support log displaying feature.");
        EndChild();
    }

}

void ShowInStockWindow(bool isopen) {
    static bool opened = false;
    vector<stockCategory> categories;
    vector<stockItem> items;
    stockItem dest;
    static int count = 0;
    static int durance = 0;
    static uid id;
    static int sz1;
    static int sz2 = 0;
    static string comment;
    if (isopen) {
        opened = true;
        categories = StockMan.getCategories();
        sz1 = categories.size();
    }
    if (opened) {
        OpenPopup("In/Out Stock");
        BeginPopupModal("In/Out Stock", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        {
            static int combo_idx = 0;
            if (BeginCombo("Category", combo_idx < sz1 ? categories[combo_idx].name.c_str() : "")) {
                for (int i = 0; i < sz1; i++) {
                    const bool is_selected = (combo_idx == i);
                    if (Selectable(categories[i].name.c_str())) {
                        combo_idx = i;
                        items = StockMan.getItems(categories[i].id, set<uid>());
                        sz2 = items.size();
                    }
                    if (is_selected) {
                        SetItemDefaultFocus();
                    }
                }
                EndCombo();
            }
            static int combo_idx2 = 0;
            if (BeginCombo("ItemName", combo_idx2 < sz2 ? items[combo_idx2].name.c_str() : "")) {
                for (int i = 0; i < sz2; i++) {
                    const bool is_selected = (combo_idx2 == i);
                    if (Selectable(items[i].name.c_str())) {
                        combo_idx2 = i;
                        id = items[i].id;
                    }
                    if (is_selected) {
                        SetItemDefaultFocus();
                    }
                }
                EndCombo();
            }
            InputInt("item count", &count);
            InputInt("item durance(in days)", &durance);
            static char c_comment[64];
            InputText("Operation comment", c_comment, 64);
            Separator();
            if (Button("Confirm")) {
                comment = string(c_comment);
                //this is the crucial point!
                StockMan.InStock(id, count, durance, comment);
                //
                opened = false;
                CloseCurrentPopup();

            }
            SameLine();
            if (Button("Cancel")) {
                opened = false;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
}

void ShowEditUser(bool opt) {
    if (opt) {
        static vector<userInfo> users;
        static map<userType, string> strmap = { {guest,"guest"},{worker,"worker"},{admin,"admin"} };
        static int sz = 0;
        if (refresh_required) {
            users = StockMan.getUsers();
            sz = users.size();
        }
        BeginChild("Edit Users", vec_content, true, wndflg_content);
        {
            if (BeginTable("usertable", 4)) {
                
                TableSetupColumn("Username");
                TableSetupColumn("UserType");
                TableSetupColumn("UserId");
                TableSetupColumn("Operation");
                TableHeadersRow();
                for (int i = 0; i < sz; i++) {
                    TableNextColumn();
                    Text("%s", users[i].username.c_str());
                    TableNextColumn();
                    Text("%s", strmap[users[i].privilege]);
                    TableNextColumn();
                    Text("%ld", users[i].userid);
                    TableNextColumn();
                    if (Button("Edit")) {

                    }
                    if (Button("Remove")) {

                    }
                    if (Button("Info")) {

                    }
                }
                EndTable();
            }
        }
        EndChild();
    }
}

void ShowStat(bool opt, statrange range) {
    static statrange rng = year;
    static map<statrange, int> cntmap = { {week,7},{month,30},{year,12} };
    static float fontHeight = CalcTextSize("A").y;
    if (opt) {
        static ImVec2 vec_cateshow = ImVec2(300,704);
        static ImVec2 vec_mainshow = ImVec2(598,704);
        static ImVec2 vec_graph = ImVec2(580,323);
        static ImVec2 vec_graph_content = ImVec2(575, 310);

        static vector<tuple<string, int>> count_data_c;
        static vector<tuple<string, int>> currency_data_c;
        static int cate_sz = 0;

        static vector<tuple<string, int>> count_data;
        static vector<tuple<string, int>> currency_data;

        static float* count_arr = nullptr;
        static float* currency_arr = nullptr;

        static int cnt_max_c =0;
        static int currency_max_c=0;

        static int cnt_max = 0;
        static int currency_max = 0;

        static const ImU32 count_color = ColorConvertFloat4ToU32(ImVec4(68.0/255.0,171.0/255.0,235.0/255.0,1.0));
        static const ImU32 currency_color = ColorConvertFloat4ToU32(ImVec4(237.0 / 255.0, 195.0 / 255.0, 63.0 / 255.0, 1.0));
        if (refresh_required) {
           //fetch data and sort it

            //
            cnt_max_c = 0;
            currency_max_c = 0;
            cnt_max = 0;
            currency_max = 0;
            for (int i = 0; i < cate_sz; i++) {
                if (get<1>(count_data_c[i]) > cnt_max_c)cnt_max_c = get<1>(count_data_c[i]);
                if (get<1>(currency_data_c[i]) > currency_max_c)currency_max_c = get<1>(currency_data_c[i]);
            }
            for (int i = 0; i < cntmap[rng];i++) {
                if (get<1>(count_data[i]) > cnt_max)cnt_max = get<1>(count_data_c[i]);
                if (get<1>(currency_data[i]) > currency_max)currency_max = get<1>(currency_data_c[i]);
            }
            if (count_arr != nullptr) 
                count_arr = new float[count_data.size()];
            memset(count_arr, 0, count_data.size() * sizeof(float));
            for (int i = 0; i < count_data.size(); i++) {
                count_arr[i] = get<1>(count_data[i]);
            }

            if (currency_arr != nullptr)
                currency_arr = new float[currency_data.size()];
            memset(currency_arr, 0, currency_data.size() * sizeof(float));
            for (int i = 0; i < currency_data.size(); i++)
                currency_arr[i] = get<1>(currency_data[i]);
        }
        BeginChild("Statistics", vec_content, true, wndflg_content);
        {
            BeginChild("Cateshow", vec_cateshow, true, wndflg_content);
            {
                Text("Category View");
                static bool is_count = true;
                if (Button("count view")) {
                    is_count = true;
                }
                SameLine();
                if ((Button("currency view"))) {
                    is_count = false;
                }
                ImDrawList* drawlist = GetWindowDrawList();
                for (int i = 0; i < cate_sz; i++) {
                    BeginGroup();
                    ImVec2 v1 = GetCursorScreenPos();
                    drawlist->PushClipRect(v1, ImVec2(v1.x + 50, v1.y + fontHeight+5));
                    Text(get<0>(count_data_c[i]).c_str());
                    drawlist->PopClipRect();
                    SameLine();
                    ImVec2 v2 = GetCursorScreenPos();
                    drawlist->AddRectFilled(v2, ImVec2(v2.x + get<1>((is_count ? count_data_c : currency_data_c)[i]) * 50.0 / (is_count ? cnt_max_c : cnt_max), fontHeight), is_count ? count_color : currency_color);
                    EndGroup();
                }
            }
            EndChild();
            SameLine();
            BeginChild("Mainshow", vec_mainshow, true, wndflg_content); {
                if (Button("Year View")) {
                    rng = year;
                }SameLine();
                if (Button("Month View")) {
                    rng = month;
                }SameLine();
                if (Button("Week View")) {
                    rng = week;
                }
                Separator();
                BeginChild("count graph", vec_graph, true);
                {
                    Text("Item count");
                    Separator();
                    if(cnt_max !=0)PlotHistogram("##values", count_arr, cnt_max,0,nullptr,0.0f,1.0f,vec_graph_content);
                }
                EndChild();
                Separator();
                BeginChild("price graph", vec_graph, true);
                {
                    Text("Currency");
                    Separator();
                    if(currency_max)PlotHistogram("##values", count_arr, currency_max, 0, nullptr, 0.0f, 1.0f, vec_graph_content);

                }
                EndChild();
            }
            EndChild();
        }
        EndChild();
    }
}

//Function Name :   ShowStockManage
//Ability       :   Display a window that provides ability to manage stock item information(including adding/removing/editing categories/items/
//                  attributes.
//Parameter     :   opt -   is true when the window should be displayed
void ShowStockManage(bool opt) {
    if (opt) {
        static vector<stockAttr> Attributes;
        static vector<tuple<uid, bool*>> check;
        static vector<stockCategory> Categories;
        static vector<stockItem> current_items;

        static uid current_cate;
        static set<uid> current_attrs;
        if (refresh_required) {
            current_items = StockMan.getItems(current_cate, current_attrs);
            check.resize(Attributes.size());
            for (int i = 0; i < check.size(); i++) {
                get<1>(check[i]) = new bool;
                *get<1>(check[i]) = true;
            }
            Categories = StockMan.getCategories();
            Attributes = StockMan.getAttrs();
        }

        BeginChild("Stock Management", vec_content, true, wndflg_content);
        static ImVec2 vec_content2 = ImVec2(250, 704);
        BeginChild("Stock Categories", vec_content2, true);
        {
            PushFont(fontmap[normal][caption][regular]);
            Text("Select category");
            PopFont();
            static ImVec2 vec_button = ImVec2(280, 100);
            int sz = Categories.size();
            Separator();
            if (sz == 0) {
                Text("Currently there are not any \ncategories yet.");
                if (Button("Press HERE to add one.")) {
                    showAddCategory(true);
                }
            }
            else for (int i = 0; i < sz; i++) {
                stockCategory current = Categories[i];
                if (Button(current.name.c_str(), vec_button)) {
                    current_cate = current.id;
                }
                if (BeginPopupContextItem((string("category_") + to_string(i)).c_str())) {
                    if (Selectable("Add category")) {
                        showAddCategory(true);
                    }
                    if (Selectable("Remove cateogry")) {
                        showRemoveCategory(current.id, true);
                    }
                    if (Selectable("Edit category")) {
                        showEditCategory(current.id, true);
                    }
                    EndPopup();
                }
            }
        }
        EndChild();
        SameLine();
        static ImVec2 vec_content3 = ImVec2(645, 704);
        BeginChild("Stock content", vec_content3, true);
        {
            static ImVec2 vec_checkbox = ImVec2(630, 100);
            static ImVec2 vec_innercontent = ImVec2(630, 580);
            BeginChild("Checkbox", vec_checkbox, false);
            {
                int sz = Attributes.size();
                PushFont(fontmap[normal][caption][regular]);
                Text("Attribute filter");
                PopFont();
                if (sz == 0) {
                    Text("Currently there are not any \nAttributes yet.");
                    if (Button("Press HERE to add one.")) {
                        showAddAttr(true);
                    }
                }
                for (int i = 0; i < sz; i++) {
                    if (Checkbox(Attributes[i].name.c_str(), get<1>(check[i]))) {
                        if (*get<1>(check[i])) {
                            current_attrs.insert(Attributes[i].id);
                        }
                        else
                        {
                            set<uid>::iterator iter = current_attrs.find(Attributes[i].id);
                            if (iter != current_attrs.end())current_attrs.erase(iter);
                        }
                    };
                }
            }
            EndChild();
            Separator();
            BeginChild("inner stock content", vec_innercontent, false);
            {
                PushFont(fontmap[normal][caption][regular]);
                Text("Items");
                PopFont();
                int sz = current_items.size();
                if (sz == 0) {
                    Text("Currently there are not any \nitems in this category yet.");
                    if (Button("Press HERE to add one.")) {
                        showAddItem(true);
                    }
                }
                if (BeginTable("content_table", 4)) {
                    for (int i = 0; i < sz; i++) {
                        TableNextColumn();
                        if (Selectable(current_items[i].name.c_str())) {

                        }
                        if (BeginPopupContextItem((string("itemview_") + to_string(i)).c_str())) {
                            if (Selectable("view details")) {
                                ShowItemDetail(current_items[i].id);
                            }
                            if (Selectable("remove item")) {
                                showRemoveItem(current_items[i].id);
                            }
                            if (Selectable("edit information")) {
                                showEditItem(current_items[i].id);
                            }
                            EndPopup();
                        }
                    }
                    EndTable();
                }
            }
            EndChild();
        }
        EndChild();

        EndChild();
    }
}

//Function Name :   ShowOverView
//Ability       :   show an window with overview information(outdate items, not enough items, full items) in it.
//Parameter     :   opt:the window is displayed when opt is true
void ShowOverView(bool opt) {
    if (opt) {
        static bool isfirstopen = true;
        static vector<tuple<infoType, stockCategory, stockItem, int, int>> info;
        int sz = info.size();
        if (isfirstopen) {
            info = StockMan.overview();
            sz = info.size();
        }
        BeginChild("OverView", vec_content, true, wndflg_content);

        for (int i = 0; i < sz; i++) {
            BeginChild(i * 667, ImVec2(900, 200), true);
            {
                switch (get<0>(info[i]))
                {
                case outdate:
                    Text("Item %s %s is partially out of durance.\nMax-durance: %d ,outdatad items count: %d", get<1>(info[i]).name.c_str(), get<2>(info[i]).name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                case notenough:
                    Text("Item %s %s is around empty.\nCurrent remaining: %d ,Minimun required: %d\nPlease add in time.", get<1>(info[i]).name.c_str(), get<2>(info[i]).name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                case full:
                    Text("Item %s %s is already full.\nCurrent count: %d\nMax-capacity: %d", get<1>(info[i]).name.c_str(), get<2>(info[i]).name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                default:
                    break;
                }
            }
            EndChild();
        }
        if (Button("refresh")) {
            //
            isfirstopen = true;
        }
        EndChild();
    }
}


//Function Name :   ShowStockDetail
//Ability       :   Display a Popup modal containing the detail information of a certain item.
//Parameter     :   dest    -   the uid of item to be displayed
//                  isopen  -   is true when the window turns from hidden to visible.   
void ShowItemDetail(uid dest, bool isopen) {
    static uid id = 0;
    static bool opened = false;
    static stockItem item;
    static string labels = "";
    static int sz;
    if (isopen) {
        item = StockMan.getItem(dest);
        opened = true;
        id = dest;
        sz = item.attrs.size();
        for (int i = 0; i < sz; i++) {
            labels += " ";
            labels += item.attrs[i].name;
        }
    }
    if (opened) {
        OpenPopup("stockdetail");
        BeginPopupModal("stockdetail");
        {
            static ImVec2 vec_header = ImVec2(600, 180);
            BeginChild("vec_header", vec_header, true);
            {
                Text("Item name: %s", item.name.c_str());
                Text("Category: %s", item.category.name.c_str());
                Separator();
                Text("Item description: %s", item.desc.c_str());
                Text("Item attributes : %s", labels.c_str());
            }
            EndChild();
            if (Button("Close")) {
                opened = false;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
}

void showAddUser(bool isopen) {
    static bool opened = false;
    static char c_username[32];
    static char c_password[32];
    if (isopen) {
        opened = true;
    }
    if (opened) {
        OpenPopup("Add User");
        BeginPopupModal("Add User", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        {

            InputText("Username", c_username, 32);
            InputText("Password", c_password, 32);
            static result res = success;
            static const char* strs[] = { "guest","worker","admin" };
            static const userType prvs[] = { guest,worker,admin };
            static int idx = 0;
            if (BeginCombo("UserType", strs[idx])) {
                for (int i = 0; i < 3; i++) {
                    const bool selected = (idx == i);
                    if (Selectable(strs[i])) {
                        idx = i;
                    }
                    if (selected) {
                        SetItemDefaultFocus();
                    }
                }
            }
            Separator();
            if (res == success) {
                if (Button("Confirm")) {
                    res = StockMan.regist(string(c_username), string(c_password), prvs[idx]);
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                }
                SameLine();
                if (Button("Cancel")) {
                    opened = false;
                    CloseCurrentPopup();
                }
            }
            else {
                switch (res)
                {
                case item_already_exist:
                    Text("This username already exist.Try another one.");
                    break;
                case bad_password:
                    Text("Invalid Password or username,which should only consist of letters and numbers");
                    break;
                default:
                    Text("Unknown error.Retry or restart.");
                    break;
                }
                SameLine();
                if (Button("Got it")) {
                    res = success;
                    memset(c_username, 0, 32 * sizeof(char));
                    memset(c_password, 0, 32 * sizeof(char));
                }
            }
        }
        EndPopup();
    }
}
