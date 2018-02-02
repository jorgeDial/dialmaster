#include<stdlib.h>
#include<ncurses.h>
#include<string>
#include<sys/ioctl.h>
#include<stdio.h>
#define ESCAPE 27
#define ENTER 10
//Variables Globales
//Variables donde almacenaremos el tamaño de nuestra pantalla
int windowsDimY, windowsDimX;

typedef struct arguments
{
    char listProcess[5][30];
    char listStates[5][30];
    char listGeneralControls[3][30];
    char listAdvancedControls[5][30];
    WINDOW *w1;
    WINDOW *w2;
    WINDOW *w3;
    int maxItemsProcess;
    int maxItemsStates;
    int maxItemsGeneralControls;
    int maxItemsAdvancedControls;
    int distanceW1;
    int distanceW2;
    int viewDetails;
    int escapeApp1; 
    int escapeApp2;
    int keyPress;
    int p;
    int s;
    int c;
    int activeWindow;
    float timeToRefresh;
    int activeBrush;
} Args;

void refreshWindows(Args Arg);
void printProcess(Args Arg,int i);
void printAdvancedControls(Args Arg,int i);
void printOnSecondWindows(Args Arg);
void highlightOption(Args Arg);
Args activateWindows(Args Arg);
void initializeScreen();

int main() 
{     
    // se inicializa Ncurses
    initscr();
    cbreak();
    crmode();
    /* Inicializaciones variables */
    Args Arg = 
    {
        .listProcess = {"dialserver","dialreport","dialcontact","asterisk","iptables"},
        .listStates={"OK","RIP","Starting...","Stopping...","???"},
        .listGeneralControls={"F1-MainLog","F2-WebServiceLog","F3-SpreadLog"},
        .listAdvancedControls={"start","stop","reload","restart","status"}
    };
    // nuevas ventanas (Lineas, Columnas, posición Y, posición X)
    getmaxyx(stdscr, windowsDimY, windowsDimX);
    Arg.w1=newwin(windowsDimY-7,35,1,1); 
    Arg.w2=newwin(5,windowsDimX-5,windowsDimY-6,1);
    Arg.w3=newwin(windowsDimY-7,windowsDimX-40,1,36);
    Arg.maxItemsProcess=sizeof(Arg.listProcess)/sizeof(Arg.listProcess[0]);
    Arg.maxItemsStates=sizeof(Arg.listStates)/sizeof(Arg.listStates[0]);
    Arg.maxItemsGeneralControls=sizeof(Arg.listGeneralControls)/sizeof(Arg.listGeneralControls[0]);
    Arg.maxItemsAdvancedControls=sizeof(Arg.listAdvancedControls)/sizeof(Arg.listAdvancedControls[0]);
    Arg.distanceW1=2;
    Arg.distanceW2=30;
    Arg.viewDetails=0;
    Arg.escapeApp1='q';
    Arg.escapeApp2='Q'; 
    Arg.keyPress;
    Arg.p=0;
    Arg.s=0;
    Arg.c=0;
    Arg.activeWindow=1;
    Arg.timeToRefresh=1;
    Arg.activeBrush=1;

    /* Init App */
    initializeScreen();
    mvwprintw(Arg.w1,1,Arg.distanceW1,"%s","PROCESOS             ESTADO");
    // imprimo Arg.listProcess - se seleciona el primer elemento
    
    for(Arg.p=0;Arg.p<Arg.maxItemsProcess;Arg.p++) 
    {
        if(Arg.p==0) 
        {
            // se enciende el primer itemobjects
            Arg.activeBrush=1;
        }
        else
        {
            // se apaga el primer itemobjects
            Arg.activeBrush=0;
        }
        printProcess(Arg,Arg.p);
    }

    for(Arg.c=0;Arg.c<Arg.maxItemsGeneralControls;Arg.c++) 
    {
        mvwprintw(Arg.w2,2,(Arg.distanceW2*Arg.c)+2,"%s",Arg.listGeneralControls[Arg.c]);
    }
 
    Arg.p=0;
    Arg.c=0;

    //entrada teclado  
    refreshWindows(Arg);
    keypad(Arg.w1, TRUE); // se permite meter numeros en la pestaña w1
    bool exitWhile=false;
    while(!exitWhile)
    {
        refreshWindows(Arg);
        if(Arg.activeWindow==1)
        {
            Arg=activateWindows(Arg);
        }
        Arg.keyPress=wgetch(Arg.w1);
        if( (Arg.keyPress==Arg.escapeApp1) || (Arg.keyPress==Arg.escapeApp2) )
        {
            exitWhile=true;
        }
    }
    //Cierro las ventanas
    delwin(Arg.w1);
    delwin(Arg.w2);
    delwin(Arg.w3);
    endwin();
    return 0;
}

void refreshWindows(Args Arg)
{
    //Función para el resize de las ventanas
    refresh();
    getmaxyx(stdscr, windowsDimY, windowsDimX);
    /*
    Arg.w1=newwin(windowsDimY-7,35,1,1); 
    Arg.w2=newwin(5,windowsDimX-5,windowsDimY-6,1);
    Arg.w3=newwin(windowsDimY-7,windowsDimX-40,1,36);
    */
    wresize(Arg.w1,windowsDimY-7,35);
    wresize(Arg.w2,5,windowsDimX-5);
    wresize(Arg.w3,windowsDimY-7,windowsDimX-40);
    mvwin(Arg.w1,1,1);
    mvwin(Arg.w2,windowsDimY-6,1);
    mvwin(Arg.w3,1,36);
    wclear(Arg.w1);
    wclear(Arg.w2);
    wclear(Arg.w3);
    box(Arg.w1,0,0);
    box(Arg.w2,0,0);
    box(Arg.w3,0,0);
    wrefresh(Arg.w1);
    wrefresh(Arg.w2);
    wrefresh(Arg.w3);
    bkgd(COLOR_PAIR(1));
}

