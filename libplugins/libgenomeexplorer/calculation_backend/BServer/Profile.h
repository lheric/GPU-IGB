//
// Profile.h
//
#pragma once

//#define TRACE


#ifdef TRACE
#define DLABEL(msg) { cout << msg << endl; }
#define DTRACE(pred, msg) { if (pred) cout << "  Success " << (msg) << endl<< endl; \
							else cout << "  Failure " << (msg) << endl<< endl; }
#define DPRINT2(msg1, msg2) { cout << msg1 << msg2 << endl; }
#else
#define DLABEL(msg)
#define DTRACE(pred, msg)
#define DPRINT2(msg1, msg2)
#endif

