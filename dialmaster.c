#include<stdlib.h>
#include<ncurses.h>

typedef struct 
{
    WINDOW *w1;
    WINDOW *w2;
    WINDOW *w3;
}multiWindows;

typedef struct 
{
    multiWindows (w1,w2,w3);
    char listProcess[][25];
    char listStates[][25];
    char listGeneralControls[][25];
    char listAdvancedControls[][25];
    int maxItemsProcess;
    int maxItemsStates;
    int maxItemsGeneralControls;
    int maxItemsAdvancedControls;
    int distanceW1;
    int distanceW2;
    int viewDetails;
    char escapeApp; 
    int com1;
    int p;
    int s;
    int c;
    int activeWindow;
    float timeToRefresh;
}Args;

void printProcess(Args Arg)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //p -> variable de index process
    //s -> variable de index status
    char itemobjects[30];
    sprintf(itemobjects,"%s  %s",Arg.listProcess[Arg.p],Arg.listStates[Arg.s]);
    mvwprintw(Arg.multiWindow.w1,Arg.p+3,Arg.distanceW1,"%s",itemobjects);
}

void refreshWindows(Args Arg)
{
    //Función para dibujar las ventanas
    if(Arg.viewDetails==0)
    {
        mvwprintw(Arg.multiWindow.w1,1,Arg.distanceW1,"%s","PROCESOS             ESTADO");
    }
    else
    {
        mvwprintw(Arg.multiWindow.w1,1,Args.distanceW1,"%s%c-%c","PROCESO: ",Args.listProcess[Args.p],Args.listStates[Args.s]);
    }
    // imprimo listProcess - se seleciona el primer elemento
    for(Args.p=0;Args.p<Args.maxItemsProcess;Args.p++) 
    {
        if(p==0) 
        {
            wattron(Arg.multiWindow.w1,A_STANDOUT); // se enciende el primer itemobjects
        }
        else
        {
            wattroff(Arg.multiWindow.w1,A_STANDOUT); // se apaga el primer itemobjects
        }
        printProcess(Arg.multiWindow.w1,Args.listProcess,Args.listStates,Args.p,Args.s,Args.distanceW1);
    }

    for(c=0;c<maxItemsControls;c++) 
    {
        mvwprintw(Arg.multiWindow.w2,2,(distanceW2*c)+2,"%s",listGeneralControls[c]);
    }
    box(Arg.multiWindow.w1,0,0); 
    wrefresh(Arg.multiWindow.w1);
    box(Arg.multiWindow.w2,0,0); 
    wrefresh(Arg.multiWindow.w2);
    box(Arg.multiWindow.w3,0,0); 
    wrefresh(Arg.multiWindow.w3);
    return;
}

Args activateWindows(Args Arg)
{
    //Función donde la primera ventana se activa
    //Margen derecho para la caja de elementos
    if(viewDetails==0)
    {
        printProcess(Arg.multiWindow.w1,listProcess,listStates,p,s,distanceW1);
    }
    else
    {
        mvwprintw(Arg.multiWindow.w1,c+3,distanceW1,"%s",listAdvancedControls[c]);
    }
    //Si flecha arriba se sube en la listobjectsa, si flecha abajo se baja en la listobjectsa
    switch(com1) 
    {
        case KEY_UP:
        {
            if(viewDetails==0)
            {
                p--;
                p = (p<0) ? maxItemsProcess-1 : p;
            }
            else
            {
                c--;
                c = (c<0) ? maxAdvancedControls-1 : c;
            }
            break;
        }
        case KEY_DOWN:
        {
            if(viewDetails==0)
            {
                p++;
                p = (p>=maxItemsProcess) ? 0 : p;
            }
            else
            {
                c++;
                c = (c>=maxAdvancedControls) ? 0 : c;
            }
            break;
        }
        case KEY_RIGHT:
        {
            viewDetails=1;
            break;
        }
        case KEY_LEFT:
        {
            viewDetails=0;
            break;
        }
    }
    // pinto el elemento seleccionado
    wattron(Arg.multiWindow.w1,A_STANDOUT);
    if(viewDetails==0)
    {
        printProcess(Arg.multiWindow.w1,listProcess,listStates,p,s,distanceW1);
    }
    else
    {
        refreshWindow2(Arg.multiWindow.w1,listAdvancedControls,c,distanceW1);
    }
    wattroff(Arg.multiWindow.w1,A_STANDOUT);
    refreshWindows(Arg);
    Arg.p=p;
    Arg.c=c;
    Arg.viewDetails=viewDetails;
    return Arg;
}

