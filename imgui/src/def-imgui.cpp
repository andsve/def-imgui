// imgui.cpp
// Extension lib defines
#define LIB_NAME "imgui"
#define MODULE_NAME "imgui"

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// include the Defold SDK
#include <dmsdk/sdk.h>
// #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "imgui/imgui.h"
extern "C" {
    #include "cimgui.h"
}
// #define
// #include "imgui/cimgui.h"
// #include "imgui/imgui_impl_defold.h"
#include "imgui/imgui_impl_opengl2.h"

static bool                 g_MouseJustPressed[5] = { false, false, false, false, false };
// static GLFWcursor*          g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

static int ImguiSetMouseInput(lua_State* L)
{
    int top = lua_gettop(L);
    ImGuiIO& io = ImGui::GetIO();

    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    if (io.WantSetMousePos)
    {
        return luaL_error(L, "WantSetMousePos not supported yet.");
    } else {
        io.MousePos = ImVec2(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
    }

    io.MouseDown[0] = luaL_checknumber(L, 3);
    io.MouseDown[1] = luaL_checknumber(L, 4);
    io.MouseDown[2] = luaL_checknumber(L, 5);

    // io.MouseWheelH += luaL_checknumber(L, 3);
    io.MouseWheel += luaL_checknumber(L, 6);

    /*
    // get mouse button field
    luaL_checktable(L, 1);
    lua_getfield(L, 1, "mouse_buttons");

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }
    */

    assert(top == lua_gettop(L));
    return 0;
}

static int ImguiTest(lua_State* L)
{
    int top = lua_gettop(L);
    ImGuiIO& io = ImGui::GetIO();


    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");
    io.DisplaySize = ImVec2(luaL_checknumber(L, 1), luaL_checknumber(L, 2));

    ImGui::ShowDemoWindow();
    assert(top == lua_gettop(L));
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"set_mouse_input", ImguiSetMouseInput},
    {"test", ImguiTest},
    {0, 0}
};


static void LuaInit(lua_State* L)
{
    printf("igGetVersion: %s\n", igGetVersion());
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeImgui(dmExtension::AppParams* params)
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeImgui(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(960, 640);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result UpdateImgui(dmExtension::Params* params)
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
    return dmExtension::RESULT_OK;
}

static void PostRenderImgui(dmExtension::AppParams* params)
{
    // if (event->m_Event == dmExtension::EVENT_ID_POST_RENDER) {
        // clean state
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // ImGui_ImplOpenGL2_NewFrame();
        // ImGui::NewFrame();
        // ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    // }
}

static dmExtension::Result AppFinalizeImgui(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeImgui(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// imgui is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION2(imgui, LIB_NAME, AppInitializeImgui, AppFinalizeImgui, InitializeImgui, UpdateImgui, 0, FinalizeImgui, PostRenderImgui)
