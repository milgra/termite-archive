
    #ifndef particle_h
    #define particle_h

    #include <stdio.h>
    #include "voxel.c"

    typedef struct _particle_t
    {
        v3_t dir;
        char finished;
        voxel_t* model;
        
    } particle_t;

    particle_t* particle_create( voxel_t* voxel , v3_t direction );

    #endif

    #if __INCLUDE_LEVEL__ == 0  // if we are compiled as implementation


    void particle_delete( void* pointer )
    {
        particle_t* particle = pointer;
        
        RPL( particle->model , NULL );
    }


    particle_t* particle_create( 	voxel_t* voxel ,
    								v3_t direction )
    {
        particle_t* particle = mtmem_calloc( sizeof( particle_t ) , particle_delete );

        particle->dir = direction;
        
        RPL( particle->model , voxel );

        return particle;
    }


    #endif
