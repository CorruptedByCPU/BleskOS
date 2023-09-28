//BleskOS

/*
* MIT License
* Copyright (c) 2023 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void draw_vertical_scrollbar(dword_t x, dword_t y, dword_t heigth, dword_t rider_position, dword_t rider_size);
void draw_horizontal_scrollbar(dword_t x, dword_t y, dword_t width, dword_t rider_position, dword_t rider_size);
dword_t calculate_scrollbar_rider_position(dword_t scrollbar_size, dword_t rider_size, dword_t full_document_size, dword_t showed_document_size, dword_t showed_document_line);
dword_t calculate_scrollbar_rider_size(dword_t scrollbar_size, dword_t full_document_size, dword_t showed_document_size);
dword_t get_scrollbar_rider_value(dword_t scrollbar_size, dword_t rider_size, dword_t rider_position, dword_t full_document_size, dword_t showed_document_size);