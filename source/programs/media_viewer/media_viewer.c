//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void initalize_media_viewer(void) {
 media_viewer_program_interface_memory = create_program_interface_memory(((dword_t)&draw_media_viewer), PROGRAM_INTERFACE_FLAG_NO_NEW_BUTTON);
 media_viewer_sound_state = MEDIA_VIEWER_SOUND_NO_FILE;
}

void media_viewer(void) {
 set_program_interface(media_viewer_program_interface_memory);
 set_program_value(PROGRAM_INTERFACE_CHANGE_BETWEEN_FILES_METHOD_POINTER, (dword_t)media_viewer_change_between_files);
 program_interface_add_keyboard_event(KEY_F1, (dword_t)media_viewer_open_file);
 program_interface_add_keyboard_event(KEY_F2, (dword_t)media_viewer_save_file);
 program_interface_add_keyboard_event(KEY_F4, (dword_t)media_viewer_close_file);
 program_interface_add_keyboard_event(KEY_H, (dword_t)media_viewer_image_reverse_horizontally);
 program_interface_add_keyboard_event(KEY_V, (dword_t)media_viewer_image_reverse_vertically);
 program_interface_add_keyboard_event(KEY_L, (dword_t)media_viewer_image_turn_left);
 program_interface_add_keyboard_event(KEY_R, (dword_t)media_viewer_image_turn_right);
 program_interface_add_keyboard_event(KEY_UP, (dword_t)media_viewer_key_up_event);
 program_interface_add_keyboard_event(KEY_DOWN, (dword_t)media_viewer_key_down_event);
 program_interface_add_keyboard_event(KEY_LEFT, (dword_t)media_viewer_key_left_event);
 program_interface_add_keyboard_event(KEY_RIGHT, (dword_t)media_viewer_key_right_event);
 program_interface_add_keyboard_event(KEY_SPACE, (dword_t)media_viewer_key_space_event);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_IMAGE_REVERSE_HORIZONTALLY, (dword_t)media_viewer_image_reverse_horizontally);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_IMAGE_REVERSE_VERTICALLY, (dword_t)media_viewer_image_reverse_vertically);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_IMAGE_TURN_LEFT, (dword_t)media_viewer_image_turn_left);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_IMAGE_TURN_RIGHT, (dword_t)media_viewer_image_turn_right);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_SOUND_PROGRESS, (dword_t)media_viewer_click_on_sound_progress_square);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_SOUND_VOLUME_MINUS_10, (dword_t)media_viewer_key_left_event);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_SOUND_VOLUME_PLUS_10, (dword_t)media_viewer_key_right_event);
 program_interface_add_click_zone_event(MEDIA_VIEWER_CLICK_ZONE_SOUND_PLAY_STOP, (dword_t)media_viewer_key_space_event);
 program_interface_redraw();

 while(1) {
  wait_for_user_input();
  move_mouse_cursor();

  //close program
  if(keyboard_code_of_pressed_key==KEY_ESC || (mouse_click_button_state==MOUSE_CLICK && get_mouse_cursor_click_board_value()==CLICK_ZONE_BACK)) {
   media_viewer_stop_sound();
   return;
  }

  //process interface events
  program_interface_process_keyboard_event();
  program_interface_process_mouse_event();

  //process mouse events on image
  if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
   dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));

   //move image by mouse dragging
   if(mouse_click_button_state==MOUSE_DRAG && get_program_value(PROGRAM_INTERFACE_SELECTED_CLICK_ZONE)==MEDIA_VIEWER_CLICK_ZONE_IMAGE) {
    //move horizontally
    if(image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]!=0) {
     dword_t movement = (mouse_cursor_x_dnd-mouse_cursor_x_previous_dnd);
     image_info[IMAGE_INFO_DRAW_X] -= movement;
     if(movement<0x80000000) {
      if(image_info[IMAGE_INFO_DRAW_X]>image_info[IMAGE_INFO_WIDTH]) {
       image_info[IMAGE_INFO_DRAW_X] = 0;
      }
     }
     else if(image_info[IMAGE_INFO_DRAW_X]>(image_info[IMAGE_INFO_WIDTH]-image_info[IMAGE_INFO_DRAW_WIDTH])) {
      image_info[IMAGE_INFO_DRAW_X] = (image_info[IMAGE_INFO_WIDTH]-image_info[IMAGE_INFO_DRAW_WIDTH]);
     }
    }

    //move vertically
    if(image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]!=0) {
     dword_t movement = (mouse_cursor_y_dnd-mouse_cursor_y_previous_dnd);
     image_info[IMAGE_INFO_DRAW_Y] -= movement;
     if(movement<0x80000000) {
      if(image_info[IMAGE_INFO_DRAW_Y]>image_info[IMAGE_INFO_HEIGTH]) {
       image_info[IMAGE_INFO_DRAW_Y] = 0;
      }
     }
     else if(image_info[IMAGE_INFO_DRAW_Y]>(image_info[IMAGE_INFO_HEIGTH]-image_info[IMAGE_INFO_DRAW_HEIGTH])) {
      image_info[IMAGE_INFO_DRAW_Y] = (image_info[IMAGE_INFO_HEIGTH]-image_info[IMAGE_INFO_DRAW_HEIGTH]);
     }
    }

    //calculate actual scrollbars
    calculate_image_scrollbars((dword_t)image_info);

    //redraw changes
    media_viewer_draw_image();
   }

   //change zoom of image
   if(mouse_wheel!=0) {
    if(mouse_wheel<0x80000000 && get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM)<800) {
     set_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM, get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM)+10);
     media_viewer_image_recalculate_zoom();
     program_interface_redraw();
    }
    else if(get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM)>10) {
     set_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM, get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM)-10);
     media_viewer_image_recalculate_zoom();
     program_interface_redraw();
    }
   }
  }
 }
}

