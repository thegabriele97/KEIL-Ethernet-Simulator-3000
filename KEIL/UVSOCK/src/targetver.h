
#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                  // Specifies that the minimum required platform is Windows XP.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE               // Specifies that the minimum required platform is Internet Explorer 6.0.
#define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
#endif

/* UV_VERSION **MUST** be defined correctly for the module!
   eg UV_VERSION=3 or UV_VERSION=4

   Failing to do so WILL result in very horrible bugs where the same variable/object is declared
   differently in different DLLs or EXEs. This will cause crashes and errors that are very difficult
   to find... You have been warned!!!!

   NOTE: Now using the compiler version to define UV_VERSION
 */

// Undefine UV_VERSION if it was defined

/* Redefine based on compiler version */

// Ignore UV_VERSION for compiling the resources
#ifdef _MSC_VER

// UV3 compiler is version 14.00.xxx
#if (_MSC_VER == 1400)
#ifdef UV_VERSION
#undef UV_VERSION
#endif
#define UV_VERSION  3
#pragma message("Building for UV Version 3...")
#endif

// UV4 compiler is version 15.00.xxx
#if (_MSC_VER == 1500)
#ifdef UV_VERSION
#undef UV_VERSION
#endif
#define UV_VERSION  4
#pragma message("Building for UV Version 4...")
#endif

// UV4 compiler is version 15.00.xxx
#if (_MSC_VER > 1500)
// is version defined?
#ifndef UV_VERSION
#define UV_VERSION  4
#endif
// was defined and less than 4
#if UV_VERSION < 4
#undef UV_VERSION
#define UV_VERSION  4
#endif
// printf info
#if UV_VERSION == 4
#pragma message("Building for UV Version 4...")
#elif UV_VERSION == 5
#pragma message("Building for UV Version 5...")
#endif
#endif

#ifndef UV_VERSION
#error "You are attempting to compile uVision with an incorrect toolchain!"
#endif

#endif // #ifdef _MSC_VER

