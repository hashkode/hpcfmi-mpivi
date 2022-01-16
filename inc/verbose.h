//
// Created by tobias on 14.01.22.
//

#ifndef MPI_VI_VERBOSE_H
#define MPI_VI_VERBOSE_H

// verbosity setting
#define VERBOSE_INFO
//#define VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#ifndef VERBOSE_INFO
#define VERBOSE_INFO
#endif
#endif

#endif//MPI_VI_VERBOSE_H
