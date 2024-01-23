//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák, defdefred
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void initalize_performance_rating(void) {
  PERFORMANCE_RATING_NBTASK_PER_PAGE=(graphic_screen_y-26)/10;
}

void performance_rating(void) {
  redraw_performance_rating();

  while(1) {
    wait_for_user_input();
    move_mouse_cursor();

    dword_t click_value = get_mouse_cursor_click_board_value();
    if(mouse_click_button_state==MOUSE_CLICK && click_value==PERFORMANCE_RATING_CLICK_ZONE_BACK) {
      return;
    }

    switch (keyboard_value) {
      case KEY_UP:
        if (PERFORMANCE_RATING_CURRENT_TASK > 0) { PERFORMANCE_RATING_CURRENT_TASK--; };
        redraw_performance_rating();
        break;
      case KEY_DOWN:
        if (PERFORMANCE_RATING_CURRENT_TASK < PERFORMANCE_RATING_NBTASK-1) { PERFORMANCE_RATING_CURRENT_TASK++; };
        redraw_performance_rating();
        break;
      case KEY_ENTER:
        performance_rating_run_task(PERFORMANCE_RATING_CURRENT_TASK);
        redraw_performance_rating();
        break;
      case KEY_I:
        performance_rating_inspect_task(PERFORMANCE_RATING_CURRENT_TASK);
        redraw_performance_rating();
        break;
      case KEY_A:
        PERFORMANCE_RATING_CURRENT_TASK=0;
        redraw_performance_rating();
        for (; PERFORMANCE_RATING_CURRENT_TASK<PERFORMANCE_RATING_NBTASK; PERFORMANCE_RATING_CURRENT_TASK++) {
          performance_rating_run_task(PERFORMANCE_RATING_CURRENT_TASK);
          redraw_performance_rating();
          wait_for_user_input();
          move_mouse_cursor();
			 if( keyboard_value == KEY_ESC ) {break;}
        }
        break;
      case KEY_PAGE_UP:
        break;
      case KEY_PAGE_DOWN:
        break;
      case KEY_ESC:
        return;
    }
  }
}

void redraw_performance_rating(void) {
  dword_t color,line=2, page, task;
  clear_screen(0xbb00dd);
  clear_click_board();

  print("Results are time in microseconds for 1x run and 128x runs", 10, line, BLACK);
  print("x1", graphic_screen_x-10*8*2, line, BLACK);
  print("x128", graphic_screen_x-10*8, line, BLACK);
  draw_line(0,12,graphic_screen_x,12,BLACK);
  line+=14;

  page=PERFORMANCE_RATING_CURRENT_TASK / PERFORMANCE_RATING_NBTASK_PER_PAGE; 

  for (task=page*PERFORMANCE_RATING_NBTASK_PER_PAGE; task<PERFORMANCE_RATING_NBTASK && task<(page+1)*PERFORMANCE_RATING_NBTASK_PER_PAGE; task++) {
    if ( task == PERFORMANCE_RATING_CURRENT_TASK ) { color=WHITE; } else { color=BLACK; };
    print(performance_rating_tasks[task].name, 10, line, color);
    print_var(performance_rating_tasks[task].result >> 7, graphic_screen_x-10*8*2, line, color);
    print_var(performance_rating_tasks[task].result, graphic_screen_x-10*8, line, color);
    line+=10;
  }

  print("[ESC] Quit | [ARROWS] Choose | [ENTER] Run | [I] Inspect | [A] Run all", 10, graphic_screen_y-12, BLACK);
  add_zone_to_click_board(10, graphic_screen_y-12, 11*8, 8, PERFORMANCE_RATING_CLICK_ZONE_BACK);
  redraw_screen();
}

void performance_rating_run_task(dword_t task_number) {  // get time of 128 runs
  message_window("Running tasks...");
  redraw_screen();
  mouse_movement_x = graphic_screen_x - mouse_cursor_x - (MOUSE_CURSOR_WIDTH>>1);
  mouse_movement_y = graphic_screen_y - mouse_cursor_y - (MOUSE_CURSOR_HEIGTH>>1);
  move_mouse_cursor(); // mouse need to be on expected position
  reset_timer();

  for(PERFORMANCE_RATING_CURRENT_RUN=0; PERFORMANCE_RATING_CURRENT_RUN<PERFORMANCE_RATING_RUN_COUNT; PERFORMANCE_RATING_CURRENT_RUN++) {
    performance_rating_tasks[task_number].run();
  }
  performance_rating_tasks[task_number].result = get_timer_value_in_microseconds();
}

