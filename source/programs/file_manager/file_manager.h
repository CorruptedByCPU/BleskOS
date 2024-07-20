//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define FILE_MANAGER_FOLDER_STRUCTURE_MEMORY (PROGRAM_INTERFACE_FILE_PROGRAM_AREA+0)
#define FILE_MANAGER_FOLDER_PATH_NAMES (PROGRAM_INTERFACE_FILE_PROGRAM_AREA+1)
#define FILE_MANAGER_SCROLLBAR_RIDER_POSITION (PROGRAM_INTERFACE_FILE_PROGRAM_AREA+2)

enum {
 CLICK_ZONE_FILE_MANAGER_BACK = 1000,
 CLICK_ZONE_FILE_MANAGER_REFRESH_DEVICES,
 CLICK_ZONE_FILE_MANAGER_CHANGE_VIEW_WINDOW,
 CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW,
 CLICK_ZONE_FILE_MANAGER_GO_TO_PREVIOUS_FOLDER,
 CLICK_ZONE_FILE_MANAGER_CREATE_NEW_FOLDER,
 CLICK_ZONE_FILE_MANAGER_DELETE,
 CLICK_ZONE_FILE_MANAGER_RENAME,
 CLICK_ZONE_FILE_MANAGER_COPY,
 CLICK_ZONE_FILE_MANAGER_PASTE,
 CLICK_ZONE_FILE_MANAGER_SCROLLBAR,
 CLICK_ZONE_FILE_MANAGER_FIRST_CONNECTED_PARTITION,
 CLICK_ZONE_FILE_MANAGER_LAST_CONNECTED_PARTITION = (CLICK_ZONE_FILE_MANAGER_FIRST_CONNECTED_PARTITION+9),
 CLICK_ZONE_FILE_MANAGER_FIRST_ITEM,
};

enum {
 FILE_MANAGER_EVENT_REDRAW = PROGRAM_DEFINED_EVENTS,
};

enum {
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_BACK = 1,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_LIST,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_ICONS,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_NAME,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_EXTENSION,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_SIZE,
 CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_DATE_OF_CREATION,
};

enum {
 CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_BACK = 1,
 CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_LEFT,
 CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_ENTER,
 CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_RIGHT,
};

enum {
 CLICK_ZONE_FILE_MANAGER_NAME_INPUT_CANCEL = 1,
 CLICK_ZONE_FILE_MANAGER_NAME_INPUT_APPROVE,
};

#define FILE_MANAGER_WIDTH_OF_CONNECTED_PARTITIONS 22*8

#define FILE_MANAGER_NUMBER_OF_CHARS_IN_ICON_LINE 10
#define FILE_MANAGER_NUMBER_OF_PIXELS_IN_ICON_DIMENSION FILE_MANAGER_NUMBER_OF_CHARS_IN_ICON_LINE*8

struct scrollbar_info_t file_manager_scrollbar_info;

dword_t file_manager_program_interface_memory = 0;
dword_t file_manager_number_of_lines_on_screen, file_manager_number_of_columns_on_screen, file_manager_number_of_items_on_screen;

byte_t *file_manager_copied_file_memory;
struct file_descriptor_t file_manager_copied_file_descriptor;

//TODO: rewrite later when code of text area will be rewritten
dword_t *file_manager_text_area;

void initalize_file_manager(void);
void file_manager(void);

void file_manager_event_click_on_connected_partitions(void);
void file_manager_event_key_page_up(void);
void file_manager_event_key_page_down(void);

dword_t file_manager_event_key_up(void);
dword_t file_manager_event_key_down(void);
dword_t file_manager_event_key_left(void);
dword_t file_manager_event_key_right(void);
dword_t file_manager_event_key_enter(void);
dword_t file_manager_event_click_on_files(void);
dword_t file_manager_event_go_to_previous_folder(void);
void file_manager_event_scrollbar_change(dword_t value);

void file_manager_change_view_window(void);
dword_t file_manager_preview_window(void);

dword_t file_manager_keyboard_event_c(void);
dword_t file_manager_rename_file_in_folder(void);
dword_t file_manager_delete_file_in_folder(void);
dword_t file_manager_create_folder_in_folder(void);
dword_t file_manager_copy_file(void);
dword_t file_manager_keyboard_event_v(void);
dword_t file_manager_paste_file(void);

void redraw_file_manager(void);
void file_manager_new_tab(void);
void file_manager_close_tab(void);

