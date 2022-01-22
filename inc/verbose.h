//
// Created by tobias on 14.01.22.
//

/**
 * This header is used to control the verbosity behaviour of the project. Supported levels are:
 * VERBOSE_INFO
 * VERBOSE_DEBUG
 *
 * Add a #define with one of the above levels in a file for local verbosity or uncomment the respective line in this header for global verbosity setting.
 */

#ifndef MPI_VI_VERBOSE_H
#define MPI_VI_VERBOSE_H

// verbosity setting
//#define VERBOSE_INFO
//#define VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#ifndef VERBOSE_INFO
#define VERBOSE_INFO
#endif
#endif

#endif//MPI_VI_VERBOSE_H