void draw_media_viewer(void) {
 clear_program_interface_before_drawing();
 draw_program_interface("Media viewer", "", 0x0089FF, BLACK);

 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)==0) {
  print("Supported file formats", 10, 30, WHITE);
  print("Images: JPG/PNG/GIF/BMP/QOI", 10, 50, WHITE);
  print("Sound: MP3/WAV/CDDA", 10, 70, WHITE);
 }
 else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));

  //draw buttons
  draw_full_square(0, 20, screen_width, 20, 0x00C0FF);
  draw_button("[l] Turn left", 8, 22, 120, 16);
  add_zone_to_click_board(8, 22, 120, 16, MEDIA_VIEWER_CLICK_ZONE_IMAGE_TURN_LEFT);
  draw_button("[r] Turn right", 8+120+8, 22, 120, 16);
  add_zone_to_click_board(8+120+8, 22, 120, 16, MEDIA_VIEWER_CLICK_ZONE_IMAGE_TURN_RIGHT);
  draw_button("[h] Reverse left-right", 8+120+8+120+8, 22, 180, 16);
  add_zone_to_click_board(8+120+8+120+8, 22, 180, 16, MEDIA_VIEWER_CLICK_ZONE_IMAGE_REVERSE_HORIZONTALLY);
  draw_button("[v] Reverse up-down", 8+120+8+120+8+180+8, 22, 180, 16);
  add_zone_to_click_board(8+120+8+120+8+180+8, 22, 180, 16, MEDIA_VIEWER_CLICK_ZONE_IMAGE_REVERSE_VERTICALLY);

  //draw image on screen
  draw_resized_image((dword_t)image_info);
  add_zone_to_click_board(image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_DRAW_HEIGTH], MEDIA_VIEWER_CLICK_ZONE_IMAGE);

  //add scrollbars
  if(image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]!=0) {
   program_interface_add_vertical_scrollbar(MEDIA_VIEWER_CLICK_ZONE_IMAGE_VERTICAL_SCROLLBAR, ((dword_t)image_info)+IMAGE_INFO_DRAW_HEIGTH*4, ((dword_t)image_info)+IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION*4, ((dword_t)image_info)+IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE*4, ((dword_t)&media_viewer_image_vertical_scrollbar_event));
   add_zone_to_click_board(image_info[IMAGE_INFO_SCREEN_X]+image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_SCREEN_Y], 10, image_info[IMAGE_INFO_DRAW_HEIGTH], MEDIA_VIEWER_CLICK_ZONE_IMAGE_VERTICAL_SCROLLBAR);
  }
  if(image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]!=0) {
   program_interface_add_horizontal_scrollbar(MEDIA_VIEWER_CLICK_ZONE_IMAGE_HORIZONTAL_SCROLLBAR, ((dword_t)image_info)+IMAGE_INFO_DRAW_WIDTH*4, ((dword_t)image_info)+IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION*4, ((dword_t)image_info)+IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE*4, ((dword_t)&media_viewer_image_horizontal_scrollbar_event));
   add_zone_to_click_board(image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y]+image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_DRAW_WIDTH], 10, MEDIA_VIEWER_CLICK_ZONE_IMAGE_HORIZONTAL_SCROLLBAR);
  }

  //print zoom
  dword_t zoom = get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM), percent_char_column = 152+8*3;
  print("Zoom:", 104, screen_height-6-7, BLACK);
  draw_full_square(152, screen_height-19, 40, 20, 0x0089FF);
  print_var(zoom, 152, screen_height-6-7, BLACK);
  if(zoom<100) {
   percent_char_column = 152+8*2;
   if(zoom<10) {
    percent_char_column = 152+8*1;
   }
  }
  draw_char('%', percent_char_column, screen_height-6-7, BLACK);
 }
 else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  //set state if this is jump between files
  if(media_viewer_sound_state==MEDIA_VIEWER_SOUND_NO_FILE) {
   media_viewer_sound_state = MEDIA_VIEWER_SOUND_STATE_STOPPED;
  }

  //draw bottom buttons
  set_program_value(PROGRAM_INTERFACE_BOTTOM_LINE_DRAW_COLUMN, 0);
  draw_bottom_line_button("[left] Volume -10", MEDIA_VIEWER_CLICK_ZONE_SOUND_VOLUME_MINUS_10);
  draw_bottom_line_button("[right] Volume +10", MEDIA_VIEWER_CLICK_ZONE_SOUND_VOLUME_PLUS_10);

  //draw play/stop button
  if(media_viewer_sound_state==MEDIA_VIEWER_SOUND_STATE_PLAYING) {
   draw_button("[space] Stop sound", 10, screen_height-42, 200, 14);
  }
  else {
   draw_button("[space] Play sound", 10, screen_height-42, 200, 14);
  }
  add_zone_to_click_board(10, screen_height-42, 200, 14, MEDIA_VIEWER_CLICK_ZONE_SOUND_PLAY_STOP);

  //print volume
  print("Volume:", 10, screen_height-80+3, WHITE);
  print_var(sound_volume, 10+64, screen_height-80+3, WHITE);

  //print length of file
  struct audio_file_t *audio_info = (struct audio_file_t *) (get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));
  print(":  :  /  :  :", screen_width-10-24-24-24-24-24, screen_height-37, WHITE);
  dword_t seconds = audio_info->length_seconds;
  dword_t minutes = audio_info->length_minutes;
  dword_t hours = audio_info->length_hours;
  if(seconds<10) {
   print("0", screen_width-10-16, screen_height-37, WHITE);
   print_var(seconds, screen_width-10-8, screen_height-37, WHITE);
  }
  else {
   print_var(seconds, screen_width-10-16, screen_height-37, WHITE);
  }
  if(minutes<10) {
   print("0", screen_width-10-16-24, screen_height-37, WHITE);
   print_var(minutes, screen_width-10-16-16, screen_height-37, WHITE);
  }
  else {
   print_var(minutes, screen_width-10-16-24, screen_height-37, WHITE);
  }
  if(hours<10) {
   print("0", screen_width-10-16-24-24, screen_height-37, WHITE);
   print_var(hours, screen_width-10-16-24-16, screen_height-37, WHITE);
  }
  else {
   print_var(hours, screen_width-10-16-24-24, screen_height-37, WHITE);
  }

  //print length of played part of file
  seconds = audio_info->played_length_seconds;
  minutes = audio_info->played_length_minutes;
  hours = audio_info->played_length_hours;
  draw_full_square(screen_width-10-24-24-24-24-24-16, screen_height-37, 64, 8, BLACK);
  if(seconds<10) {
   print("0", screen_width-82-16, screen_height-37, WHITE);
   print_var(seconds, screen_width-82-8, screen_height-37, WHITE);
  }
  else {
   print_var(seconds, screen_width-82-16, screen_height-37, WHITE);
  }
  if(minutes<10) {
   print("0", screen_width-82-16-24, screen_height-37, WHITE);
   print_var(minutes, screen_width-82-16-16, screen_height-37, WHITE);
  }
  else {
   print_var(minutes, screen_width-82-16-24, screen_height-37, WHITE);
  }
  if(hours<10) {
   print("0", screen_width-82-16-24-24, screen_height-37, WHITE);
   print_var(hours, screen_width-82-16-24-16, screen_height-37, WHITE);
  }
  else {
   print_var(hours, screen_width-82-16-24-24, screen_height-37, WHITE);
  }
  print(":  :", screen_width-10-24-24-24-24-24, screen_height-37, WHITE);

  //draw square of played part of file
  draw_empty_square(10, screen_height-60, screen_width-20, 10, WHITE);
  draw_full_square(11, screen_height-59, ((audio_info->actually_played_position/1024)*(screen_width-22)/(audio_info->output_length/1024)), 8, 0x0900FF);
  add_zone_to_click_board(11, screen_height-59, screen_width-22, 8, MEDIA_VIEWER_CLICK_ZONE_SOUND_PROGRESS);
 }
}

