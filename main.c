#include "ms_string.h"

char string[] = "A string\tof ,,tokens\nand some  more tokens   ";
char seps[]   = " ,\t\n";
char *token;

int main( void )
{
   printf( "Tokens:\n" );

   printf("%d\n", (int)ms_strlen("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"));

   // Establish string and get the first token:
   token = ms_strtok( string, seps ); // C4996
   // Note: strtok is deprecated; consider using strtok_s instead
   int x = 0;
   while( token != NULL && x++ < 100)
   {
      // While there are tokens in "string"
      printf(">%s\n", token);

      // Get next token:
      token = ms_strtok( NULL, seps ); // C4996
   }
}
