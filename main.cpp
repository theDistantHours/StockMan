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
void ShowInStockWindow(bool opt);
void ShowEditUser(bool opt);
void ShowYearlyStat(bool opt);
void ShowMonthlyStat(bool opt);
void ShowWeeklyStat(bool opt);
void ShowStockDetail(bool opt);
void ShowStockManage(bool opt);
void ShowOverView(bool opt);

//this is a modal


result validateLogin(loginToken token, userType type);

//Modal helper functions
void showLoginNeeded(bool isopen = false, userType requiredType = guest);
void showAbout(bool isopen = false);

void showAddCategory(bool isopen = false);
void showRemoveCategory(uid id = 0, bool isopen = false);
void showEditCategory(uid id = 0, bool isopen = false);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

stockMan StockMan;
static ImVec2 vec_content = ImVec2(920, 720);
static ImGuiWindowFlags wndflg_content = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
static bool isloggedin = false;
static bool showloginwindow = true;
static bool showregisterwindow = false;

static bool showInStockWindow = false;

static bool showEditUser = false;
static bool showLoginWindow = false;
static bool showYealystat = false;
static bool showMonthlystat = false;
static bool showWeeklystat = false;
static bool showUsermanage = false;
static bool showLogwindow = false;
static bool showDatabaseSelection = false;
static bool showStockManage = false;

static bool showOverview = true;
static bool refresh_required = true;

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
                        showInStockWindow = true;
                    else
                        showLoginNeeded(true, worker);
                }
                if (MenuItem("Out Stock")) {
                    if (validateLogin(currentLoginToken, worker) == success)
                        showInStockWindow = true;
                    else
                        showLoginNeeded(true, worker);

                }
                ImGui::EndMenu();
            }
            if (BeginMenu("Edit")) {
                if (ImGui::MenuItem("Add/Remove Category")) {
                    if (validateLogin(currentLoginToken, worker) == success)showStockManage = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Attribute")) {
                    if (validateLogin(currentLoginToken, worker) == success)showStockManage = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Add/Remove Items")) {
                    if (validateLogin(currentLoginToken, worker) == success)showStockManage = true;
                    else
                    {
                        showLoginNeeded(true, worker);
                    }
                }
                if (ImGui::MenuItem("Edit Item Properties")) {
                    if (validateLogin(currentLoginToken, worker) == success)showStockManage = true;
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
                    if (validateLogin(currentLoginToken, admin) == success)
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
        showLoginNeeded();
        showAbout();

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
                        if (!validateLogin(currentLoginToken, worker))showLoginNeeded(1, worker);
                        else {
                            showInStockWindow = true;
                        }
                    }
                    if (Button("View Log", ImVec2(300, 137))) {
                        if (!validateLogin(currentLoginToken, admin)) 
                            showLoginNeeded(1, admin);
                        
                        else {
                            showLogwindow = true;
                        }
                    }
                    if (Button("Manage Stock \n       Info", ImVec2(300, 137))) {
                        if (!validateLogin(currentLoginToken, worker))showLoginNeeded(1, worker);
                        else {
                            showStockManage = true;
                        }
                    }
                    if (Button("Manage Users\n &Database", ImVec2(300, 137))) {
                        if (!validateLogin(currentLoginToken, admin))showLoginNeeded(1, admin);
                        else {
                            showEditUser = true;
                        }
                    }
                    PopFont();
                }
                EndChild();
                SameLine();
                ShowOverView(showOverview);
                ShowLogWindow(showLogwindow);
                ShowInStockWindow(showInStockWindow);
                ShowEditUser(showEditUser);
                ShowYearlyStat(showYealystat);
                ShowMonthlyStat(showMonthlystat);
                ShowWeeklyStat(showWeeklystat);
                ShowStockManage(showStockManage);
            }
            End();
        }

