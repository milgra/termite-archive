
    #ifndef grid2d_h
    #define grid2d_h

    #include <stdio.h>
    #include "../framework/core/mtvec.c"
    #include "../framework/core/mtmap.c"
    #include "../framework/tools/math2.c"

    typedef struct
    {
        int col;
        int row;

        float g;
        float f;
        float h;

        char closed;
        char opened;
        char wall;

        void* parent;

    } grid2d_node_t;

    void grid2d_find( mtvec_t* nodes , mtvec_t* result , int cols , int rows , int start , int end );
    char grid2d_has_wall_inbetween( mtvec_t* nodes , int x0 , int y0 , int x1 , int y1 , int cols , int rows );


    #endif

    #if __INCLUDE_LEVEL__ == 0



    char grid2d_has_wall_inbetween( mtvec_t* nodes , int x0 , int y0 , int x1 , int y1 , int cols , int rows )
    {
    
        // http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
    
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        
        int x = x0;
        int y = y0;
        
        int n = 1 + dx + dy;
        
        int x_inc = (x1 > x0) ? 1 : -1;
        int y_inc = (y1 > y0) ? 1 : -1;
        
        int error = dx - dy;
        
        dx *= 2;
        dy *= 2;

        for (; n > 0; --n)
        {
            int index = y * cols + x;
            grid2d_node_t* block = nodes->data[ index ];
            
            if ( block->wall == 1 ) return 1;

            if (error > 0)
            {
                x += x_inc;
                error -= dy;
            }
            else
            {
                y += y_inc;
                error += dx;
            }
        }
        
        return 0;
    
    }


    // adds node to openlist keeping f order

    void grid2d_insertnode( mtvec_t* openList , grid2d_node_t* node )
    {

        char added = 0;
        for ( int i = 0 ; i < openList->length ; i++ )
        {
            grid2d_node_t* actual = openList->data[ i ];
            if ( node->f > actual->f )
            {
                mtvec_addatindex( openList , node , i );
                added = 1;
                break;
            }
        }
        if ( added == 0 )
        {
            mtvec_add( openList , node );
        }

    }

    /* finds path between start and end */

    void grid2d_find( mtvec_t* nodes , mtvec_t* result , int cols , int rows , int start , int end )
    {
        if ( start == end ) return;
        
        grid2d_node_t* startNode = nodes->data[ start ];
        grid2d_node_t* endNode = nodes->data[ end ];

        if ( endNode->wall == 1 )
        {
            return;
        }

        mtvec_t* openList = result;

        float weight = 1.0; // speed up heuristics

        startNode->g = 0.0;
        startNode->f = 0.0;

        mtvec_add( openList , startNode );

        startNode->opened = 1;

        while ( openList->length > 0 )
        {

            // pop the position of node which has the minimum `f` value.
            grid2d_node_t* node = mtvec_tail( openList );
            mtvec_removeatindex( openList , openList->length - 1 );
            node->closed = 1;
            
            // printf( "checking %i %i\n" ,node->col ,node->row  );

            // if reached the end position, construct the path and return it
            if (node == endNode )
            {
                if ( openList->length > 0 )
                {
            
                    int border = openList->length;
                    grid2d_node_t* tempNode = endNode;

                    while ( tempNode != NULL )
                    {
                        // printf( "adding node %i %i\n" , tempNode->row , tempNode->col );
                        mtvec_add( openList , tempNode );
                        tempNode = tempNode->parent;
                    }
                    // clear temporary values
                    mtvec_removeinrange( openList , 0 , border - 1 );
                    mtvec_reverse( openList );
                    
                }

                return;
            }

            // get neigbours of the current node
            mtvec_t* neighbours = mtvec_alloc( );

            int northindex  = ( ( node->row - 1     ) * cols ) + node->col;
            int southindex  = ( ( node->row + 1     ) * cols ) + node->col;
            int eastindex   = ( ( node->row         ) * cols ) + node->col - 1;
            int westindex   = ( ( node->row         ) * cols ) + node->col + 1;
            
            grid2d_node_t* enode = NULL;
            grid2d_node_t* wnode = NULL;
            grid2d_node_t* nnode = NULL;
            grid2d_node_t* snode = NULL;

            if ( node->col > 0        ) enode = nodes->data[ eastindex  ];
            if ( node->col < cols - 1 ) wnode = nodes->data[ westindex  ];
            if ( node->row > 0        ) nnode = nodes->data[ northindex ];
            if ( node->row < rows - 1 ) snode = nodes->data[ southindex  ];

            if ( enode != NULL ) mtvec_add( neighbours , enode );
            if ( wnode != NULL ) mtvec_add( neighbours , wnode );
            if ( nnode != NULL ) mtvec_add( neighbours , nnode );
            if ( snode != NULL ) mtvec_add( neighbours , snode );

            int neindex = ( ( node->row - 1 ) * cols ) + node->col - 1;
            int nwindex = ( ( node->row - 1 ) * cols ) + node->col + 1;
            int seindex = ( ( node->row + 1 ) * cols ) + node->col - 1;
            int swindex = ( ( node->row + 1 ) * cols ) + node->col + 1;

            if ( node->col > 0 && node->row > 0 && enode != NULL && nnode != NULL && enode->wall == 0 && nnode->wall == 0 ) mtvec_add( neighbours , nodes->data[ neindex ] );
            if ( node->col < cols - 1 && node->row > 0 && wnode != NULL && nnode != NULL && wnode->wall == 0 && nnode->wall == 0 ) mtvec_add( neighbours , nodes->data[ nwindex ] );
            if ( node->col > 0 && node->row < rows - 1 && enode != NULL && snode != NULL && enode->wall == 0 && snode->wall == 0 ) mtvec_add( neighbours , nodes->data[ seindex ] );
            if ( node->col < cols - 1 && node->row < rows - 1 && wnode !=NULL && snode != NULL && wnode->wall == 0 && snode->wall == 0 ) mtvec_add( neighbours , nodes->data[ swindex ] );

            for ( int index = 0 ; index < neighbours->length ; ++index )
            {
                grid2d_node_t* neighbor = neighbours->data[ index ];

                // printf( "neighbor %i-%i g : %f\n" , neighbor->row , neighbor->col , neighbor->g );

                if ( neighbor->closed == 1 ) {
                    continue;
                }

                // get the distance between current node and the neighbor
                // and calculate the next g score
                float ng = node->g + ( ( neighbor->col == node->col || neighbor->row == node->row ) ? 1 : sqrtf( 2.0 ) );

                // check if the neighbor has not been inspected yet, or
                // can be reached with smaller cost from the current node
                if ( neighbor->opened == 0 || ng < neighbor->g )
                {
                    int diffX = abs( neighbor->col - endNode->col );
                    int diffY = abs( neighbor->row - endNode->row );
                    int heuristic = diffX < diffY ? diffY : diffX;  // chebysev

                    if ( weight * heuristic > neighbor->h ) neighbor->h = weight * heuristic;
                    neighbor->g = ng;
                    neighbor->f = neighbor->g + neighbor->h;
                    neighbor->parent = node;

                    if ( neighbor->opened == 0 )
                    {
                        neighbor->opened = 1;
                        grid2d_insertnode( openList , neighbor );
                    }
                    else
                    {
                        // the neighbor can be reached with smaller cost.
                        // Since its f value has been updated, we have to
                        // update its position in the open list
                        mtvec_remove( openList, neighbor );
                        grid2d_insertnode( openList , neighbor );
                    }
                }
            } // end for each neighbor

            mtmem_release( neighbours );
        } // end while not open list empty

        // fail to find the path
        return;
    }
    #endif
