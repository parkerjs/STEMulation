#ifndef EXPORT_LIBRARY_H
#define EXPORT_LIBRARY_H

namespace library
{

#ifdef STEMULATION_SHARED_LIBRARY
#ifdef EXPORT_STEMULATION_DLL
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define EXPORT_STEM __attribute__ ((visibility("default")))
#elif defined _WIN32
#define EXPORT_STEM __declspec(dllexport)
#else
#define EXPORT_STEM
#endif
#else
#ifdef _WIN32
#define EXPORT_STEM __declspec(dllimport)
#else
#define EXPORT_STEM
#endif
#endif
#else // static library
#define EXPORT_STEM
#endif

}

#endif