void media_viewer_draw_image(void) {
 draw_resized_image(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
 redraw_screen();
}

void media_viewer_open_file(void) {
 //pause sound
 media_viewer_stop_sound();

 //open file
 file_dialog_open_file_extensions_clear_mem();
 file_dialog_open_file_add_extension("qoi");
 file_dialog_open_file_add_extension("bmp");
 file_dialog_open_file_add_extension("jpg");
 file_dialog_open_file_add_extension("png");
 file_dialog_open_file_add_extension("gif");
 file_dialog_open_file_add_extension("wav");
 file_dialog_open_file_add_extension("mp3");
 file_dialog_open_file_add_extension("cdda");
 dword_t new_file_mem = file_dialog_open();
 if(new_file_mem==0) {
  return; //file not loaded
 }

 //add file entry
 set_program_value(PROGRAM_INTERFACE_SELECTED_FILE_SAVE_VALUE, get_program_value(PROGRAM_INTERFACE_SELECTED_FILE));
 add_file((word_t *)file_dialog_file_name, 0, 0, 0, 0, 0);
 
 //set file entry
 if(is_loaded_file_extension("jpg")==STATUS_TRUE || is_loaded_file_extension("qoi")==STATUS_TRUE || is_loaded_file_extension("bmp")==STATUS_TRUE || is_loaded_file_extension("png")==STATUS_TRUE || is_loaded_file_extension("gif")==STATUS_TRUE) { //image  
  //convert image
  show_message_window("Decoding image...");
  if(is_loaded_file_extension("qoi")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY, convert_qoi_to_image_data(new_file_mem));
  }
  else if(is_loaded_file_extension("bmp")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY, convert_bmp_to_image_data(new_file_mem));
  }
  else if(is_loaded_file_extension("png")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY, convert_png_to_image_data(new_file_mem, file_dialog_file_size));
  }
  else if(is_loaded_file_extension("gif")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY, convert_gif_to_image_data(new_file_mem, file_dialog_file_size));
  }
  else if(is_loaded_file_extension("jpg")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY, convert_jpg_to_image_data(new_file_mem, file_dialog_file_size));
  }
  free(new_file_mem);
  if(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY)==STATUS_ERROR) {
   remove_file(get_program_value(PROGRAM_INTERFACE_SELECTED_FILE));
   set_program_value(PROGRAM_INTERFACE_SELECTED_FILE, get_program_value(PROGRAM_INTERFACE_SELECTED_FILE_SAVE_VALUE));
   error_window("Error during decoding file, more info in system log");
   return;
  }

  //set variables
  set_file_value(MEDIA_VIEWER_FILE_TYPE, MEDIA_VIEWER_FILE_IMAGE);
  set_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM, 100);
  media_viewer_sound_state = MEDIA_VIEWER_SOUND_NO_FILE;
  
  //calculate image dimensions, position on screen and scrollbars
  media_viewer_image_recalculate_zoom();
 }
 else if(is_loaded_file_extension("mp3")==STATUS_TRUE || is_loaded_file_extension("wav")==STATUS_TRUE || is_loaded_file_extension("cdda")==STATUS_TRUE) {
  //convert file
  if(is_loaded_file_extension("mp3")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY, (dword_t)process_audio_file(AUDIO_FILE_TYPE_MP3, (byte_t *)new_file_mem, file_dialog_file_size));
  }
  else if(is_loaded_file_extension("wav")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY, (dword_t)process_audio_file(AUDIO_FILE_TYPE_WAV, (byte_t *)new_file_mem, file_dialog_file_size));
  }
  else if(is_loaded_file_extension("cdda")==STATUS_TRUE) {
   set_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY, (dword_t)process_audio_file(AUDIO_FILE_TYPE_CDDA, (byte_t *)new_file_mem, file_dialog_file_size));
  }
  free(new_file_mem);

  //test errors
  if(get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY)==STATUS_ERROR) {
   remove_file(get_program_value(PROGRAM_INTERFACE_SELECTED_FILE));
   set_program_value(PROGRAM_INTERFACE_SELECTED_FILE, get_program_value(PROGRAM_INTERFACE_SELECTED_FILE_SAVE_VALUE));
   error_window("Error during decoding file, more info in system log");
   return;
  }
  
  //set other values of file info
  set_file_value(MEDIA_VIEWER_FILE_TYPE, MEDIA_VIEWER_FILE_SOUND);
  
  //set state
  media_viewer_sound_state = MEDIA_VIEWER_SOUND_STATE_STOPPED; 
 }
}

