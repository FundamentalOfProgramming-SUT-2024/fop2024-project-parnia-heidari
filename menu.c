#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>  
#include <locale.h> 
#include <ncurses.h> 
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <pthread.h>

#include "gameplay.c"

int max_y, max_x;

char musiclist[6][100]={"Fading Footsteps in the Forest.mp3","Passages .mp3","Tales Beneath the Willow .mp3","The old Oak .mp3","The Vagabond .mp3","None"};
int current_music=0;


///////////////////////////////////////////////////////

void button_maker(int x, int y, bool is_selected, const char *label) {
    int label_length= strlen(label);

    for (int i=1; i<3; i++) {
        mvprintw(x+i, y, "|");
        mvprintw(x+i, y+20, "|");
    }

    for (int i = 1; i<20; i++) {
        mvprintw(x, y+i, "_");
        mvprintw(x+2, y+i, "_");
    }

    mvprintw(x+1, y+(20-label_length)/2 +1, "%s", label);

    if (is_selected) {
        attron(A_REVERSE);
        mvprintw(x+1, y+(20-label_length)/2 +1, "%s", label);
        attroff(A_REVERSE);
    }

    refresh();
}

/////////////////////////////////////////////////

void welcome_printer(){
    attron(A_BOLD);
    mvprintw(max_y/4, max_x/2-10,"  WELCOME TO ROGUE!");
    attroff(A_BOLD);
    mvprintw(max_y/4+1, max_x/2-10," Adventure awaits..,");
    mvprintw(max_y/2+3, max_x/2-10,"   press Q to quit");
}

///////////////////////////////////////////////////

int check_username(char user[]){
    FILE *ptr=fopen("users.txt","r");
    char info[100];
    while(fgets(info,100,ptr)){
        info[strcspn(info, "\n")]='\0';
        if(strcmp(user,info)==0){
            fclose(ptr);
            return 0;}
        char pass[100];
        char email[100];
        char breaker[100];
        char a[100]; char b[100]; char c[100];
        fgets(pass,100,ptr);
        fgets(email,100,ptr);
        fgets(breaker,100,ptr);
        fgets(a,100,ptr);
        fgets(b,100,ptr);
        fgets(c,100,ptr);
    }
    fclose(ptr);
    return 1;
}

///////////////////////////////////////////////////

int make_username(char new_user[]){
    curs_set(1);
    move(max_y/3, max_x/2-10);
    int i = 0;
    echo();
    while(1) {
        int ch=getch();
        if (ch=='\n' || i >= 99){
            new_user[i]='\0';
            break;
        }
        else if (ch==KEY_BACKSPACE){
            if (i>0){
                i--;
                mvprintw(max_y/3, max_x/2-10 +i," ");
                move(max_y/3, max_x/2-10 +i);
                refresh();}
            else{move(max_y/3, max_x/2-10);}
        }
        else if(ch==KEY_DC){
            return 0;
        }
        else{
            new_user[i++]=ch;
            refresh();}
    }
    curs_set(0);
    if(check_username(new_user)==1){
        move(max_y/5*4, max_x/2-30);
        clrtoeol();
        return 1;}
    else{
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(max_y/5*4, max_x/2-27,"This Title Belongs to Another!");
        attroff(A_BOLD | A_UNDERLINE);
        move(max_y/3, max_x/2-10);
        clrtoeol();
        make_username(new_user);
    }
}

//////////////////////////////////////////////////////////

int check_password(char pass[100]){
    int i=0;
    int has_upper=0;
    int has_lower=0;
    int has_digit=0;
    while(pass[i]!='\0'){
        if(isdigit(pass[i])){has_digit=1;}
        if(isupper(pass[i])){has_upper=1;}
        if(islower(pass[i])){has_lower=1;}
        ++i;
    }
    if(strlen(pass)<7){return 1;}
    else if(has_lower==0){return 2;}
    else if(has_upper==0){return 3;}
    else if(has_digit==0){return 4;}
    else{return 0;}
}

