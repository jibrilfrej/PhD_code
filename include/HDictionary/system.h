/**
 \file system.h
 \brief System access for various informations.
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \date 2012
 \note IOTA Information Retrieval System
 \details Utilisé pour connaitre la taille de la mémoire physique pour
 réduire la taille maximum des allocations, et pour le nombre
 de processeurs pour adapater la création des threads.
 \ingroup System
  */

#ifndef system_h
#define system_h
#include <sys/types.h>
#ifdef __ANDROID__
	#include <linux/sysctl.h>
#else
	#include <sys/sysctl.h>
#endif
#include <unistd.h>
#include <inttypes.h>

namespace SYSTEM {
	
    
    
    typedef uint64_t U64; ///< Unsigned 64 bits integer
    typedef int64_t  I64; ///< Signed 64 bits integer
    typedef uint32_t U32; ///< Unsigned 32 bits integer
    typedef int32_t  I32; ///< Signed 32 bits integer
    typedef uint16_t U16; ///< Unsigned 16 bits integer
    typedef int16_t  I16; ///< Signed 16 bits integer
    typedef uint8_t  U8;  ///< Unsigned 8 bits integer
    typedef int8_t   I8;  ///< Signed 8 bits integer
    
    /// Return the number of available CPU in the machine that runs this code.
    /** \details Used for threading */
    unsigned NCPU();
    
    /// Return the memory size available in the machine runining this code.
	unsigned long MEMSIZE();
    
    
#ifdef __APPLE__
	inline
	unsigned NCPU() {
		int name[2];
		size_t len;
		unsigned data;
		name[0] = CTL_HW;
		name[1] = HW_NCPU;
		len = sizeof(data);
		sysctl(name, 2, &data, &len, NULL, 0);
		return data;
	}
#else
	inline
	unsigned NCPU() {
		return sysconf(_SC_NPROCESSORS_ONLN);
	}
#endif
    
#ifdef __APPLE__
	//NE PAS UTILISER sysctl sous linux
	inline
	unsigned long MEMSIZE() {
		int name[2];
		size_t len;
		unsigned long data;
		name[0] = CTL_HW;
		name[1] = HW_MEMSIZE;
		len = sizeof(data);
		sysctl(name, 2, &data, &len, NULL, 0);
		return data;
	}
#else
	inline
	unsigned long MEMSIZE() {
		unsigned long pages = sysconf(_SC_PHYS_PAGES);
		unsigned long page_size = sysconf(_SC_PAGE_SIZE);
		return pages * page_size;
	}
#endif
	
}

#endif