void media_viewer_change_between_files(void) {
 if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  media_viewer_stop_sound();
  media_viewer_sound_state = MEDIA_VIEWER_SOUND_NO_FILE;
 }
}

void media_viewer_save_file(void) {
 //pause sound
 media_viewer_stop_sound();

 if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  dword_t file_format_number = window_for_choosing_file_format(4, "[j] JPG\0[g] GIF\0[b] BMP\0[q] QOI");
  if(file_format_number==0xFFFFFFFF) {
   return;
  }
  show_message_window("Converting image...");
  if(file_format_number==0) {
   file_dialog_save_set_extension("jpg");
   convert_image_data_to_jpg(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY), 90);
  }
  else if(file_format_number==1) {
   file_dialog_save_set_extension("gif");
   convert_image_data_to_gif(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  }
  else if(file_format_number==2) {
   file_dialog_save_set_extension("bmp");
   convert_image_data_to_bmp(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  }
  else if(file_format_number==3) {
   file_dialog_save_set_extension("qoi");
   convert_image_data_to_qoi(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  }
  file_dialog_save(converted_file_memory, converted_file_size);
  free(converted_file_memory);
 }
 else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  struct audio_file_t *audio_info = (struct audio_file_t *) (get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));

  if(audio_info->type==AUDIO_FILE_TYPE_MP3) {
   file_dialog_save_set_extension("mp3");
   file_dialog_save((dword_t)audio_info->file_pointer, audio_info->file_size);
  }
  else if(audio_info->type==AUDIO_FILE_TYPE_WAV) {
   file_dialog_save_set_extension("wav");
   file_dialog_save((dword_t)audio_info->file_pointer, audio_info->file_size);
  }
  else if(audio_info->type==AUDIO_FILE_TYPE_CDDA) {
   file_dialog_save_set_extension("wav");
   convert_sound_data_to_wav(audio_info->file_pointer, audio_info->file_size, 16, 2, 44100);
   file_dialog_save(converted_file_memory, converted_file_size);
   free(converted_file_memory);
  }
 }
}

