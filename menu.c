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

int allowed_to_resume=0;


///////////////////////////////////////////////////////

void pause_function(char *user){
    FILE *ptr=fopen("users.txt","r");
    FILE *temp=fopen("temp.txt","w");
    char info[100];
    while(fgets(info,100,ptr)){
        char pass[100];
        char email[100];
        char breaker[100];
        int gold; int score; int exp;
        fgets(pass,100,ptr);
        fgets(email,100,ptr);
        fscanf(ptr,"%d\n",&gold);
        fscanf(ptr,"%d\n",&score);
        fscanf(ptr,"%d\n",&exp);
        fscanf(ptr,"%d",&allowed_to_resume);
        fgets(breaker,100,ptr);
        info[strcspn(info, "\n")]='\0';
        if(strcmp(user,info)==0){
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n1%s",info,pass,email,gold+gold_to_be_added,score+score_to_be_added,exp,breaker);
        }
        else{
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n%d%s",info,pass,email,gold,score,exp,allowed_to_resume,breaker);
        }
    }  
    fclose(ptr);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt","users.txt");
    clear();
}

void loose_page(char *user){
    FILE *ptr=fopen("users.txt","r");
    FILE *temp=fopen("temp.txt","w");
    char info[100];
    while(fgets(info,100,ptr)){
        char pass[100];
        char email[100];
        char breaker[100];
        int gold; int score; int exp;
        fgets(pass,100,ptr);
        fgets(email,100,ptr);
        fscanf(ptr,"%d\n",&gold);
        fscanf(ptr,"%d\n",&score);
        fscanf(ptr,"%d\n",&exp);
        fscanf(ptr,"%d",&allowed_to_resume);
        fgets(breaker,100,ptr);
        info[strcspn(info, "\n")]='\0';
        if(strcmp(user,"")==0){break;}
        if(strcmp(user,info)==0 && strcmp(user,"Guest")!=0){
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n0%s",info,pass,email,gold+gold_to_be_added,score+score_to_be_added,exp,breaker);
        }
        else{
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n%d%s",info,pass,email,gold,score,exp,allowed_to_resume,breaker);
        }
    }  
    fclose(ptr);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt","users.txt");
    clear();
    attron(A_BOLD | COLOR_PAIR(21));
    mvprintw(10,75,"You have fallen, brave warrior...");
    mvprintw(11,55,"Your journey ends here, but your legend will echo through the ages.");
    attroff(A_BOLD | COLOR_PAIR(21));
    attron(A_BOLD | COLOR_PAIR(10));
    mvprintw(13,85,"Gold +%d",gold_to_be_added);
    mvprintw(14,85,"Score +%d",score_to_be_added);
    attroff(A_BOLD | COLOR_PAIR(10));

mvprintw(20,50,"                                 _____  _____");
mvprintw(21,50,"                                <     `/     |");
mvprintw(22,50,"                                 >          (");
mvprintw(23,50,"                                |   _     _  |");
mvprintw(24,50,"                                |  |_) | |_) |");
mvprintw(25,50,"                                |  | \\ | |   |");
mvprintw(26,50,"                                |            |");
mvprintw(27,50,"                 ______.______%%_|            |__________  _____");
mvprintw(28,50,"               _/                                       \\|     |");
mvprintw(29,50,"              |                                                <");
mvprintw(30,50,"              |_____.-._________              ____/|___________|");
mvprintw(31,50,"                                |            |");
mvprintw(32,50,"                                |            |");
mvprintw(33,50,"                                |            |");
mvprintw(34,50,"                                |            |");
mvprintw(35,50,"                                |   _        <");
mvprintw(36,50,"                                |__/         |");
mvprintw(37,50,"                                 / `--.      |");
mvprintw(38,50,"                               %%|            |%%");
mvprintw(39,50,"                           |/.%%%%|          -< @%%%%%%");
mvprintw(40,50,"                           `\\%%`@|     v      |@@%%@%%%%    ");
mvprintw(41,50,"                         .%%%%%%@@@|%%    |    %% @@@%%%%@%%%%%%%%");
mvprintw(42,50,"                    _.%%%%%%%%%%%%@@@@@@%%%%_/%%\\_%%@@%%%%@@@@@@@%%%%%%%%%%%%");

mvprintw(28,120,"        '.,");
mvprintw(29,120,"          'b      *");
mvprintw(30,120,"           '$    #.");
mvprintw(31,120,"            $:   #:");
mvprintw(32,120,"            *#  @):");
mvprintw(33,120,"            :@,@):   ,.**:'");
mvprintw(34,120,"  ,         :@@*: ..**'");
mvprintw(35,120,"   '#o.    .:(@'.@*\"'");
mvprintw(36,120,"      'bq,..:,@@*'   ,*");
mvprintw(37,120,"      ,p$q8,:@)'  .p*'");
mvprintw(38,120,"     '    '@@Pp@@*'");
mvprintw(39,120,"           Y7'.'");
mvprintw(40,120,"          :@):.");
mvprintw(41,120,"         .:@:'.");
mvprintw(42,120,"       .::(@:.    ");

mvprintw(28,10,"                            '.,");
mvprintw(29,10,"                              'b      *");
mvprintw(30,10,"                               '$    #.");
mvprintw(31,10,"                                $:   #:");
mvprintw(32,10,"                                *#  @):");
mvprintw(33,10,"                                :@,@):   ,.**:'");
mvprintw(34,10,"                      ,         :@@*: ..**'");
mvprintw(35,10,"                       '#o.    .:(@'.@*\"'");
mvprintw(36,10,"                          'bq,..:,@@*'   ,*");
mvprintw(37,10,"                          ,p$q8,:@)'  .p*'");
mvprintw(38,10,"                         '    '@@Pp@@*'");
mvprintw(39,10,"                               Y7'.'");
mvprintw(40,10,"                              :@):.");
mvprintw(41,10,"                             .:@:'.");
mvprintw(42,10,"                           .::(@:.    ");

refresh();
getch();
clear();

}

