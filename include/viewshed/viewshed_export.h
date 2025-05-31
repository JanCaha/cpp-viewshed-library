#if defined( _WIN32 ) || defined( __CYGWIN__ )
#ifdef VIEWSHED_EXPORTS
#define DLL_API __declspec( dllexport )
#else
#define DLL_API __declspec( dllimport )
#endif
#else
#define DLL_API
#endif