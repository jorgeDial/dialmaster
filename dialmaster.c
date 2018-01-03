#include<stdlib.h>
#include<ncurses.h>
#include<sys/ioctl.h>
#include<stdio.h>

typedef struct 
{
    int counter;
    int viewDetails;
}ActivateWindow;

void refreshWindows(WINDOW *w1, WINDOW *w2, WINDOW *w3)
{
    //Función para dibujar las ventanas
    box(w1,0,0); 
    wrefresh(w1);
    box(w2,0,0); 
    wrefresh(w2);
    box(w3,0,0); 
    wrefresh(w3);
    return;
}

void refreshWindow1(WINDOW *w1,char listProcess[][15],char listStates[][15],int p,int s,int distanceW1)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //p -> variable de index process
    //s -> variable de index status
    char itemobjects[30];
    sprintf(itemobjects,"%s  %s",listProcess[p],listStates[s]);
    mvwprintw(w1,p+3,distanceW1,"%s",itemobjects);
}

void refreshWindow2(WINDOW *w1,char listAdvancedControls[][25],int c,int distanceW1)
{
    //Función para dibujar la primera ventana que cambia frecuentemente
    //c -> variable de index advanced controls
    mvwprintw(w1,c+3,distanceW1,"%s",listAdvancedControls[c]);
}

ActivateWindow activateWindows1(WINDOW *w1,char listProcess[][15],char listStates[][15],char listAdvancedControls[][25],int p,int s,int c,int com1,int distanceW1,int viewDetails)
{
    //Función donde la primera ventana se activa
    int maxItemsProcess=sizeof(listProcess)/sizeof(listProcess[0]);
    int maxAdvancedControls=sizeof(listProcess)/sizeof(listProcess[0]);
    //Margen derecho para la caja de elementos
    refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
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
                c = (c<0) ? maxItemsProcess-1 : c;
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
    wattron(w1,A_STANDOUT);
    if(viewDetails==0)
    {
        refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
    }
    else
    {
        refreshWindow2(w1,listAdvancedControls,c,distanceW1);
    }
    wattroff(w1,A_STANDOUT);
    ActivateWindow Res;
    Res.counter=p;
    Res.viewDetails=viewDetails;
    return Res;
}

void main() 
{     
    // se inicializa Ncurses
    initscr();
    crmode();
    // cogemos tamaño ventana 
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int lines = w.ws_row;
    int columns = w.ws_col;
    /* Inicializaciones variables */
    WINDOW *w1;
    WINDOW *w2;
    WINDOW *w3;
    // nuevas ventanas (Lineas, Columnas, posición Y, posición X)
    w1 = newwin(26,35,1,1); 
    w2 = newwin(5,155,27,1);
    w3 = newwin(26,120,1,36);
    char listProcess[][15] = {"DialServer ","DialReport ","DialContact","Asterisk   ","ipTables   "};
    char listStates[][15] = {"            OK","           RIP","   Starting...","   Stopping...","           ???"};
    char listGeneralControls[][25] = {"F1-MainLog  ","F2-WebServiceLog   ","F3-SpreadLog"};
    char listAdvancedControls[][25] = {"start  ","stop   ","restart","reload "};
    int distanceW1=2;
    int distanceW2=25;
    int viewDetails=0;
    char escapeApp='q'; 
    int com1;
    int p=0,s=0,c=0,activeWindow=1;
    float timeToRefresh=1;
    ActivateWindow Res;

    /* Init App */
    refreshWindows(w1,w2,w3);
    
    mvwprintw(w1,1,distanceW1,"%s","PROCESOS             ESTADO");
    // imprimo listProcess - se seleciona el primer elemento
    int maxItemsProcess = sizeof(listProcess)/sizeof(listProcess[0]);
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
        refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
    }

    int maxItemsControls = sizeof(listGeneralControls)/sizeof(listGeneralControls[0]);
    for(c=0;c<maxItemsControls;c++) 
    {
        mvwprintw(w2,2,(distanceW2*c)+2,"%s",listGeneralControls[c]);
    }

    // se refresca las consolas
    refreshWindows(w1,w2,w3);
 
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
            Res=activateWindows1(w1,listProcess,listStates,listAdvancedControls,p,s,c,com1,distanceW1,viewDetails);
            p=Res.counter;
            viewDetails=Res.viewDetails;
        }
    }
    //Cierro las ventanas
    delwin( w1 );
    endwin();
}
