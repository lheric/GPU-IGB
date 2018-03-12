//
// dbgnew.h
//
// If you put an include sequence like this
//
// #include <stdlib.h>
// #include <crtdbg.h>
// #include "..\Common\dbgnew.h"
//
// in a C++ file, and do a DEBUG build, any memory leaks will be displayed when
// the app exits. This is how you create leak-free code in MS VS C++.
//
#ifdef _DEBUG  
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)  
#else  
#define DEBUG_CLIENTBLOCK  
#endif // _DEBUG  

#ifdef _DEBUG  
#define new DEBUG_CLIENTBLOCK  
#endif  