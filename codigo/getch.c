
// Flechas
#define ARRIBA 521
#define ABAJO 522
#define DERECHA 523
#define IZQUIERDA 524
#define BORRAR 525
#define INICIO 526
#define FIN 527

// Codigos de getch  detecta tecla tomados de
// https://ubuntuforums.org/showthread.php?t=2276177

int getch(void)
{
 int ch;
 struct termios oldt;
 struct termios newt;
 tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
 newt = oldt; /* copy old settings to new settings */
 newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
 tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
 ch = getchar(); /* standard getchar call */
 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
 return ch; /*return received char */
}

int detectaTecla (){
     int x = ' ';
     int y = ' ';
     int z = ' ';

     x = getch();
     
     if (x == 127)
        return BORRAR;

     if (x == 27)
     {
      y = getch();
      z = getch();
     }

     if (x == 27 && y == 91)
     {
      switch (z)
      {
        //A
       case 65:
           return ARRIBA;
           break;
       //B
       case 66:
           return ABAJO;
           break;
       //C
       case 67:
           return DERECHA;
           break;
       //D
       case 68:
        return IZQUIERDA;
        break;
        
      case 72:
        return INICIO;
        break;
      
      case 70:
        return FIN;
        break;

      case 127:
        return BORRAR;
        break;

       default:
        return x;

      }
    }

}