#pragma endregion
        PopFont();
        ImGui::PopStyleVar();


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
    if (isopen)usrType = requiredType;
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
void showAddCategory(bool isopen)
{
    static bool opened = false;
    static result res = success;
    static string hint;
    if (isopen)opened = true;
    if (opened) {
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
                }
                Separator();
            }
            else {
                if (Button("Confirm")) {
                    dest.type_name = string(catename);
                    dest.type_desc = string(catedesc);

                    static result res = StockMan.addCategory(dest, currentLoginToken, string(com));
                    if (res == success) {
                        opened = false;
                        CloseCurrentPopup();
                    }
                    else {
                        switch (res)
                        {
                        case category_already_exist:
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
            Text("Are you sure to remove category %s?\nAll items inside it will be deleted.\nThis operation cannot be undone.",StockMan.getCategoryName(dest_id).c_str());
            static char comment[32];
            static string strcomment;
            InputText("(Enter your comment here)", comment, 32);
            if (Button("Yes")) {
                refresh_required = true;
                strcomment = string(comment);
                if (strcomment.size() == 0)strcomment = "No comment";
                StockMan.removeCategory(dest_id, currentLoginToken, string(comment));
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
                        if (res == category_already_exist) {
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
        EndChild();
    }

}

void ShowInStockWindow(bool opt) {
    if (opt) {
        BeginChild("In/Out Stock", vec_content, true, wndflg_content);

        EndChild();
    }
}

void ShowEditUser(bool opt) {
    if (opt) {
        BeginChild("Edit Users", vec_content, true, wndflg_content);

        EndChild();
    }
}

void ShowYearlyStat(bool opt) {
    if (opt) {
        BeginChild("Yearly Statistics", vec_content, true, wndflg_content);

        EndChild();
    }
}

void ShowMonthlyStat(bool opt) {
    if (opt) {
        BeginChild("Monthly Statistics", vec_content, true, wndflg_content);

        EndChild();
    }
}

void ShowWeeklyStat(bool opt) {
    if (opt) {
        BeginChild("Weekly Statistics", vec_content, true, wndflg_content);

        EndChild();
    }
}
void ShowStockDetail(bool opt) {
    if (opt) {
        BeginChild("Stock Details", vec_content, true, wndflg_content);

        EndChild();
    }
}
void ShowStockManage(bool opt) {
    if (opt) {
        static vector<stockAttr> Attributes;
        static vector<bool*> check;
        static vector<stockCategory> Categories;
        static vector<stockItem> current_items;

        static uid current_cate;
        static vector<uid> current_attrs;
        if (refresh_required) {
            current_items = StockMan.getItems(current_cate, current_attrs);
            check.resize(Attributes.size());
            for (int i = 0; i < check.size(); i++) {
                check[i] = new bool;
                *check[i] = true;
            }
            Categories = StockMan.getCategories();
            Attributes = StockMan.getAttributes();
        }

        BeginChild("Stock Management", vec_content, true, wndflg_content);
        static ImVec2 vec_content2 = ImVec2(300, 700);
        BeginChild("Stock Categories", vec_content2, false);
        {
            
            static ImVec2 vec_button = ImVec2(280, 100);
            int sz = Categories.size();
            for (int i = 0; i < sz; i++) {
                stockCategory current = Categories[i];
                if (Button(current.type_name.c_str(), vec_button)) {
                    current_cate = current.type_id;
                }
                if (BeginPopupContextItem((string("category_") + to_string(i)).c_str())) {
                    if (Selectable("Add category")) {
                        showAddCategory(true);
                    }
                    if (Selectable("Remove cateogry")) {
                        showRemoveCategory(current.type_id,true);
                    }
                    if (Selectable("Edit category")) {
                        showEditCategory(current.type_id, true);
                    }
                    EndPopup();
                }
            }
        }
        EndChild();
        static ImVec2 vec_content3 = ImVec2(400, 700);
        BeginChild("Stock content", vec_content3, false);
        {
            static ImVec2 vec_checkbox = ImVec2(380, 100);
            static ImVec2 vec_innercontent = ImVec2(380, 500);
            BeginChild("Checkbox", vec_checkbox, false);
            {
                int sz = Attributes.size();
                for (int i = 0; i < sz; i++) {
                    Checkbox(Attributes[i].attr_name.c_str(), check[i]);
                }
            }
            EndChild();
            Separator();
            BeginChild("inner stock content", vec_innercontent, false);
            {
                int sz = current_items.size();
                BeginTable("content_table", 4);
                for (int i = 0; i < sz; i++) {
                    TableNextColumn();
                    if (Selectable(current_items[i].item_name.c_str())) {

                    }
                    if (BeginPopupContextItem((string("itemview_") + to_string(i)).c_str())) {
                        if (Selectable("view details")) {

                        }
                        if (Selectable("remove item")) {

                        }
                        if (Selectable("edit information")) {

                        }
                        EndPopup();
                    }
                }
            }
            EndChild();
        }
        EndChild();

        EndChild();
    }
}
void ShowOverView(bool opt) {
    if (opt) {
        static bool isfirstopen = true;
        if (isfirstopen) {
            //refresh overview here
        }
        BeginChild("OverView", vec_content, true, wndflg_content);
        static vector<tuple<infoType, stockCategory, stockItem, int, int>> info;
        int sz = info.size();
        for (int i = 0; i < sz; i++) {
            BeginChild(i * 667, ImVec2(900, 200), true);
            {
                switch (get<0>(info[i]))
                {
                case outdate:
                    Text("Item %s %s is partially out of durance.\nMax-durance: %d ,outdatad items count: %d", get<1>(info[i]).type_name.c_str(), get<2>(info[i]).item_name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                case notenough:
                    Text("Item %s %s is around empty.\nCurrent remaining: %d ,Minimun required: %d\nPlease add in time.", get<1>(info[i]).type_name.c_str(), get<2>(info[i]).item_name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                case full:
                    Text("Item %s %s is already full.\nCurrent count: %d\nMax-capacity: %d", get<1>(info[i]).type_name.c_str(), get<2>(info[i]).item_name.c_str(), get<3>(info[i]), get<4>(info[i]));
                    break;
                default:
                    break;
                }
            }
            EndChild();
        }
        if (Button("refresh")) {
              //
        }
        EndChild();
    }
}
