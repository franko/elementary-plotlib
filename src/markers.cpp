
#include <math.h>

#include "agg_path_storage.h"
#include "agg_ellipse.h"

#include "markers.h"
#include "trans.h"
#include "sg_path.h"

struct symbol_reg {
    const char *name;
    canvas_object *(*builder)(bool&);
};

static canvas_object *build_circle(bool& stroke);
static canvas_object *build_square(bool& stroke);
static canvas_object *build_triangle(bool& stroke);
static canvas_object *build_diamond(bool& stroke);
static canvas_object *build_plus(bool& stroke);
static canvas_object *build_cross(bool& stroke);

const unsigned NB_SYMBOLS = 6;
static struct symbol_reg builder_table[NB_SYMBOLS+1] = {
    {"circle",   build_circle},
    {"square",   build_square},
    {"triangle", build_triangle},
    {"diamond",  build_diamond},
    {"plus",     build_plus},
    {"cross",    build_cross},
    {NULL, NULL}
};

canvas_object *
build_circle(bool& stroke)
{
    sg_ellipse* c = new sg_ellipse();
    c->self().init(0.0, 0.0, 0.5, 0.5);
    stroke = false;
    return c;
}

canvas_object *
build_square(bool& stroke)
{
    sg_path* p = new sg_path();

    agg::path_storage& square = p->self();
    square.move_to(-0.5, -0.5);
    square.line_to( 0.5, -0.5);
    square.line_to( 0.5,  0.5);
    square.line_to(-0.5,  0.5);
    square.close_polygon();

    stroke = false;
    return p;
}

canvas_object *
build_triangle(bool& stroke)
{
    sg_path* p = new sg_path();

    agg::path_storage& triangle = p->self();

    double ht = 0.86602540378444;
    triangle.move_to(-0.5, -ht/3);
    triangle.line_to( 0.5, -ht/3);
    triangle.line_to( 0.0,  2*ht/3);
    triangle.close_polygon();

    stroke = false;
    return p;
}

canvas_object *
build_diamond(bool& stroke)
{
    sg_path* p = new sg_path();

    agg::path_storage& square = p->self();
    square.move_to(-0.5,  0.0);
    square.line_to( 0.0,  0.5);
    square.line_to( 0.5,  0.0);
    square.line_to( 0.0, -0.5);
    square.close_polygon();

    stroke = false;
    return p;
}

canvas_object *
build_plus(bool& stroke)
{
    sg_path* p = new sg_path();

    agg::path_storage& plus = p->self();
    plus.move_to(-0.5,  0.0);
    plus.line_to( 0.5,  0.0);
    plus.move_to( 0.0, -0.5);
    plus.line_to( 0.0,  0.5);

    stroke = true;
    return p;
}

canvas_object *
build_cross(bool& stroke)
{
    sg_path* p = new sg_path();

    agg::path_storage& plus = p->self();
    plus.move_to(-0.5, -0.5);
    plus.line_to( 0.5,  0.5);
    plus.move_to(-0.5,  0.5);
    plus.line_to( 0.5, -0.5);

    stroke = true;
    return p;
}

canvas_object*
new_marker_symbol_raw(const char *req_name, bool& stroke)
{
    struct symbol_reg *reg;
    for (reg = builder_table; reg->name != NULL; reg++)
    {
        if (strcmp (reg->name, req_name) == 0)
            return reg->builder(stroke);
    }

    return builder_table[0].builder(stroke);
}

canvas_object*
new_marker_symbol (const char *req_name)
{
    bool stroke;
    canvas_object* s = new_marker_symbol_raw(req_name, stroke);

    graphics::transform::scaling *ss = new graphics::transform::scaling(s);
    canvas_object* sf = ss;
    if (stroke)
        sf = new graphics::transform::stroke(sf);
    return sf;
}

canvas_object*
new_marker_symbol (int n)
{
    n = (n-1) % NB_SYMBOLS;
    return new_marker_symbol(builder_table[n].name);
}