void media_viewer_close_file(void) {
 //pause sound
 media_viewer_stop_sound();

 if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  delete_image(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
 }
 else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  destroy_audio_file((struct audio_file_t *)get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));
  media_viewer_sound_state = MEDIA_VIEWER_SOUND_NO_FILE;
 }
}

void media_viewer_image_vertical_scrollbar_event(void) {
 dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
 image_info[IMAGE_INFO_DRAW_Y] = get_scrollbar_rider_value(image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_HEIGTH], image_info[IMAGE_INFO_DRAW_HEIGTH]);
 media_viewer_draw_image();
}

void media_viewer_image_horizontal_scrollbar_event(void) {
 dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
 image_info[IMAGE_INFO_DRAW_X] = get_scrollbar_rider_value(image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_WIDTH], image_info[IMAGE_INFO_DRAW_WIDTH]);
 media_viewer_draw_image();
}

void media_viewer_image_reverse_horizontally(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  image_reverse_horizontally(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  program_interface_redraw();
 }
}

void media_viewer_image_reverse_vertically(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  image_reverse_vertically(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  program_interface_redraw();
 }
}

void media_viewer_image_turn_left(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  image_turn_left(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  media_viewer_image_recalculate_zoom();
  program_interface_redraw();
 }
}

void media_viewer_image_turn_right(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  image_turn_right(get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  media_viewer_image_recalculate_zoom();
  program_interface_redraw();
 }
}

