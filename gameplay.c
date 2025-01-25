#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h> 
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#include "map.c"

struct inv{
    char enchantments[5][40];
    int enchantment_num;
    char weapons[4][40];
    int weapon_num;
    char foods[5][40];
    int food_num;

};

struct inv inventory;

////////////////////////////////////////////////////////

int current_x,current_y;
char prev;

void character_mover(room **rooms, int total_room){
    while(1){
    int c=getch();
    int next_x,next_y=-1;
    switch (c){
        case KEY_UP:
            next_x=current_x; next_y=current_y-1;
            break;
        case KEY_DOWN:
            next_x=current_x; next_y=current_y+1;
            break;
        case KEY_LEFT:
            next_x=current_x-1; next_y=current_y;
            break;
        case KEY_RIGHT:
            next_x=current_x+1; next_y=current_y;
            break;
        default:
            break;
    }
    if(next_y>=0 && (mvinch(next_y,next_x)=='.' || mvinch(next_y,next_x)=='+' || mvinch(next_y,next_x)=='#')){
        mvprintw(current_y,current_x,"%c",prev);
        prev=mvinch(next_y,next_x);
        mvprintw(next_y,next_x,"%c",'@');
        current_x=next_x;
        current_y=next_y;
        refresh();
    }
    else if(mvinch(next_y,next_x)==L'🏺'){
        mvprintw(0,0,"Press G to grab Enchantment.");
        int g=getchar();
        if(g=='g'){
            if(inventory.enchantment_num>=7){
                mvprintw("Inventory full!");
            }
            else{
                
                inventory.items[inventory.item_num]=""
                inventory.item_num+=1;

            }
        }


        
    }
    

    }
}

void character_generator(room **rooms, int total_room){
    inventory.food_num=0;
    inventory.enchantment_num=0;
    inventory.weapon_num=1;
    inventory.weapons[0]="Mace";
    int w=rooms[0]->width;
    int h=rooms[0]->height;
    int x,y;
    do{
    x=rand()%(w-2) +2 + rooms[0]->x; 
    y=rand()%(h-2) +2 + rooms[0]->y;}
    while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.' || mvinch(y,x+2)!='.' || mvinch(y,x-2)!='.');
    mvprintw(y,x,"@");
    refresh();
    current_x=x; current_y=y;
    prev='.';
    character_mover(rooms, total_room);
}

int main(){
    room **rooms=map_generator();
    character_generator(rooms,total_room);
}