dword_t file_manager_event_interface[] = { 0,
 //change of selected partition
 KEYBOARD_EVENT_PRESSED_KEY, KEY_PAGE_UP, (dword_t)(&file_manager_event_key_page_up), NO_EVENT,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_PAGE_DOWN, (dword_t)(&file_manager_event_key_page_down), NO_EVENT,
 MOUSE_EVENT_CLICK_ON_ZONES, CLICK_ZONE_FILE_MANAGER_FIRST_CONNECTED_PARTITION, CLICK_ZONE_FILE_MANAGER_LAST_CONNECTED_PARTITION, (dword_t)(&file_manager_event_click_on_connected_partitions), NO_EVENT,
 
 //refresh devices
 KEYBOARD_EVENT_PRESSED_KEY, KEY_F8, (dword_t)(&refresh_devices), FILE_MANAGER_EVENT_REDRAW,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_REFRESH_DEVICES, (dword_t)(&refresh_devices), FILE_MANAGER_EVENT_REDRAW,

 //events in file zone
 KEYBOARD_EVENT_PRESSED_KEY, KEY_UP, (dword_t)(&file_manager_event_key_up), RETURN_EVENT_FROM_METHOD,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_DOWN, (dword_t)(&file_manager_event_key_down), RETURN_EVENT_FROM_METHOD,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_LEFT, (dword_t)(&file_manager_event_key_left), RETURN_EVENT_FROM_METHOD,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_RIGHT, (dword_t)(&file_manager_event_key_right), RETURN_EVENT_FROM_METHOD,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_ENTER, (dword_t)(&file_manager_event_key_enter), RETURN_EVENT_FROM_METHOD,
 MOUSE_EVENT_CLICK_ON_ZONES, CLICK_ZONE_FILE_MANAGER_FIRST_ITEM, 0xFFFFFFFF, (dword_t)(&file_manager_event_click_on_files), RETURN_EVENT_FROM_METHOD,
 VERTICAL_SCROLLBAR_EVENT, CLICK_ZONE_FILE_MANAGER_SCROLLBAR, (dword_t)(&file_manager_scrollbar_info), (dword_t)(&file_manager_event_scrollbar_change),

 //go to previous folder
 KEYBOARD_EVENT_PRESSED_KEY, KEY_B, (dword_t)(&file_manager_event_go_to_previous_folder), RETURN_EVENT_FROM_METHOD,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_GO_TO_PREVIOUS_FOLDER, (dword_t)(&file_manager_event_go_to_previous_folder), RETURN_EVENT_FROM_METHOD,

 //change view / paste file
 KEYBOARD_EVENT_PRESSED_KEY, KEY_V, (dword_t)(&file_manager_keyboard_event_v), RETURN_EVENT_FROM_METHOD,

 //change view
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_CHANGE_VIEW_WINDOW, (dword_t)(&file_manager_change_view_window), NO_EVENT,

 //image preview
 KEYBOARD_EVENT_PRESSED_KEY, KEY_P, (dword_t)(&file_manager_preview_window), RETURN_EVENT_FROM_METHOD,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW, (dword_t)(&file_manager_preview_window), RETURN_EVENT_FROM_METHOD,

 //rename file
 KEYBOARD_EVENT_PRESSED_KEY, KEY_R, (dword_t)(&file_manager_rename_file_in_folder), RETURN_EVENT_FROM_METHOD,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_RENAME, (dword_t)(&file_manager_rename_file_in_folder), RETURN_EVENT_FROM_METHOD,

 //delete file
 KEYBOARD_EVENT_PRESSED_KEY, KEY_DELETE, (dword_t)(&file_manager_delete_file_in_folder), RETURN_EVENT_FROM_METHOD,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_DELETE, (dword_t)(&file_manager_delete_file_in_folder), RETURN_EVENT_FROM_METHOD,

 //create folder / copy file
 KEYBOARD_EVENT_PRESSED_KEY, KEY_C, (dword_t)(&file_manager_keyboard_event_c), RETURN_EVENT_FROM_METHOD,

 //create folder
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_CREATE_NEW_FOLDER, (dword_t)(&file_manager_create_folder_in_folder), RETURN_EVENT_FROM_METHOD,

 //copy file
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_COPY, (dword_t)(&file_manager_copy_file), RETURN_EVENT_FROM_METHOD,

 //paste file
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_PASTE, (dword_t)(&file_manager_paste_file), RETURN_EVENT_FROM_METHOD,

 END_OF_EVENTS,
};

dword_t file_manager_view_window_event_interface[] = { 0,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_ESC, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_BACK,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_BACK, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_BACK,

 KEYBOARD_EVENT_PRESSED_KEY, KEY_L, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_LIST,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_LIST, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_LIST,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_I, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_ICONS,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_ICONS, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_ICONS,

 KEYBOARD_EVENT_PRESSED_KEY, KEY_N, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_NAME,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_NAME, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_NAME,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_E, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_EXTENSION,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_EXTENSION, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_EXTENSION,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_S, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_SIZE,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_SIZE, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_SIZE,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_D, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_DATE_OF_CREATION,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_DATE_OF_CREATION, 0, CLICK_ZONE_FILE_MANAGER_VIEW_WINDOW_SORT_BY_DATE_OF_CREATION,

 END_OF_EVENTS,
};

dword_t file_manager_preview_window_event_interface[] = { 0,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_ESC, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_BACK,
 MOUSE_EVENT_CLICK_ON_ZONE, 0, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_BACK,

 KEYBOARD_EVENT_PRESSED_KEY, KEY_LEFT, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_LEFT,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_LEFT, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_LEFT,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_RIGHT, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_RIGHT,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_RIGHT, 0, CLICK_ZONE_FILE_MANAGER_PREVIEW_WINDOW_RIGHT,

 END_OF_EVENTS,
};

dword_t file_manager_name_input_event_interface[] = { 0,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_ESC, 0, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_CANCEL,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_CANCEL, 0, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_CANCEL,
 KEYBOARD_EVENT_PRESSED_KEY, KEY_ENTER, 0, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_APPROVE,
 MOUSE_EVENT_CLICK_ON_ZONE, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_APPROVE, 0, CLICK_ZONE_FILE_MANAGER_NAME_INPUT_APPROVE,

 TEXT_AREA_WITH_PERMANENT_FOCUS_EVENT, 0,

 END_OF_EVENTS,
};