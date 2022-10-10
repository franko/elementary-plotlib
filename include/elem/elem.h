/*
** Elementary Plot -- a plotting library for C++. https://github.com/franko/elementary-plotlib
**
** Copyright (C) 2018-2019 Francesco Abbate. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#ifndef ELEM_PLOT_H_
#define ELEM_PLOT_H_

#ifdef __cplusplus
#include "elem/elem_cpp.h"
#include "elem/elem_constants.h"
#include "elem/elem_generator.h"
#else
#include "elem/elem_c.h"
#endif

// On macOS the UI event loops needs to run on the main thread
// so we set the variable above to configure accordingly.
#if __APPLE__
#define ELEM_UI_MAIN_THREAD
#endif

#ifdef ELEM_UI_MAIN_THREAD
#define ELEM_USER_MAIN() static int elem_user_main_()
#define ELEM_GUI_LOOP() int main() { \
    return elem::InitializeAndRun(elem_user_main_); \
}
#else
#define ELEM_USER_MAIN() int main()
#define ELEM_GUI_LOOP()
#endif

#endif
