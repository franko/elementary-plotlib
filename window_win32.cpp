
static HINSTANCE g_windows_instance = 0;
static int g_windows_cmd_show = 0;

LRESULT CALLBACK window_w32::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC paintDC;

    void* user_data = reinterpret_cast<void*>(::GetWindowLong(hWnd, GWL_USERDATA));
    window_win32* win = (user_data ? reinterpret_cast<window_win32*>(user_data) : 0);

    if(win == 0) {
        if(msg == WM_DESTROY) {
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    HDC dc = ::GetDC(win->m_hwnd);
    win->m_current_dc = dc;
    LRESULT ret = 0;

    switch(msg)
    {
        //--------------------------------------------------------------------
    case WM_CREATE:
        break;

        //--------------------------------------------------------------------
    case WM_SIZE:
        app->m_specific->create_pmap(LOWORD(lParam), HIWORD(lParam),
                                     &app->rbuf_window());

        app->trans_affine_resizing(LOWORD(lParam), HIWORD(lParam));
        app->on_resize(LOWORD(lParam), HIWORD(lParam));
        app->force_redraw();
        app->m_specific->m_is_ready = false;

        break;

        //--------------------------------------------------------------------
    case WM_ERASEBKGND:
        break;

        //--------------------------------------------------------------------
    case WM_PAINT:
        paintDC = ::BeginPaint(hWnd, &ps);
        app->m_specific->m_current_dc = paintDC;
        if(app->m_specific->m_redraw_flag)
        {
            app->on_draw();
            app->m_specific->m_redraw_flag = false;
        }
        app->m_specific->display_pmap(paintDC, &app->rbuf_window());
        app->on_post_draw(paintDC);
        app->m_specific->m_current_dc = 0;
        ::EndPaint(hWnd, &ps);

        app->m_specific->m_is_mapped = true;
        app->m_specific->m_is_ready = true;
        break;

        //--------------------------------------------------------------------
    case WM_COMMAND:
        break;

        //--------------------------------------------------------------------
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

        //--------------------------------------------------------------------
    default:
        ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }
    app->m_specific->m_current_dc = 0;
    ::ReleaseDC(app->m_specific->m_hwnd, dc);

    return ret;
}

bool window_win32::init(unsigned width, unsigned height, unsigned flags)
{
    m_window_flags = flags;

    int wflags = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

    WNDCLASS wc;
    wc.lpszClassName = "PlotAppClass";
    wc.lpfnWndProc = (WNDPROC) wnd_proc;
    wc.style = wflags;
    wc.hInstance = g_windows_instance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = "PlotppMenu";
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    ::RegisterClass(&wc);

    wflags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    if(m_window_flags & window_resize)
    {
        wflags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    const char* caption = m_caption.cstr();
    m_hwnd = ::CreateWindow("PlotAppClass", caption, wflags, 100, 100, width, height, 0, 0, g_windows_instance, 0);

    if(m_hwnd == 0) {
        return false;
    }

    RECT rct;
    ::GetClientRect(m_hwnd, &rct);

    ::MoveWindow(m_hwnd,               // handle to window
                 100,                  // horizontal position
                 100,                  // vertical position
                 width + (width - (rct.right - rct.left)),
                 height + (height - (rct.bottom - rct.top)),
                 FALSE);

    ::SetWindowLong(m_hwnd, GWL_USERDATA, (LONG)this);

    resize(width, height);
    m_target.render();

#if 0
    m_specific->create_pmap(width, height, &m_rbuf_window);
    m_initial_width = width;
    m_initial_height = height;
    on_init();
    m_specific->m_redraw_flag = true;
#endif

    ::ShowWindow(m_hwnd, g_windows_cmd_show);
    ::SetForegroundWindow(m_hwnd);
    return true;
}

void
window_win32::prepare()
{
    if (g_windows_instance == 0)
    {
        g_windows_instance = GetModuleHandle(NULL);
        g_windows_cmd_show = SW_SHOWNORMAL;
    }
}