void win_page(char *user){
    FILE *ptr=fopen("users.txt","r");
    FILE *temp=fopen("temp.txt","w");
    char info[100];
    while(fgets(info,100,ptr)){
        char pass[100];
        char email[100];
        char breaker[100];
        int gold; int score; int exp;
        fgets(pass,100,ptr);
        fgets(email,100,ptr);
        fscanf(ptr,"%d\n",&gold);
        fscanf(ptr,"%d\n",&score);
        fscanf(ptr,"%d\n",&exp);
        fscanf(ptr,"%d",&allowed_to_resume);
        fgets(breaker,100,ptr);
        info[strcspn(info, "\n")]='\0';
        if(strcmp(user,"")==0){break;}
        if(strcmp(user,info)==0 && strcmp(user,"Guest")!=0){
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n0%s",info,pass,email,gold+gold_to_be_added,score+score_to_be_added,exp+1,breaker);
        }
        else{
            fprintf(temp,"%s\n%s%s%d\n%d\n%d\n%d%s",info,pass,email,gold,score,exp,allowed_to_resume,breaker);
        }
    }  
    fclose(ptr);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt","users.txt");
    clear();
    attron(A_BOLD | COLOR_PAIR(9));
    mvprintw(10,75,"Congratulations, Fierce Warrior!");
    mvprintw(11,77,"You have escaped the dungeon!");
    attroff(A_BOLD | COLOR_PAIR(9));
    attron(A_BOLD | COLOR_PAIR(10));
    mvprintw(13,87,"Gold +%d",gold_to_be_added);
    mvprintw(14,87,"Score +%d",score_to_be_added);
    mvprintw(15,85,"Experience +1");
    attroff(A_BOLD | COLOR_PAIR(10));
mvprintw(20,60,"                                  |>>>");
mvprintw(21,60,"                                  |");
mvprintw(22,60,"                    |>>>      _  _|_  _         |>>>");
mvprintw(23,60,"                    |        |;| |;| |;|        |");
mvprintw(24,60,"                _  _|_  _    \\\\.    .  /    _  _|_  _");
mvprintw(25,60,"               |;|_|;|_|;|    \\\\:. ,  /    |;|_|;|_|;|");
mvprintw(26,60,"               \\\\..      /    ||;   . |    \\\\.    .  /");
mvprintw(27,60,"                \\\\.  ,  /     ||:  .  |     \\\\:  .  /");
mvprintw(28,60,"                 ||:   |_   _ ||_ . _ | _   _||:   |");
mvprintw(29,60,"                 ||:  .|||_|;|_|;|_|;|_|;|_|;||:.  |");
mvprintw(30,60,"                 ||:   ||.    .     .      . ||:  .|");
mvprintw(31,60,"                 ||: . || .     . .   .  ,   ||:   |       \\,/");
mvprintw(32,60,"                 ||:   ||:  ,  _______   .   ||: , |            /`\\");
mvprintw(33,60,"                 ||:   || .   /+++++++\\    . ||:   |");
mvprintw(34,60,"                 ||:   ||.    |+++++++| .    ||: . |");
mvprintw(35,60,"              __ ||: . ||: ,  |+++++++|.  . _||_   |");
mvprintw(36,60,"     ____--`~    '--~~__|.    |+++++__|----~    ~`---,              ___");
mvprintw(37,60,"-~--~                   ~---__|,--~'                  ~~----_____-~'   `~----~~");

mvprintw(28,130,"               ,@@@@@@@,");
mvprintw(29,130,"       ,,,.   ,@@@@@@/@@,  .oo8888o.");
mvprintw(30,130,"    ,&%%%%&%%&&%%,@@@@@/@@@@@@,8888\\88/8o");
mvprintw(31,130,"   ,%%&\\%%&&%%&&%%,@@@\\@@@/@@@88\\88888/88'");
mvprintw(32,130,"   %%&&%%&%%&/%%&&%%@@\\@@/ /@@@88888\\88888'");
mvprintw(33,130,"   %%&&%%/ %%&%%&&@@\\ V /@@' `88\\8 `/88'");
mvprintw(34,130,"   `&%%\\ ` /%%&'    |.|        \\ '|8'");
mvprintw(35,130,"       |o|        | |         | |");
mvprintw(36,130,"       |.|        | |         | |");
mvprintw(37,130,"    \\/ ._\\//_/__/  ,\\_//__\\/.  \\_//__/_");

mvprintw(28,20,"               ,@@@@@@@,");
mvprintw(29,20,"       ,,,.   ,@@@@@@/@@,  .oo8888o.");
mvprintw(30,20,"    ,&%%%%&%%&&%%,@@@@@/@@@@@@,8888\\88/8o");
mvprintw(31,20,"   ,%%&\\%%&&%%&&%%,@@@\\@@@/@@@88\\88888/88'");
mvprintw(32,20,"   %%&&%%&%%&/%%&&%%@@\\@@/ /@@@88888\\88888'");
mvprintw(33,20,"   %%&&%%/ %%&%%&&@@\\ V /@@' `88\\8 `/88'");
mvprintw(34,20,"   `&%%\\ ` /%%&'    |.|        \\ '|8'");
mvprintw(35,20,"       |o|        | |         | |");
mvprintw(36,20,"       |.|        | |         | |");
mvprintw(37,20,"    \\/ ._\\//_/__/  ,\\_//__\\/.  \\_//__/_");

mvprintw(2,15,"              .");
mvprintw(3,15,"               	");				
mvprintw(4,15,"              |		");			
mvprintw(5,15,"     .               /");				
mvprintw(6,15,"      \\       I     	");			
mvprintw(7,15,"                  /");
mvprintw(8,15,"        \\  ,g88R_");
mvprintw(9,15,"          d888(`  ).                   _");
mvprintw(10,15," -  --==  888(     ).=--           .+(`  )`.");
mvprintw(11,15,"          Y8P(       '`.          :(   .    )");
mvprintw(12,15,"        .+(`(      .   )     .--  `.  (    ) )");
mvprintw(13,15,"       ((    (..__.:'-'   .=(   )   ` _`  ) )");
mvprintw(14,15,"       `(       ) )       (   .  )     (   )  ._");
mvprintw(15,15,"         ` __.:'   )     (   (   ))     `-'.:(`  )");
mvprintw(16,15,"      ( )       --'       `- __.'         :(      ))");
mvprintw(17,15,"     (_.'          .')                    `(    )  ))");
mvprintw(18,15,"                  (_  )                     ` __.:'");

mvprintw(10,125,"             _                           ");       
mvprintw(11,125,"           (`  ).                   _        ");   
mvprintw(12,125,"          (     ).              .:(`  )`.     ");  
mvprintw(13,125,"         _(       '`.          :(   .    )     "); 
mvprintw(14,125,"     .=(`(      .   )     .--  `.  (    ) )    ");  
mvprintw(15,125,"    ((    (..__.:'-'   .+(   )   ` _`  ) )     ");            
mvprintw(16,125,"    `(       ) )       (   .  )     (   )  ._   ");
mvprintw(17,125,"      ` __.:'   )     (   (   ))     `-'.-(`  ) ");
mvprintw(18,125,"   ( )       --'       `- __.'         :(      )) ");
mvprintw(19,125,"  (_.'          .')                    `(    )  ))");
mvprintw(20,125,"               (_  )                     ` __.:'    ");   
                                        	
refresh();
getch();
clear();

}