//////////////////////////////////////////////////////////

char *random_pass_generator(){
    char *pass=malloc(11*sizeof(char));
    for(int i=0;i<8;i++){
        *(pass+i)=(rand()%10)+'0';
    }
    *(pass+8)='a';
    *(pass+9)='A';
    *(pass+10)='\0';
    return pass;
}

//////////////////////////////////////////////////////////

int make_password(char pass[100]){
    mvprintw(max_y/9*8-2, max_x/2-35,"Press \\ to receive a secret word, forged by fate,");
    curs_set(1);
    move(max_y/2-2, max_x/2-10);
    clrtoeol();
    int i = 0;
    while(1) {
        int ch=getch();
        if (ch=='\n' || i >= 99) {
            pass[i]='\0';
            break;
        }
        else if (ch==KEY_BACKSPACE){
            if (i>0){
                i--;
                move(max_y/2-2, max_x/2-10 +i);
                printw(" ");
                move(max_y/2-2, max_x/2-10 +i);
                refresh();}
            else{move(max_y/2-2, max_x/2-10);}
        }
        else if(ch==KEY_DC){
            return 0;
        }
        else if(ch=='\\'){
            move(max_y/2-2, max_x/2-10 +i);
            printw(" ");
            move(max_y/2-2, max_x/2-10 +i);
            refresh();
            move(max_y/5*4-2,max_x/2-22);
            clrtoeol;
            printw("Try %s",random_pass_generator());
            move(max_y/2-2, max_x/2-10 +i);
        }
        else{
            pass[i++]=ch;
            refresh();}
    }
    curs_set(0);
    switch (check_password(pass)){
        case 1:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            mvprintw(max_y/5*4, max_x/2-33,"Thy cipher must be of seven characters or more,");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 2:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            mvprintw(max_y/5*4, max_x/2-40,"Thy cipher must bear at least one character of the lower script,");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 3:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            printw("Thou must ensure thy password hath at least one letter of the grand form,");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 4:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            mvprintw(max_y/5*4, max_x/2-35,"Thy secret word must needs bear at least one numeral,");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 0:
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            return 1;
    }
}

///////////////////////////////////////////////////////////////////////////////

int check_email(char email[100]){
    int i=0, has_atsign=0, has_dot=0;
    while(email[i]!='\0'){
        if(email[i]=='@'){
            has_atsign=i;
            ++i;
            break;
        }
        else{++i;}
    }
    if(has_atsign==0){return 0;}
    while(email[i]!='\0'){
        if(email[i]=='.'){
            has_dot=i;
            ++i;
            break;
        }
        else{++i;}
    }
    if(has_dot==0){return 0;}
    else if(has_dot-has_atsign==1){return 0;}
    else if(has_dot==strlen(email)-1){return 0;}
    return 1;
}

///////////////////////////////////////////////////////////////////////

int make_email(char email[100]){
    curs_set(1);
    move(max_y/2+3, max_x/2-10);
    clrtoeol();
    int i = 0;
    while(1) {
        int ch=getch();
        if (ch=='\n' || i >= 99) {
            email[i]='\0';
            break;
        }
        else if (ch==KEY_BACKSPACE){
            if (i>0){
                i--;
                move(max_y/2+3, max_x/2-10 +i);
                printw(" ");
                move(max_y/2+3, max_x/2-10 +i);
                refresh();}
            else{move(max_y/2+3, max_x/2-10);}
        }
        else if(ch==KEY_DC){
            return 0;
        }
        else{
            email[i++]=ch;
            refresh();}
    }
    curs_set(0);
    if(check_email(email)==0){
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(max_y/5*4, max_x/2-35,"Thy missive must needs be crafted in the proper manner,");
        attroff(A_BOLD | A_UNDERLINE);
        make_email(email);
    }
    else{
        move(max_y/5*4, max_x/2-35);
        clrtoeol();
        return 1;
    }
}

//////////////////////////////////////////////////////////////////////////////////

