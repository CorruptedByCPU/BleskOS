//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void program_layout_set_dimensions(dword_t x, dword_t y, dword_t width, dword_t height) {
 program_layout_draw_x = program_layout_x = x;
 program_layout_draw_y = program_layout_y = y;
 program_layout_width = width;
 program_layout_height = height;
}

void program_layout_set_dimensions_window(dword_t width, dword_t height) {
 program_layout_draw_x = program_layout_x = (graphic_screen_x_center-(width/2)+10);
 program_layout_draw_y = program_layout_y = (graphic_screen_y_center-(height/2)+10);
 program_layout_width = (width-20);
 program_layout_height = (height-20);
}

void program_layout_add_element(dword_t height) {
 program_layout_draw_x = program_layout_x;
 program_layout_draw_y += (height+10);
}

void program_layout_add_text_line(void) {
 program_layout_add_element(10);
}

void program_layout_add_button(void) {
 program_layout_add_element(20);
}