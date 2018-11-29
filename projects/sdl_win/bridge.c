
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ShellApi.h>
#include "../../sources/bridge.h"

void bridge_init( void )
{


}

void bridge_kill( void )
{


}

void bridge_buy( char* item )
{

    bridge_open( "https://paypal.me/milgra" );

}

void bridge_open( char* url )
{

    ShellExecute(0,"open",url,NULL,NULL,1);

}