int create_account(){
    clear();
    mvprintw(max_y/4, max_x/2-50,"Dear Questor, please enter the name you wish to be recognized by in this realm,");
    mvprintw(max_y/3, max_x/2-30,"Questor's Title : ");
    mvprintw(max_y/2-2, max_x/2-30,"Cipher of Trust : ");
    mvprintw(max_y/2+3, max_x/2-33,"Electronic Missive : ");
    mvprintw(max_y/9*8, max_x/2-23,"Press delete to go back,");
    refresh();
    
    char new_user[100];
    if(make_username(new_user)==0)
        return 0;

    char pass[100];
    if(make_password(pass)==0)
        return 0;

    char email[100];
    if(make_email(email)==0)
        return 0;

    FILE *fptr=fopen("users.txt","a");
    fputs(new_user,fptr);
    fputs("\n",fptr);
    fputs(pass,fptr);
    fputs("\n",fptr);
    fputs(email,fptr);
    fputs("\n",fptr);
    fputs("0",fptr); //gold
    fputs("\n",fptr);
    fputs("0",fptr); //score
    fputs("\n",fptr);
    fputs("0",fptr); //experience
    fputs("\n",fptr);
    fputs("$$$$$$$$$$$$$$$$$$$$",fptr);
    fputs("\n",fptr);
    fclose(fptr);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

char *check_user_login(char user[]){
    FILE *ptr=fopen("users.txt","r");
    char info[100];
    while(fgets(info,100,ptr)){
        char pass[100];
        char email[100];
        char breaker[100];
        int gold; int score; int exp;
        fgets(pass,100,ptr);
        fgets(email,100,ptr);
        fscanf(ptr,"%d",&gold);
        fscanf(ptr,"%d",&score);
        fscanf(ptr,"%d",&exp);
        fgets(breaker,100,ptr);
        info[strcspn(info, "\n")]='\0';
        if(strcmp(user,info)==0){
            char *pass_copy = malloc(strlen(pass) +1);
            strcpy(pass_copy, pass);
            fclose(ptr);
            return pass_copy;
        }
    }
    fclose(ptr);
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////

char *login_username(char user[]){
    curs_set(1);
    move(max_y/3, max_x/2-10);
    int i = 0;
    echo();
    while(1) {
        int ch=getch();
        if (ch=='\n' || i >= 99){
            user[i]='\0';
            break;
        }
        else if (ch==KEY_BACKSPACE){
            if (i>0){
                i--;
                mvprintw(max_y/3, max_x/2-10 +i," ");
                move(max_y/3, max_x/2-10 +i);
                refresh();}
            else{move(max_y/3, max_x/2-10);}
        }
        else if(ch==KEY_DC){
            return NULL;
        }
        else{
            user[i++]=ch;
            refresh();}
    }
    curs_set(0);
    char *pass=check_user_login(user);
    if(pass==NULL){
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(max_y/5*3, max_x/2-30,"No such name exists in this realm,");
        attroff(A_BOLD | A_UNDERLINE);
        move(max_y/3, max_x/2-10);
        clrtoeol();
        login_username(user);
    }
    else{
        move(max_y/5*3, max_x/2-30);
        clrtoeol();
        return pass;}
}

///////////////////////////////////////////////////////////////////////////

char *login_password(char *real_pass,char user[]){
    mvprintw(max_y/8*7,max_x/2-35,"Forgotten your secret cipher? Press \\ to unseal it,");
    curs_set(1);
    move(max_y/2-2, max_x/2-10);
    clrtoeol();
    char *pass=(char *)malloc(100*sizeof(char));
    int i = 0;
    while(1) {
        int ch=getch();
        if (ch=='\n' || i >= 99) {
            pass[i]='\0';
            break;
        }
        else if (ch==KEY_BACKSPACE){
            if (i>0){
                i--;
                move(max_y/2-2, max_x/2-10 +i);
                printw(" ");
                move(max_y/2-2, max_x/2-10 +i);
                refresh();}
            else{move(max_y/2-2, max_x/2-10);}
        }
        else if(ch==KEY_DC){
            return NULL;
        }
        else if(ch=='\\'){
            clear();
            curs_set(0);
            mvprintw(max_y/2-2, max_x/2-40,"I swear upon mine honor, 'tis I, %s, attempting to enter the gates,",user);
            mvprintw(max_y/2-1, max_x/2-37, "Should I speak falsely, may the flames of perdition consume me,");
            mvprintw(max_y/2+3,max_x/2-20,"Press Enter to continue,");
            mvprintw(max_y/2+5,max_x/2-17,"Press Q to quit,");
            refresh();
            while(1){
                int ch=getch();
                if(ch=='\n'){
                    clear();
                    mvprintw(max_y/2-1, max_x/2-30,"Your Cipher of Trust is : %s",real_pass);
                    refresh();
                    napms(4000);
                    return NULL;
                }
                else if(ch=='q'){
                    return NULL;
                }
            }
        }
        else{
            pass[i++]=ch;
            refresh();}
    }
    curs_set(0);
    pass[strcspn(pass, "\n")]='\0';
    real_pass[strcspn(real_pass, "\n")]='\0';
    if(strcmp(pass,real_pass)==0){
        move(max_y/5*3, max_x/2-40);
        clrtoeol();
        char *user_copy = malloc(strlen(user) +1);
        strcpy(user_copy, user);
        return user_copy;}
    else{
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(max_y/5*3, max_x/2-35,"Thy password is not as it should be, noble one,");
        attroff(A_BOLD | A_UNDERLINE);
        move(max_y/2-2, max_x/2-10);
        clrtoeol();
        return login_password(real_pass,user);
    }
}

/////////////////////////////////////////////////////////////////////////////////

char *login(){
    clear();
    mvprintw(max_y/4, max_x/2-50,"Greetings, fair soul! Please declare your Title and Cipher of Trust to proceed,");
    mvprintw(max_y/3, max_x/2-30,"Questor's Title : ");
    mvprintw(max_y/2-2, max_x/2-30,"Cipher of Trust : ");
    mvprintw(max_y/9*8,max_x/2-25,"Press delete to go back,");
    mvprintw(max_y/9*8-2,max_x/2-40,"Press '=' to enter as a humble guest, with no name to bear,");
    refresh();
    char user[100];
    char *real_pass=login_username(user);
    if(real_pass==NULL){return NULL;}
    return login_password(real_pass,user);
}

//////////////////////////////////////////////////////////////////////////////

typedef struct {
    char username[100];
    int gold;
    int score;
    int exp;
} Player;

void scoreboard(char *userr){
    cbreak(); 
    init_color(COLOR_YELLOW, 800, 800, 0);
    init_color(COLOR_GREEN,300,700,0);
    init_pair(30,COLOR_YELLOW,COLOR_BLACK);
    init_pair(31,COLOR_CYAN,COLOR_BLACK);
    init_pair(32,COLOR_GREEN,COLOR_BLACK);
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 30;
    int win_width = 100;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
    scrollok(win, TRUE);
    wattron(win,A_BOLD);
    mvwprintw(win,2,45,"Scoreboard");
    wattroff(win,A_BOLD);
    
    FILE *ptr = fopen("users.txt", "r");
    if (!ptr) {
        mvwprintw(win, 3, 5, "Error: Cannot open users.txt");
        wrefresh(win);
        getch();
        delwin(win);
        return;
    }

    Player players[100];
    int total_players = 0;

    while (fgets(players[total_players].username, 100, ptr)) {
        players[total_players].username[strcspn(players[total_players].username, "\n")] = '\0';  // Remove newline

        char pass[100], email[100], breaker[100];
        fgets(pass, 100, ptr);
        fgets(email, 100, ptr);
        fscanf(ptr, "%d", &players[total_players].gold); fgetc(ptr);
        fscanf(ptr, "%d", &players[total_players].score); fgetc(ptr);
        fscanf(ptr, "%d", &players[total_players].exp); fgetc(ptr);
        fgets(breaker, 100, ptr); 

        total_players++;
    }
    fclose(ptr);

    for (int i = 0; i < total_players - 1; i++) {
        for (int j = i + 1; j < total_players; j++) {
            if (players[j].score > players[i].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    for (int i = 0; i < total_players; i++) {
        if (i == 0) {
            wattron(win, COLOR_PAIR(30) | A_STANDOUT);
            mvwprintw(win, 3 + i, 5, "🥇 The Grandmaster");
        } else if (i == 1) {
            wattron(win, COLOR_PAIR(31) | A_STANDOUT);
            mvwprintw(win, 3 + i, 5, "🥈 The Ascendant");
        } else if (i == 2) {
            wattron(win, COLOR_PAIR(32) | A_STANDOUT);
            mvwprintw(win, 3 + i, 5, "🥉 The Challenger");
        } else {
            mvwprintw(win, 3 + i, 5, "%d.", i + 1);
        }

        if (strcmp(players[i].username, userr) == 0) {
            wattron(win, A_BLINK);
        }

        mvwprintw(win, 3 + i, 23, "%s | Gold: %d | Score: %d | Exp: %d",
                  players[i].username, players[i].gold, players[i].score, players[i].exp);

        wattroff(win, A_BLINK);
        wattroff(win, COLOR_PAIR(30) | A_STANDOUT);
        wattroff(win, COLOR_PAIR(31) |A_STANDOUT);
        wattroff(win, COLOR_PAIR(32) | A_STANDOUT);
    }
    wrefresh(win);
    int ch;
    while (1) {
        ch = getch();
        if (ch == KEY_UP) {
            wscrl(win, 1);
        } else if (ch == KEY_DOWN) {
            wscrl(win, -1);
        }
        else{break;}
        wrefresh(win);
    }
    werase(win);
    wrefresh(win);
    delwin(win);  
    touchwin(stdscr);     
    refresh(); 
}


/////////////////////////////////////////////////////////////////////////////

void first_page();
void setting(char *user);
void menu(char *user);


void setting(char *user){
    noecho();
    curs_set(0);
    char *d="Difficulty";
    char *e="Music";
    char *f="Character Design";
    attron(A_BOLD);
    mvprintw(max_y/5+3, max_x/2-3,"Setting");
    attroff(A_BOLD);
    mvprintw(max_y/5+22, max_x/2-9,"Press e to go back,");
    button_maker(max_y/5+7,max_x/2-10,1,d);
    button_maker(max_y/5+12,max_x/2-10,0,e);
    button_maker(max_y/5+17,max_x/2-10,0,f);
    int cur_but=1;
    while(1){
        int cmdd=getch();
        switch(cmdd){
        case KEY_UP:
        cur_but-=1;
        if(cur_but==0) cur_but=3;
        if(cur_but==1){
            button_maker(max_y/5+7,max_x/2-10,1,d);
            button_maker(max_y/5+12,max_x/2-10,0,e);}
        else if(cur_but==2){
            button_maker(max_y/5+12,max_x/2-10,1,e);
            button_maker(max_y/5+17,max_x/2-10,0,f);}
        else if(cur_but==3){
            button_maker(max_y/5+17,max_x/2-10,1,f);
            button_maker(max_y/5+7,max_x/2-10,0,d);}
        break;
        case KEY_DOWN:
            cur_but+=1;
            if(cur_but==4) cur_but=1;
            if(cur_but==2){
                button_maker(max_y/5+7,max_x/2-10,0,d);
                button_maker(max_y/5+12,max_x/2-10,1,e);}
            else if(cur_but==3){
                button_maker(max_y/5+12,max_x/2-10,0,e);
                button_maker(max_y/5+17,max_x/2-10,1,f);}
            else if(cur_but==1){
                button_maker(max_y/5+17,max_x/2-10,0,f);
                button_maker(max_y/5+7,max_x/2-10,1,d);}
            break;
            case 'e':
                clear();
                menu(user);
                break;
            case '\n':
                if(cur_but==1){
                    int screen_height, screen_width;
                    getmaxyx(stdscr, screen_height, screen_width);
                    int win_height = 20;
                    int win_width = 60;
                    int start_y = (screen_height - win_height) / 2;
                    int start_x = (screen_width - win_width) / 2;
                    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
                    box(win,ACS_PLUS, ACS_DIAMOND);
                    wattron(win,A_BOLD);
                    mvwprintw(win,2,14,"Choose your preferred difficulty,");
                    wattroff(win,A_BOLD);
                    wattron(win,A_ITALIC);
                    mvwprintw(win,4,14,"Note: The higher the difficulty,");
                    mvwprintw(win,5,13,"the more damage the monsters will deal,");
                    mvwprintw(win,6,13,"but you'll earn more points in return,");
                    wattroff(win,A_ITALIC);
                    char options[3][10]={"Easy","Medium","Hard"};
                    for(int i=1;i<=3;i++){
                        if(i==difficulty){wattron(win,A_BOLD | A_BLINK);}
                        mvwprintw(win,6+2*i,25,"%d : %s",i,options[i-1]);
                        if(i==difficulty){wattroff(win,A_BOLD | A_BLINK);}
                    }
                    wrefresh(win);
                    int temp=getch();
                    int tempp=temp-'0';
                    if(tempp==1){difficulty=1;}
                    else if(tempp==2){difficulty=2;}
                    else if(tempp==3){difficulty=3;}
                    werase(win);
                    wrefresh(win);
                    delwin(win);  
                    touchwin(stdscr);     
                    refresh(); 
                }
                else if(cur_but==2){
                    int screen_height, screen_width;
                    getmaxyx(stdscr, screen_height, screen_width);
                    int win_height = 20;
                    int win_width = 60;
                    int start_y = (screen_height - win_height) / 2;
                    int start_x = (screen_width - win_width) / 2;
                    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
                    box(win,ACS_PLUS, ACS_DIAMOND);
                    wattron(win,A_BOLD);
                    mvwprintw(win,2,10,"Choose your preferred music,");
                    wattroff(win,A_BOLD);
                    wattron(win,A_ITALIC);
                    wattroff(win,A_ITALIC);
                    for(int i=0;i<6;i++){
                        if(i==current_music){wattron(win,A_BOLD | A_BLINK);}
                        mvwprintw(win,5+2*i,10,"%d : %s",i+1,musiclist[i]);
                        if(i==current_music){wattroff(win,A_BOLD | A_BLINK);}
                    }
                    wrefresh(win);
                    int temp=getch();
                    int tempp=temp-'0';
                    if(tempp>=1 && tempp<=5){
                        music_on=1;
                        current_music=tempp-1;
                        strcpy(song,musiclist[current_music]);
                        pthread_t music_thread;
                        pthread_create(&music_thread, NULL, play_music, NULL);
                    }
                    else if(tempp==6){
                        current_music=tempp-1;
                        music_on=0;}
                    werase(win);
                    wrefresh(win);
                    delwin(win);  
                    touchwin(stdscr);     
                    refresh(); 
                }
                else if(cur_but==3){
                    int screen_height, screen_width;
                    getmaxyx(stdscr, screen_height, screen_width);
                    int win_height = 20;
                    int win_width = 60;
                    int start_y = (screen_height - win_height) / 2;
                    int start_x = (screen_width - win_width) / 2;
                    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
                    box(win,ACS_PLUS, ACS_DIAMOND);
                    wattron(win,A_BOLD);
                    mvwprintw(win,2,10,"Choose your preferred character,");
                    wattroff(win,A_BOLD);
                    for(int i=0;i<6;i++){
                        if(i==current_character){wattron(win,A_BLINK);}
                        mvwprintw(win,5+2*i,10,"%d : %lc",i+1,characterlist[i]);
                        if(i==current_character){wattroff(win,A_BLINK);}
                    }
                    wrefresh(win);
                    int temp=getch();
                    int tempp=temp-'0';
                    if(tempp>=1 && tempp<=6){
                        current_character=tempp-1;
                    }
                    werase(win);
                    wrefresh(win);
                    delwin(win);  
                    touchwin(stdscr);     
                    refresh(); 
                }
                break;
            default:
                break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void menu(char *user){
    noecho();
    curs_set(0);
    clear();
    attron(A_BOLD);
    mvprintw(max_y/5-2, max_x/2-7,"Welcome, %s!",user);
    mvprintw(max_y/5+27, max_x/2-7,"Press e to exit,");
    attroff(A_BOLD);
    char *c="New Game";
    char *d="Resume Game";
    char *e="Scoreboard";
    char *f="Profile";
    char *g="Setting";
    int cur_but=1;
    button_maker(max_y/5+2,max_x/2-10,1,c);
    button_maker(max_y/5+7,max_x/2-10,0,d);
    button_maker(max_y/5+12,max_x/2-10,0,e);
    button_maker(max_y/5+17,max_x/2-10,0,f);
    button_maker(max_y/5+22,max_x/2-10,0,g);
    refresh();
    while(1){
        int cmdd=getch();
        switch(cmdd){
        case KEY_UP:
        cur_but-=1;
        if(cur_but==0) cur_but=5;
        if(cur_but==1){
            button_maker(max_y/5+2,max_x/2-10,1,c);
            button_maker(max_y/5+7,max_x/2-10,0,d);}
        else if(cur_but==2){
            button_maker(max_y/5+7,max_x/2-10,1,d);
            button_maker(max_y/5+12,max_x/2-10,0,e);}
        else if(cur_but==3){
            button_maker(max_y/5+12,max_x/2-10,1,e);
            button_maker(max_y/5+17,max_x/2-10,0,f);}
        else if(cur_but==4){
            button_maker(max_y/5+17,max_x/2-10,1,f);
            button_maker(max_y/5+22,max_x/2-10,0,g);}
        else if(cur_but==5){
            button_maker(max_y/5+22,max_x/2-10,1,g);
            button_maker(max_y/5+2,max_x/2-10,0,c);}
        break;
        case KEY_DOWN:
            cur_but+=1;
            if(cur_but==6) cur_but=1;
            if(cur_but==2){
                button_maker(max_y/5+2,max_x/2-10,0,c);
                button_maker(max_y/5+7,max_x/2-10,1,d);}
            else if(cur_but==3){
                button_maker(max_y/5+7,max_x/2-10,0,d);
                button_maker(max_y/5+12,max_x/2-10,1,e);}
            else if(cur_but==4){
                button_maker(max_y/5+12,max_x/2-10,0,e);
                button_maker(max_y/5+17,max_x/2-10,1,f);}
            else if(cur_but==5){
                button_maker(max_y/5+17,max_x/2-10,0,f);
                button_maker(max_y/5+22,max_x/2-10,1,g);}
            else if(cur_but==1){
                button_maker(max_y/5+22,max_x/2-10,0,g);
                button_maker(max_y/5+2,max_x/2-10,1,c);}
            break;
            case 'e':
                clear();
                first_page();
                break;
            case '\n':
                if(cur_but==5){
                    clear();
                    setting(user);
                }
                else if(cur_but==3){
                    scoreboard(user);
                }
                else if(cur_but==4){
                    int screen_height, screen_width;
                    getmaxyx(stdscr, screen_height, screen_width);
                    int win_height = 20;
                    int win_width = 60;
                    int start_y = (screen_height - win_height) / 2;
                    int start_x = (screen_width - win_width) / 2;
                    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
                    box(win,ACS_PLUS, ACS_DIAMOND);
                    wattron(win,A_BOLD);
                    mvwprintw(win,2,4,"Profile :");
                    wattroff(win,A_BOLD);
                    FILE *ptr=fopen("users.txt","r");
                    char info[100];
                    while(fgets(info,100,ptr)){
                        char pass[100];
                        char email[100];
                        char breaker[100];
                        int gold; int score; int exp;
                        fgets(pass,100,ptr);
                        fgets(email,100,ptr);
                        fscanf(ptr,"%d",&gold);
                        fscanf(ptr,"%d",&score);
                        fscanf(ptr,"%d",&exp);
                        fgets(breaker,100,ptr);
                        info[strcspn(info, "\n")]='\0';
                        if(strcmp(user,info)==0){
                            mvwprintw(win,4,4,"Username : %s",info);
                            mvwprintw(win,6,4,"Password : %s",pass);
                            mvwprintw(win,8,4,"Email : %s",email);
                            mvwprintw(win,10,4,"Total gold collected : %d",gold);
                            mvwprintw(win,12,4,"Score : %d",score);
                            mvwprintw(win,14,4,"Total experience : %d",exp);
                            mvwprintw(win,3,40,"      ***   ");
                            mvwprintw(win,4,40,"    *******  ");
                            mvwprintw(win,5,40,"   ********* "); 
                            mvwprintw(win,6,40,"/\\* ### ### */\\");
                            mvwprintw(win,7,40,"|    @ / @    |");
                            mvwprintw(win,8,40,"\\/\\    ^    /\\/");
                            mvwprintw(win,9,40,"   \\  ===  /  ");
                            mvwprintw(win,10,40,"    \\_____/  ");
                            mvwprintw(win,11,40,"     _|_|_  ");
                            mvwprintw(win,12,40,"  *$$$$$$$$$*");
                            fclose(ptr);
                            break;
                        }  
                    }
                    wrefresh(win);
                    int c=getch();
                    werase(win);
                    wrefresh(win);
                    delwin(win);  
                    touchwin(stdscr);     
                    refresh(); 
                }
                break;
            default:
                break;

        }
    }

}

//////////////////////////////////////////////////////////////////////////////

void first_page(){

    curs_set(0);
    welcome_printer();

    char *a ="Login";
    char *b="Create Account";
    button_maker(max_y/3, max_x/2-10, true, a);
    button_maker(max_y/2-2, max_x/2-10, false, b);

    int current_button=1;

    while(1){
        int cmd=getch();
        switch (cmd){
            case KEY_UP:
                if(current_button==2){
                    clear();
                    welcome_printer();
                    button_maker(max_y/3, max_x/2-10, true, a);
                    button_maker(max_y/2-2, max_x/2-10, false, b);
                    current_button=1;
                    refresh();
                }
                break; 
            case KEY_DOWN:
                if(current_button==1){
                    clear();
                    welcome_printer();
                    button_maker(max_y/3, max_x/2-10, false, a);
                    button_maker(max_y/2-2, max_x/2-10, true, b);
                    current_button=2;
                    refresh();
                }
                break; 
            case 'q':
                endwin();
                return;
            case '\n':
                if(current_button==2){
                    if(create_account()==0){
                        clear();
                        first_page();}
                    else{
                        clear();
                        attron(A_BOLD);
                        mvprintw(max_y/3, max_x/2-37,"The gates to the realm are now open,Enjoy your journey!");
                        attroff(A_BOLD);
                        refresh();
                        napms(4000);
                        clear();
                        first_page();
                    }
                    break;
                }
                else if(current_button==1){
                    char *user=login();
                    if(user==NULL){
                        clear();
                        first_page();
                    }
                    else{
                        menu(user);
                    }
                }
                break;
            default:
                break; 
        }
    }


}

int main() {
    strcpy(song,musiclist[current_music]);
    pthread_t music_thread;
    pthread_create(&music_thread, NULL, play_music, NULL);
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    getmaxyx(stdscr, max_y, max_x);
    first_page();
    pthread_join(music_thread, NULL);
    SDL_Quit();
    return 0;
}

