#include<ncurses.h>

typedef struct 
{
    int counter;
    int activeWindow;
}activateWindow;

void refreshWindows(WINDOW *w1,WINDOW *w2)
{
    //Bordes
    box(w1,0,0); 
    box(w2,0,0);
    wrefresh(w1);
    wrefresh(w2);
    return;
}

void refreshWindow1(WINDOW *w1,char listProcess[][15],char listStates[][15],int p,int s,int distanceW1)
{
    //p -> variable de index process
    //s -> variable de index status
    char itemobjects[30];
    sprintf(itemobjects,"%s - %s",listProcess[p],listStates[s]);
    mvwprintw(w1,p+1,distanceW1,"%s",itemobjects);
}

void refreshWindow2(WINDOW *w2,char listControls[][15],int c,int distanceW2)
{
    //c -> variable de index control
    char itemobjects[30];
    sprintf(itemobjects,"%s",listControls[c]);
    mvwprintw(w2,c+1,distanceW2,"%s",itemobjects);
}

activateWindow activateWindows1(WINDOW *w1,char listProcess[][15],char listStates[][15],int p,int s,int com1,int maxItemsProcess,int distanceW1,int activeWindow)
{
    // margen derecho para la caja de elementos
    refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
    // si flecha arriba se sube en la listobjectsa, si flecha abajo se baja en la listobjectsa
    switch(com1) 
    {
        case KEY_UP:
        {
            p--;
            p = (p<0) ? maxItemsProcess-1 : p;
            break;
        }
        case KEY_DOWN:
        {
            p++;
            p = (p>=maxItemsProcess) ? 0 : p;
            break;
        }
        case KEY_LEFT:
        {
            activeWindow=2;
        }
    }
    // pinto el elemento seleccionado
    wattron(w1,A_STANDOUT);
    refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
    wattroff(w1,A_STANDOUT);
    activateWindow Res;
    Res.counter=p;
    Res.activeWindow=activeWindow;
    return Res;
}

activateWindow activateWindows2(WINDOW *w2,char listControls[][15],int c,int com2,int maxItemsControls,int distanceW2,int activeWindow)
{
    // margen derecho para la caja de elementos
    refreshWindow2(w2,listControls,c,distanceW2);
    // si flecha arriba se sube en la listobjectsa, si flecha abajo se baja en la listobjectsa
    switch(com2) 
    {
        case KEY_UP:
        {
            c--;
            c = (c<0) ? maxItemsControls-1 : c;
            break;
        }
        case KEY_DOWN:
        {
            c++;
            c = (c>=maxItemsControls) ? 0 : c;
            break;
        }
        case KEY_LEFT:
        {
            activeWindow=1;
        }
    }
    // pinto el elemento seleccionado
    wattron(w2,A_STANDOUT);
    refreshWindow2(w2,listControls,c,distanceW2);
    wattroff(w2,A_STANDOUT);
    activateWindow Res;
    Res.counter=c;
    Res.activeWindow=activeWindow;
    return Res;
}

void main() 
{     
    // se inicializa Ncurses
    initscr();
    crmode();
    noecho();
    /* Inicializaciones variables */
    WINDOW *w1;
    WINDOW *w2;
    w1 = newwin(10,30,1,1); // nueva ventana (Lineas, Columnas, posición Y, posición X)
    w2 = newwin(15,25,1,30); // nueva ventana (Lineas, Columnas, posición Y, posición X)
    int maxItemsProcess=5;
    int maxItemsControls=4;
    char listProcess[][15] = { "Dialserver", "Dialreport", "Dialcontact", "Asterisk", "ipTables" };
    char listStates[][15] = { "OK", "Died", "Starting...", "Stopping...", "???"};
    char listControls[][15] = { "Start", "Stop", "Restart", "Reload"};
    int distanceW1=2;
    int distanceW2=10;
    char escapeApp='q'; 
    int com1,com2,p=0,s=2,c=1,activeWindow=1;
    activateWindow Res;

    /* Init App */
    refreshWindows(w1,w2);
    
    // imprimo listProcess - se seleciona el primer elemento
    for(p=0;p<maxItemsProcess;p++) 
    {
        if(p==0) 
        {
            s=3;
            wattron(w1,A_STANDOUT); // se enciende el primer itemobjects
        }
        else
        {
            s=1;
            wattroff(w1,A_STANDOUT); // se apaga el primer itemobjects
        }
        refreshWindow1(w1,listProcess,listStates,p,s,distanceW1);
    }
    
    for(c=0;c<maxItemsControls;c++) 
    {
        if(c==0) 
        {
            wattron(w2,A_STANDOUT); // se enciende el primer itemobjects
        }
        else
        {
            wattroff(w2,A_STANDOUT); // se apaga el primer itemobjects
        }
        refreshWindow2(w2,listControls,c,distanceW2);
    }

    // se refresca las consolas
    wrefresh(w1); 
    wrefresh(w2); 
 
    p=0;
    c=0;
    noecho();

    keypad(w1,TRUE); // se permite meter numeros
    keypad(w2,TRUE); // se permite meter numeros
    curs_set(0); // se esconde el cursor normal

    //entrada teclado
    while((com1=wgetch(w1))!=escapeApp)
    { 
        com2=wgetch(w2);
        if(activeWindow==1)
        {
            Res=activateWindows1(w1,listProcess,listStates,p,s,com1,maxItemsProcess,distanceW1,activeWindow);
            p=Res.counter;
            activeWindow=Res.activeWindow;
            //printf("\nMess:1 - Counter: %d - ActiveWindow: %d",p,activeWindow);
        }
        else if(activeWindow==2)
        {
            Res=activateWindows2(w2,listControls,c,com2,maxItemsControls,distanceW2,activeWindow);
            c=Res.counter;
            activeWindow=Res.activeWindow;
            //printf("\nMess:2 - Counter: %d - ActiveWindow: %d",c,activeWindow);
        }
    }
    delwin( w1 );
    delwin( w2 );
    endwin();
}