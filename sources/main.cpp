
    #ifdef __cplusplus

    // Steam

    #ifdef STEAM

    #include "../projects/steam/CInventory.hpp"

    #endif

    extern "C"
    {
    #endif

    // OpenGL

    #if defined WINDOWS

    #define GLEW_STATIC
    #include "GL/glew.h"
    #include <windows.h>
    #include <ShellApi.h>

    #elif defined(IOS)

    #include "../projects/sdl_ios/Bridge.h"
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>

    #elif defined(ANDROID)

    #include "../projects/sdl_droid/app/jni/src/bridge.h"
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    #endif

    #include "SDL.h"

    #include <time.h>
    #include <stdio.h>

    #include "ui.c"
    #include "scene.c"
    #include "renderer.c"
    #include "defaults.c"
    #include "../framework/core/mtbus.c"
    #include "../framework/core/mtcstr.c"
    #include "../framework/tools/settings.c"

    float scale = 1.0;

    int32_t width = 1000;
    int32_t height = 750;

    SDL_Window* window;
    SDL_GLContext context;

	char quit = 0;


	void main_openurl( const char* url )
	{

        #if defined(IOS)

        bridge_open( ( char* ) url );

        #elif defined(ANDROID)

        bridge_open( ( char* ) url );

        #elif defined(OSX)

        char newurl[100];
        snprintf( newurl , 100 , "open %s", url );
        system( newurl );

        #elif defined(WINDOWS)

        ShellExecute(0,"open",url,NULL,NULL,1);

        #elif defined(RASPBERRY) || defined(UBUNTU)

        char newurl[100];
        snprintf( newurl , 100 , "xdg-open %s", url );
        system( newurl );

        #endif

	}


    void main_donate( char* item )
    {

        #if defined(RASPBERRY)
        main_openurl( "https://paypal.me/milgra" );
        #elif defined(IOS)
        bridge_buy( item );
        #elif defined(ANDROID)
        bridge_buy( item );
        #elif defined(STEAM)
        steam_buy( item );
        #endif

    }


    void main_onmessage( const char* name , void* data )
    {

        if ( strcmp( name , "DONATE") == 0 )
        {
            main_donate( ( char* ) data );
        }
        else if ( strcmp( name , "FEEDBACK") == 0 )
        {
            main_openurl( "http://www.milgra.com/termite.html" );
        }
        else if ( strcmp( name , "HOMEPAGE") == 0 )
        {
            main_openurl( "http://www.milgra.com" );
        }
        else if ( strcmp( name , "FULLSCREEN") == 0 )
        {
            int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;

            char fullscreen = SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN_DESKTOP;

            if ( fullscreen == 1 ) SDL_SetWindowFullscreen( window , flags );
            else SDL_SetWindowFullscreen( window , flags | SDL_WINDOW_FULLSCREEN_DESKTOP );
        }
        else if ( strcmp( name , "RESET") == 0 )
        {
            settings_reset( );
        }
        else if ( strcmp( name , "EXIT") == 0 )
        {
			quit = 1;
        }

    }


    void main_init( )
    {

        srand( ( unsigned int ) time( NULL ) );

        char* basepath = SDL_GetPrefPath( "milgra" , "termite" );

        settings_init( basepath , (char* ) "termite.state" );
        defaults_init( );

        mtbus_init( );
        mtbus_subscribe( "UI" , main_onmessage );

		v2_t dimensions = {.x = width * scale , .y= height * scale };
		defaults.display_size = dimensions;

        #if defined(IOS)
        bridge_init( ); // request donation prices from app store, init text scaling
        #elif defined(ANDROID)
        bridge_init( ); // request donation prices from app store, init text scaling
        #elif defined(STEAM)
        steam_init();   // request donation inventory items/prices from steam store
        #endif

        int framebuffer = 0;
        int renderbuffer = 0;

        glGetIntegerv( GL_FRAMEBUFFER_BINDING , &framebuffer );
        glGetIntegerv( GL_RENDERBUFFER_BINDING , &renderbuffer );

        renderer_init( framebuffer , renderbuffer , basepath  );
        scene_init( );
        ui_init( );

        mtbus_notify( "CTL", "RESIZE" , &dimensions );

        int level = 0;
        mtbus_notify( "CTL", "LOAD" , &level );

        SDL_free( basepath );

        #ifdef OSX
        main_onmessage( ( char* ) "FULLSCREEN" , NULL );  // workaround for SDL2 bug
        #endif

    }


    void main_free( )
    {

        scene_free( );
        renderer_free( );
        ui_free( );

        settings_free( );
        mtbus_free();

        #ifdef STEAM
        SteamAPI_Shutdown();
        #endif
    }


    void main_loop( )
    {
        char drag = 0;
        SDL_Event event;

        while( !quit )
        {

            while( SDL_PollEvent( &event ) != 0 )
            {

                if( event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION  )
                {

                    int x = 0, y = 0;
                    SDL_GetMouseState( &x, &y );

                    v2_t dimensions = {.x = x * scale , .y= y * scale };

                    if ( event.type == SDL_MOUSEBUTTONDOWN )
                    {
                        drag = 1;
                        mtbus_notify( "CTL", "TOUCHDOWN" , &dimensions );
                    }
                    else if ( event.type == SDL_MOUSEBUTTONUP )
                    {
                        drag = 0;
                        mtbus_notify( "CTL", "TOUCHUP" , &dimensions );
                    }
                    else if ( event.type == SDL_MOUSEMOTION && drag == 1 )
                    {
                        mtbus_notify( "CTL", "TOUCHMOVE" , &dimensions );
                    }

                }
//                else if( event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP || event.type == SDL_FINGERMOTION  )
//                {
//                    int x = event.tfinger.x;
//                    int y = event.tfinger.y;
//
//                    input_t input = { 0 };
//                    input.stringa = "mouse";
//                    input.floata = x * 2;
//                    input.floatb = y * 2;
//
//                    if ( event.type == SDL_FINGERDOWN ) { input.type = kInputTypeTouchDown; drag = 1; }
//                    if ( event.type == SDL_FINGERUP ) { input.type = kInputTypeTouchUp; drag = 0; }
//                    if ( event.type == SDL_FINGERMOTION && drag == 1 ) input.type = kInputTypeTouchDrag;
//
//                    if ( input.type > 0 ) controller_input( controller , &input );
//
//                }
                else if ( event.type == SDL_QUIT )
                {
                    quit = 1;
                }
                else if ( event.type == SDL_WINDOWEVENT )
                {

                    if ( event.window.event == SDL_WINDOWEVENT_RESIZED )
                    {

                        width = event.window.data1;
                        height = event.window.data2;

                        v2_t dimensions = {.x = width * scale , .y= height * scale };
                        defaults.display_size = dimensions;

                        mtbus_notify( "CTL", "RESIZE" , &dimensions );

                    }

                }
                else if ( event.type == SDL_KEYUP )
                {
                    switch( event.key.keysym.sym )
                    {
                        case SDLK_f :
                            main_onmessage( ( char* ) "FULLSCREEN", NULL );
                            break;

                        case SDLK_ESCAPE :
                            main_onmessage( ( char* ) "FULLSCREEN", NULL );
                            break;
                    }
                }
                else if ( event.type == SDL_APP_WILLENTERFOREGROUND )
                {

                    mtbus_notify( "CTL", "RESETTIME" , NULL );    // reset scene timer to avoid giant step

                }

            }



            // update simulation

            uint32_t ticks = SDL_GetTicks( );

            mtbus_notify( "CTL", "UPDATE" , &ticks );
            mtbus_notify( "CTL", "RENDER" , &ticks );

            SDL_GL_SwapWindow( window );

            #ifdef STEAM
            SteamAPI_RunCallbacks();
            #endif
        }

    }


    #if defined WINDOWS
    char initGlew( )
    {

        GLint GlewInitResult = glewInit( );

        if ( GLEW_OK != GlewInitResult )
        {

            const GLubyte* error = glewGetErrorString( GlewInitResult );
            printf("ERROR: %s\n",error);
            return 1;

        }

        return 0;

    }
    #endif


    int main ( int argc ,
               char * argv [ ] )
    {

        #ifdef RASPBERRY
        printf( "Please use the KMS OpenGL driver. sudo raspi-config -> Advanced options -> GL Driver -> G1 GL (Full KMS)\n" );
        #elif defined( WINDOWS )
        printf( "OpenGL 2.1 is needed. Please update your drivers if the game is not running.\n" );
        #endif

        // enable high dpi

        SDL_SetHint( SDL_HINT_VIDEO_HIGHDPI_DISABLED , "0" );

        // init sdl

        if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) == 0 )
        {

            // setup opengl version

            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION , 2 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION , 1 );

            SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER , 1 );
            SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE , 24 );

            // window size should be full screen on phones, scaled down on desktops

            SDL_DisplayMode displaymode;
            SDL_GetCurrentDisplayMode( 0 , &displaymode );

            if ( displaymode.w < 800 || displaymode.h < 400 )
            {
                width = displaymode.w;
                height = displaymode.h;
            }
            else
            {
                width = displaymode.w * 0.8;
                height = displaymode.h * 0.8;
            }

            #if defined(IOS) && defined(ANDROID)
            width = displaymode.w;
            height = displaymode.h;
            #endif

            // create window

            window = SDL_CreateWindow( "Termite",
                                        SDL_WINDOWPOS_UNDEFINED ,
                                        SDL_WINDOWPOS_UNDEFINED ,
                                        width ,
                                        height ,
                                        SDL_WINDOW_OPENGL |
                                        SDL_WINDOW_SHOWN |
                                        #if defined(IOS) || defined(ANDROID)
                                        SDL_WINDOW_ALLOW_HIGHDPI |
                                        SDL_WINDOW_FULLSCREEN_DESKTOP );
                                        #else
                                        SDL_WINDOW_RESIZABLE );
                                        #endif

            if ( window != NULL )
            {

                // create context

                context = SDL_GL_CreateContext( window );

                if( context != NULL )
                {
                    // calculate scaling

                    int nw;
                    int nh;

                    SDL_GL_GetDrawableSize( window , &nw , &nh );

                    scale = nw / width;

                    // try to set up vsync

                    if ( SDL_GL_SetSwapInterval( 1 ) < 0 ) printf( "SDL swap interval error %s\n" , SDL_GetError( ) );

                    #if defined WINDOWS
                    if ( initGlew( ) == 0 ) {
                    #endif

                    // SDL_StartTextInput( );

                    main_init( );
                    main_loop( );
                    main_free( );

                    // SDL_StopTextInput();

                    #if defined WINDOWS
                    } else printf( "GLEW init error\n" );
                    #endif

                    // cleanup

                    SDL_GL_DeleteContext( context );

                }
                else printf( "SDL context creation error %s\n" , SDL_GetError( ) );

                // cleanup

                SDL_DestroyWindow( window );
            }
            else printf( "SDL window creation error %s\n" , SDL_GetError( ) );

            // cleanup

            SDL_Quit();
        }
        else printf( "SDL init error %s\n" , SDL_GetError( ) );

        return 0;
    }

    #ifdef __cplusplus
    }
    #endif
