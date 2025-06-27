#include <cstring>
#include <curses.h>
#include <iostream>
#include <fstream>

using namespace std;

#define COLOR_HEADER 1
#define COLOR_MENU 2
#define COLOR_WINDOW 3
#define COLOR_HIGHLIGHT 4

const char *file_menu[] = {"New", "Open", "Save", "Exit", NULL};
const char *edit_menu[] = {"Cut", "Copy", "Paste", NULL};

WINDOW *edit_win = nullptr;

string filename;


void show_about();
void show_submenu(const char** items, const char* title);

WINDOW* draw_menu_window(const char* title, int height, int width);
void draw_workspace(string name);
void draw_header();

//Реализация функций
void draw_header() 
{
    attron(COLOR_PAIR(COLOR_HEADER));
    mvhline(0, 0, ' ', COLS);  // Заполнить строку цветом
    mvprintw(0, 2, "F1 - File");
    mvprintw(0, 14, "F2 - Edit");
    mvprintw(0, 26, "F3 - About");
    mvprintw(0, 40, "F4 - Exit");
    attroff(COLOR_PAIR(COLOR_HEADER));
    refresh();
}

void show_about() 
{
    const char* message = "Simple Notepad. Press [ESC] for close this window.";
    int width = strlen(message) + 6;
    int height = 5;

    WINDOW* win_about = draw_menu_window("About", height, width);
    mvwprintw(win_about, 2, 3, "%s", message);
    wrefresh(win_about);

    // Ждём ESC
    while (wgetch(win_about) != 27) {}

    delwin(win_about);
    touchwin(stdscr);
    refresh();
    //
    draw_header();     
    draw_workspace(filename);  
}

WINDOW* draw_menu_window(const char* title, int height, int width) 
{
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    WINDOW* win_menu = newwin(height, width, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(COLOR_WINDOW));
    box(win_menu, 0, 0);
    mvwprintw(win_menu, 0, 2, " %s ", title);
    wrefresh(win_menu);
    return win_menu;
}
 
 void draw_workspace(string name)
{
    if (!edit_win)
    {
        int height = LINES - 3;
        int width = COLS - 4;
        int start_y = 2;
        int start_x = 2;

        edit_win = newwin(height, width, start_y, start_x);
    }

    werase(edit_win);  
    box(edit_win, 0, 0);

    string title = " " + name + " ";
    int width = getmaxx(edit_win);
    int title_x = (width - title.length()) / 2;
    mvwprintw(edit_win, 0, title_x, "%s", title.c_str());

    wrefresh(edit_win);
}

void show_submenu(const char** items, const char* title) 
{
    int count = 0;
    while (items[count]) count++;

    int width = 20;
    int height = count + 4;

    WINDOW* menu_win = draw_menu_window(title, height, width);

    int choice = 0;
    int ch;
    keypad(menu_win, TRUE);

    while (1) 
    {
        for (int i = 0; i < count; ++i) 
        {
            if (i == choice) 
            {
                wattron(menu_win, COLOR_PAIR(COLOR_HIGHLIGHT));
                mvwprintw(menu_win, i + 2, 2, "%s", items[i]);
                wattroff(menu_win, COLOR_PAIR(COLOR_HIGHLIGHT));
            } 
            else 
            {
                mvwprintw(menu_win, i + 2, 2, "%s", items[i]);
            }
        }
        wrefresh(menu_win);

        ch = wgetch(menu_win);
        if (ch == 27)  // ESC
            break;
        else if (ch == KEY_UP)
            choice = (choice + count - 1) % count;
        else if (ch == KEY_DOWN)
            choice = (choice + 1) % count;
        else if (ch == '\n') 
        {
            break;
        }
    }

    delwin(menu_win);
    touchwin(stdscr);
    refresh();
    
    draw_header();     
    draw_workspace(filename);  
}

//Точка входа в программу
int main(int argc, char *argv[])
{
    if(argc == 2)                                    
    {
        filename = argv[1];                          
        
        if (filename.find(".txt") == string::npos)          
        {
            filename += ".txt";                             
        }
        
        ofstream outfile(filename);                         
        
        if (!outfile)                                       
        {
            cerr << "Error: failed to create a file " << filename << "\n";         
            return 1;                                                               
        }                                                               
    }
    else
    {
        cerr << "Error: the argument was not passed!" << filename << "\n";    
        filename = "Untitled.txt";
    }
    
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) 
    {
        start_color();
        init_pair(COLOR_HEADER, COLOR_WHITE, COLOR_BLUE);
        init_pair(COLOR_MENU, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_WINDOW, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_CYAN);
    }
    
    draw_header();
    draw_workspace(filename);
    
    int ch;
    
    while((ch = getch()) != KEY_F(4))
    {
        switch(ch)
        {
            case KEY_F(1):
                show_submenu(file_menu, "File");
            break;
            
            case KEY_F(2):
                show_submenu(edit_menu, "Edit");
            break;
            
            case KEY_F(3):
                show_about(); 
            break;
        }
    }
    
    //draw_header();
    //draw_workspace(filename);
    
    if (edit_win)
        delwin(edit_win);
    endwin();
    return 0;
}
