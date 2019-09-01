## Window surface stuff

Inside the graphics namespace:

```c++
struct display_window {
    virtual void update_region(image& img, const agg::rect_i& r) = 0;
    virtual void update_region_request(image& img, const agg::rect_i& r) = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual int status() = 0;
};

class window_surface {
    bool resize(unsigned ww, unsigned hh);
    void render();
    void draw();

    void attach_window(display_window* win);

    int attach(plot* p, const char* slot_str);

    void slot_refresh(unsigned index);
    void slot_update(unsigned index);
    void save_slot_image(unsigned index);
    void restore_slot_image(unsigned index);
private:
    window_part m_part;
    agg::pod_bvector<plot_ref> m_plots;
    display_window* m_window;
    canvas* m_canvas;
};
```

## XWindow implementation

Important: classes that contains a window_surface will call the function

```c++
// m_window is passed as a graphics::display_window.
m_surface.attach_window(m_window);
```

```c++
class xwindow : public graphics::display_window {
    xwindow(graphics::window_surface& window_surface);
private:
    graphics::window_surface& m_target;
};
```

## Top level Window wrapper

Template class. Can use either window_win32 or xwindow and template's argument.

```c++
class window_gen<xwindow> {
public:
    // attach and slot_refresh functions just calls the window_surface's functions
    // on the same name.
    int attach(graphics::plot* p, const char* slot_str);
    void slot_refresh(unsigned index);

    void start(unsigned width, unsigned height, unsigned flags);
    void wait();

private:
    graphics::window_surface m_surface;
    Window m_window;
};
```
