//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_UTIL_H
#define MPI_VI_UTIL_H

// verbosity setting
#define VERBOSE_INFO
//#define VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#ifndef VERBOSE_INFO
#define VERBOSE_INFO
#endif
#endif

#endif //MPI_VI_UTIL_H