void main() 
{     
    // se inicializa Ncurses
    initscr();
    crmode();
    /* Inicializaciones variables */
    WINDOW *w1;
    WINDOW *w2;
    WINDOW *w3;
    // nuevas ventanas (Lineas, Columnas, posición Y, posición X)
    w1 = newwin(26,35,1,1); 
    w2 = newwin(5,155,27,1);
    w3 = newwin(26,120,1,36);
    char listProcess[][25] = {"DialServer ","DialReport ","DialContact","Asterisk   ","ipTables   "};
    char listStates[][25] = {"            OK","           RIP","   Starting...","   Stopping...","           ???"};
    char listGeneralControls[][25] = {"F1-MainLog  ","F2-WebServiceLog   ","F3-SpreadLog"};
    char listAdvancedControls[][25] = {"start  ","stop   ","restart","reload "};
    int maxItemsProcess=sizeof(listProcess)/sizeof(listProcess[0]);
    int maxItemsStates=sizeof(listStates)/sizeof(listStates[0]);
    int maxItemsGeneralControls=sizeof(listGeneralControls)/sizeof(listGeneralControls[0]);
    int maxItemsAdvancedControls=sizeof(listAdvancedControls)/sizeof(listAdvancedControls[0]);
    int distanceW1=2;
    int distanceW2=25;
    int viewDetails=0;
    char escapeApp='q'; 
    int com1;
    int p=0,s=0,c=0,activeWindow=1;
    float timeToRefresh=1;

    multiWindows multiWindow(w1,w2,w3);
    Args Arg(multiWindow,listProcess,listStates,listGeneralControls,listAdvancedControls,maxItemsProcess,maxItemsStates,maxItemsGeneralControls,maxItemsAdvancedControls,distanceW1,distanceW2,viewDetails,escapeApp,com1,p,s,c,activeWindow,timeToRefresh);

    /* Init App */
    mvwprintw(w1,1,distanceW1,"%s","PROCESOS             ESTADO");
    // imprimo listProcess - se seleciona el primer elemento
    for(p=0;p<maxItemsProcess;p++) 
    {
        if(p==0) 
        {
            wattron(w1,A_STANDOUT); // se enciende el primer itemobjects
        }
        else
        {
            wattroff(w1,A_STANDOUT); // se apaga el primer itemobjects
        }
        printProcess(Arg);
    }

    for(c=0;c<maxItemsControls;c++) 
    {
        mvwprintw(w2,2,(distanceW2*c)+2,"%s",listGeneralControls[c]);
    }
    // se refresca las consolas
    box(w1,0,0); 
    wrefresh(w1);
    box(w2,0,0); 
    wrefresh(w2);
    box(w3,0,0); 
    wrefresh(w3);
 
    p=0;
    c=0;
    noecho();

    keypad(w1,TRUE); // se permite meter numeros
    keypad(stdscr,TRUE); // se permite gastar los teclados extra
    curs_set(0); // se esconde el cursor normal

    //entrada teclado
    while((com1=wgetch(w1))!=escapeApp)
    {
        if(activeWindow==1)
        {
            Arg=activateWindows(Arg);
            p=Arg.p;
            c=Arg.c;
            viewDetails=Arg.viewDetails;
        }
    }
    //Cierro las ventanas
    delwin(w1);
    endwin();
}