void media_viewer_key_up_event(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  image_info[IMAGE_INFO_DRAW_Y]-=50;
  if(image_info[IMAGE_INFO_DRAW_Y]>0x80000000) {
   image_info[IMAGE_INFO_DRAW_Y] = 0;
  }
  media_viewer_image_recalculate_scrollbars();
  media_viewer_draw_image();
 }
}

void media_viewer_key_down_event(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
  dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
  media_viewer_image_recalculate_scrollbars();
  image_info[IMAGE_INFO_DRAW_Y]+=50;
  if(image_info[IMAGE_INFO_DRAW_Y]>(image_info[IMAGE_INFO_HEIGTH]-image_info[IMAGE_INFO_DRAW_HEIGTH])) {
   image_info[IMAGE_INFO_DRAW_Y] = (image_info[IMAGE_INFO_HEIGTH]-image_info[IMAGE_INFO_DRAW_HEIGTH]);
  }
  media_viewer_image_recalculate_scrollbars();
  media_viewer_draw_image();
 }
}

void media_viewer_key_left_event(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0) {
  if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
   dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
   image_info[IMAGE_INFO_DRAW_X]-=50;
   if(image_info[IMAGE_INFO_DRAW_X]>0x80000000) {
    image_info[IMAGE_INFO_DRAW_X] = 0;
   }
   media_viewer_image_recalculate_scrollbars();
   media_viewer_draw_image();
  }
  else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
   if(sound_volume>=10) {
    sound_set_volume((sound_volume-10));
    program_interface_redraw();
   }
   else if(sound_volume>0) {
    sound_set_volume(0);
    program_interface_redraw();
   }
  }
 }
}

void media_viewer_key_right_event(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0) {
  if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_IMAGE) {
   dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
   media_viewer_image_recalculate_scrollbars();
   image_info[IMAGE_INFO_DRAW_X]+=50;
   if(image_info[IMAGE_INFO_DRAW_X]>(image_info[IMAGE_INFO_WIDTH]-image_info[IMAGE_INFO_DRAW_WIDTH])) {
    image_info[IMAGE_INFO_DRAW_X] = (image_info[IMAGE_INFO_WIDTH]-image_info[IMAGE_INFO_DRAW_WIDTH]);
   }
   media_viewer_image_recalculate_scrollbars();
   media_viewer_draw_image();
  }
  else if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
   if(sound_volume<=90) {
    sound_set_volume((sound_volume+10));
    program_interface_redraw();
   }
   else if(sound_volume<100) {
    sound_set_volume(100);
    program_interface_redraw();
   }
  }
 }
}

void media_viewer_key_space_event(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0) {
  if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
   if(media_viewer_sound_state==MEDIA_VIEWER_SOUND_STATE_STOPPED) {
    struct audio_file_t *audio_info = (struct audio_file_t *) (get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));

    //if is file fully played, restart playing
    if(audio_info->actually_played_position==audio_info->output_length) {
     audio_calculate_time_of_sound_data_offset(audio_info, 0);
    }

    //play sound
    play_audio_file(audio_info, audio_info->actually_played_position);
    media_viewer_sound_state = MEDIA_VIEWER_SOUND_STATE_PLAYING;
    create_task(media_viewer_task_sound_update_playing_square, TASK_TYPE_PERIODIC_INTERRUPT, 100);
   }
   else if(media_viewer_sound_state==MEDIA_VIEWER_SOUND_STATE_PLAYING) {
    //stop sound
    media_viewer_stop_sound();
   }

   program_interface_redraw();
  }
 }
}

