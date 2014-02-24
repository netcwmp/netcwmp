 
AM_CPPFLAGS = -DUSE_CWMP_MEMORY_POOL -I./  -I$(top_srcdir)/libcwmp/src/inc  -I$(top_srcdir)/libpool/include  -I$(top_srcdir)/libxmlet/include   -I$(top_srcdir)/libcwmp/include 




bin_PROGRAMS = testall

testall_SOURCES = abts.c testutil.c test_model.c test_rpc.c test_pool.c




testall_LDADD =  -L$(top_srcdir)/libpool/src -L$(top_srcdir)/libxmlet/src -L$(top_srcdir)/libcwmp/src -lpool -lxmlet  -lcwmp  -lpthread 


testall_CPPFLAGS = $(AM_CPPFLAGS) 



