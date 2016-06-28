/*
# Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef USE_GUI

#ifdef _WIN32
# define NOMINMAX
# include <Windows.h>
#endif

#include "Render/GlfwUIRenderImpl.hpp"
#include "NVXIO/Application.hpp"

nvxio::GlfwUIImpl::GlfwUIImpl(vx_context context, TargetType type, const std::string & name) :
    OpenGLRenderImpl(type, name),
    context_(context),
    window_(NULL),
    keyboardCallback_(NULL),
    keyboardCallbackContext_(NULL),
    mouseCallback_(NULL),
    mouseCallbackContext_(NULL),
    glfwInitialized(false),
    scaleRatio(1.0)
{
}

nvxio::GlfwUIImpl::~GlfwUIImpl()
{
    close();
}

bool nvxio::GlfwUIImpl::open(const std::string& title, vx_uint32 width, vx_uint32 height, vx_uint32 format, bool visible)
{
    NVXIO_ASSERT(format == VX_DF_IMAGE_RGBX);

    windowTitle_ = title;

    if (!nvxio::Application::get().initGui())
    {
        NVXIO_PRINT("Error: Failed to init GUI");
        return false;
    }

    vx_uint32 wndWidth = width, wndHeight = height;
    const GLFWvidmode * mode = NULL;

    if (visible)
    {
        int count = 0;
        GLFWmonitor ** monitors = glfwGetMonitors(&count);

        if (count == 0)
        {
            NVXIO_PRINT("Glfw: no monitors found");
            return false;
        }

        int maxPixels = 0;
        for (int i = 0; i < count; ++i)
        {
            const GLFWvidmode* currentMode = glfwGetVideoMode(monitors[i]);
            int currentPixels = currentMode->width * currentMode->height;

            if (maxPixels < currentPixels)
            {
                mode = currentMode;
                maxPixels = currentPixels;
            }
        }

#ifdef _WIN32
        int clientWidth = GetSystemMetrics(SM_CXFULLSCREEN),
            clientHeight = GetSystemMetrics(SM_CYFULLSCREEN);
#else
        int clientWidth = mode->width, clientHeight = mode->height;
#endif

        if ((width <= (vx_uint32)clientWidth) && (height <= (vx_uint32)clientHeight))
        {
            wndWidth = width;
            wndHeight = height;
        }
        else
        {
            vx_float64 widthRatio = static_cast<vx_float64>(clientWidth) / width;
            vx_float64 heightRatio = static_cast<vx_float64>(clientHeight) / height;
            scaleRatio = std::min(widthRatio, heightRatio);

            wndWidth = static_cast<vx_uint32>(scaleRatio * width);
            wndHeight = static_cast<vx_uint32>(scaleRatio * height);
        }
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef USE_GLES
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    if (!visible)
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    window_ = glfwCreateWindow(wndWidth, wndHeight,
                               windowTitle_.c_str(),
                               NULL, NULL);
    if (!window_)
    {
        NVXIO_PRINT("Error: Failed to create GLFW window");
        glfwInitialized = false;
        return false;
    }

    if (visible)
    {
        // as it's said in documentation, actual window and context
        // parameters may differ from specified ones. So, we need to query
        // actual params and use them later.
        glfwGetFramebufferSize(window_, (int *)&wndWidth, (int *)&wndHeight);

#ifdef _WIN32
        // update sizes
        vx_float64 widthRatio = static_cast<vx_float64>(wndWidth) / width;
        vx_float64 heightRatio = static_cast<vx_float64>(wndHeight) / height;
        scaleRatio = std::min(heightRatio, widthRatio);

        wndHeight = static_cast<vx_uint32>(scaleRatio * height);
        vx_float64 aspectRatio = static_cast<vx_float64>(width) / height;
        wndWidth = static_cast<vx_uint32>(aspectRatio * scaleRatio * height);

        // update window size
        glfwSetWindowSize(window_, wndWidth, wndHeight);
#endif

        NVXIO_ASSERT(mode != nullptr);

        int xpos = (mode->width - wndWidth) >> 1;
        int ypos = (mode->height - wndHeight) >> 1;
        glfwSetWindowPos(window_, xpos, ypos);
    }

    glfwInitialized = true;
    glfwSetWindowUserPointer(window_, this);
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
    glfwMakeContextCurrent(window_);

    // Must be done after glfw is initialized!
    return initGL(context_, wndWidth, wndHeight, visible);
}

void nvxio::GlfwUIImpl::getCursorPos(vx_float64 & x, vx_float64 & y) const
{
    glfwGetCursorPos(window_, &x, &y);

    x /= scaleRatio;
    y /= scaleRatio;
}

void nvxio::GlfwUIImpl::key_fun(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    GlfwUIImpl* impl = static_cast<GlfwUIImpl*>(glfwGetWindowUserPointer(window));

    if (impl->keyboardCallback_ && action == GLFW_PRESS)
    {
        double x = 0, y = 0;
        impl->getCursorPos(x, y);

        if (key == GLFW_KEY_ESCAPE)
            key = 27;

        (impl->keyboardCallback_)(impl->keyboardCallbackContext_, tolower(key),
                                  static_cast<vx_uint32>(x),
                                  static_cast<vx_uint32>(y));
    }
}

void nvxio::GlfwUIImpl::setOnKeyboardEventCallback(OnKeyboardEventCallback callback, void* context)
{
    keyboardCallback_ = callback;
    keyboardCallbackContext_ = context;

    glfwSetKeyCallback(window_, key_fun);
}

void nvxio::GlfwUIImpl::setOnMouseEventCallback(OnMouseEventCallback callback, void* context)
{
    mouseCallback_ = callback;
    mouseCallbackContext_ = context;

    glfwSetMouseButtonCallback(window_, mouse_button);
    glfwSetCursorPosCallback(window_, cursor_pos);
}

void nvxio::GlfwUIImpl::mouse_button(GLFWwindow* window, int button, int action, int /*mods*/)
{
    GlfwUIImpl* impl = static_cast<GlfwUIImpl*>(glfwGetWindowUserPointer(window));

    if (impl->mouseCallback_)
    {
        Render::MouseButtonEvent event = Render::MouseMove;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_RELEASE)
                event = Render::LeftButtonUp;
            else
                event = Render::LeftButtonDown;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_RELEASE)
                event = Render::RightButtonUp;
            else
                event = Render::RightButtonDown;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_RELEASE)
                event = Render::MiddleButtonUp;
            else
                event = Render::MiddleButtonDown;
        }

        double x = 0, y = 0;
        impl->getCursorPos(x, y);

        (impl->mouseCallback_)(impl->mouseCallbackContext_, event,
                               static_cast<vx_uint32>(x),
                               static_cast<vx_uint32>(y));
    }
}

void nvxio::GlfwUIImpl::cursor_pos(GLFWwindow* window, double x, double y)
{
    GlfwUIImpl* impl = static_cast<GlfwUIImpl*>(glfwGetWindowUserPointer(window));

    x /= impl->scaleRatio;
    y /= impl->scaleRatio;

    if (impl->mouseCallback_)
    {
        (impl->mouseCallback_)(impl->mouseCallbackContext_, Render::MouseMove,
                               static_cast<vx_uint32>(x),
                               static_cast<vx_uint32>(y));
    }
}

bool nvxio::GlfwUIImpl::flush()
{
    glfwMakeContextCurrent(window_);

     if (glfwWindowShouldClose(window_))
         return false;

    glfwSwapBuffers(window_);
    glfwPollEvents();

    clearGlBuffer();

    return true;
}

void nvxio::GlfwUIImpl::close()
{
    if (glfwInitialized)
    {
        glfwMakeContextCurrent(window_);
        finalGL();
        glfwDestroyWindow(window_);
        window_ = NULL;
        glfwInitialized = false;
    }
}

void nvxio::GlfwUIImpl::setOpenGlContext()
{
    glfwMakeContextCurrent(window_);
}

#endif // USE_GUI