void media_viewer_image_recalculate_scrollbars(void) {
 dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));

 //calculate scrollbars
 calculate_image_scrollbars((dword_t)image_info);

 //calculate size of image with scrollbars
 if(image_info[IMAGE_INFO_WIDTH]<screen_width) {
  image_info[IMAGE_INFO_DRAW_WIDTH] = image_info[IMAGE_INFO_WIDTH];
 }
 else {
  image_info[IMAGE_INFO_DRAW_WIDTH] = screen_width;
 }
 if(image_info[IMAGE_INFO_HEIGTH]<(screen_height-60)) {
  image_info[IMAGE_INFO_DRAW_HEIGTH] = image_info[IMAGE_INFO_HEIGTH];
 }
 else {
  image_info[IMAGE_INFO_DRAW_HEIGTH] = (screen_height-60);
 }
 if(image_info[IMAGE_INFO_WIDTH]>screen_width && image_info[IMAGE_INFO_HEIGTH]>(screen_height-60)) { //both scrollbars
  image_info[IMAGE_INFO_DRAW_HEIGTH] -= 10;
  image_info[IMAGE_INFO_DRAW_WIDTH] -= 10;
 }
 else if(image_info[IMAGE_INFO_HEIGTH]>(screen_height-60) && image_info[IMAGE_INFO_WIDTH]<=screen_width) { //vertical scrollbar
  if((screen_width-image_info[IMAGE_INFO_WIDTH])<20 || (screen_width-image_info[IMAGE_INFO_WIDTH])>0x80000000) { //also horizontal scrollbar
   image_info[IMAGE_INFO_DRAW_HEIGTH] -= 10;
   image_info[IMAGE_INFO_DRAW_WIDTH] -= 10;
  }
 }
 else if(image_info[IMAGE_INFO_HEIGTH]<=(screen_height-60) && image_info[IMAGE_INFO_WIDTH]>screen_width) { //horizontal scrollbar
  if((screen_height-60-image_info[IMAGE_INFO_HEIGTH])<20 || (screen_height-60-image_info[IMAGE_INFO_HEIGTH])>0x80000000) { //also vertical scrollbar
   image_info[IMAGE_INFO_DRAW_HEIGTH] -= 10;
   image_info[IMAGE_INFO_DRAW_WIDTH] -= 10;
  }
 }

 //calculate scrollbars after size change
 calculate_image_scrollbars((dword_t)image_info);
}

void media_viewer_image_recalculate_zoom(void) {
 dword_t *image_info = (dword_t *) (get_file_value(MEDIA_VIEWER_FILE_IMAGE_INFO_MEMORY));
 dword_t zoom = get_file_value(MEDIA_VIEWER_FILE_IMAGE_ZOOM);
 
 //calculate width and height by zoom
 image_info[IMAGE_INFO_WIDTH]=(image_info[IMAGE_INFO_REAL_WIDTH]*zoom/100);
 image_info[IMAGE_INFO_HEIGTH]=(image_info[IMAGE_INFO_REAL_HEIGHT]*zoom/100);
 image_info[IMAGE_INFO_DRAW_WIDTH]=image_info[IMAGE_INFO_WIDTH];
 image_info[IMAGE_INFO_DRAW_HEIGTH]=image_info[IMAGE_INFO_HEIGTH];

 //set dimensions of image for screen
 if(image_info[IMAGE_INFO_WIDTH]<screen_width) {
  image_info[IMAGE_INFO_SCREEN_X] = (screen_x_center-(image_info[IMAGE_INFO_WIDTH]/2));
  image_info[IMAGE_INFO_DRAW_X] = 0;
 }
 else {
  image_info[IMAGE_INFO_DRAW_WIDTH] = screen_width;
  image_info[IMAGE_INFO_SCREEN_X] = 0;
  if(image_info[IMAGE_INFO_DRAW_X]>(image_info[IMAGE_INFO_WIDTH]-screen_width)) {
   image_info[IMAGE_INFO_DRAW_X] = (image_info[IMAGE_INFO_WIDTH]-screen_width);
  }
 }
 if(image_info[IMAGE_INFO_HEIGTH]<(screen_height-60)) {
  image_info[IMAGE_INFO_SCREEN_Y] = (screen_y_center+10-(image_info[IMAGE_INFO_HEIGTH]/2));
  image_info[IMAGE_INFO_DRAW_Y] = 0;
 }
 else {
  image_info[IMAGE_INFO_DRAW_HEIGTH] = (screen_height-60);
  image_info[IMAGE_INFO_SCREEN_Y] = 40;
  if(image_info[IMAGE_INFO_DRAW_Y]>(image_info[IMAGE_INFO_HEIGTH]-(screen_height-60))) {
   image_info[IMAGE_INFO_DRAW_Y] = (image_info[IMAGE_INFO_HEIGTH]-(screen_height-60));
  }
 }
 
 //set scrollbars
 media_viewer_image_recalculate_scrollbars();
}

