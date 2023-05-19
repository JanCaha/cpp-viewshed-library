#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "cellevent.h"

using viewshed::CellEvent;

TEST( CellEventTest, size )
{
    int size;
#if ( CELL_EVENT_USE_FLOAT )
    size = 36;
#else
    size = 64;
#endif
    ASSERT_EQ( sizeof( CellEvent ), size );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}