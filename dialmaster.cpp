#include<stdlib.h>
#include<ncurses.h>
#include<string>
#include<sys/ioctl.h>
#include<stdio.h>
// Hay que pasarle los array de chars a la variable Arg como punteros luego corregir la estetica pero 
// sobre todo que pueda coger la información de un log

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
    char escapeApp1; 
    char escapeApp2;
    int com1;
    int p;
    int s;
    int c;
    int activeWindow;
    float timeToRefresh;
    int activeBrush;
    int maxWindowsRow;
    int maxWindowsColumns;
} Args;

void clearWindows(Args Arg,int maxWindowsRow,int maxWindowsColumns)
{
    //Para hacer una limpieza de la ventana
    int i;
    for(i=1;i<maxWindowsRow;i++) 
    {
        mvwprintw(Arg.w1,i,Arg.distanceW1,"%s","                               ");
    }
}

void resizeWindows(Args Arg)
{
    //Función para el resize de las ventanas
    struct winsize windowGeneral;
    ioctl(0, TIOCGWINSZ, &windowGeneral);
    int maxWindowsRow=windowGeneral.ws_row;
    int maxWindowsColumns=windowGeneral.ws_col;
    //printf("row: %d",maxWindowsRow);
    //printf("columns: %d",maxWindowsColumns);
    //Extendo la w1 a toda pantalla para acclarar los rastros de las actualizaciones de las ventanas
    Arg.w1 = newwin(maxWindowsRow,maxWindowsColumns,-1,-1); 
    box(Arg.w1,0,0); 
    wrefresh(Arg.w1);
    
    Arg.w1 = newwin(maxWindowsRow-15,35,1,1); 
    Arg.w2 = newwin(5,155,maxWindowsRow-14,1);
    Arg.w3 = newwin(maxWindowsRow-15,120,1,36);
    box(Arg.w1,0,0);
    wrefresh(Arg.w1);
    box(Arg.w2,0,0);
    wrefresh(Arg.w2);
    box(Arg.w3,0,0);
    wrefresh(Arg.w3);
    refresh();
    clearWindows(Arg,maxWindowsRow,maxWindowsColumns);
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

void refreshWindows(Args Arg)
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
    return;
}

Args activateWindows(Args Arg)
{
    //Función donde la primera ventana se activa
    //printf("Tecla: %d",Arg.com1);
    
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
    refreshWindows(Arg);
    return Arg;
}

int main() 
{     
    // se inicializa Ncurses
    initscr();
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
    Arg.w1 = newwin(26,35,1,1); 
    Arg.w2 = newwin(5,155,27,1);
    Arg.w3 = newwin(26,120,1,36);
    Arg.maxItemsProcess=sizeof(Arg.listProcess)/sizeof(Arg.listProcess[0]);
    Arg.maxItemsStates=sizeof(Arg.listStates)/sizeof(Arg.listStates[0]);
    Arg.maxItemsGeneralControls=sizeof(Arg.listGeneralControls)/sizeof(Arg.listGeneralControls[0]);
    Arg.maxItemsAdvancedControls=sizeof(Arg.listAdvancedControls)/sizeof(Arg.listAdvancedControls[0]);
    Arg.distanceW1=2;
    Arg.distanceW2=30;
    Arg.viewDetails=0;
    Arg.escapeApp1='q';
    Arg.escapeApp2='Q'; 
    Arg.com1;
    Arg.p=0;
    Arg.s=0;
    Arg.c=0;
    Arg.activeWindow=1;
    Arg.timeToRefresh=1;
    Arg.activeBrush=1;

    /* Init App */
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
    noecho();

    keypad(Arg.w1,TRUE); // se permite meter numeros
    curs_set(0); // se esconde el cursor normal

    //entrada teclado
    char keyPress=Arg.com1=wgetch(Arg.w1);
    while( (keyPress!=Arg.escapeApp1) && (keyPress!=Arg.escapeApp2) )
    {
        keyPress=Arg.com1=wgetch(Arg.w1);
        resizeWindows(Arg);
        if(Arg.activeWindow==1)
        {
            Arg=activateWindows(Arg);
        }
    }
    //Cierro las ventanas
    delwin(Arg.w1);
    delwin(Arg.w2);
    delwin(Arg.w3);
    endwin();
    return 0;
}