void printProcess(Args Arg,int i)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //p -> variable de index process
    //s -> variable de index status
    char itemobjects[30];
    
    sprintf(itemobjects,"%s - %s",Arg.listProcess[i],Arg.listStates[Arg.s]);
    if(Arg.activeBrush)
    {
        wattron(Arg.w1,A_STANDOUT);
    }
    else
    {
        wattroff(Arg.w1,A_STANDOUT);
    }
    mvwprintw(Arg.w1,i+3,Arg.distanceW1,"%s",itemobjects);
    return;
}

void printAdvancedControls(Args Arg,int i)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //c -> variable de index controls
    
    if(Arg.activeBrush)
    {
        wattron(Arg.w1,A_STANDOUT);
    }
    else
    {
        wattroff(Arg.w1,A_STANDOUT);
    }
    mvwprintw(Arg.w1,i+3,Arg.distanceW1,"%s",Arg.listAdvancedControls[i]);
    return;
}

void printOnSecondWindows(Args Arg)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //c -> variable de index controls
    char systemCommand[50]="cat /home/fran/Escritorio/prueba";
    system(systemCommand);
    mvwprintw(Arg.w2,3,3,"%c",'c');
    return;
}

void highlightOption(Args Arg)
{
    //Función para dibujar las ventanas
    wattroff(Arg.w1,A_STANDOUT);
    
    if(Arg.viewDetails==0)
    {
        mvwprintw(Arg.w1,1,Arg.distanceW1,"%s","PROCESOS             ESTADO");
        wattron(Arg.w1,A_STANDOUT);
        // imprimo Arg.listProcess - se seleciona el primer elemento
        int i;
        for(i=0;i<Arg.maxItemsProcess;i++) 
        {
            if(Arg.p<i) 
            {
                Arg.activeBrush=0;
            }
            else if(Arg.p==i)
            {
                Arg.activeBrush=1;
            }
            else
            {
                Arg.activeBrush=0;
            }
            printProcess(Arg,i);
        }
    }
    else
    {
        mvwprintw(Arg.w1,1,Arg.distanceW1,"%s%s%s","Proc: ",Arg.listProcess[Arg.p],Arg.listStates[Arg.s]);
        wattron(Arg.w1,A_STANDOUT);
        // imprimo Arg.listProcess - se seleciona el primer elemento
        int i;
        for(i=0;i<Arg.maxItemsAdvancedControls;i++) 
        {
            if(Arg.c<i) 
            {
                Arg.activeBrush=0;
            }
            else if(Arg.c==i)
            {
                Arg.activeBrush=1;
            }
            else
            {
                Arg.activeBrush=0;
            }
            printAdvancedControls(Arg,i);
        }
    }
    wattroff(Arg.w1,A_STANDOUT);
    for(Arg.c=0;Arg.c<Arg.maxItemsGeneralControls;Arg.c++) 
    {
        mvwprintw(Arg.w2,2,(Arg.distanceW2*Arg.c)+2,"%s",Arg.listGeneralControls[Arg.c]);
    }
    box(Arg.w2,0,0);
    wrefresh(Arg.w2);
    return;
}

Args activateWindows(Args Arg)
{
    //Función donde la primera ventana se activa
    
    switch(Arg.keyPress) 
    {
        case KEY_UP:
        {
            if(Arg.viewDetails==0)
            {
                Arg.p--;
                Arg.p = (Arg.p<0) ? Arg.maxItemsProcess-1 : Arg.p;
            }
            else
            {
                Arg.c--;
                Arg.c = (Arg.c<0) ? Arg.maxItemsAdvancedControls-1 : Arg.c;
            }
            break;
        }
        case KEY_DOWN:
        {
            if(Arg.viewDetails==0)
            {
                Arg.p++;
                Arg.p = (Arg.p>=Arg.maxItemsProcess) ? 0 : Arg.p;
            }
            else
            {
                Arg.c++;
                Arg.c = (Arg.c>=Arg.maxItemsAdvancedControls) ? 0 : Arg.c;
            }
            break;
        }
        case KEY_RIGHT:
        {
            Arg.viewDetails=1;
            break;
        }
        case KEY_LEFT:
        {
            Arg.viewDetails=0;
            break;
        }
        case 10:
        {
            //case 10 -> KEY_ENTER
            if(Arg.viewDetails==0)
            {
                //
            }
            else
            {
                char systemCommand[50];
                sprintf(systemCommand,"%s  %s %s","service",Arg.listProcess[Arg.p],Arg.listAdvancedControls[Arg.c]);
                system(systemCommand);
            }
            break;
        }
        case 265:
        {
            //case 265 -> KEY_F1
            printOnSecondWindows(Arg);
            break;
        }
    }
    highlightOption(Arg);
    return Arg;
}

void initializeScreen()
{
    if (initscr()!=NULL)
    {
        if(has_colors())
        {
            getmaxyx(stdscr, windowsDimY, windowsDimX);
            start_color(); //se enciendo los colores
            init_pair(1,COLOR_WHITE,COLOR_BLUE);
            init_pair(2,COLOR_RED,COLOR_WHITE);
            init_pair(3,COLOR_WHITE,COLOR_RED);
            curs_set(0); // se esconde el cursor normal
            noecho();
            bkgd(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
        }
    }
}