
class window_win32 {
public:
    window_win32(render_target& tgt);
    ~window_win32();

    bool init(unsigned width, unsigned height, unsigned flags);

    static void prepare();
    static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void run();
    void close();

    void update_region(graphics::image& src_img, const agg::rect_i& r);

    void lock()   { m_mutex.lock();   }
    void unlock() { m_mutex.unlock(); }

private:
    void create_pmap(unsigned width, unsigned height,
                     rendering_buffer* wnd);

    void display_pmap(HDC dc, const rendering_buffer* src,
                      const agg::rect_base<int> *rect = 0);

    bool save_pmap(const char* fn, unsigned idx,
                   const rendering_buffer* src);

    void close();

    unsigned      m_window_flags;
    str           m_caption;

    HDC           m_current_dc;
#if 0
    pix_format_e  m_format;
    pix_format_e  m_sys_format;
    bool          m_flip_y;
    unsigned      m_bpp;
    unsigned      m_sys_bpp;
    HWND          m_hwnd;
    pixel_map     m_pmap_window;
    BITMAPINFO*   m_bmp_draw;
    pixel_map     m_pmap_img[platform_support::max_images];
    int           m_cur_x;
    int           m_cur_y;
    unsigned      m_input_flags;
    bool          m_redraw_flag;
    LARGE_INTEGER m_sw_freq;
    LARGE_INTEGER m_sw_start;

    bool m_is_mapped;
    bool m_is_ready;

#endif
    pthread::mutex m_mutex;

    static void bitmap_info_resize (BITMAPINFO* bmp, unsigned w, unsigned h);

};