void performance_rating_inspect_task(dword_t task_number) {  // get time of 128 runs
  message_window("Running tasks...");
  redraw_screen();
  mouse_movement_x = graphic_screen_x - mouse_cursor_x - (MOUSE_CURSOR_WIDTH>>1);
  mouse_movement_y = graphic_screen_y - mouse_cursor_y - (MOUSE_CURSOR_HEIGTH>>1);
  move_mouse_cursor(); // mouse need to be on expected position

  for(PERFORMANCE_RATING_CURRENT_RUN=0; PERFORMANCE_RATING_CURRENT_RUN<PERFORMANCE_RATING_RUN_COUNT; PERFORMANCE_RATING_CURRENT_RUN++) {
    clear_screen(0x123456+PERFORMANCE_RATING_CURRENT_RUN<<5); // to make screen redraw visible
    performance_rating_tasks[task_number].run();
    redraw_screen();
  }
}

void performance_rating_task0() {
  clear_screen(0x1234566+PERFORMANCE_RATING_CURRENT_RUN<<5);
}

void performance_rating_task1() { // on screen center
  mouse_movement_x = -3;
  mouse_movement_y = -3; 
  move_mouse_cursor();
}

void performance_rating_task2() { // on screen border rigth
  mouse_movement_y = -3;
  move_mouse_cursor();
}

void performance_rating_task3() { //on screen border bottom
  mouse_movement_x = -3;
  move_mouse_cursor();
}

void performance_rating_task4() { // full screen
  redraw_screen();
}

void performance_rating_task5() { // 25% screen center
  redraw_part_of_framebuffer( graphic_screen_x_center>>1, graphic_screen_y_center>>1, graphic_screen_x>>1, graphic_screen_y>>1);
}

void performance_rating_task6() { // 25% screen half rigth
  redraw_part_of_framebuffer( graphic_screen_x_center+(graphic_screen_x_center>>1), graphic_screen_y_center>>1, graphic_screen_x>>1, graphic_screen_y>>1);
}

void performance_rating_task7() { // 25% screen half bottom
  redraw_part_of_framebuffer( graphic_screen_x_center>>1, graphic_screen_y_center+(graphic_screen_y_center>>1), graphic_screen_x>>1, graphic_screen_y>>1);
}

void performance_rating_task8() { //  Scalable font print
  scalable_font_print("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789", graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task9() { // bitmap font print
  print("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789", graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task10() { // print numeric variable 
  print_var(3473898+(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task11() { // print unicode string ÀÁÂÃÄÅÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝßàáâãäåèéêëìíîïñòóôõöùúûüýÿčďěľňŕřšťž
  print_unicode((word_t *)"\uc000\uc100\uc200\uc300\uc400\uc500\uc800\uc900\uca00\ucb00\ucc00\ucd00\uce00\ucf00\ud100\ud200\ud300\ud400\ud500\ud600\ud900\uda00\udb00\udc00\udd00\udf00\ue000\ue100\ue200\ue300\ue400\ue500\ue800\ue900\uea00\ueb00\uec00\ued00\uee00\uef00\uf100\uf200\uf300\uf400\uf500\uf600\uf900\ufa00\ufb00\ufc00\ufd00\uff00\u0d01\u0f01\u1b01\u3e01\u4801\u5501\u5901\u6101\u6501\u7e01",
    graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task12() { // print hexa number
  print_hex(3467423+(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task13() { // print ascii
  print_ascii("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789", graphic_screen_x_center-(PERFORMANCE_RATING_CURRENT_RUN<<2), graphic_screen_y-(PERFORMANCE_RATING_CURRENT_RUN<<1), BLACK);  
}
void performance_rating_task14() { // todo 
}
void performance_rating_task15() { // todo 
}