////////////////////////////////////////////////////////

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
mvprintw(1,100,"                                 ==(W{==========-      /===-                      ");  
mvprintw(2,100,"                                   ||  (.--.)         /===-_---~~~~~~~~~------____  ");
mvprintw(3,100,"                                   | \\_,|**|,__      |===-~___                _,-' `");
mvprintw(4,100,"                      -==\\\\        `\\ ' `--'   ),    `//~\\\\   ~~~~`---.___.-~~      ");
mvprintw(5,100,"                  ______-==|        /`\\_. .__/\\ \\    | |  \\\\           _-~`         ");
mvprintw(6,100,"            __--~~~  ,-/-==\\\\      (   | .  |~~~~|   | |   `\\        ,'             ");
mvprintw(7,100,"         _-~       /'    |  \\\\     )__/==0==-\\<>/   / /      \\      /               ");
mvprintw(8,100,"       .'        /       |   \\\\      /~\\___/~~\\/  /' /        \\   /'                ");
mvprintw(9,100,"      /  ____  /         |    \\`\\.__/-~~   \\  |_/'  /          \\/'                  ");
mvprintw(10,100,"     /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                   ");
mvprintw(11,100,"                       \\_|      /        _) | ;  ),   __--~~                        ");
mvprintw(12,100,"                         '~~--_/      _-~/- |/ \\   '-~ \\                            ");
mvprintw(13,100,"                        {\\__--_/}    / \\\\_>-|)<__\\      \\                           ");
mvprintw(14,100,"                        /'   (_/  _-~  | |__>--<__|      |                          ");
mvprintw(15,100,"                       |   _/) )-~     | |__>--<__|      |                          ");
mvprintw(16,100,"                       / /~ ,_/       / /__>---<__/      |                          ");
mvprintw(17,100,"                      o-o _//        /-~_>---<__-~      /                           ");
mvprintw(18,100,"                      (^(~          /~_>---<__-      _-~                            ");
mvprintw(19,100,"                     ,/|           /__>--<__/     _-~                               ");
mvprintw(20,100,"                  ,//('(          |__>--<__|     /                  .----_          ");
mvprintw(21,100,"                 ( ( '))          |__>--<__|    |                 /' _---_~\\        ");
mvprintw(22,100,"              `-)) )) (           |__>--<__|    |               /'  /     ~\\`\\      ");
mvprintw(23,100,"             ,/,'//( (             \\__>--<__\\    \\            /'  //        ||     "); 
mvprintw(24,100,"           ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'      "); 
mvprintw(25,100,"         `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/               ");   
mvprintw(26,100,"       ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~               ");     
mvprintw(27,100,"        ;'( ')/ ,)(                              ~~~~~~~~~~                 ");        
mvprintw(28,100,"       ' ') '( (/                                                     ");
mvprintw(28,10,"                    |>>>                        |>>>");
mvprintw(29,10,"                    |                           |");
mvprintw(30,10,"                _  _|_  _                   _  _|_  _");
mvprintw(31,10,"               | |_| |_| |                 | |_| |_| |");
mvprintw(32,10,"               \\  .      /                 \\ .    .  /");
mvprintw(33,10,"                \\    ,  /                   \\    .  /");
mvprintw(34,10,"                 | .   |_   _   _   _   _   _| ,   |");
mvprintw(35,10,"                 |    .| |_| |_| |_| |_| |_| |  .  |");
mvprintw(36,10,"                 | ,   | .    .     .      . |    .|");
mvprintw(37,10,"                 |   . |  .     . .   .  ,   |.    |");
mvprintw(38,10,"     ___----_____| .   |.   ,  _______   .   |   , |---~_____");
mvprintw(39,10,"_---~            |     |  .   /+++++++\\    . | .   |         ~---_");
mvprintw(40,10,"                 |.    | .    |+++++++| .    |   . |              ~-_");
mvprintw(41,10,"              __ |   . |   ,  |+++++++|.  . _|__   |                 ~-_");
mvprintw(42,10,"     ____--`~    '--~~__ .    |++++ __|----~    ~`---,              ___^~-__");
mvprintw(43,10,"-~--~                   ~---__|,--~'                  ~~----_____-~'   `~----~");
    attron(A_BOLD | A_STANDOUT);
    mvprintw(max_y/4, max_x/2-8,"WELCOME TO ROGUE!");
    attroff(A_BOLD | A_STANDOUT);
    mvprintw(max_y/4+1, max_x/2-10," Adventure awaits...");
    mvprintw(max_y/2+3, max_x/2-10,"   press Q to quit");
    refresh();
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
        fgets(a,100,ptr);
        fgets(b,100,ptr);
        fgets(c,100,ptr);
        fgets(breaker,100,ptr);
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
    mvprintw(max_y/9*8-2, max_x/2-35,"Press \\ to receive a secret word, forged by fate.");
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
            mvprintw(max_y/5*4, max_x/2-33,"Thy cipher must be of seven characters or more.");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 2:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            mvprintw(max_y/5*4, max_x/2-40,"Thy cipher must bear at least one character of the lower script.");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 3:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            printw("Thou must ensure thy password hath at least one letter of the grand form.");
            attroff(A_BOLD | A_UNDERLINE);
            make_password(pass);
            break;
        case 4:
            attron(A_BOLD | A_UNDERLINE);
            move(max_y/5*4, max_x/2-45);
            clrtoeol();
            mvprintw(max_y/5*4, max_x/2-35,"Thy secret word must needs bear at least one numeral.");
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
        mvprintw(max_y/5*4, max_x/2-35,"Thy missive must needs be crafted in the proper manner.");
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
    mvprintw(max_y/4, max_x/2-50,"Dear Questor, please enter the name you wish to be recognized by in this realm.");
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
    fputs("0$$$$$$$$$$$$$$$$$$$$",fptr);
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
        fscanf(ptr,"%d\n",&gold);
        fscanf(ptr,"%d\n",&score);
        fscanf(ptr,"%d\n",&exp);
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
        mvprintw(max_y/5*3, max_x/2-30,"No such name exists in this realm.");
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
    mvprintw(max_y/8*7,max_x/2-35,"Forgotten your secret cipher? Press \\ to unseal it.");
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
            mvprintw(max_y/2-1, max_x/2-37, "Should I speak falsely, may the flames of perdition consume me.");
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
        mvprintw(max_y/5*3, max_x/2-35,"Thy password is not as it should be, noble one.");
        attroff(A_BOLD | A_UNDERLINE);
        move(max_y/2-2, max_x/2-10);
        clrtoeol();
        return login_password(real_pass,user);
    }
}

