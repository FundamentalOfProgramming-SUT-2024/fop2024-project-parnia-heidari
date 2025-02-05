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


#include "map.c"
#include "monster.c"

char musiclist[6][100]={"Fading Footsteps in the Forest.mp3","Passages .mp3","Tales Beneath the Willow .mp3","The old Oak .mp3","The Vagabond .mp3","None"};
int current_music=0;

int gold_to_be_added=0;
int score_to_be_added=0;

struct inv{
    char enchantments[5][40]; //max 5
    int enchantment_num;
    char weapons[20][40]; // unlimited
    int weapon_num;
    int current_weapon;
    int dagger_num;
    int wand_num;
    int arrow_num;
    char foods[5][40]; //max 5
    int food_quality[5];
    int food_num;
    int gold;
    int score;
    int health;
    int health_timer;
    int health_timer2;
    int hunger;
    int strength;
    int strength_timer;
    int speed;
    int speed_timer;
    int visited_floors[6];
    int floor_num;
    int current_floor;
};

struct inv inventory;

/////////////////////////////////////////////////////////
char song[200];

int music_on = 1; 
void *play_music(void *arg){
    while (1) {
        if (!music_on) {
            Mix_HaltMusic();
            SDL_Delay(500);
            continue;
        }
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        Mix_Music *music = Mix_LoadMUS(song);
        Mix_PlayMusic(music, -1);
        while (Mix_PlayingMusic()) {
            SDL_Delay(100);
            if (!music_on) {
                Mix_HaltMusic();
                break;
            }
        }
        Mix_FreeMusic(music);
    }
    Mix_CloseAudio();
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

void score_printer(){
    attron(COLOR_PAIR(10));
    mvprintw(1,171,"Score : %d",inventory.score);
    attroff(COLOR_PAIR(10));
    refresh();
}

void hunger_printer(){
    if(inventory.hunger>500){inventory.hunger=500;}
    int num=inventory.hunger/50 +1;
    mvprintw(44,172,"             ");
    attron(COLOR_PAIR(9));
    mvprintw(44,163,"Hunger : ");
    for(int i=0;i<num;i++){
        mvprintw(44,172+i,"#");
    }
    attroff(COLOR_PAIR(9));
    refresh();
}

void health_printer(){
    if(inventory.health>300){inventory.health=300;}
    int num=inventory.health/30 +1;
    mvprintw(44,11,"           ");
    attron(COLOR_PAIR(9));
    mvprintw(44,1,"Health : ");
    for(int i=0;i<num;i++){
        mvprintw(44,11+i,"#");
    }
    attroff(COLOR_PAIR(9));
    refresh();
}

void monster_health_printer(){
    int num=monster_health/5 +1;
    mvprintw(43,18,"              ");
    attron(COLOR_PAIR(21));
    mvprintw(43,1,"Monster Health : ");
    for(int i=0;i<num;i++){
        mvprintw(43,18+i,"#");
    }
    attroff(COLOR_PAIR(21));
    refresh();
}

int health_checker(){
    if(inventory.health<=0){
        return 1;
    }
    else{
        return 0;
    }
}

void monster_health_checker(){
    if(monster_health<0){
        monster_file_editor_1(monster_x,monster_y);
        attron(A_ITALIC);
        mvprintw(0,0,"                                                                                  ");
        mvprintw(0,0,"You have slayed the enemy!");
        attroff(A_ITALIC);
        mvprintw(monster_y,monster_x,".");
        monster_health=0;
        whos_turn_is_it=1;
        monster_x=0; monster_y=0;
        allow_to_move=1;
        if(monster_mode=='D' || monster_mode=='F'){inventory.score+=10;}
        else if(monster_mode=='G' || monster_mode=='S'){inventory.score+=20;}
        if(monster_mode=='U'){inventory.score+=30;}
        monster_mode='0';
        first_time_we_see_it=0;
        mvprintw(43,1,"                       ");
        refresh();
        score_printer();
    }
}

//////////////////////////////////////////////////////////////////////////

void map_saver(FILE *fptr){
    int x,y;
    getmaxyx(stdscr,y,x);
    for(int j=0;j<y;j++){
        for(int i=0;i<x;i++){
            if(items[i][j][0]=='\0' || mvinch(j,i)=='+'){
                if(mvinch(j,i)==' '){continue;}
                fprintf(fptr,"0 %d %d : %c\n",i,j,mvinch(j,i));
            }
            else{
                fprintf(fptr,"0 %d %d : %s\n",i,j,items[i][j]);
                if(strcmp(items[i][j],"lower wall")!=0 && strcmp(items[i][j],"pillar")!=0 && strcmp(items[i][j],"winner")!=0
                && strcmp(items[i][j],"Mace")!=0 && strcmp(items[i][j],"Dagger")!=0 && strcmp(items[i][j],"Magic wand")!=0 
                && strcmp(items[i][j],"Arrows")!=0 && strcmp(items[i][j],"Sword")!=0 && strcmp(items[i][j],"Used arrow")!=0
                && strcmp(items[i][j],"lower wall green")!=0 && strcmp(items[i][j],"lower wall purple")!=0 && strcmp(items[i][j],"Found Trap")!=0
                && strcmp(items[i][j],"side wall green")!=0 && strcmp(items[i][j],"side wall purple")!=0 && strcmp(items[i][j],"lower wall yellow")!=0
                && strcmp(items[i][j],"side wall yellow")!=0 && strcmp(items[i][j],"upper wall yellow")!=0 && strcmp(items[i][j],"Trap")!=0
                && strcmp(items[i][j],"upper wall green")!=0 && strcmp(items[i][j],"upper wall purple")!=0){
                    i+=1;}
            }
        }
    }
}

void map_explorer(FILE *fptr, int x,int y,room **rooms){
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    int x_moves[60]={0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,-1,-2,-3,-4,-5,1,1,1,1,-1,-1,-1,-1,2,2,2,-2,-2,-2,-3,-3,3,3,4,-4,1,1,1,1,-1,-1,-1,-1,2,2,2,-2,-2,-2,-3,-3,3,3,4,-4};
    int y_moves[60]={1,2,3,4,5,-1,-2,-3,-4,-5,0,0,0,0,0,0,0,0,0,0,1,2,3,4,1,2,3,4,1,2,3,1,2,3,1,2,1,2,1,1,-1,-2,-3,-4,-1,-2,-3,-4,-1,-2,-3,-1,-2,-3,-1,-2,-1,-2,-1,-1};
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        int printed=0;
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        for(int i=0;i<60;i++){
        if(cx==x+x_moves[i] && cy==y+y_moves[i]){
            if(cx==nightmareroom.x_door && cy==nightmareroom.y_door) fprintf(temp,"1 %d %d : %s\n",cx,cy,object);
            else if(cx>=nightmareroom.x1 && cx<=nightmareroom.x2 &&
            cy>=nightmareroom.y1 && cy<=nightmareroom.y2){
                fprintf(temp,"0 %d %d : %s\n",cx,cy,object);}
            else fprintf(temp,"1 %d %d : %s\n",cx,cy,object);
            printed=1;
            if(object[0]=='<' || object[0]=='>'){
                attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
                mvprintw(cy,cx,"%c",object[0]);
                attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
            }
            else if(object[1]=='\0'){
                mvprintw(cy,cx,"%c",object[0]);
                if(object[0]=='D' || object[0]=='F' || object[0]=='G' || object[0]=='S' || object[0]=='U'){
                    init_pair(21,COLOR_RED,COLOR_BLACK);
                    attron(COLOR_PAIR(21));
                    mvprintw(cy,cx,"%c",object[0]);
                    attroff(COLOR_PAIR(21));
                    if(first_time_we_see_it==0){
                        monster_mode=object[0];
                        if(object[0]=='D' || object[0]=='F'){
                            monster_health=30;
                            monster_moves_num=5;}
                        else if(object[0]=='S'){
                            monster_health=40;
                            monster_moves_num=200;}
                        else if(object[0]=='G'){
                            monster_health=40;
                            monster_moves_num=10;}
                        else{monster_health=45;
                            monster_moves_num=10;}
                        monster_health_printer();
                        first_time_we_see_it=1;
                    }
                    monster_x=cx; monster_y=cy;
                }
            }
            else{
                strcpy(items[cx][cy],object);
                if(strcmp(object,"Obsidian")==0){mvprintw(cy,cx,"%lc",L'🔮');}
                else if(strcmp(object,"winner")==0){
                    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
                    mvprintw(cy,cx,"%lc",L'᪥');
                attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);}
                else if(strcmp(object,"Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
                else if(strcmp(object,"Found trap")==0){mvprintw(cy,cx,"&");}
                else if(strcmp(object,"Trap")==0){mvprintw(cy,cx,".");}
                else if(strcmp(object,"Magical Food")==0){
                    attron(COLOR_PAIR(6));
                    mvprintw(cy,cx,"%lc",L'🍖');
                    attroff(COLOR_PAIR(6));}
                else if(strcmp(object,"Exquisite Food")==0){
                    attron(COLOR_PAIR(3));
                    mvprintw(cy,cx,"%lc",L'🍖');
                    attroff(COLOR_PAIR(3));}
                else if(strcmp(object,"Food")==0){mvprintw(cy,cx,"%lc",L'🍖');}
                else if(strcmp(object,"Used arrow")==0){mvprintw(cy,cx,"%lc",L'➴');}
                else if(strcmp(object,"Arrows")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'➴');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Magic wand")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'☦');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Dagger")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'🗡');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Sword")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'⚔');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Mace")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'⚒');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Enchantment of Damage")==0){
                    attron(COLOR_PAIR(5));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(5));}
                else if(strcmp(object,"Enchantment of Speed")==0){
                    attron(COLOR_PAIR(4));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(4));}
                else if(strcmp(object,"Enchantment of Health")==0){
                    attron(COLOR_PAIR(3));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(3));}
                else if(strcmp(object,"pillar")==0){
                    attron(A_BOLD);
                    mvprintw(cy,cx,"%c",'O');
                    attroff(A_BOLD);}
                else if(strcmp(object,"lower wall")==0){mvprintw(cy,cx,"%lc",L'—');}
                else if(strcmp(object,"Fake Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"Fake Enchantment")==0){mvprintw(cy,cx,"%lc",L'🏺');}
            else if(strcmp(object,"lower wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"upper wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"side wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"lower wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"upper wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"side wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"lower wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"upper wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"side wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(10));}
            }
            }
        }
        if(printed==0){fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    remove(mapfile);
    rename("temp.txt",mapfile);
    fclose(temp);
}

void nightmare_map_explorer(FILE *fptr, int x,int y){
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    int x_moves[12]={1,1,1,2,-1,-1,-1,-2,0,0,0,0};
    int y_moves[12]={0,1,-1,0,0,1,-1,0,1,2,-1,-2};
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        int printed=0;
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        for(int i=0;i<12;i++){
        if(cx==x+x_moves[i] && cy==y+y_moves[i]){
            fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);
            printed=1;
            if(object[0]=='<' || object[0]=='>'){
                attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
                mvprintw(cy,cx,"%c",object[0]);
                attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
            }
            else if(object[1]=='\0'){
                mvprintw(cy,cx,"%c",object[0]);
            }
            else{
                strcpy(items[cx][cy],object);
                if(strcmp(object,"Obsidian")==0){mvprintw(cy,cx,"%lc",L'🔮');}
                else if(strcmp(object,"Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
                else if(strcmp(object,"Magical Food")==0){
                    attron(COLOR_PAIR(6));
                    mvprintw(cy,cx,"%lc",L'🍖');
                    attroff(COLOR_PAIR(6));}
                else if(strcmp(object,"Exquisite Food")==0){
                    attron(COLOR_PAIR(3));
                    mvprintw(cy,cx,"%lc",L'🍖');
                    attroff(COLOR_PAIR(3));}
                else if(strcmp(object,"Food")==0){mvprintw(cy,cx,"%lc",L'🍖');}
                else if(strcmp(object,"Arrows")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'➴');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Magic wand")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'☦');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Dagger")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'🗡');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Sword")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'⚔');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Mace")==0){
                    attron(COLOR_PAIR(2));
                    mvprintw(cy,cx,"%lc",L'⚒');
                    attroff(COLOR_PAIR(2));}
                else if(strcmp(object,"Enchantment of Damage")==0){
                    attron(COLOR_PAIR(5));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(5));}
                else if(strcmp(object,"Enchantment of Speed")==0){
                    attron(COLOR_PAIR(4));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(4));}
                else if(strcmp(object,"Enchantment of Health")==0){
                    attron(COLOR_PAIR(3));
                    mvprintw(cy,cx,"%lc",L'🏺');
                    attroff(COLOR_PAIR(3));}
                else if(strcmp(object,"pillar")==0){
                    attron(A_BOLD);
                    mvprintw(cy,cx,"%c",'O');
                    attroff(A_BOLD);}
                else if(strcmp(object,"lower wall")==0){mvprintw(cy,cx,"%lc",L'—');}
                else if(strcmp(object,"Fake Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"Fake Enchantment")==0){mvprintw(cy,cx,"%lc",L'🏺');}
            else if(strcmp(object,"lower wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"upper wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"side wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(9));}
                else if(strcmp(object,"lower wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"upper wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"side wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"lower wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"upper wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"side wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(10));}
                }
            }
        }
        if(printed==0){fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    remove(mapfile);
    rename("temp.txt",mapfile);
    fclose(temp);
}

void map_loader(){
    FILE *fptr=fopen(mapfile,"r");
    int cx,cy,found;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(object[0]=='\0'){
            continue;
        }
        else if(object[0]=='<' || object[0]=='>'){
            attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
            mvprintw(cy,cx,"%c",object[0]);
            attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
        }
        else if(strcmp(object,"..")==0){mvprintw(cy,cx,"..");}
        else if(object[1]=='\0'){
            mvprintw(cy,cx,"%c",object[0]);
            if(object[0]=='D' || object[0]=='F' || object[0]=='G' || object[0]=='S' || object[0]=='U'){
                    init_pair(21,COLOR_RED,COLOR_BLACK);
                    attron(COLOR_PAIR(21));
                    mvprintw(cy,cx,"%c",object[0]);
                    attroff(COLOR_PAIR(21));
                }
        }
        else{
            strcpy(items[cx][cy],object);
            if(strcmp(object,"Obsidian")==0){mvprintw(cy,cx,"%lc",L'🔮');}
            else if(strcmp(object,"Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"winner")==0){
                    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
                    mvprintw(cy,cx,"%lc",L'᪥');
                attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);}
            else if(strcmp(object,"Magical Food")==0){
                attron(COLOR_PAIR(6));
                mvprintw(cy,cx,"%lc",L'🍖');
                attroff(COLOR_PAIR(6));}
            else if(strcmp(object,"Exquisite Food")==0){
                attron(COLOR_PAIR(3));
                mvprintw(cy,cx,"%lc",L'🍖');
                attroff(COLOR_PAIR(3));}
            else if(strcmp(object,"Food")==0){mvprintw(cy,cx,"%lc",L'🍖');}
            else if(strcmp(object,"Found trap")==0){mvprintw(cy,cx,"&");}
            else if(strcmp(object,"Trap")==0){mvprintw(cy,cx,".");}
            else if(strcmp(object,"Used arrow")==0){mvprintw(cy,cx,"%lc",L'➴');}
            else if(strcmp(object,"Arrows")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'➴');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Magic wand")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'☦');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Dagger")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'🗡');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Sword")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'⚔');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Mace")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'⚒');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Enchantment of Damage")==0){
                attron(COLOR_PAIR(5));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(5));}
            else if(strcmp(object,"Enchantment of Speed")==0){
                attron(COLOR_PAIR(4));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(4));}
            else if(strcmp(object,"Enchantment of Health")==0){
                attron(COLOR_PAIR(3));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(3));}
            else if(strcmp(object,"pillar")==0){
                attron(A_BOLD);
                mvprintw(cy,cx,"%c",'O');
                attroff(A_BOLD);}
            else if(strcmp(object,"lower wall")==0){mvprintw(cy,cx,"%lc",L'—');}
            else if(strcmp(object,"Fake Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"Fake Enchantment")==0){mvprintw(cy,cx,"%lc",L'🏺');}
            else if(strcmp(object,"lower wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"upper wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"side wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(9));}
                else if(strcmp(object,"lower wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"upper wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"side wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"lower wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"upper wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"side wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(10));}
        }
    }
    fclose(fptr);
}

void grabber(int x,int y){
    FILE *fptr=fopen(mapfile,"r");
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(cx==x && cy==y){
            fprintf(temp,"1 %d %d : .\n",cx+1,cy);
            fprintf(temp,"1 %d %d : .\n",cx,cy);}
        else{fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    fclose(fptr);
    remove(mapfile);
    rename("temp.txt",mapfile);
    fclose(temp);
}

void dropper(int x,int y, char thing[100]){
    FILE *fptr=fopen(mapfile,"r");
    FILE *temp=fopen("temp.txt","w");
    int cx,cy,found;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(cx==x && cy==y){
            fprintf(temp,"1 %d %d : %s\n",cx,cy,thing);}
        else if(cx==x+1 && cy==y){continue;}
        else{fprintf(temp,"%d %d %d : %s\n",found,cx,cy,object);}
    }
    fclose(fptr);
    remove(mapfile);
    rename("temp.txt",mapfile);
    fclose(temp);
}

void print_current_state(){
    FILE *fptr=fopen(mapfile,"r");
    int found,cx,cy;
    char object[100];
    while(fscanf(fptr, "%d %d %d : ",&found,&cx,&cy)==3){
        fgets(object,100,fptr);
        object[strlen(object)-1]='\0';
        if(found==1){
        if(object[0]=='\0'){
            continue;
        }
        else if(object[0]=='<' || object[0]=='>'){
            attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
            mvprintw(cy,cx,"%c",object[0]);
            attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
        }
        else if(strcmp(object,"..")==0){mvprintw(cy,cx,"..");}
        else if(object[1]=='\0'){
            mvprintw(cy,cx,"%c",object[0]);
            if(object[0]=='D' || object[0]=='F' || object[0]=='G' || object[0]=='S' || object[0]=='U'){
                    init_pair(21,COLOR_RED,COLOR_BLACK);
                    attron(COLOR_PAIR(21));
                    mvprintw(cy,cx,"%c",object[0]);
                    attroff(COLOR_PAIR(21));
                }
        }
        else{
            strcpy(items[cx][cy],object);
            if(strcmp(object,"Obsidian")==0){mvprintw(cy,cx,"%lc",L'🔮');}
            else if(strcmp(object,"Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"winner")==0){
                    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
                    mvprintw(cy,cx,"%lc",L'᪥');
                attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);}
            else if(strcmp(object,"Magical Food")==0){
                attron(COLOR_PAIR(6));
                mvprintw(cy,cx,"%lc",L'🍖');
                attroff(COLOR_PAIR(6));}
            else if(strcmp(object,"Exquisite Food")==0){
                attron(COLOR_PAIR(3));
                mvprintw(cy,cx,"%lc",L'🍖');
                attroff(COLOR_PAIR(3));}
            else if(strcmp(object,"Food")==0){mvprintw(cy,cx,"%lc",L'🍖');}
            else if(strcmp(object,"Found trap")==0){mvprintw(cy,cx,"&");}
            else if(strcmp(object,"Trap")==0){mvprintw(cy,cx,".");}
            else if(strcmp(object,"Used arrow")==0){mvprintw(cy,cx,"%lc",L'➴');}
            else if(strcmp(object,"Arrows")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'➴');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Magic wand")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'☦');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Dagger")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'🗡');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Sword")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'⚔');
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Mace")==0){
                attron(COLOR_PAIR(2));
                mvprintw(cy,cx,"%lc",L'⚒'); 
                attroff(COLOR_PAIR(2));}
            else if(strcmp(object,"Enchantment of Damage")==0){
                attron(COLOR_PAIR(5));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(5));}
            else if(strcmp(object,"Enchantment of Speed")==0){
                attron(COLOR_PAIR(4));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(4));}
            else if(strcmp(object,"Enchantment of Health")==0){
                attron(COLOR_PAIR(3));
                mvprintw(cy,cx,"%lc",L'🏺');
                attroff(COLOR_PAIR(3));}
            else if(strcmp(object,"pillar")==0){
                attron(A_BOLD);
                mvprintw(cy,cx,"%c",'O');
                attroff(A_BOLD);}
            else if(strcmp(object,"lower wall")==0){mvprintw(cy,cx,"%lc",L'—');}
            else if(strcmp(object,"Fake Gold")==0){mvprintw(cy,cx,"%lc",L'🧈');}
            else if(strcmp(object,"Fake Enchantment")==0){mvprintw(cy,cx,"%lc",L'🏺');}
            else if(strcmp(object,"lower wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"upper wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(9));}
            else if(strcmp(object,"side wall green")==0){
                attron(COLOR_PAIR(9));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(9));}
                else if(strcmp(object,"lower wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"upper wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"side wall purple")==0){
                attron(COLOR_PAIR(19));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(19));}
            else if(strcmp(object,"lower wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"%lc",L'—');
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"upper wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"_");
                attroff(COLOR_PAIR(10));}
            else if(strcmp(object,"side wall yellow")==0){
                attron(COLOR_PAIR(10));
                mvprintw(cy,cx,"|");
                attroff(COLOR_PAIR(10));}
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int current_x,current_y;
char prev;

void info_saver(){
    FILE *ptr=fopen(infofile,"w");
    fprintf(ptr,"%d\n",inventory.enchantment_num);
    for(int i=0;i<inventory.enchantment_num;i++){
        fprintf(ptr,"%s\n",inventory.enchantments[i]);
    }
    fprintf(ptr,"%d\n",inventory.weapon_num);
    for(int i=0;i<inventory.weapon_num;i++){
        fprintf(ptr,"%s\n",inventory.weapons[i]);
    }
    fprintf(ptr,"%d\n",inventory.current_weapon);
    fprintf(ptr,"%d\n",inventory.dagger_num);
    fprintf(ptr,"%d\n",inventory.wand_num);
    fprintf(ptr,"%d\n",inventory.arrow_num);
    fprintf(ptr,"%d\n",inventory.food_num);
    for(int i=0;i<inventory.food_num;i++){
        fprintf(ptr,"%s\n",inventory.foods[i]);
    }
    for(int i=0;i<inventory.food_num;i++){
        fprintf(ptr,"%d\n",inventory.food_quality[i]);
    }
    fprintf(ptr,"%d\n",inventory.gold);
    fprintf(ptr,"%d\n",inventory.score);
    fprintf(ptr,"%d\n",inventory.health);
    fprintf(ptr,"%d\n",inventory.health_timer);
    fprintf(ptr,"%d\n",inventory.health_timer2);
    fprintf(ptr,"%d\n",inventory.hunger);
    fprintf(ptr,"%d\n",inventory.strength);
    fprintf(ptr,"%d\n",inventory.strength_timer);
    fprintf(ptr,"%d\n",inventory.speed);
    fprintf(ptr,"%d\n",inventory.speed_timer);
    for(int i=0;i<6;i++){
        fprintf(ptr,"%d\n",inventory.visited_floors[i]);
    }
    fprintf(ptr,"%d\n",inventory.floor_num);
    fprintf(ptr,"%d\n",inventory.current_floor);
    fclose(ptr);
}

void mapdetail_saver(){
    FILE *ptr=fopen(mapdetailfile,"w");
    fprintf(ptr,"%d\n",current_x);
    fprintf(ptr,"%d\n",current_y);
    fprintf(ptr,"%c\n",prev);
    fprintf(ptr,"%d\n",enchantmentroom.exists);
    if(enchantmentroom.exists){
        fprintf(ptr,"%d\n",enchantmentroom.x1 );
        fprintf(ptr,"%d\n",enchantmentroom.x2);
        fprintf(ptr,"%d\n",enchantmentroom.y1);
        fprintf(ptr,"%d\n",enchantmentroom.y2);
        fprintf(ptr,"%d\n",enchantmentroom.x_door);
        fprintf(ptr,"%d\n",enchantmentroom.y_door);
    }
    fprintf(ptr,"%d\n",nightmareroom.exists);
    if(nightmareroom.exists){
        fprintf(ptr,"%d\n",nightmareroom.x1 );
        fprintf(ptr,"%d\n",nightmareroom.x2);
        fprintf(ptr,"%d\n",nightmareroom.y1);
        fprintf(ptr,"%d\n",nightmareroom.y2);
        fprintf(ptr,"%d\n",nightmareroom.x_door);
        fprintf(ptr,"%d\n",nightmareroom.y_door);
    }
    fprintf(ptr,"%d\n",goldroom.exists);
    if(goldroom.exists){
        fprintf(ptr,"%d\n",goldroom.x1 );
        fprintf(ptr,"%d\n",goldroom.x2);
        fprintf(ptr,"%d\n",goldroom.y1);
        fprintf(ptr,"%d\n",goldroom.y2);
        fprintf(ptr,"%d\n",goldroom.x_door);
        fprintf(ptr,"%d\n",goldroom.y_door);
    }
    fclose(ptr);
}


void info_loader(){
    FILE *ptr=fopen(infofile,"r");
    fscanf(ptr,"%d\n",&inventory.enchantment_num);
    for(int i=0;i<inventory.enchantment_num;i++){
        fscanf(ptr,"%s\n",inventory.enchantments[i]);
    }
    fscanf(ptr,"%d\n",&inventory.weapon_num);
    for(int i=0;i<inventory.weapon_num;i++){
        fscanf(ptr,"%s\n",inventory.weapons[i]);
    }
    fscanf(ptr,"%d\n",&inventory.current_weapon);
    fscanf(ptr,"%d\n",&inventory.dagger_num);
    fscanf(ptr,"%d\n",&inventory.wand_num);
    fscanf(ptr,"%d\n",&inventory.arrow_num);
    fscanf(ptr,"%d\n",&inventory.food_num);
    for(int i=0;i<inventory.food_num;i++){
        fscanf(ptr,"%s\n",inventory.foods[i]);
    }
    for(int i=0;i<inventory.food_num;i++){
        fscanf(ptr,"%d\n",&inventory.food_quality[i]);
    }
    fscanf(ptr,"%d\n",&inventory.gold);
    fscanf(ptr,"%d\n",&inventory.score);
    fscanf(ptr,"%d\n",&inventory.health);
    fscanf(ptr,"%d\n",&inventory.health_timer);
    fscanf(ptr,"%d\n",&inventory.health_timer2);
    fscanf(ptr,"%d\n",&inventory.hunger);
    fscanf(ptr,"%d\n",&inventory.strength);
    fscanf(ptr,"%d\n",&inventory.strength_timer);
    fscanf(ptr,"%d\n",&inventory.speed);
    fscanf(ptr,"%d\n",&inventory.speed_timer);
    for(int i=0;i<6;i++){
        fscanf(ptr,"%d\n",&inventory.visited_floors[i]);
    }
    fscanf(ptr,"%d\n",&inventory.floor_num);
    fscanf(ptr,"%d\n",&inventory.current_floor);
    fclose(ptr);
}

void mapdetail_loader(){
    FILE *ptr=fopen(mapdetailfile,"r");
    fscanf(ptr,"%d\n",&current_x);
    fscanf(ptr,"%d\n",&current_y);
    fscanf(ptr,"%c\n",&prev);
    fscanf(ptr,"%d\n",&enchantmentroom.exists);
    if(enchantmentroom.exists){
        fscanf(ptr,"%d\n",&enchantmentroom.x1 );
        fscanf(ptr,"%d\n",&enchantmentroom.x2);
        fscanf(ptr,"%d\n",&enchantmentroom.y1);
        fscanf(ptr,"%d\n",&enchantmentroom.y2);
        fscanf(ptr,"%d\n",&enchantmentroom.x_door);
        fscanf(ptr,"%d\n",&enchantmentroom.y_door);
    }
    fscanf(ptr,"%d\n",&nightmareroom.exists);
    if(nightmareroom.exists){
        fscanf(ptr,"%d\n",&nightmareroom.x1 );
        fscanf(ptr,"%d\n",&nightmareroom.x2);
        fscanf(ptr,"%d\n",&nightmareroom.y1);
        fscanf(ptr,"%d\n",&nightmareroom.y2);
        fscanf(ptr,"%d\n",&nightmareroom.x_door);
        fscanf(ptr,"%d\n",&nightmareroom.y_door);
    }
    fscanf(ptr,"%d\n",&goldroom.exists);
    if(goldroom.exists){
        fscanf(ptr,"%d\n",&goldroom.x1 );
        fscanf(ptr,"%d\n",&goldroom.x2);
        fscanf(ptr,"%d\n",&goldroom.y1);
        fscanf(ptr,"%d\n",&goldroom.y2);
        fscanf(ptr,"%d\n",&goldroom.x_door);
        fscanf(ptr,"%d\n",&goldroom.y_door);
    }
    fclose(ptr);
}


////////////////////////////////////////////////////////

int nightmare=1;

int character_mover(room **rooms, int total_room){
    while(1){
    int c=getch();
    int next_x,next_y=-1;
    switch (c){
        case '8':
            next_x=current_x; next_y=current_y-inventory.speed;
            break;
        case '2':
            next_x=current_x; next_y=current_y+inventory.speed;
            break;
        case '4':
            next_x=current_x-inventory.speed; next_y=current_y;
            break;
        case '6':
            next_x=current_x+inventory.speed; next_y=current_y;
            break;
        case '9':
            next_x=current_x+inventory.speed; next_y=current_y-inventory.speed;
            break;
        case '7':
            next_x=current_x-inventory.speed; next_y=current_y-inventory.speed;
            break;
        case '3':
            next_x=current_x+inventory.speed; next_y=current_y+inventory.speed;
            break;
        case '1':
            next_x=current_x-inventory.speed; next_y=current_y+inventory.speed;
            break;
        case '\n':
            int screen_height, screen_width;
            getmaxyx(stdscr, screen_height, screen_width);
            int win_height = 20;
            int win_width = 60;
            int start_y = (screen_height - win_height) / 2;
            int start_x = (screen_width - win_width) / 2;
            WINDOW *win = newwin(win_height, win_width, start_y, start_x);
            box(win,ACS_PLUS, ACS_DIAMOND);
            wattron(win, A_BOLD);
            mvwprintw(win, 1, 25,"Pause");
            wattroff(win, A_BOLD);
            mvwprintw(win, 3, 3, "Press:");
            mvwprintw(win, 4, 3, "1 : How to Play");
            mvwprintw(win, 5, 3, "2 : Save and Quit");
            wrefresh(win);
            int a=getch();
            if(a=='2'){
                werase(win);
                wrefresh(win);
                delwin(win);  
                touchwin(stdscr);     
                refresh(); 
                return -2;
            }
            if(a=='1'){
                werase(win);
                wrefresh(win);
                box(win,ACS_PLUS, ACS_DIAMOND);
                mvwprintw(win,2,2,"Enter -> Pause");
                mvwprintw(win,3,2,"m -> View the entire map");
                mvwprintw(win,4,2,"s -> Look for traps");
                mvwprintw(win,5,2,"f -> Move fast in one direction");
                mvwprintw(win,6,2,"e -> Inventory");
                mvwprintw(win,7,2,"Space -> Attack");
                mvwprintw(win,8,2,"a -> Repeat the previous attack(Long-ranged weapons)");
                wrefresh(win);
                getch();
                werase(win);
                wrefresh(win);
                delwin(win);  
                touchwin(stdscr);     
                refresh(); 
            }
            else{
                werase(win);
                wrefresh(win);
                delwin(win);  
                touchwin(stdscr);     
                refresh(); 
            }
            break;
        case 'm':
            clear();
            map_loader();
            refresh();
            char chh=getch();
            while(chh!='m'){
                chh=getch();
            }
            clear();
            print_current_state();
            attron(COLOR_PAIR(10));
            mvprintw(0,172,"Gold : %d",inventory.gold);
            attroff(COLOR_PAIR(10));
            score_printer();
            hunger_printer();
            health_printer();
            FILE *fptr=fopen(mapfile,"r");
            nightmare_map_explorer(fptr,current_x,current_y);
            fclose(fptr);
            mvprintw(current_y,current_x,"%lc",characterlist[current_character]);
            refresh();
            break;
        case 's':
            int xx_moves[8]={1,1,0,-1,-1,-1,0,1};
            int yy_moves[8]={0,-1,-1,-1,0,1,1,1};
            for(int i=0;i<8;i++){
                if(strcmp(items[current_x+xx_moves[i]][current_y+yy_moves[i]],"Trap")==0){
                    grabber(current_x+xx_moves[i],current_y+yy_moves[i]);
                    char trapp[100]="Found trap";
                    dropper(current_x+xx_moves[i],current_y+yy_moves[i],trapp);
                    strcpy(items[current_x+xx_moves[i]][current_y+yy_moves[i]],"Found trap");
                    mvprintw(current_y+yy_moves[i],current_x+xx_moves[i],"&");
                    refresh();
                }
            }
            break;
        case 'f':
            chh=getch();
            int ch=chh-'0';
            int x_moves[10]={0,-inventory.speed,0,inventory.speed,-inventory.speed,0,inventory.speed,-inventory.speed,0,inventory.speed};
            int y_moves[10]={0,inventory.speed,inventory.speed,inventory.speed,0,0,0,-inventory.speed,-inventory.speed,-inventory.speed};
            if(ch==5 || ch==0 || ch>9 || ch<0){break;}
            mvprintw(current_y,current_x,"%c",prev);
            while(mvinch(current_y + y_moves[ch], current_x + x_moves[ch])=='.' || mvinch(current_y + y_moves[ch], current_x + x_moves[ch])=='+' || mvinch(current_y + y_moves[ch], current_x + x_moves[ch])=='#'){
                current_x=current_x + x_moves[ch];
                current_y=current_y + y_moves[ch];
            if(next_y>=0 && strcmp(items[current_x][current_y],"Trap")==0){
               mvprintw(0,0,"                                                                                ");
                mvprintw(0,0,"You stepped on trap! 30 damage taken!");
                grabber(next_x,next_y);
                char trapp[100]="Found trap";
                dropper(next_x,next_y,trapp);
                strcpy(items[next_x][next_y],"Found trap");
                mvprintw(next_y,next_x,"&");
                inventory.health-=30;
                health_printer();
                if(health_checker()) return 0;
                refresh();
            }
            if(next_y>=0 && strcmp(items[next_x][next_y],"Found trap")==0){
                mvprintw(0,0,"                                                                                ");
                mvprintw(0,0,"You stepped on trap! 50 damage taken!");
                inventory.health-=50;
                health_printer();
                if(health_checker()) return 0;
                refresh();
            }
            if(current_x==goldroom.x_door && current_y==goldroom.y_door){
            mvprintw(0,0,"                                                                                ");
            mvprintw(0,0,"You have entered the Gold room! Victory is close!");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                                  ");
            refresh();
            if(strcmp(song,"Eye of the Tiger.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"Eye of the Tiger.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
        }
            if(current_x==enchantmentroom.x_door && current_y==enchantmentroom.y_door){
            mvprintw(0,0,"                                                                                ");
            mvprintw(0,0,"You have entered the Enchantment room! Each move you make will cause you damage!");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                                  ");
            refresh();
            if(strcmp(song,"enchantmentroom.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"enchantmentroom.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
        }
        if(current_x==nightmareroom.x_door && current_y==nightmareroom.y_door){
            mvprintw(0,0,"You have entered the Nightmare room.. Do not trust what you see..");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                   ");
            refresh();
            if(strcmp(song,"nightmareroom.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"nightmareroom.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            nightmare*=(-1);
        }
        if(current_x>enchantmentroom.x1 && current_x<enchantmentroom.x2 
         && current_y>enchantmentroom.y1 && current_y<enchantmentroom.y2){
            inventory.health-=10;
            health_printer();
            if(health_checker()) return 0;
         }
        if(inventory.hunger>0){inventory.hunger-=1;}
        if(inventory.hunger%50==0){
            if(inventory.hunger>=0){
            hunger_printer();}
        }
        if(inventory.hunger<=0){
            inventory.health-=2;
            health_printer();
            if(health_checker()) return 0;
        }
        for(int i=0;i<inventory.food_num;i++){
            inventory.food_quality[i]-=1;
            if(strcmp(inventory.foods[i],"Exquisite Food")==0 && inventory.food_quality[i]<0){
                strcpy(inventory.foods[i],"Food");
                inventory.food_quality[i]=500;
            }
            else if(strcmp(inventory.foods[i],"Magical Food")==0 && inventory.food_quality[i]<0){
                strcpy(inventory.foods[i],"Food");
                inventory.food_quality[i]=500;
            }
        }
        if(inventory.speed_timer>0){
            inventory.speed_timer--;
            if(inventory.speed_timer==0){inventory.speed=1;}
        }
        if(inventory.strength_timer>0){
            inventory.strength_timer--;
            if(inventory.strength_timer==0){inventory.strength=1;}
        }
        if(inventory.hunger>=450){
            if(inventory.health_timer2>0){
                inventory.health_timer2-=1;
                if(inventory.health_timer2==0){inventory.health_timer=5;}}
            inventory.health+=inventory.health_timer;
            health_printer();
            refresh();
        }
        FILE *fptr=fopen(mapfile,"r");
        if(nightmare==1)map_explorer(fptr,current_x,current_y,rooms);
        for(int i=nightmareroom.x1; i<=nightmareroom.x2; i++){
            for(int j=nightmareroom.y1; j<=nightmareroom.y2; j++){
                if(i==nightmareroom.x_door && j==nightmareroom.y_door) continue;
                mvprintw(j,i," ");
            }
        }
        refresh();
        if(nightmare==-1){
            nightmare_map_explorer(fptr,current_x,current_y);
            mvprintw(current_y,current_x,"%lc",characterlist[current_character]);}
        fclose(fptr);
        refresh();
    }
            prev=mvinch(current_y,current_x);
            mvprintw(current_y,current_x,"%lc",characterlist[current_character]);
            break;
        case 'e':
            screen_height, screen_width;
            getmaxyx(stdscr, screen_height, screen_width);
            win_height = 20;
            win_width = 60;
            start_y = (screen_height - win_height) / 2;
            start_x = (screen_width - win_width) / 2;
            win = newwin(win_height, win_width, start_y, start_x);
            box(win,ACS_PLUS, ACS_DIAMOND);
            wattron(win, A_BOLD);
            mvwprintw(win, 1, 25,"Inventory");
            wattroff(win, A_BOLD);
            mvwprintw(win, 3, 3, "Press:");
            mvwprintw(win, 4, 3, "1 : Foods");
            mvwprintw(win, 5, 3, "2 : Enchantments");
            mvwprintw(win, 6, 3, "3 : Weapons");
            mvwprintw(win, 8, 3, "e : Exit");
            wrefresh(win);
            int cmd=getch();
            switch (cmd){
                case '1':
                    wclear(win);
                    box(win, ACS_PLUS, ACS_DIAMOND);
                    wattron(win, A_BOLD);
                    mvwprintw(win, 1, 27,"Foods");
                    wattroff(win, A_BOLD);
                    for(int i=1;i<=inventory.food_num;i++){
                        mvwprintw(win, i+3, 3, "%d : %s",i,inventory.foods[i-1]);
                    }
                    mvwprintw(win, inventory.food_num+7, 3, "e : Eat");
                    mvwprintw(win, inventory.food_num+8, 3, "d : Drop");
                    mvwprintw(win, inventory.food_num+10, 17, "Press anything to exit.");
                    wrefresh(win);
                    char cmd2=getchar();
                    switch (cmd2){
                        case 'e':
                            mvwprintw(win, inventory.food_num+7, 13, "Select which one you want to eat.");
                            mvwprintw(win, inventory.food_num+7, 3, "          ");
                            mvwprintw(win, inventory.food_num+8, 3, "           ");
                            wrefresh(win);
                            char cmdd=getchar();
                            int cmd3=cmdd - '0';
                            if(cmd3>=1 && cmd3<=inventory.food_num){
                                int whichone=0;
                                if(strcmp(inventory.foods[cmd3-1],"Magical Food")==0){
                                    whichone=2;
                                    inventory.speed=2;
                                    inventory.speed_timer=10;
                                }
                                else if(strcmp(inventory.foods[cmd3-1],"Exquisite Food")==0){
                                    whichone=1;
                                    inventory.strength=2;
                                    inventory.strength_timer=10;
                                }
                                inventory.hunger+=(inventory.food_quality[cmd3-1]);
                                if(inventory.food_quality[cmd3-1]<=0){
                                    mvprintw(0,0,"                                                      ");
                                    mvprintw(0,0,"You ate Rotten Food! %d damage taken.",inventory.food_quality[cmd3-1]);
                                    refresh();
                                }
                                if(inventory.hunger>500){inventory.hunger=500;}
                                else if(inventory.hunger<0){inventory.hunger=0;}
                                werase(win);
                                wrefresh(win);
                                delwin(win);  
                                touchwin(stdscr);     
                                refresh();  
                                hunger_printer();
                                inventory.food_num--;
                                for(int i=cmd3-1;i<inventory.food_num;i++){
                                    strcpy(inventory.foods[i],inventory.foods[i+1]);
                                    inventory.food_quality[i]=inventory.food_quality[i+1];
                                }
                                if(whichone==1){
                                    mvprintw(0,0,"                                                               ");
                                    mvprintw(0,0,"You have extra strength for 10 moves!");
                                    refresh();
                                }
                                else if(whichone==2){
                                    mvprintw(0,0,"                                                               ");
                                    mvprintw(0,0,"You have extra speed for 10 moves!");
                                    refresh();
                                }
                                break;   
                            } 
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;
                        case 'd':
                            mvwprintw(win, inventory.food_num+7, 13, "Select which one you want to drop.");
                            mvwprintw(win, inventory.food_num+7, 3, "          ");
                            mvwprintw(win, inventory.food_num+8, 3, "           ");
                            wrefresh(win);
                            cmdd=getchar();
                            cmd3=cmdd - '0';
                            if(cmd3>=1 && cmd3<=inventory.food_num){
                                if(mvinch(current_y,current_x-1)=='.' && mvinch(current_y,current_x-2)=='.' && mvinch(current_y-1,current_x-1)=='.' && mvinch(current_y+1,current_x-1)=='.' && mvinch(current_y,current_x-3)=='.'){
                                    werase(win);
                                    wrefresh(win);
                                    delwin(win);  
                                    touchwin(stdscr);     
                                    refresh();  
                                    dropper(current_x-2,current_y,inventory.foods[cmd3-1]);
                                    if(strcmp(inventory.foods[cmd3-1],"Exquisite Food")==0){
                                        attron(COLOR_PAIR(3));}
                                    else if(strcmp(inventory.foods[cmd3-1],"Magical Food")==0){
                                        attron(COLOR_PAIR(6));}
                                    mvprintw(current_y,current_x-2,"%lc",L'🍖');
                                    if(strcmp(inventory.foods[cmd3-1],"Exquisite Food")==0){
                                        strcpy(items[current_x-2][current_y],"Exquisite Food");
                                        attroff(COLOR_PAIR(3));}
                                    else if(strcmp(inventory.foods[cmd3-1],"Magical Food")==0){
                                        strcpy(items[current_x-2][current_y],"Magical Food");
                                        attroff(COLOR_PAIR(6));}
                                    else{strcpy(items[current_x-2][current_y],"Food");}
                                    refresh(); 
                                    inventory.food_num--;
                                    for(int i=cmd3-1;i<inventory.food_num;i++){
                                        strcpy(inventory.foods[i],inventory.foods[i+1]);
                                        inventory.food_quality[i]=inventory.food_quality[i+1];
                                    }  
                                }
                                else{
                                    werase(win);
                                    wrefresh(win);
                                    delwin(win);  
                                    touchwin(stdscr);     
                                    refresh(); 
                                    mvprintw(0,0,"You can't drop anything here.");
                                    refresh(); 
                                    napms(2000);
                                    mvprintw(0,0,"                               ");
                                    refresh(); 
                                }
                                break;
                            }
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;
                        default:
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;
                    }
                    break;
                case '2':
                    wclear(win);
                    box(win, ACS_PLUS, ACS_DIAMOND);
                    wattron(win, A_BOLD);
                    mvwprintw(win, 1, 24,"Enchantments");
                    wattroff(win, A_BOLD);
                    for(int i=1;i<=inventory.enchantment_num;i++){
                        mvwprintw(win, i+3, 3, "%d : %s",i,inventory.enchantments[i-1]);
                    }
                    mvwprintw(win, inventory.enchantment_num+7, 3, "u : Use");
                    mvwprintw(win, inventory.enchantment_num+8, 3, "d : Drop");
                    mvwprintw(win, inventory.enchantment_num+10, 17, "Press anything to exit.");
                    wrefresh(win);
                    cmd2=getchar();
                    switch (cmd2){
                        case 'u':
                            mvwprintw(win, inventory.enchantment_num+7, 13, "Select which one you want to use.");
                            mvwprintw(win, inventory.enchantment_num+7, 3, "          ");
                            mvwprintw(win, inventory.enchantment_num+8, 3, "           ");
                            wrefresh(win);
                            char cmdd=getchar();
                            int cmd3=cmdd - '0';
                            if(cmd3>=1 && cmd3<=inventory.enchantment_num){
                                werase(win);
                                wrefresh(win);
                                delwin(win);  
                                touchwin(stdscr);     
                                refresh(); 
                                if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Speed")==0){
                                    inventory.speed=2;
                                    inventory.speed_timer=10;
                                    mvprintw(0,0,"                                                               ");
                                    mvprintw(0,0,"You have extra speed for 10 moves!");
                                    refresh();
                                }
                                else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Damage")==0){
                                    inventory.strength=2;
                                    inventory.strength_timer=10;
                                    mvprintw(0,0,"                                                               ");
                                    mvprintw(0,0,"You have extra strength for 10 moves!");
                                    refresh();
                                }
                                else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Health")==0){
                                    inventory.health_timer=10;
                                    inventory.health_timer2=10;
                                    mvprintw(0,0,"                                                               ");
                                    mvprintw(0,0,"You will heal twice as fast for 10 moves.");
                                    refresh();
                                }
                                inventory.enchantment_num--;
                                for(int i=cmd3-1;i<inventory.enchantment_num;i++){
                                    strcpy(inventory.enchantments[i],inventory.enchantments[i+1]);
                                }
                                break;   
                            } 
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;
                        case 'd':
                            mvwprintw(win, inventory.enchantment_num+7, 13, "Select which one you want to drop.");
                            mvwprintw(win, inventory.enchantment_num+7, 3, "          ");
                            mvwprintw(win, inventory.enchantment_num+8, 3, "           ");
                            wrefresh(win);
                            cmdd=getchar();
                            cmd3=cmdd - '0';
                            if(cmd3>=1 && cmd3<=inventory.enchantment_num){
                                if(mvinch(current_y,current_x-1)=='.' && mvinch(current_y,current_x-2)=='.' && mvinch(current_y-1,current_x-1)=='.' && mvinch(current_y+1,current_x-1)=='.' && mvinch(current_y,current_x-3)=='.'){
                                    werase(win);
                                    wrefresh(win);
                                    delwin(win);  
                                    touchwin(stdscr);     
                                    refresh();  
                                    dropper(current_x-2,current_y,inventory.enchantments[cmd3-1]);
                                    if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Health")==0){
                                        attron(COLOR_PAIR(3));}
                                    else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Speed")==0){
                                        attron(COLOR_PAIR(4));}
                                    else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Damage")==0){
                                        attron(COLOR_PAIR(5));}
                                    mvprintw(current_y,current_x-2,"%lc",L'🏺');
                                    if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Health")==0){
                                        strcpy(items[current_x-2][current_y],"Enchantment of Health");
                                        attroff(COLOR_PAIR(3));}
                                    else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Speed")==0){
                                        strcpy(items[current_x-2][current_y],"Enchantment of Speed");
                                        attroff(COLOR_PAIR(4));}
                                    else if(strcmp(inventory.enchantments[cmd3-1],"Enchantment of Damage")==0){
                                        strcpy(items[current_x-2][current_y],"Enchantment of Damage");
                                        attroff(COLOR_PAIR(5));}
                                    refresh(); 
                                    inventory.enchantment_num--;
                                    for(int i=cmd3-1;i<inventory.enchantment_num;i++){
                                        strcpy(inventory.enchantments[i],inventory.enchantments[i+1]);
                                    }  
                                }
                                else{
                                    werase(win);
                                    wrefresh(win);
                                    delwin(win);  
                                    touchwin(stdscr);     
                                    refresh(); 
                                    mvprintw(0,0,"                                                                    ");
                                    mvprintw(0,0,"You can't drop anything here.");
                                    refresh(); 
                                }
                                break;
                            }
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;

                        default:
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);     
                            refresh();  
                            break;
                    }
                    break;
                case '3':
                    wclear(win);
                    box(win, ACS_PLUS, ACS_DIAMOND);
                    wattron(win, A_BOLD);
                    mvwprintw(win, 1, 27,"Weapons");
                    wattroff(win, A_BOLD);
                    mvwprintw(win, 3, 3, "Press to see details:");
                    for(int i=1;i<=inventory.weapon_num;i++){
                        if(i-1==inventory.current_weapon){wattron(win,A_BOLD | A_BLINK);}
                        mvwprintw(win, i+5, 3, "%d : %s",i,inventory.weapons[i-1]);
                        if(i-1==inventory.current_weapon){wattroff(win,A_BOLD | A_BLINK);}
                    }
                    mvwprintw(win, inventory.weapon_num+8, 3, "Press anything to exit.");
                    wrefresh(win);
                    char wichh=getch();
                    int wich=wichh-'0';
                    if(wich>=1 && wich<=inventory.weapon_num){
                        if(strcmp(inventory.weapons[wich-1],"Magic wand")==0)
                        {if(wand_details(inventory.wand_num)==1){
                            inventory.current_weapon=wich-1;
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            mvprintw(0,0,"                                                                    ");
                            mvprintw(0,0,"Equipped Magic Wand.");     
                            refresh();  }
                            else{
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            refresh(); }}
                        else if(strcmp(inventory.weapons[wich-1],"Mace")==0)
                        {if(mace_details()==1){
                            inventory.current_weapon=wich-1;
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            mvprintw(0,0,"                                                                    ");
                            mvprintw(0,0,"Equipped Mace.");     
                            refresh();  }
                            else{
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            refresh(); }}
                        else if(strcmp(inventory.weapons[wich-1],"Dagger")==0)
                        {if(dagger_details(inventory.dagger_num)==1){
                            inventory.current_weapon=wich-1;
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            mvprintw(0,0,"                                                                    ");
                            mvprintw(0,0,"Equipped Dagger.");     
                            refresh();  }
                            else{
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            refresh(); }}
                        else if(strcmp(inventory.weapons[wich-1],"Sword")==0)
                        {if(sword_details()==1){
                            inventory.current_weapon=wich-1;
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            mvprintw(0,0,"                                                                    ");
                            mvprintw(0,0,"Equipped Sword.");     
                            refresh();  }
                            else{
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            refresh(); }}
                        else if(strcmp(inventory.weapons[wich-1],"Arrows")==0)
                        {if(arrow_details(inventory.arrow_num)==1){
                            inventory.current_weapon=wich-1;
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            mvprintw(0,0,"                                                                    ");
                            mvprintw(0,0,"Equipped Arrows.");     
                            refresh();  }
                            else{
                            werase(win);
                            wrefresh(win);
                            delwin(win);  
                            touchwin(stdscr);
                            refresh(); }}
                    }
                    else{
                        werase(win);
                        wrefresh(win);
                        delwin(win);  
                        touchwin(stdscr);
                        refresh();}
                    
                    break;
                case 'e':
                    werase(win);
                    wrefresh(win);
                    delwin(win);  
                    touchwin(stdscr);     
                    refresh();  
                    break;
            default:
                werase(win);
                wrefresh(win);
                delwin(win);  
                touchwin(stdscr);     
                refresh(); 
                break;
            }  
            break;
        case ' ':
        if(monster_mode!='0'){
            if(whos_turn_is_it==1){
                if(strcmp(inventory.weapons[inventory.current_weapon],"Mace")==0 || strcmp(inventory.weapons[inventory.current_weapon],"Sword")==0){
                    weapon_user(inventory.weapons[inventory.current_weapon],0,current_x,current_y,inventory.strength);
                    monster_health_printer();
                    monster_health_checker();
                    if(abs(monster_x-current_x)<=1 && abs(monster_y-current_y)<=1) whos_turn_is_it=0;
                }
                else{
                    char dirr=getch();
                    int dir=dirr - '0';
                    if(dir>=1 && dir<=9 && dir!=5){
                        last_dir=dir;
                        if(strcmp(inventory.weapons[inventory.current_weapon],"Arrows")==0 && inventory.arrow_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Out of Arrows!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Dagger")==0 && inventory.dagger_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Your dagger has rusted!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Magic wand")==0 && inventory.wand_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Your wand is worn out!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        weapon_user(inventory.weapons[inventory.current_weapon],dir,current_x,current_y,inventory.strength);
                        if(abs(monster_x-current_x)<=1 && abs(monster_y-current_y)<=1) whos_turn_is_it=0;
                        monster_health_printer();
                        monster_health_checker();
                        if(strcmp(inventory.weapons[inventory.current_weapon],"Arrows")==0){inventory.arrow_num-=1;}
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Dagger")==0){inventory.dagger_num-=1;}
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Magic wand")==0){inventory.wand_num-=1;}
                    }
                }
            }
            else if(whos_turn_is_it==0){
                inventory.health-=monster_damage(monster_mode, difficulty);
                whos_turn_is_it=1;
                health_printer();
                if(health_checker()) return 0;
            }
        }
            break;
        case 'a':
        if(monster_mode!='0'){
            if(whos_turn_is_it==1){
                if(strcmp(inventory.weapons[inventory.current_weapon],"Mace")==0 || strcmp(inventory.weapons[inventory.current_weapon],"Sword")==0){
                    weapon_user(inventory.weapons[inventory.current_weapon],0,current_x,current_y,inventory.strength);
                    monster_health_printer();
                    monster_health_checker();
                    if(abs(monster_x-current_x)<=1 && abs(monster_y-current_y)<=1) whos_turn_is_it=0;
                }
                else{
                    int dir=last_dir;
                    if(dir>=1 && dir<=9 && dir!=5){
                        last_dir=dir;
                        if(strcmp(inventory.weapons[inventory.current_weapon],"Arrows")==0 && inventory.arrow_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Out of Arrows!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Dagger")==0 && inventory.dagger_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Your dagger has rusted!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Magic wand")==0 && inventory.wand_num==0){
                            mvprintw(0,0,"                                                                    ");
                            attron(A_ITALIC);
                            mvprintw(0,0,"Your wand is worn out!");
                            attroff(A_ITALIC);
                            refresh();
                            break;
                        }
                        weapon_user(inventory.weapons[inventory.current_weapon],dir,current_x,current_y,inventory.strength);
                        if(abs(monster_x-current_x)<=1 && abs(monster_y-current_y)<=1) whos_turn_is_it=0;
                        monster_health_printer();
                        monster_health_checker();
                        if(strcmp(inventory.weapons[inventory.current_weapon],"Arrows")==0){inventory.arrow_num-=1;}
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Dagger")==0){inventory.dagger_num-=1;}
                        else if(strcmp(inventory.weapons[inventory.current_weapon],"Magic wand")==0){inventory.wand_num-=1;}
                    }
                }
            }
            else if(whos_turn_is_it==0){
                inventory.health-=monster_damage(monster_mode, difficulty);
                whos_turn_is_it=1;
                health_printer();
                if(health_checker()) return 0;
            }
        }
            break;
        default:
            break;
    }
    init_color(COLOR_RED,1000,0,0);
    if(monster_mode!='0' && monster_moves_num>0 && (abs(monster_x-current_x)>1 || abs(monster_y-current_y)>1)){
        monster_moves_num--;
        if(monster_mover(monster_mode,monster_x,monster_y,current_x,current_y)==1){
            inventory.health-=monster_damage(monster_mode,difficulty);
        }
    }
    if((abs(monster_x-current_x)>12 || abs(monster_y-current_y)>9) && (monster_x!=0 || monster_y!=0)){
        monster_mode='0';
        monster_moves_num=0;
        first_time_we_see_it=0;
        monster_x=0; monster_y=0;
        whos_turn_is_it=1;
        mvprintw(43,1,"                                  ");
        refresh();
    }
    chtype ch = mvinch(next_y,next_x);
    char stairs = ch & A_CHARTEXT;
    if(stairs=='<'){
        mvprintw(0,0,"                                                                                ");
        mvprintw(0,0,"Press < to advance to the next floor!");
        char v=getch();
        if(v=='<'){
            return 1;
        }
    }
    if(stairs=='>'){
        mvprintw(0,0,"                                                                                ");
        mvprintw(0,0,"Press > to advance to go back to the previous floor!");
        char v=getch();
        if(v=='>'){
            mvprintw(0,0,"                      ");
            refresh();
            return -1;
        }
    }
    if(next_y>=0 && strcmp(items[next_x][next_y],"winner")==0){
        mvprintw(0,0,"                                                                                ");
        mvprintw(0,0,"Press w to win!");
        char v=getch();
        if(v=='w'){
            return 2;
        }
    }
    if(next_y>=0 && strcmp(items[next_x][next_y],"Trap")==0){
        mvprintw(0,0,"                                                                                ");
        mvprintw(0,0,"You stepped on trap! 30 damage taken!");
        grabber(next_x,next_y);
        char trapp[100]="Found trap";
        dropper(next_x,next_y,trapp);
        strcpy(items[next_x][next_y],"Found trap");
        inventory.health-=30;
        mvprintw(next_y,next_x,"&");
        health_printer();
        if(health_checker()) return 0;
        refresh();
    }
    if(next_y>=0 && strcmp(items[next_x][next_y],"Found trap")==0){
        mvprintw(0,0,"                                                                                ");
        mvprintw(0,0,"You stepped on trap! 50 damage taken!");
        inventory.health-=50;
        health_printer();
        if(health_checker()) return 0;
        refresh();
    }
    if(next_y>=0 && (mvinch(next_y,next_x)=='.' || mvinch(next_y,next_x)=='+' || mvinch(next_y,next_x)=='#')){
        mvprintw(current_y,current_x,"%c",prev);
        prev=mvinch(next_y,next_x);
        mvprintw(next_y,next_x,"%lc",characterlist[current_character]);
        current_x=next_x;
        current_y=next_y;
        if(current_x==enchantmentroom.x_door && current_y==enchantmentroom.y_door){
            mvprintw(0,0,"                                                                                ");
            mvprintw(0,0,"You have entered the Enchantment room! Each move you make will cause you damage!");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                                  ");
            refresh();
            if(strcmp(song,"enchantmentroom.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"enchantmentroom.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
        }
        if(current_x==goldroom.x_door && current_y==goldroom.y_door){
            mvprintw(0,0,"                                                                                ");
            mvprintw(0,0,"You have entered the Gold room! Victory is close!");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                                  ");
            refresh();
            if(strcmp(song,"Eye of the Tiger.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"Eye of the Tiger.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
        }
        if(current_x==nightmareroom.x_door && current_y==nightmareroom.y_door){
            mvprintw(0,0,"                                                                        ");
            mvprintw(0,0,"You have entered the Nightmare room.. Do not trust what you see..");
            refresh();
            napms(2000);
            mvprintw(0,0,"                                                                   ");
            refresh();
            if(strcmp(song,"nightmareroom.mp3")==0){
                strcpy(song,musiclist[current_music]);
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            else{
                strcpy(song,"nightmareroom.mp3");
                pthread_t music_thread;
                pthread_create(&music_thread, NULL, play_music, NULL);
            }
            nightmare*=(-1);
        }
        if(current_x>enchantmentroom.x1 && current_x<enchantmentroom.x2 
         && current_y>enchantmentroom.y1 && current_y<enchantmentroom.y2){
            inventory.health-=10;
            health_printer();
            if(health_checker()) return 0;
         }
        if(inventory.hunger>0){inventory.hunger-=1;}
        if(inventory.hunger%50==0){
            if(inventory.hunger>=0){
            hunger_printer();}
        }
        if(inventory.hunger<=0){
            inventory.health-=2;
            health_printer();
            if(health_checker()) return 0;
        }
        for(int i=0;i<inventory.food_num;i++){
            inventory.food_quality[i]-=1;
            if(strcmp(inventory.foods[i],"Exquisite Food")==0 && inventory.food_quality[i]<0){
                strcpy(inventory.foods[i],"Food");
                inventory.food_quality[i]=500;
            }
            else if(strcmp(inventory.foods[i],"Magical Food")==0 && inventory.food_quality[i]<0){
                strcpy(inventory.foods[i],"Food");
                inventory.food_quality[i]=500;
            }
        }
        if(inventory.speed_timer>0){
            inventory.speed_timer--;
            if(inventory.speed_timer==0){inventory.speed=1;}
        }
        if(inventory.strength_timer>0){
            inventory.strength_timer--;
            if(inventory.strength_timer==0){inventory.strength=1;}
        }
        if(inventory.hunger>=450){
            if(inventory.health_timer2>0){
                inventory.health_timer2-=1;
                if(inventory.health_timer2==0){inventory.health_timer=5;}}
            inventory.health+=inventory.health_timer;
            health_printer();
            refresh();
        }
        FILE *fptr=fopen(mapfile,"r");
        if(nightmare==1)map_explorer(fptr,current_x,current_y,rooms);
        for(int i=nightmareroom.x1; i<=nightmareroom.x2; i++){
            for(int j=nightmareroom.y1; j<=nightmareroom.y2; j++){
                if(i==nightmareroom.x_door && j==nightmareroom.y_door) continue;
                mvprintw(j,i," ");
            }
        }
        refresh();
        if(nightmare==-1){
            nightmare_map_explorer(fptr,current_x,current_y);
            mvprintw(current_y,current_x,"%lc",characterlist[current_character]);}
        fclose(fptr);
        refresh();
    }
    else if(next_y>=0){
    if(strcmp(items[next_x][next_y],"Enchantment of Health")==0 || strcmp(items[next_x][next_y],"Enchantment of Speed")==0 || strcmp(items[next_x][next_y],"Enchantment of Damage")==0){
        mvprintw(0,0,"                                                                    ");
        mvprintw(0,0,"Press G to grab %s.",items[next_x][next_y]);
        refresh();
        int g=getchar();
        if(g=='g'){
            if(inventory.enchantment_num>=5){
                mvprintw(0,0,"                                                             ");
                mvprintw(0,0,"Inventory full!");
                refresh();
            }
            else{
                strcpy(inventory.enchantments[inventory.enchantment_num],items[next_x][next_y]);
                inventory.enchantment_num+=1;
                mvprintw(next_y,next_x,".");
                mvprintw(next_y,next_x+1,".");
                mvprintw(0,0,"                                                                    ");
                mvprintw(0,0,"Grabbed %s.",items[next_x][next_y]);
                grabber(next_x,next_y);
                refresh();
                memset(items[next_x][next_y], 0, 40);
            }
        }
        refresh(); 
    }
    else if(strcmp(items[next_x][next_y],"Sword")==0 || strcmp(items[next_x][next_y],"Dagger")==0 || strcmp(items[next_x][next_y],"Magic wand")==0 || strcmp(items[next_x][next_y],"Arrows")==0 || strcmp(items[next_x][next_y],"Used arrow")==0){
        mvprintw(0,0,"                                                                    ");
        mvprintw(0,0,"Press G to grab %s.",items[next_x][next_y]);
        refresh();
        int g=getchar();
        if(g=='g'){
            int s=1;
            for(int i=0;i<inventory.weapon_num;i++){
                if(strcmp(items[next_x][next_y],inventory.weapons[i])==0 && strcmp(items[next_x][next_y],"Magic wand")==0){
                    mvprintw(0,0,"                                                    ");
                    mvprintw(0,0,"Your wand has been repaired.");
                    s=0;
                }
                else if(strcmp(items[next_x][next_y],inventory.weapons[i])==0 && strcmp(items[next_x][next_y],"Dagger")==0){
                    mvprintw(0,0,"                                                    ");
                    mvprintw(0,0,"Your dagger has been polished.");
                    s=0;
                }
                else if(strcmp(items[next_x][next_y],inventory.weapons[i])==0 && strcmp(items[next_x][next_y],"Arrows")==0){
                    mvprintw(0,0,"                                                    ");
                    mvprintw(0,0,"You picked up some arrows.");
                    s=0;
                }
            }
            if(s){
                strcpy(inventory.weapons[inventory.weapon_num],items[next_x][next_y]);
                inventory.weapon_num+=1;
                mvprintw(0,0,"                                                                   ");
                mvprintw(0,0,"Grabbed %s.",items[next_x][next_y]);
            }
            mvprintw(next_y,next_x,".");
            mvprintw(next_y,next_x+1,".");
            grabber(next_x,next_y);
            refresh();
            if(strcmp(items[next_x][next_y],"Arrows")==0){inventory.arrow_num+=20;}
            else if(strcmp(items[next_x][next_y],"Used arrow")==0){inventory.arrow_num+=1;}
            else if(strcmp(items[next_x][next_y],"Dagger")==0){inventory.dagger_num+=10;}
            else if(strcmp(items[next_x][next_y],"Magic wand")==0){inventory.wand_num+=8;}
            memset(items[next_x][next_y], 0, 40);
        }
        refresh(); 
    }
    else if(strcmp(items[next_x][next_y],"Food")==0 || strcmp(items[next_x][next_y],"Exquisite Food")==0 || strcmp(items[next_x][next_y],"Magical Food")==0){
        mvprintw(0,0,"                                                                    ");
        mvprintw(0,0,"Press G to grab %s.",items[next_x][next_y]);
        refresh();
        int g=getchar();
        if(g=='g'){
            if(inventory.food_num>=5){
                mvprintw(0,0,"                                                    ");
                mvprintw(0,0,"Inventory full!");
                refresh();
            }
            else{
                strcpy(inventory.foods[inventory.food_num],items[next_x][next_y]);
                inventory.food_quality[inventory.food_num]=500;
                inventory.food_num+=1;
                mvprintw(next_y,next_x,".");
                mvprintw(next_y,next_x+1,".");
                mvprintw(0,0,"                                                    ");
                mvprintw(0,0,"Grabbed %s.",items[next_x][next_y]);
                grabber(next_x,next_y);
                refresh();
                memset(items[next_x][next_y], 0, 40);
            }
        }
        refresh(); 
    }
    else if(strcmp(items[next_x][next_y],"Gold")==0){
        mvprintw(next_y,next_x,".");
        mvprintw(next_y,next_x+1,".");
        memset(items[next_x][next_y], 0, 40);
        int random=rand()%10 +5; //each gold is 5 to 15 gold
        inventory.gold+=random;
        inventory.score+=random;
        score_printer();
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"Grabbed %d pieces of Gold.", random);
        grabber(next_x,next_y);
        attron(COLOR_PAIR(10));
        mvprintw(0,172,"Gold : %d",inventory.gold);
        attroff(COLOR_PAIR(10));
        refresh(); 
    }
    else if(strcmp(items[next_x][next_y],"Obsidian")==0){
        mvprintw(next_y,next_x,".");
        mvprintw(next_y,next_x+1,".");
        memset(items[next_x][next_y], 0, 40);
        int random=rand()%10 +30; //each obsidian is 30 to 40 gold
        inventory.gold+=random;
        inventory.score+=random;
        score_printer();
        mvprintw(0,0,"                                                                   ");
        mvprintw(0,0,"You found Obsidian! Grabbed %d pieces of Gold.", random);
        grabber(next_x,next_y);
        attron(COLOR_PAIR(10));
        mvprintw(0,172,"Gold : %d",inventory.gold);
        attroff(COLOR_PAIR(10));
        refresh(); 
    }
    }
    }
}

void make_character_at_beggining(){
    inventory.food_num=0;
    inventory.enchantment_num=0;
    inventory.weapon_num=1;
    strcpy(inventory.weapons[0],"Mace");
    inventory.current_weapon=0;
    inventory.dagger_num=0;
    inventory.wand_num=0;
    inventory.arrow_num=0;
    inventory.gold=0;
    inventory.score=0;
    inventory.health=500;
    inventory.health_timer=5;
    inventory.health_timer2=0;
    inventory.hunger=500;
    inventory.speed=1;
    inventory.speed_timer=0;
    inventory.strength=1;
    inventory.strength_timer=0;
}

int character_generator(room **rooms, int total_room, int floor){
    inventory.current_floor=floor;
    inventory.visited_floors[floor]=1;
    int w=rooms[0]->width;
    int h=rooms[0]->height;
    int x,y;
    do{
    x=rand()%(w-2) +2 + rooms[0]->x; 
    y=rand()%(h-2) +2 + rooms[0]->y;}
    while(mvinch(y,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
    current_x=x; current_y=y;
    prev='.';
    FILE* fptr=fopen(mapfile,"w");
    map_saver(fptr);
    fclose(fptr);
    refresh();
    clear();
    mvprintw(y,x,"%lc",characterlist[current_character]);
    attron(COLOR_PAIR(10));
    mvprintw(0,172,"Gold : %d",inventory.gold);
    attroff(COLOR_PAIR(10));
    score_printer();
    health_printer();
    hunger_printer();
    refresh();
    FILE* fptr2=fopen(mapfile,"r");
    map_explorer(fptr2,x,y,rooms);
    mvprintw(0,0,"You have entered floor %d!",floor);
    refresh();
    int a=character_mover(rooms,total_room);
    fclose(fptr2);
    return a;
}    

int character_loader(room **rooms, int total_room, int floor){
    attron(COLOR_PAIR(10));
    mvprintw(0,172,"Gold : %d",inventory.gold);
    attroff(COLOR_PAIR(10));
    score_printer();
    health_printer();
    hunger_printer();
    refresh();
    FILE* fptr2=fopen(mapfile,"r");
    map_explorer(fptr2,current_x,current_y,rooms);
    mvprintw(0,0,"You have entered floor %d!",floor);
    refresh();
    int a=character_mover(rooms,total_room);
    fclose(fptr2);
    return a;
}    

///////////////////////////////////////////////////////////////////////////

int new_game(char *user){
    curs_set(0);
    sprintf(infofile, "%s.info.txt", user);
    int floor=1;
    int total_floor=rand()%2 +4; //4 or 5
    //int total_floor=1;
    inventory.floor_num=total_floor;
    memset(inventory.visited_floors,0,6*sizeof(int));
    make_character_at_beggining();
    while(1){
    clear();
    refresh();
    sprintf(mapfile, "%s.floor%d.txt", user, floor);
    sprintf(mapdetailfile, "%s.floor%ddetail.txt", user, floor);
    int z;
    room **rooms;
    if(inventory.visited_floors[floor]==0){
        enchantmentroom.exists=0;
        nightmareroom.exists=0;
        goldroom.exists=0;
        rooms=map_generator(floor,total_floor);
        z=character_generator(rooms,total_room,floor);
        mapdetail_saver();
        inventory.visited_floors[floor]=1;}
    else{
        memset(items,'\0',sizeof(items));
        print_current_state();
        refresh();
        mapdetail_loader();
        inventory.visited_floors[floor]=1;
        z=character_loader(rooms,total_room,floor);
        mapdetail_saver();
    }
    if(z==1){
        floor+=1;
        if(inventory.visited_floors[floor]==0){
            inventory.score+=40;
        }
    }
    else if(z==-1){
        floor-=1;
    }
    else if(z==2){
        gold_to_be_added=inventory.gold;
        score_to_be_added=inventory.score;
        return 1; //WIN
    }
    else if(z==0){
        gold_to_be_added=inventory.gold;
        score_to_be_added=inventory.score;
        return -1; //LOSE
    }
    else if(z==-2){
        info_saver();
        mapdetail_saver();
        return 0; //pause
    }
    }   
}

int resume_game(char *user){
    curs_set(0);
    sprintf(infofile, "%s.info.txt", user);
    info_loader();
    int floor=inventory.current_floor;
    int total_floor=inventory.floor_num;
    while(1){
    sprintf(mapfile, "%s.floor%d.txt", user, floor);
    sprintf(mapdetailfile, "%s.floor%ddetail.txt", user, floor);
    clear();
    refresh();
    int z;
    room **rooms;
    if(inventory.visited_floors[floor]==0){
        enchantmentroom.exists=0;
        nightmareroom.exists=0;
        goldroom.exists=0;
        rooms=map_generator(floor,total_floor);
        z=character_generator(rooms,total_room,floor);
        mapdetail_saver();
        inventory.visited_floors[floor]=1;}
    else{
        memset(items,'\0',sizeof(items));
        mapdetail_loader();
        print_current_state();
        refresh();
        inventory.visited_floors[floor]=1;
        z=character_loader(rooms,total_room,floor);
        mapdetail_saver();
    }
    if(z==1){
        floor+=1;
        if(inventory.visited_floors[floor]==0){
            inventory.score+=40;
        }
    }
    else if(z==-1){
        floor-=1;
    }
    else if(z==2){
        gold_to_be_added=inventory.gold;
        score_to_be_added=inventory.score;
        return 1; //WIN
    }
    else if(z==0){
        gold_to_be_added=inventory.gold;
        score_to_be_added=inventory.score;
        return -1; //LOSE
    }
    else if(z==-2){
        info_saver();
        mapdetail_saver();
        return 0; //pause
    }
    }   
}