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

struct render_target {
    virtual bool resize(unsigned width, unsigned height) = 0;
    virtual void render() = 0;
    virtual void draw() = 0;
};

class window_surface : public render_target {
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

```c++
class xwindow : public graphics::display_window {
private:
    graphics::render_target& m_target;
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