/////////////////////////////////////////////////////////////////////////////////

void menu(char *user);

char *login(){
    clear();
    mvprintw(max_y/4, max_x/2-50,"Greetings, fair soul! Please declare your Title and Cipher of Trust to proceed.");
    mvprintw(max_y/3, max_x/2-30,"Questor's Title : ");
    mvprintw(max_y/2-2, max_x/2-30,"Cipher of Trust : ");
    mvprintw(max_y/9*8,max_x/2-25,"Press delete to go back,");
    mvprintw(max_y/9*8-2,max_x/2-40,"Press '=' to enter as a humble guest, with no name to bear.");
    refresh();
    char c=getch();
    char user[100];
    if(c=='='){
        strcpy(user,"Guest");
        menu(user);
    }
    else{
        mvprintw(max_y/9*8-2,max_x/2-40,"                                                                        ");
        refresh();
        char *real_pass=login_username(user);
        if(real_pass==NULL){return NULL;}
        return login_password(real_pass,user);}
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
    box(win,ACS_PLUS, ACS_DIAMOND);
    wattron(win,A_BOLD);
    mvwprintw(win,2,45,"Scoreboard");
    wattroff(win,A_BOLD);
    
    
    FILE *ptr = fopen("users.txt", "r");

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
    int start=0;
    int end=start+20;
    int place_to_print=4;
    if(total_players<20){end=total_players;}
    for (int i = start; i < end; i++) {
        if (i == 0) {
            wattron(win, COLOR_PAIR(30) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥇 The Grandmaster");
        } else if (i == 1) {
            wattron(win, COLOR_PAIR(31) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥈 The Ascendant");
        } else if (i == 2) {
            wattron(win, COLOR_PAIR(32) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥉 The Challenger");
        } else {
            mvwprintw(win, place_to_print, 5, "%d.", i + 1);
        }

        if (strcmp(players[i].username, userr) == 0) {
            wattron(win, A_BLINK);
        }

        mvwprintw(win, place_to_print++, 24, "%s | Gold: %d | Score: %d | Exp: %d",
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
            start--;
            wclear(win);
            if(start<0){start=0;}
        } else if (ch == KEY_DOWN) {
            start++;
            wclear(win);
            if(start>total_players){start=total_players;}
        }
        else{break;}
        box(win,ACS_PLUS, ACS_DIAMOND);
        place_to_print=4;
        mvwprintw(win,2,45,"Scoreboard");
            for (int i = start; i < end; i++) {
        if (i == 0) {
            wattron(win, COLOR_PAIR(30) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥇 The Grandmaster");
        } else if (i == 1) {
            wattron(win, COLOR_PAIR(31) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥈 The Ascendant");
        } else if (i == 2) {
            wattron(win, COLOR_PAIR(32) | A_STANDOUT);
            mvwprintw(win, place_to_print, 5, "🥉 The Challenger");
        } else {
            mvwprintw(win, place_to_print, 5, "%d.", i + 1);
        }

        if (strcmp(players[i].username, userr) == 0) {
            wattron(win, A_BLINK);
        }

        mvwprintw(win, place_to_print++, 24, "%s | Gold: %d | Score: %d | Exp: %d",
                  players[i].username, players[i].gold, players[i].score, players[i].exp);

        wattroff(win, A_BLINK);
        wattroff(win, COLOR_PAIR(30) | A_STANDOUT);
        wattroff(win, COLOR_PAIR(31) |A_STANDOUT);
        wattroff(win, COLOR_PAIR(32) | A_STANDOUT);
    }
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
    mvprintw(max_y/5+22, max_x/2-9,"Press e to go back");
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
    clear();
mvprintw(20,10,"        _");
mvprintw(21,10,"       (_)");
mvprintw(22,10,"       |=|");
mvprintw(23,10,"       |=|");
mvprintw(24,10,"   /|__|_|__|\\");
mvprintw(25,10,"  (    ( )    )");
mvprintw(26,10,"   \\|\\/\"/\\/|/");
mvprintw(27,10,"     |  Y  |");
mvprintw(28,10,"     |  |  |");
mvprintw(29,10,"     |  |  |");
mvprintw(30,10,"    _|  |  |");
mvprintw(31,10," __/ |  |  |\\");
mvprintw(32,10,"/  \\ |  |  |  \\");
mvprintw(33,10,"   __|  |  |   |");
mvprintw(34,10,"/\\/  |  |  |   |\\");
mvprintw(35,10," <   +\\ |  |\\ />  \\");
mvprintw(36,10,"  >   + \\  | LJ    |");
mvprintw(37,10,"        + \\|+  \\  < \\");
mvprintw(38,10,"  (O)      +    |    )");
mvprintw(39,10,"   |             \\  /\\ ");
mvprintw(40,10," ( | )   (o)      \\/  )");
mvprintw(41,10,"_\\\\|//__( | )______)_/ ");
mvprintw(42,10,"        \\\\|//");
    noecho();
    curs_set(0);
    attron(A_BOLD);
    mvprintw(max_y/5-2, max_x/2-7,"Welcome, %s!",user);
    mvprintw(max_y/5+27, max_x/2-7,"Press e to exit");
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
                else if(cur_but==1){
                    int a=new_game(user);
                    if(a==1){
                        win_page(user);
                        menu(user);
                    }
                    else if(a==-1){
                        loose_page(user);
                        menu(user);
                    }
                    else if(a==0){
                        pause_function(user);
                        menu(user);
                    }
                }
                else if(cur_but==2){
                    FILE *ptr=fopen("users.txt","r");
                    char info[100];
                    while(fgets(info,100,ptr)){
                        char pass[100];
                        char email[100];
                        char breaker[100];
                        int gold; int score; int exp;
                        fgets(pass,100,ptr);
                        fgets(email,100,ptr);
                        fscanf(ptr,"%d\n",&gold);
                        fscanf(ptr,"%d\n",&score);
                        fscanf(ptr,"%d\n",&exp);
                        fscanf(ptr,"%d",&allowed_to_resume);
                        fgets(breaker,100,ptr);
                        info[strcspn(info, "\n")]='\0';
                        if(strcmp(user,info)==0){
                        fclose(ptr);
                        break;}
                    }  
                    if(allowed_to_resume){
                        int a=resume_game(user);
                        if(a==1){
                            win_page(user);
                            menu(user);
                        }
                        else if(a==-1){
                            loose_page(user);
                            menu(user);
                        }
                        else if(a==0){
                            pause_function(user);
                            menu(user);
                        }
                    }
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
                        fscanf(ptr,"%d\n",&gold);
                        fscanf(ptr,"%d\n",&score);
                        fscanf(ptr,"%d\n",&exp);
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
                exit(0);
                return;
            case '\n':
                if(current_button==2){
                    if(create_account()==0){
                        clear();
                        first_page();}
                    else{
                        clear();
                        attron(A_BOLD);
                        mvprintw(max_y/3, max_x/2-32,"The gates to the realm are now open,Enjoy your journey!");
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
    init_color(COLOR_YELLOW, 800, 800, 0);
    init_pair(10,COLOR_YELLOW, COLOR_BLACK);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_GREEN,300,700,0);
    init_pair(9,COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_CYAN);
    init_color(COLOR_RED,800,0,0);
    init_pair(5, COLOR_RED, COLOR_RED);
    init_pair(19,COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_GREEN);
    init_pair(4, COLOR_CYAN, COLOR_CYAN);
    init_color(COLOR_MAGENTA,1000,0,1000);
    init_pair(2,COLOR_MAGENTA,COLOR_BLACK);
    init_color(COLOR_YELLOW, 800, 800, 0);
    init_pair(6,COLOR_YELLOW,COLOR_YELLOW);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    getmaxyx(stdscr, max_y, max_x);
    first_page();
    pthread_join(music_thread, NULL);
    SDL_Quit();
    return 0;
}

