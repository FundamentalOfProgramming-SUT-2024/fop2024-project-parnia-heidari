#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h> 
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <pthread.h>
#include <unistd.h>


wchar_t characterlist[6]={L'🯅',L'🯉',L'♛',L'♚',L'♜',L'♞'};
int current_character=0;

int allow_to_move=1;
int monster_health=0;
int first_time_we_see_it=0;
char monster_mode='0';
int monster_x=0,monster_y=0;
int monster_prevx=0, monster_prevy=0;
int whos_turn_is_it=1;
int last_dir=0;
int monster_moves_num=0;

void monster_file_editor_1(int x,int y){ //removes the monster's current position from the file
    FILE *fptr=fopen("window_state.txt","r");
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(cx==x && cy==y){
            fprintf(temp,"1 %d %d : .\n",cx,cy);}
        else{fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    fclose(fptr);
    remove("window_state.txt");
    rename("temp.txt","window_state.txt");
    fclose(temp);
}

void monster_file_editor_2(int x,int y, char monster){ //adds the new location of the monster to the file
    FILE *fptr=fopen("window_state.txt","r");
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(cx==x && cy==y){
            fprintf(temp,"1 %d %d : %c\n",cx,cy,monster);}
        else{fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    fclose(fptr);
    remove("window_state.txt");
    rename("temp.txt","window_state.txt");
    fclose(temp);
}


int monster_mover(char monster,int mx,int my, int dx, int dy){ //mx & my -> monster, dx & dy -> destination
    monster_prevx=monster_x; monster_prevy=monster_y;
    int x_moves[4]={1,0,-1,0};
    int y_moves[4]={0,-1,0,1};
    if(allow_to_move){
    for(int i=0;i<4;i++){
        if(dx==(mx+x_moves[i]) && dy==(my+y_moves[i])){return 1;} //this means damage
        else if( abs(dx - (mx+x_moves[i]))<=abs(dx - mx) && abs(dy - (my+y_moves[i]))<=abs(dy - my) && mvinch(my+y_moves[i],mx+x_moves[i])=='.'){
            attron(COLOR_PAIR(21));
            mvprintw(my+y_moves[i],mx+x_moves[i],"%c",monster);
            attroff(COLOR_PAIR(21));
            mvprintw(my,mx,".");
            monster_x=mx+x_moves[i]; monster_y=my+y_moves[i];
            refresh();
            monster_file_editor_1(monster_prevx,monster_prevy);
            monster_file_editor_2(monster_x,monster_y,monster);
            return 0;
        }
    }
    int x_moves2[2];
    int y_moves2[2];
    if(mx==dx){
        x_moves2[0]=0; x_moves2[1]=0;
        y_moves2[0]=1; y_moves2[1]=-1;
    }
    else if(my==dy){
        x_moves2[0]=1; x_moves2[1]=-1;
        y_moves2[0]=0; y_moves2[1]=0;
    }
    for(int i=0;i<2;i++){
        if(mvinch(my+y_moves2[i],mx+x_moves2[i])=='.'){
            attron(COLOR_PAIR(21));
            mvprintw(my+y_moves2[i],mx+x_moves2[i],"%c",monster);
            attroff(COLOR_PAIR(21));
            mvprintw(my,mx,".");
            monster_x=mx+x_moves2[i]; monster_y=my+y_moves2[i];
            refresh();
            monster_file_editor_1(monster_prevx,monster_prevy);
            monster_file_editor_2(monster_x,monster_y,monster);
            return 0;
        }
    }}
}

int monster_damage(char monster, int difficulty){
    int d;
    if(monster=='D'){
        d=(difficulty)*5;
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"The Demon hits you—%d damage taken!",d);
        refresh();
        attroff(A_ITALIC);
        return d;
    }
    else if(monster=='F'){
        d=(difficulty+1)*5;
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"The Fire Breathing Monster hits you—%d damage taken!",d);
        refresh();
        attroff(A_ITALIC);
        return d;
    }
    else if(monster=='G'){
        d=(difficulty+2)*5;
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"The Giant hits you—%d damage taken!",d);
        refresh();
        attroff(A_ITALIC);
        return d;
    }
    else if(monster=='S'){
        d=(difficulty+3)*5;
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"The Snake hits you—%d damage taken!",d);
        refresh();
        attroff(A_ITALIC);
        return d;
    }
    else if(monster=='U'){
        d=(difficulty+5)*5;
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"The Undead hits you—%d damage taken!",d);
        refresh();
        attroff(A_ITALIC);
        return d;
    }
}

