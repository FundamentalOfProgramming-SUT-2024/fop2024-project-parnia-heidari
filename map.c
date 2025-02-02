#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h> 
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <pthread.h>

int max_y, max_x;

int difficulty=1;

//45,185

//we are gonna have 7 to 10 rooms.
//each room is gonna be 10*4 minimum and 14*8 maximum.(not counting the walls)

typedef struct{
    int x; //coordinates of the top left corner
    int y;
    int width;
    int height;
    int doors;
    int door_coordinates[3][3]; //each room can have max 3 doors, max 1 on each side, and the side on which the door is, its x, and its y is gonna be stored here.
    int neighbours[10];
    int neighbour_num;
    int staircase[3]; //the first index shows if it has a staircase or not, the next 2 are its coordinates.
    int pillar_num; //max 2
    int gold_num; //max 1
    int enchant_num; //max 1
    int food_num; //max 1
    int weapon_num; //max 1
    int monster_num; //max 1
}room;

//the sides of the room are named like this: Top:1, Right:2, Bottom:3, Left:4

char items[200][60][40]; //we will store each item on the floor here.

int nightmare_room=-1;
int enchantment_room=-1;
int gold_room=-1;

int check_room_collision(int y, int x, int h, int w){ //doesnt allow any rooms within a 10 block radius of an existing room.
    for (int i=x-10; i <= x+w+11 ; i++) {
        for (int j=y-10; j <= y+h+11; j++) {
            if (mvinch(j, i) == '|' || mvinch(j, i) == '_' || mvinch(j, i) == '.' || mvinch(j, i) == L'—') {
                return 1;
            }
        }
    }
    return 0;}

//////////////////////////////////////////////////////////////////////////////////////////

void room_generator(room *current){
    int x,y,w,h;
    do{
        current->x=rand()%(max_x-30) +10;
        current->y=rand()%(max_y-15) +2;
        current->width=rand()%4 +10;
        current->height=rand()%4 +4;
        current->doors=0;
        current->door_coordinates[0][0]=0;
        current->door_coordinates[1][0]=0;//we will need this part for generating the doors
        current->door_coordinates[2][0]=0;
        current->neighbour_num=0;
        current->staircase[0]=0;
        current->pillar_num=0;
        current->gold_num=0;
        current->enchant_num=0;
        current->food_num=0;
        current->weapon_num=0;
        current->monster_num=0;
        x=current->x; w=current->width;
        y=current->y; h=current->height;
    }while(check_room_collision(y,x,h,w));

    for(int i=x; i<=x+w+1; i++){
        mvprintw(y,i,"_");
        mvprintw(y+h+1, i,"%lc",L'—');
        strcpy(items[i][y+h+1],"lower wall");
    }
    refresh();
    for(int i=y+1; i<=y+h; i++){
        mvprintw(i,x,"|");
        mvprintw(i,x+w+1,"|");
    }
    refresh();
    for(int i=x+1; i<=x+w; i++){
        for(int j=y+1; j<=y+h; j++){
            mvprintw(j,i,".");
        }
    }
    refresh();
}

///////////////////////////////////////////////////////////////////////////////////////

