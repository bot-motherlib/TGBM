#pragma once

#ifndef TGBM_API
#ifdef TGBM_DLL
#if defined _WIN32 || defined __CYGWIN__
#define TGBM_HELPER_DLL_EXPORT __declspec(dllexport)
#define TGBM_HELPER_DLL_IMPORT __declspec(dllimport)
#else
#if __GNUC__ >= 4
#define TGBM_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define TGBM_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#else
#define TGBM_HELPER_DLL_EXPORT
#define TGBM_HELPER_DLL_IMPORT
#endif
#endif
#ifdef TGBM_EXPORTS
#define TGBM_API TGBM_HELPER_DLL_EXPORT
#else
#define TGBM_API TGBM_HELPER_DLL_IMPORT
#endif
#else
#define TGBM_API
#endif
#endif