void media_viewer_stop_sound(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND && media_viewer_sound_state==MEDIA_VIEWER_SOUND_STATE_PLAYING) {
  stop_sound();
  destroy_task(media_viewer_task_sound_update_playing_square);
  media_viewer_sound_state = MEDIA_VIEWER_SOUND_STATE_STOPPED;
 }
}

void media_viewer_click_on_sound_progress_square(void) {
 if(get_program_value(PROGRAM_INTERFACE_NUMBER_OF_FILES)!=0 && get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  struct audio_file_t *audio_info = (struct audio_file_t *) (get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));

  //stop sound
  stop_sound();
  destroy_task(media_viewer_task_sound_update_playing_square);
  
  //calculate where we should move sound
  audio_calculate_time_of_sound_data_offset(audio_info, ((audio_info->output_length/1024)*(mouse_cursor_x-11)/(screen_width-22))*1024);
  
  //play sound if sound is already playing
  if(media_viewer_sound_state==MEDIA_VIEWER_SOUND_STATE_PLAYING) {
   play_audio_file(audio_info, audio_info->actually_played_position);
   create_task(media_viewer_task_sound_update_playing_square, TASK_TYPE_PERIODIC_INTERRUPT, 100);
  }
  else {
   media_viewer_sound_state = MEDIA_VIEWER_SOUND_STATE_STOPPED;
  }
 
  program_interface_redraw();
 }
}

void media_viewer_task_sound_update_playing_square(void) {
 if(get_file_value(MEDIA_VIEWER_FILE_TYPE)==MEDIA_VIEWER_FILE_SOUND) {
  struct audio_file_t *audio_info = (struct audio_file_t *) (get_file_value(MEDIA_VIEWER_FILE_AUDIO_INFO_MEMORY));

  //end of file
  if(audio_info->actually_played_position==audio_info->output_length) {
   media_viewer_stop_sound();
   program_interface_redraw();
  }
  else {
   //update square
   draw_full_square(11, screen_height-59, ((audio_info->actually_played_position/1024)*(screen_width-22)/(audio_info->output_length/1024)), 8, 0x0900FF);
   redraw_part_of_screen(11, screen_height-59, screen_width-22, 8);

   //update time
   dword_t seconds = audio_info->played_length_seconds;
   dword_t minutes = audio_info->played_length_minutes;
   dword_t hours = audio_info->played_length_hours;
   draw_full_square(screen_width-10-24-24-24-24-24-16, screen_height-37, 64, 8, BLACK);
   if(seconds<10) {
    print("0", screen_width-82-16, screen_height-37, WHITE);
    print_var(seconds, screen_width-82-8, screen_height-37, WHITE);
   }
   else {
    print_var(seconds, screen_width-82-16, screen_height-37, WHITE);
   }
   if(minutes<10) {
    print("0", screen_width-82-16-24, screen_height-37, WHITE);
    print_var(minutes, screen_width-82-16-16, screen_height-37, WHITE);
   }
   else {
    print_var(minutes, screen_width-82-16-24, screen_height-37, WHITE);
   }
   if(hours<10) {
    print("0", screen_width-82-16-24-24, screen_height-37, WHITE);
    print_var(hours, screen_width-82-16-24-16, screen_height-37, WHITE);
   }
   else {
    print_var(hours, screen_width-82-16-24-24, screen_height-37, WHITE);
   }
   print(":  :", screen_width-10-24-24-24-24-24, screen_height-37, WHITE);
   redraw_part_of_screen(screen_width-10-24-24-24-24-24-16, screen_height-37, 64, 8);
  }
 }
}