void weapon_user(char weapon[100],int direction,int x,int y,int strength){
    int x_moves[9]={-1,0,1,-1,-1,1,-1,0,1};
    int y_moves[9]={1,1,1,0,0,0,-1,-1,-1};
    if(strcmp(weapon,"Mace")==0){
        int d=5*strength;
        for(int i=0;i<9;i++){
            chtype ch =mvinch(y+y_moves[i],x+x_moves[i]);
            char c = ch & A_CHARTEXT;
            if(c=='S'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Snake—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='U'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Undead—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='G'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Giant—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='F'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Fire Breathing Monster—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='D'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Deamon—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
        }
    }
    else if(strcmp(weapon,"Sword")==0){
        int d=10*strength;
        for(int i=0;i<9;i++){
            chtype ch =mvinch(y+y_moves[i],x+x_moves[i]);
            char c = ch & A_CHARTEXT;
            if(c=='S'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Snake—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='U'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Undead—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='G'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Giant—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='F'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Fire Breathing Monster—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
            else if(c=='D'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Deamon—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                refresh(); return;
            }
        }
    }
    else if(strcmp(weapon,"Magic wand")==0){
        int first=0;
        int d=15*strength;
        for(int i=0;i<10;i++){
            chtype ch =mvinch(y+y_moves[direction-1],x+x_moves[direction-1]);
            char c = ch & A_CHARTEXT;
            if(c=='S'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Snake—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='U'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Undead—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='G'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Giant—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='F'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Fire Breathing Monster—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='D'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Deamon—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='.'){
                if(first==0){mvprintw(y,x,"%lc",characterlist[current_character]);}
                else mvprintw(y,x,".");
                first++;
                mvprintw(y+y_moves[direction-1],x+x_moves[direction-1],"*");
                refresh();
                napms(100);
            }
            else{
                mvprintw(y,x,".");
                refresh(); return;
            }
            y+=y_moves[direction-1]; x+=x_moves[direction-1];
        }
        mvprintw(y,x,".");
    }
    else if(strcmp(weapon,"Dagger")==0){
        int d=12*strength;
        int first=0;
        for(int i=0;i<5;i++){
            chtype ch =mvinch(y+y_moves[direction-1],x+x_moves[direction-1]);
            char c = ch & A_CHARTEXT;
            if(c=='S'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Snake—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='U'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Undead—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='G'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Giant—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='F'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Fire Breathing Monster—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='D'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Deamon—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='.'){
                if(first==0){mvprintw(y,x,"%lc",characterlist[current_character]);}
                else mvprintw(y,x,".");
                first++;
                mvprintw(y+y_moves[direction-1],x+x_moves[direction-1],"%lc",L'🗡');
                refresh();
                napms(100);
            }
            else{
                mvprintw(y,x,".");
                refresh(); return;
            }
            y+=y_moves[direction-1]; x+=x_moves[direction-1];
        }
        mvprintw(y,x,".");
    }
    else if(strcmp(weapon,"Arrows")==0){
        int d=5*strength;
        int first=0;
        for(int i=0;i<5;i++){
            chtype ch =mvinch(y+y_moves[direction-1],x+x_moves[direction-1]);
            char c = ch & A_CHARTEXT;
            if(c=='S'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Snake—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='U'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Undead—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='G'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Giant—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='F'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Fire Breathing Monster—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='D'){
                attron(A_ITALIC);
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"You hit the Deamon—%d damage dealt!",d);
                attroff(A_ITALIC);
                monster_health-=d;
                if(first) mvprintw(y,x,".");
                refresh(); return;
            }
            else if(c=='.'){
                if(first==0){mvprintw(y,x,"%lc",characterlist[current_character]);}
                else mvprintw(y,x,".");
                first++;
                mvprintw(y+y_moves[direction-1],x+x_moves[direction-1],"⌁");
                refresh();
                napms(100);
            }
            else{
                if(first==0){
                    mvprintw(0,0,"                                                                   ");
                    mvprintw(0,0,"You hit the arrow to yourself!");
                    refresh();
                    refresh(); return;
                }
                mvprintw(y,x,"%lc",L'➴');
                break;
            }
            y+=y_moves[direction-1]; x+=x_moves[direction-1];
        }
        mvprintw(y,x,"%lc",L'➴');
        strcpy(items[x][y],"Used arrow");
        refresh();
        refresh(); return;
    }
}

int arrow_details(int n){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 24;
    int win_width = 60;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *weapon = newwin(win_height, win_width, start_y, start_x);
    box(weapon,ACS_PLUS, ACS_DIAMOND);
    wattron(weapon,A_BOLD);
    mvwprintw(weapon,3,3,"Arrows :");
    wattroff(weapon,A_BOLD);
    mvwprintw(weapon,6,3,"Distance : 5");
    mvwprintw(weapon,5,3,"%d left",n);
    mvwprintw(weapon,7,3,"Damage : 5");
    mvwprintw(weapon,9,3,"Press S to select.");
    mvwprintw(weapon,10,3,"Press E to go back.");
mvwprintw(weapon,2,25,"           4$$-.");                   
mvwprintw(weapon,3,25,"           4   \".");                                      
mvwprintw(weapon,4,25,"           4    ^.");                                       
mvwprintw(weapon,5,25,"           4     $ ");                                       
mvwprintw(weapon,6,25,"           4     'b ");                                      
mvwprintw(weapon,7,25,"           4      \"b.");                                     
mvwprintw(weapon,8,25,"           4        $   ");                                  
mvwprintw(weapon,9,25,"           4        $r  ");                                  
mvwprintw(weapon,10,25,"           4         $F ");                                 
mvwprintw(weapon,11,25,"-$b========4========$b====*P=->");                          
mvwprintw(weapon,12,25,"           4       *$$F  ");                                 
mvwprintw(weapon,13,25,"           4        $$\" ");                                  
mvwprintw(weapon,14,25,"           4       .$F ");                                   
mvwprintw(weapon,15,25,"           4       dP ");                                    
mvwprintw(weapon,16,25,"           4      F  ");                                     
mvwprintw(weapon,17,25,"           4     @  ");                                      
mvwprintw(weapon,18,25,"           4    . ");                                        
mvwprintw(weapon,19,25,"           J. F");                                           
mvwprintw(weapon,20,25,"          '$$ ");  
wrefresh(weapon);
char c=getch();
werase(weapon);
wrefresh(weapon);
delwin(weapon);  
if(c=='s'){return 1;}
else{return 0;}
}

int sword_details(){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 25;
    int win_width = 60;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *weapon = newwin(win_height, win_width, start_y, start_x);
    box(weapon, ACS_PLUS, ACS_DIAMOND);
    wattron(weapon,A_BOLD);
    mvwprintw(weapon,3,3,"Sword :");
    wattroff(weapon,A_BOLD);
    mvwprintw(weapon,6,3,"Close-range weapon");
    mvwprintw(weapon,7,3,"Damage : 10");
    mvwprintw(weapon,9,3,"Press S to select.");
    mvwprintw(weapon,10,3,"Press E to go back.");
mvwprintw(weapon,2,39,"     .-.");
mvwprintw(weapon,3,39,"    (0.0)");
mvwprintw(weapon,4,39,"  '=.|m|.='");
mvwprintw(weapon,5,39,"  .='/@\\`=.");
mvwprintw(weapon,6,39,"     @8@");
mvwprintw(weapon,7,39," _   8@8   _");
mvwprintw(weapon,8,39,"(@__/@8@\\__@)");
mvwprintw(weapon,9,39," `-=:8@8:=-'");
mvwprintw(weapon,10,39,"     |:|");
mvwprintw(weapon,11,39,"     |:|");
mvwprintw(weapon,12,39,"     |:|");
mvwprintw(weapon,13,39,"     |:|");
mvwprintw(weapon,14,39,"     |:|");
mvwprintw(weapon,15,39,"     |:|");
mvwprintw(weapon,16,39,"     |:|");
mvwprintw(weapon,17,39,"     |:|");
mvwprintw(weapon,18,39,"     |:|");
mvwprintw(weapon,19,39,"     |:|");
mvwprintw(weapon,20,39,"     |:|");
mvwprintw(weapon,21,39,"     |:|");
mvwprintw(weapon,22,39,"     \\:/");
mvwprintw(weapon,23,39,"      ^");
wrefresh(weapon);
char c=getch();
werase(weapon);
wrefresh(weapon);
delwin(weapon);  
if(c=='s'){return 1;}
else{return 0;}
}

int dagger_details(int n){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 21;
    int win_width = 60;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *weapon = newwin(win_height, win_width, start_y, start_x);
    box(weapon,ACS_PLUS, ACS_DIAMOND);
    wattron(weapon,A_BOLD);
    mvwprintw(weapon,3,3,"Dagger :");
    wattroff(weapon,A_BOLD);
    mvwprintw(weapon,6,3,"Distance : 5");
    mvwprintw(weapon,5,3,"%d left",n);
    mvwprintw(weapon,7,3,"Damage : 12");
    mvwprintw(weapon,9,3,"Press S to select.");
    mvwprintw(weapon,10,3,"Press E to go back.");
mvwprintw(weapon,2,35,"       .---.");
mvwprintw(weapon,3,35,"       |---|");
mvwprintw(weapon,4,35,"       |---|");
mvwprintw(weapon,5,35,"       |---|");
mvwprintw(weapon,6,35,"   .---^ - ^---.");
mvwprintw(weapon,7,35,"   :___________:");
mvwprintw(weapon,8,35,"      |  |//|");
mvwprintw(weapon,9,35,"      |  |//|");
mvwprintw(weapon,10,35,"      |  |//|");
mvwprintw(weapon,11,35,"      |  |//|");
mvwprintw(weapon,12,35,"      |  |//|");
mvwprintw(weapon,13,35,"      |  |//|");
mvwprintw(weapon,14,35,"      |  |.-|");
mvwprintw(weapon,15,35,"      |.-'**|");
mvwprintw(weapon,16,35,"       \\***/");
mvwprintw(weapon,17,35,"        \\*/");
mvwprintw(weapon,18,35,"         V");
wrefresh(weapon);
char c=getch();
werase(weapon);
wrefresh(weapon);
delwin(weapon);  
if(c=='s'){return 1;}
else{return 0;}
}

int mace_details(){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 20;
    int win_width = 60;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *weapon = newwin(win_height, win_width, start_y, start_x);
    box(weapon, ACS_PLUS, ACS_DIAMOND);
    wattron(weapon,A_BOLD);
    mvwprintw(weapon,3,3,"Mace :");
    wattroff(weapon,A_BOLD);
    mvwprintw(weapon,6,3,"Close-range weapon");
    mvwprintw(weapon,7,3,"Damage : 5");
    mvwprintw(weapon,9,3,"Press S to select.");
    mvwprintw(weapon,10,3,"Press E to go back.");
mvwprintw(weapon,2,35,"         <<()>>");
mvwprintw(weapon,3,35,"          )__(");
mvwprintw(weapon,4,35,"          )__(");
mvwprintw(weapon,5,35,"          )__(");
mvwprintw(weapon,6,35,"          )__(");
mvwprintw(weapon,7,35,"          )__(");
mvwprintw(weapon,8,35,"          )__(");
mvwprintw(weapon,9,35,"          )__(");
mvwprintw(weapon,10,35,"          )__(");
mvwprintw(weapon,11,35,"          )__(");
mvwprintw(weapon,12,35,"         _)__(_");
mvwprintw(weapon,13,35,"       .'      `.");
mvwprintw(weapon,14,35,"       | <   >  |>");
mvwprintw(weapon,15,35,"      <|   <   >|");
mvwprintw(weapon,16,35,"        `.____.'");
mvwprintw(weapon,17,35,"          V  V");
wrefresh(weapon);
char c=getch();
werase(weapon);
wrefresh(weapon);
delwin(weapon);  
if(c=='s'){return 1;}
else{return 0;}
}

int wand_details(int n){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int win_height = 22;
    int win_width = 61;
    int start_y = (screen_height - win_height) / 2;
    int start_x = (screen_width - win_width) / 2;
    WINDOW *weapon = newwin(win_height, win_width, start_y, start_x);
    box(weapon,ACS_PLUS, ACS_DIAMOND);
    wattron(weapon,A_BOLD);
    mvwprintw(weapon,3,3,"Magic Wand :");
    wattroff(weapon,A_BOLD);
    mvwprintw(weapon,6,3,"Distance : 10");
    mvwprintw(weapon,5,3,"%d left",n);
    mvwprintw(weapon,7,3,"Damage : 15");
    mvwprintw(weapon,7,3,"Stops the enemy from moving");
    mvwprintw(weapon,10,3,"Press S to select.");
    mvwprintw(weapon,12,3,"Press E to go back.");
mvwprintw(weapon,2,35,"             *");
mvwprintw(weapon,3,35,"       *   *");
mvwprintw(weapon,4,35,"     *    \\* / *");
mvwprintw(weapon,5,35,"       * --.:. *");
mvwprintw(weapon,6,35,"      *   * :\\ -");
mvwprintw(weapon,7,35,"        .*  | \\");
mvwprintw(weapon,8,35,"       * *     \\");
mvwprintw(weapon,9,35,"     .  *       \\");
mvwprintw(weapon,10,35,"      ..        /\\.");
mvwprintw(weapon,11,35,"     *          |\\)|");
mvwprintw(weapon,12,35,"   .   *         \\ |");
mvwprintw(weapon,13,35,"  . . *           |/\\");
mvwprintw(weapon,14,35,"     .* *         /  \\");
mvwprintw(weapon,15,35,"   *              \\ / \\");
mvwprintw(weapon,16,35," *  .  *           \\   \\");
mvwprintw(weapon,17,35,"    * .  ");
mvwprintw(weapon,18,35,"   *    *   "); 
mvwprintw(weapon,19,35,"  .   *    *  ");
wrefresh(weapon);
char c=getch();
werase(weapon);
wrefresh(weapon);
delwin(weapon);  
if(c=='s'){return 1;}
else{return 0;}
}