int door_checker(room **rooms,int n){//checks if all the rooms have at least one door
    for(int i=0;i<n;i++){
        if((*(*(rooms+i))).doors==0){
            return 0;
        }
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////

int side_picker(int* n1, int* n2, room* room1, room* room2){ //the dfs function for making corridors takes too long for some possibilities, this function is gonna make sure they dont happen.
    int x1=room1->x, x2=room2->x, y1=room1->y, y2=room2->y;
    int w1=room1->width; int w2=room2->width;  int h1=room1->height; int h2=room2->height;
    if(x1+w1+1<x2 && room1->door_coordinates[0][0]!=4 && room1->door_coordinates[1][0]!=4 && room1->door_coordinates[2][0]!=4
     && room2->door_coordinates[0][0]!=2 && room2->door_coordinates[1][0]!=2 && room2->door_coordinates[2][0]!=2)
     {*n1=4;  *n2=2;  return 1;}
    else if(y1-h1-1>y2+3 && room1->door_coordinates[0][0]!=1 && room1->door_coordinates[1][0]!=1 && room1->door_coordinates[2][0]!=1
     && room2->door_coordinates[0][0]!=3 && room2->door_coordinates[1][0]!=3 && room2->door_coordinates[2][0]!=3)
     {*n1=1;  *n2=3;  return 1;}
    else if(x1>w2+1+x2 && room1->door_coordinates[0][0]!=2 && room1->door_coordinates[1][0]!=2 && room1->door_coordinates[2][0]!=2
     && room2->door_coordinates[0][0]!=4 && room2->door_coordinates[1][0]!=4 && room2->door_coordinates[2][0]!=4)
     {*n1=2;  *n2=4;  return 1;}
    else if(y1+3<y2-h2-1 && room1->door_coordinates[0][0]!=3 && room1->door_coordinates[1][0]!=3 && room1->door_coordinates[2][0]!=3
     && room2->door_coordinates[0][0]!=1 && room2->door_coordinates[1][0]!=1 && room2->door_coordinates[2][0]!=1)
     {*n1=3;  *n2=1;  return 1;}
       return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////

int door_creator(room *room1, room* room2){
    int n1;int n2;
    if(side_picker(&n1,&n2,room1,room2)==0){return 0;}

    if(n1==1){
        int x=rand()%(room1->width);
        mvprintw(room1->y, x+1+room1->x, "+");
        refresh();
        room1->door_coordinates[room1->doors][0]=n1;
        room1->door_coordinates[room1->doors][1]=x+1+room1->x;
        room1->door_coordinates[room1->doors][2]=room1->y;
        room1->doors+=1;
    }
    else if(n1==3){
        int x=rand()%(room1->width);
        mvprintw( room1->y + room1->height +1 , x+1+room1->x , "+");
        refresh();
        room1->door_coordinates[room1->doors][0]=n1;
        room1->door_coordinates[room1->doors][1]=x+1+room1->x;
        room1->door_coordinates[room1->doors][2]=room1->y + room1->height+1;
        room1->doors+=1;
    }
    else if(n1==2){
        int y=rand()%(room1->height);
        mvprintw(room1->y +y +1, room1->x, "+");
        refresh();
        room1->door_coordinates[room1->doors][0]=n1;
        room1->door_coordinates[room1->doors][1]=room1->x;
        room1->door_coordinates[room1->doors][2]=room1->y +y +1;
        room1->doors+=1;
    }
    else if(n1==4){
        int y=rand()%(room1->height);
        mvprintw(room1->y +y +1, room1->x + room1->width +1, "+");
        refresh();
        room1->door_coordinates[room1->doors][0]=n1;
        room1->door_coordinates[room1->doors][1]=room1->x + room1->width +1;
        room1->door_coordinates[room1->doors][2]=room1->y +y +1;
        room1->doors+=1;
    }


    if(n2==1){
        int x=rand()%(room2->width);
        mvprintw(room2->y, x+1+room2->x, "+");
        refresh();
        room2->door_coordinates[room2->doors][0]=n2;
        room2->door_coordinates[room2->doors][1]=x+1+room2->x;
        room2->door_coordinates[room2->doors][2]=room2->y;
        room2->doors+=1;
    }
    else if(n2==3){
        int x=rand()%(room2->width);
        mvprintw( room2->y + room2->height +1 , x+1+room2->x , "+");
        refresh();
        room2->door_coordinates[room2->doors][0]=n2;
        room2->door_coordinates[room2->doors][1]=x+1+room2->x;
        room2->door_coordinates[room2->doors][2]=room2->y + room2->height+1;
        room2->doors+=1;
    }
    else if(n2==2){
        int y=rand()%(room2->height);
        mvprintw(room2->y +y +1, room2->x, "+");
        refresh();
        room2->door_coordinates[room2->doors][0]=n2;
        room2->door_coordinates[room2->doors][1]=room2->x;
        room2->door_coordinates[room2->doors][2]=room2->y +y +1;
        room2->doors+=1;
    }
    else if(n2==4){
        int y=rand()%(room2->height);
        mvprintw(room2->y +y +1, room2->x + room2->width +1, "+");
        refresh();
        room2->door_coordinates[room2->doors][0]=n2;
        room2->door_coordinates[room2->doors][1]=room2->x + room2->width +1;
        room2->door_coordinates[room2->doors][2]=room2->y +y +1;
        room2->doors+=1;
    }
    return 1;
    
}

//////////////////////////////////////////////////////////////////////////////////

int  move_back=1;
int moves_num=0;//to stop getting stuck in a loop
int dfs_corridors(int first_x, int first_y, int final_x, int final_y, int prev_x, int prev_y){
    int current_x, current_y;
    getyx(stdscr, current_y,current_x);
    if(current_x==final_x && current_y==final_y){return 1;}
    
    int should_i_go_this_way[4]={1,1,1,1};
    char what_was_there;
    int deltax[4]={0};
    int deltay[4]={0};
    if(final_x>first_x){ deltax[1]=+1;  deltax[3]=-1; }
    else{ deltax[1]=-1;  deltax[3]=+1; }
    if(final_y>first_y){ deltay[0]=+1; deltay[2]=-1;}
    else{ deltay[0]=-1; deltay[2]=+1;}
    
     for(int i = 0; i < 4; i++) {
        int nx = current_x + deltax[i];
        int ny = current_y + deltay[i];
        if(nx >= 2 && nx < max_x-2) {
            if(moves_num>80){return 0;}
            if(nx >= 1 && ny < max_y-1) {
                if( (mvinch(ny,nx)!='.' && mvinch(ny,nx)!='|' && 
                    mvinch(ny,nx)!='_' && mvinch(ny,nx)!=L'—' &&
                    mvinch(ny+1,nx)!='.' && mvinch(ny+1,nx)!='|' && 
                    mvinch(ny+1,nx)!='_' && mvinch(ny+1,nx)!=L'—' &&
                    mvinch(ny-1,nx)!='.' && mvinch(ny-1,nx)!='|' && 
                    mvinch(ny-1,nx)!='_' && mvinch(ny-1,nx)!=L'—' &&
                    mvinch(ny,nx+1)!='.' && mvinch(ny,nx+1)!='|' && 
                    mvinch(ny,nx+1)!='_' && mvinch(ny,nx+1)!=L'—' &&
                    mvinch(ny,nx-1)!='.' && mvinch(ny,nx-1)!='|' && 
                    mvinch(ny,nx-1)!='_' && mvinch(ny,nx-1)!=L'—' &&
                    (nx!=prev_x || ny!=prev_y) &&
                    (abs(final_x-nx)<abs(final_x-current_x) || abs(final_y-ny)<abs(final_y-current_y) || move_back>=1)
                    && should_i_go_this_way[i]==1) || (nx==final_x && ny==final_y))  {
                        what_was_there=mvinch(ny,nx);
                        mvprintw(ny,nx,"#");
                        refresh();
                        move(ny,nx);
                        ++moves_num;
                        move_back=0;

                        if (dfs_corridors(first_x,first_y,final_x, final_y, current_x, current_y)) 
                            return 1;
                        else{
                            mvprintw(ny,nx,"%c",what_was_there);
                            refresh();
                            move(ny,nx);
                            move_back=1;
                            should_i_go_this_way[i]=0;
                            
                        }
                }
            }
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////

int are_neighbours(room ** rooms, int n1, int n2){
    for(int i=0; i<rooms[n1]->neighbour_num;i++){
        if(rooms[n1]->neighbours[i]==n2){
            return 1;
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////

int visited[11]={0};
void are_all_connected(room **rooms,int n,int room_num){
    visited[room_num]=1;
    for(int j=0;j<rooms[room_num]->neighbour_num;j++){
        if(visited[rooms[room_num]->neighbours[j]]==0)
            are_all_connected(rooms,n,rooms[room_num]->neighbours[j]);
    }
}

int connection_checker(room **rooms,int n){
    are_all_connected(rooms,n,0);
    for(int i=0;i<n;i++){
        if(visited[i]==0){return 0;}
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////

int corridor_creator(room ** rooms, int n){
    int loop_stopper=0; 
    while(door_checker(rooms,n)==0 || connection_checker(rooms,n)==0){
        //lets pick two rooms and connect them, also they shouldnt be further than half of the screen.
        int x1,x2,y1,y2,n1,n2;
        int loop_stopper2=0;
        int distance_y=max_y/3;  int distance_x=max_x/3;
        room *room1; room *room2;
        do{
            loop_stopper2=0;
        n2=rand()%n; n1=rand()%n;
        while(n2==n1 || rooms[n1]->doors==3 || rooms[n2]->doors==3 || are_neighbours(rooms,n1,n2)){
            if(rooms[n2]->doors!=0){
                n2=rand()%n;}  //this is for avoiding getting stuck in a random picking loop, if it finds a room with no doors, it keeps it.
            n1=rand()%n;
            loop_stopper2++;
            if(loop_stopper2>200){
                return 0;
            }
        }

        room1=rooms[n1];
        room2=rooms[n2];
        x1=room1->x; x2=room2->x;
        y1=room1->y; y2=room2->y;
        loop_stopper++;
        if(loop_stopper>100){
            distance_x+=2;
            distance_y+=1;
        }
        if(loop_stopper>200){return 0;}
        }while( abs(x2-x1)>distance_x || abs(y2-y1)>distance_y );

        if(door_creator(room1,room2)==0){continue;}
    
        x1=room1->door_coordinates[room1->doors -1][1];
        y1=room1->door_coordinates[room1->doors -1][2];
        x2=room2->door_coordinates[room2->doors -1][1];
        y2=room2->door_coordinates[room2->doors -1][2];

        move(y2,x2);
        if(dfs_corridors(x2,y2,x1,y1,x2,y2)==1) { 
            mvprintw(y2,x2,"+");
            mvprintw(y1,x1,"+");
            refresh();
            moves_num=0;
            room1->neighbours[room1->neighbour_num++]=n2;
            room2->neighbours[room2->neighbour_num++]=n1;
        }
        else{
            moves_num=0;

            int side1= room1->door_coordinates[room1->doors -1][0];
            int side2= room2->door_coordinates[room2->doors -1][0];

            if(side1==1){mvprintw(y1,x1,"_");}
            else if(side1==3){mvprintw(y1,x1,"%lc",L'—');}
            else if(side1==2 || side1==4){mvprintw(y1,x1,"|");}

            if(side2==1){mvprintw(y2,x2,"_");}
            else if(side2==3){mvprintw(y2,x2,"%lc",L'—');}
            else if(side2==2 || side2==4){mvprintw(y2,x2,"|");}
            refresh();

            room1->door_coordinates[--room1->doors][0]=0;
            room2->door_coordinates[--room2->doors][0]=0;
        }
        memset(visited, 0, sizeof(visited));
    }
    return 1;
}

///////////at this point the blueprint of the map is generated, now we will generate the items

void staircase_generator(room ** rooms, int n){
    int selected;
    do{
    selected=rand()%(n-1) +1; //cant be room 0
    }while(selected==enchantment_room || selected==gold_room || selected==nightmare_room);
    rooms[selected]->staircase[0]=1;
    int w=rooms[selected]->width;
    int h=rooms[selected]->height;
    int x=rand()%w +1 + rooms[selected]->x;
    int y=rand()%h +1 + rooms[selected]->y;
    init_color(COLOR_GREEN, 0, 1000, 0); 
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    mvprintw(y,x,"<");
    attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void pillar_generator(room ** rooms, int n, int p_num){
    for(int i=0;i<p_num;i++){
        int selected;
        int loop_stopper=0;
        do{
        selected=rand()%n;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(rooms[selected]->pillar_num==2);
        int w=rooms[selected]->width;
        int h=rooms[selected]->height;
        int x,y;
        loop_stopper=0;
        do{
        x=rand()%(w-2) +2 + rooms[selected]->x; //cant make the pillar block the door or the staircase
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
        attron(A_BOLD);
        mvprintw(y,x,"O");
        attroff(A_BOLD);
        refresh();
        strcpy(items[x][y],"pillar");
        rooms[selected]->pillar_num+=1;
    }
}

/////////////////////////////////////////////////////////////////////////////////////

void window_creator(room **rooms,int total_room, int n){
    for(int i=0;i<n;i++){
        int s=0;
        int num;
        int loop_stopper=0;
        while(s==0){
        do{
        num=rand()%total_room;
        loop_stopper++;
        if(loop_stopper>200 )return;}while(num==enchantment_room || num==gold_room || num==nightmare_room);
        int x=rand()%(rooms[num]->width);
        int y=rand()%(rooms[num]->height);
        int random=rand()%3;
        if(random==0 && mvinch(rooms[num]->y, x+1+rooms[num]->x)=='_'){
            mvprintw(rooms[num]->y, x+1+rooms[num]->x, "=");
            s=1;
        }
        else if(random==1 && mvinch(y+1+rooms[num]->y, rooms[num]->x)=='|'){
            mvprintw(y+1+rooms[num]->y, rooms[num]->x, "=");
            s=1;
        }
        else if(random==2 && mvinch(y+1+rooms[num]->y, 1+rooms[num]->width+rooms[num]->x)=='|'){
            mvprintw(y+1+rooms[num]->y, 1+rooms[num]->width+rooms[num]->x, "=");
            s=1;
        }
        }
    }
    refresh();
}

////////////////////////////////////////////////////////////////////////////////////

void enchantment_room_generator(room ** rooms, int n){
    init_color(COLOR_GREEN,300,700,0);
    init_pair(9,COLOR_GREEN, COLOR_BLACK);
    for(int i=1;i<n;i++){
        int loop_stopper=0;
        if(rooms[i]->doors==1 && i!=gold_room && i!=nightmare_room){
            int w=rooms[i]->width;
            int h=rooms[i]->height;
            int num=rand()%2 +3; //3 to 5 enchantments
            for(int j=0;j<num;j++){
            int x,y;
            do{
            x=rand()%(w-2) +2 + rooms[i]->x; 
            y=rand()%(h-2) +2 + rooms[i]->y;
            loop_stopper++;
            if(loop_stopper>200 )return;}
            while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
            int random=rand()%9; //theres a 1 in 3 chance for each enchantment
            if((random%3)==0){
                init_pair(3, COLOR_GREEN, COLOR_GREEN);
                attron(A_DIM | COLOR_PAIR(3));
                mvprintw(y,x,"%lc",L'🏺');
                attroff(A_DIM | COLOR_PAIR(3));
                strcpy(items[x][y],"Enchantment of Health");
            }
            else if((random%3)==1){
                init_pair(4, COLOR_CYAN, COLOR_CYAN);
                attron(COLOR_PAIR(4));
                mvprintw(y,x,"%lc",L'🏺');
                attroff(COLOR_PAIR(4));
                strcpy(items[x][y],"Enchantment of Speed");
                }
            else{
                init_color(COLOR_RED,800,0,0);
                init_pair(5, COLOR_RED, COLOR_RED);
                attron(COLOR_PAIR(5));
                mvprintw(y,x,"%lc",L'🏺');
                attroff(COLOR_PAIR(5));
                strcpy(items[x][y],"Enchantment of Damage");
                }
            }
            attron(COLOR_PAIR(9));
            for(int j=rooms[i]->x; j<=rooms[i]->x+w+1; j++){
                mvprintw(rooms[i]->y, j, "_");
                strcpy(items[j][rooms[i]->y],"upper wall green");
                mvprintw(rooms[i]->y + h+1, j, "%lc",L'—');
                strcpy(items[j][rooms[i]->y + h+1],"lower wall green");
            }
            for(int j=rooms[i]->y+1; j<=rooms[i]->y+h; j++){
                mvprintw(j, rooms[i]->x, "|");
                strcpy(items[rooms[i]->x][j],"side wall green");
                mvprintw(j, rooms[i]->x + w+1, "|");
                strcpy(items[rooms[i]->x + w+1][j],"side wall green");
            }
            attroff(COLOR_PAIR(9));
            mvprintw(rooms[i]->door_coordinates[0][2],rooms[i]->door_coordinates[0][1],"+");
            enchantment_room=i;
            break;
        }
    }
    refresh();
}

////////////////////////////////////////////////////////////////////////////////////

void nightmare_room_generator(room ** rooms, int n){
    init_pair(19,COLOR_MAGENTA, COLOR_BLACK);
    for(int i=1;i<n;i++){
        int loop_stopper=0;
        if(rooms[i]->doors==1 && i!=gold_room && i!=enchantment_room){
            int w=rooms[i]->width;
            int h=rooms[i]->height;
            int x,y;
            do{
            x=rand()%(w-2) +2 + rooms[i]->x; 
            y=rand()%(h-2) +2 + rooms[i]->y;
            loop_stopper++;
            if(loop_stopper>200 )return;}
            while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
            mvprintw(y,x,"%lc",L'🧈');
            strcpy(items[x][y],"Fake Gold");
            loop_stopper=0;
            do{
            x=rand()%(w-2) +2 + rooms[i]->x; 
            y=rand()%(h-2) +2 + rooms[i]->y;
            loop_stopper++;
            if(loop_stopper>200 )return;}
            while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
            mvprintw(y,x,"%lc",L'🏺');
            strcpy(items[x][y],"Fake Enchantment");

            attron(COLOR_PAIR(19));
            for(int j=rooms[i]->x; j<=rooms[i]->x+w+1; j++){
                mvprintw(rooms[i]->y, j, "_");
                strcpy(items[j][rooms[i]->y],"upper wall purple");
                mvprintw(rooms[i]->y + h+1, j, "%lc",L'—');
                strcpy(items[j][rooms[i]->y + h+1],"lower wall purple");
            }
            for(int j=rooms[i]->y+1; j<=rooms[i]->y+h; j++){
                mvprintw(j, rooms[i]->x, "|");
                strcpy(items[rooms[i]->x][j],"side wall purple");
                mvprintw(j, rooms[i]->x + w+1, "|");
                strcpy(items[rooms[i]->x + w+1][j],"side wall purple");
            }
            attroff(COLOR_PAIR(19));
            mvprintw(rooms[i]->door_coordinates[0][2],rooms[i]->door_coordinates[0][1],"+");
            refresh();
            nightmare_room=i;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////

void gold_generator(room ** rooms, int n, int g_num){ //every difficulty has 3 or 4 gold pieces on the map
    for(int i=0;i<g_num;i++){
        int selected;
        int loop_stopper=0;
        do{
        selected=rand()%n;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(rooms[selected]->gold_num>=1 || selected==enchantment_room || selected==gold_room || selected==nightmare_room);
        int w=rooms[selected]->width;
        int h=rooms[selected]->height;
        int x,y;
        loop_stopper=0;
        do{
        x=rand()%(w-2) +2 + rooms[selected]->x; 
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.' || mvinch(y,x+2)!='.' || mvinch(y,x-2)!='.');
        int random=rand()%10; //theres a 1 in 10 chance of getting black gold
        if(random==0){
            mvprintw(y,x,"%lc",L'🔮');
            strcpy(items[x][y],"Obsidian");

        }
        else{
            mvprintw(y,x,"%lc",L'🧈');
             strcpy(items[x][y],"Gold");

            }
        refresh();
        rooms[selected]->gold_num+=1;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////


void enchantment_generator(room ** rooms, int n, int e_num){ //difficulty 1: 0 or 1, difficulty 2: 0 to 2, difficulty 3: 0 to 3
    for(int i=0;i<e_num;i++){
        int selected;
        int loop_stopper=0;
        do{
        selected=rand()%n;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(rooms[selected]->enchant_num>=1 || selected==enchantment_room || selected==gold_room || selected==nightmare_room);
        int w=rooms[selected]->width;
        int h=rooms[selected]->height;
        int x,y;
        loop_stopper=0;
        do{
        x=rand()%(w-2) +2 + rooms[selected]->x; 
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.');
        int random=rand()%9; //theres a 1 in 3 chance for each enchantment
        if((random%3)==0){
            init_pair(3, COLOR_GREEN, COLOR_GREEN);
            attron(A_DIM | COLOR_PAIR(3));
            mvprintw(y,x,"%lc",L'🏺');
            attroff(A_DIM | COLOR_PAIR(3));
            strcpy(items[x][y],"Enchantment of Health");
        }
        else if((random%3)==1){
            init_pair(4, COLOR_CYAN, COLOR_CYAN);
            attron(COLOR_PAIR(4));
            mvprintw(y,x,"%lc",L'🏺');
            attroff(COLOR_PAIR(4));
            strcpy(items[x][y],"Enchantment of Speed");
            }
        else{
            init_color(COLOR_RED,800,0,0);
            init_pair(5, COLOR_RED, COLOR_RED);
            attron(COLOR_PAIR(5));
            mvprintw(y,x,"%lc",L'🏺');
            attroff(COLOR_PAIR(5));
            strcpy(items[x][y],"Enchantment of Damage");
            }
        refresh();
        rooms[selected]->enchant_num+=1;
    }
}

//////////////////////////////////////////////////////////////////////////////

void weapon_generator(room ** rooms, int n){ //each floor has 1
    int selected;
    int loop_stopper=0;
    do{
    selected=rand()%n;
    loop_stopper++;
    if(loop_stopper>200 )return;}
    while(selected==enchantment_room || selected==gold_room || selected==nightmare_room);
    int w=rooms[selected]->width;
    int h=rooms[selected]->height;
    int x,y;
    loop_stopper=0;
    do{
        x=rand()%(w-2) +2 + rooms[selected]->x; 
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_stopper++;
        if(loop_stopper>200 )return;}
    while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.' || mvinch(y,x-2)!='.');
    int random=rand()%4 +1; 
    init_color(COLOR_MAGENTA,1000,0,1000);
    init_pair(2,COLOR_MAGENTA,COLOR_BLACK);
    attron(COLOR_PAIR(2));
    if(random==1){
        attron(A_BOLD);
        mvprintw(y,x,"%lc",L'⚔');
        attroff(A_BOLD);
        strcpy(items[x][y],"Sword");
    }
    else if(random==2){
        attron(A_BOLD);
        mvprintw(y,x,"%lc",L'🗡');
        attroff(A_BOLD);
        strcpy(items[x][y],"Dagger");
    }
    else if(random==3){
        attron(A_BOLD);
        mvprintw(y,x,"%lc",L'☦');
        attroff(A_BOLD);
        strcpy(items[x][y],"Magic wand");
    }
    else if(random==4){
        attron(A_BOLD);
        mvprintw(y,x,"%lc",L'➴');
        attroff(A_BOLD);
        strcpy(items[x][y],"Arrows");
    }
    
    attroff(COLOR_PAIR(2));
    refresh();
    rooms[selected]->weapon_num+=1;
}

///////////////////////////////////////////////////////////////////////////////////////

void food_generator(room ** rooms, int n){ //each floor has 2
    for(int i=0;i<2;i++){
        int selected;
        int loop_stopper=0;
        do{
        selected=rand()%n;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(rooms[selected]->food_num>=1 || selected==enchantment_room || selected==gold_room || selected==nightmare_room);
        int w=rooms[selected]->width;
        int h=rooms[selected]->height;
        int x,y;
        loop_stopper=0;
        do{
        x=rand()%(w-2) +2 + rooms[selected]->x; 
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_stopper++;
        if(loop_stopper>200 )return;}
        while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.' || mvinch(y,x+2)!='.' || mvinch(y,x-2)!='.');
        int random=rand()%4 +1;
        if(random==1 || random==4){
        mvprintw(y,x,"%lc",L'🍖');
        strcpy(items[x][y],"Food");
        }
        else if(random==2){ //aala
            attron(COLOR_PAIR(3));
            mvprintw(y,x,"%lc",L'🍖');
            attroff(COLOR_PAIR(3));
            strcpy(items[x][y],"Exquisite Food");
        }
        else if(random==3){ //magical
            init_color(COLOR_YELLOW, 800, 800, 0);
            init_pair(6,COLOR_YELLOW,COLOR_YELLOW);
            attron(COLOR_PAIR(6));
            mvprintw(y,x,"%lc",L'🍖');
            attroff(COLOR_PAIR(6));
            strcpy(items[x][y],"Magical Food");
        }
        refresh();
        rooms[selected]->food_num+=1;
    }
}

////////////////////////////////////////////////////////////////////

void monster_generator(room ** rooms, int n, int difficulty){
    int num=difficulty+2; // 3 to 5
    int loop_breaker=0;
    for(int i=0;i<num;i++){
        int selected;
        do{
        selected=rand()%n;
        loop_breaker++;
        if(loop_breaker>200) return;}
        while(rooms[selected]->monster_num>=1 || selected==enchantment_room || selected==gold_room || selected==nightmare_room);
        int w=rooms[selected]->width;
        int h=rooms[selected]->height;
        int x,y;
        loop_breaker=0;
        do{
        x=rand()%(w-2) +2 + rooms[selected]->x; 
        y=rand()%(h-2) +2 + rooms[selected]->y;
        loop_breaker++;
        if(loop_breaker>200) return;}
        while(mvinch(y,x)!='.' || mvinch(y-1,x)!='.' || mvinch(y+1,x)!='.' || mvinch(y,x+1)!='.' || mvinch(y,x-1)!='.' || mvinch(y,x-2)!='.');
        int random=rand()%17;
        if(random<=3){
            mvprintw(y,x,"D");}
        else if(random>3 && random<=7){
            mvprintw(y,x,"F");}
        else if(random>7 && random<=11){
            mvprintw(y,x,"G");}
        else if(random>11 && random<=14){
            mvprintw(y,x,"S");}
        else if(random>14 && random<=16){
            mvprintw(y,x,"U");}
        refresh();
        rooms[selected]->monster_num+=1;
    }
}

////////////////////////////////////////////////////////////////////
int total_room;

room ** map_generator(){
    memset(items,'\0',sizeof(items));
    setlocale(LC_ALL, "");
    initscr();
    srand(time(NULL));
    keypad(stdscr, TRUE);
    noecho();
    getmaxyx(stdscr, max_y, max_x);
    total_room=rand()%2 +8;
    room **rooms=malloc(total_room * sizeof(room *));
    for(int i=0;i<total_room;i++){
        *(rooms+i)=malloc(sizeof(room));
        room_generator(*(rooms+i));
    }
    int p=corridor_creator(rooms, total_room);
    while(p==0){
        memset(items,'\0',sizeof(items));
        clear();
        for(int i=0;i<total_room;i++){
            free(*(rooms+i));}
        free(rooms);
        room **rooms=malloc(total_room * sizeof(room *));
        for(int i=0;i<total_room;i++){
            *(rooms+i)=malloc(sizeof(room));
            room_generator(*(rooms+i));
        }
        p=corridor_creator(rooms, total_room);
    }
    enchantment_room_generator(rooms,total_room);
    nightmare_room_generator(rooms,total_room);
    staircase_generator(rooms,total_room);
    pillar_generator(rooms,total_room, difficulty*2+2);
    window_creator(rooms,total_room, rand()%4+2);
    enchantment_generator(rooms,total_room,rand()%(difficulty+1));
    weapon_generator(rooms, total_room);
    food_generator(rooms,total_room);
    gold_generator(rooms,total_room, rand()%2+3);
    monster_generator(rooms,total_room,difficulty);
    

    return rooms;
    
}
