#include<stdlib.h>
#include<ncurses.h>

typedef struct 
{
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
    char escapeApp; 
    int com1;
    int p;
    int s;
    int c;
    int activeWindow;
    float timeToRefresh;
    int activeBrush;
}Args;

void clearWindows(Args Arg)
{
    int i;
    for(i=0;i<10;i++) 
    {
        mvwprintw(Arg.w1,i,Arg.distanceW1,"%s","                               ");
    }
}

void printProcess(Args Arg,char listProcess[][30],char listStates[][30],int i)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //p -> variable de index process
    //s -> variable de index status
    char itemobjects[30];
    sprintf(itemobjects,"%s  %s",listProcess[i],listStates[Arg.s]);
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

void printAdvancedControls(Args Arg,char listAdvancedControls[][30],int i)
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
    mvwprintw(Arg.w1,i+3,Arg.distanceW1,"%s",listAdvancedControls[i]);
    return;
}

void refreshWindows(Args Arg,char listProcess[][30],char listStates[][30],char listAdvancedControls[][30],char listGeneralControls[][30])
{
    //Función para dibujar las ventanas
    clearWindows(Arg);
    wattroff(Arg.w1,A_STANDOUT);
    if(Arg.viewDetails==0)
    {
        mvwprintw(Arg.w1,1,Arg.distanceW1,"%s","PROCESOS             ESTADO");
        wattron(Arg.w1,A_STANDOUT);
        // imprimo listProcess - se seleciona el primer elemento
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
            printProcess(Arg,listProcess,listStates,i);
        }
    }
    else
    {
        mvwprintw(Arg.w1,1,Arg.distanceW1,"%s%s%s","Proc: ",listProcess[Arg.p],listStates[Arg.s]);
        wattron(Arg.w1,A_STANDOUT);
        // imprimo listProcess - se seleciona el primer elemento
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
            printAdvancedControls(Arg,listAdvancedControls,i);
        }
    }
    wattroff(Arg.w1,A_STANDOUT);
    for(Arg.c=0;Arg.c<Arg.maxItemsGeneralControls;Arg.c++) 
    {
        mvwprintw(Arg.w2,2,(Arg.distanceW2*Arg.c)+2,"%s",listGeneralControls[Arg.c]);
    }
    box(Arg.w1,0,0); 
    wrefresh(Arg.w1);
    box(Arg.w2,0,0); 
    wrefresh(Arg.w2);
    box(Arg.w3,0,0); 
    wrefresh(Arg.w3);
    return;
}

Args activateWindows(Args Arg,char listProcess[][30],char listStates[][30],char listAdvancedControls[][30],char listGeneralControls[][30])
{
    //Función donde la primera ventana se activa
    //Si flecha arriba se sube en la listobjectsa, si flecha abajo se baja en la listobjectsa
    switch(Arg.com1) 
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
    }
    refreshWindows(Arg,listProcess,listStates,listAdvancedControls,listGeneralControls);
    return Arg;
}

int main() 
{     
    // se inicializa Ncurses
    initscr();
    crmode();
    /* Inicializaciones variables */
    Args Arg;
    // nuevas ventanas (Lineas, Columnas, posición Y, posición X)
    Arg.w1 = newwin(26,35,1,1); 
    Arg.w2 = newwin(5,155,27,1);
    Arg.w3 = newwin(26,120,1,36);
    char listProcess[5][30]={"DialServer ","DialReport ","DialContact","Asterisk   ","ipTables   "};
    char listStates[5][30]={"            OK","           RIP","   Starting...","   Stopping...","           ???"};
    char listGeneralControls[3][30]={"F1-MainLog  ","F2-WebServiceLog   ","F3-SpreadLog"};
    char listAdvancedControls[4][30]={"start","stop","reload","restart"};
    Arg.maxItemsProcess=sizeof(listProcess)/sizeof(listProcess[0]);
    Arg.maxItemsStates=sizeof(listStates)/sizeof(listStates[0]);
    Arg.maxItemsGeneralControls=sizeof(listGeneralControls)/sizeof(listGeneralControls[0]);
    Arg.maxItemsAdvancedControls=sizeof(listAdvancedControls)/sizeof(listAdvancedControls[0]);
    Arg.distanceW1=2;
    Arg.distanceW2=30;
    Arg.viewDetails=0;
    Arg.escapeApp='q'; 
    Arg.com1;
    Arg.p=0;
    Arg.s=0;
    Arg.c=0;
    Arg.activeWindow=1;
    Arg.timeToRefresh=1;
    Arg.activeBrush=1;

    /* Init App */
    mvwprintw(Arg.w1,1,Arg.distanceW1,"%s","PROCESOS             ESTADO");
    // imprimo listProcess - se seleciona el primer elemento
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
        printProcess(Arg,listProcess,listStates,Arg.p);
    }

    for(Arg.c=0;Arg.c<Arg.maxItemsGeneralControls;Arg.c++) 
    {
        mvwprintw(Arg.w2,2,(Arg.distanceW2*Arg.c)+2,"%s",listGeneralControls[Arg.c]);
    }
    // se refresca las consolas
    box(Arg.w1,0,0); 
    wrefresh(Arg.w1);
    box(Arg.w2,0,0); 
    wrefresh(Arg.w2);
    box(Arg.w3,0,0); 
    wrefresh(Arg.w3);
 
    Arg.p=0;
    Arg.c=0;
    noecho();

    keypad(Arg.w1,TRUE); // se permite meter numeros
    curs_set(0); // se esconde el cursor normal

    //entrada teclado
    while((Arg.com1=wgetch(Arg.w1))!=Arg.escapeApp)
    {
        if(Arg.activeWindow==1)
        {
            Arg=activateWindows(Arg,listProcess,listStates,listAdvancedControls,listGeneralControls);
        }
    }
    //Cierro las ventanas
    delwin(Arg.w1);
    endwin();
    return 0;
}
