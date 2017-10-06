/** 
 \file Timer.h
 \brief Timer for testing program speed
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \date 1998
 \note IOTA Information Retrieval System
 */

#ifndef Timer_h
#define Timer_h
//***************************************************************************
// 23/12/98 JPC Supression de la defintion de getrusage pour linux
// 23/01/99 JPC Utilisation de RUSAGE_SELF dans le cas de linux
// 23/05/99 JPC correction de split() : met à jour rusageSplit (et pas rusageStart)
// 23/06/99 JPC ajout de l'operateur d'affichage <<(ostream&,const Timer&)
// 25/06/99 JPC Ajout de la notion de flux
// 11/07/99 JPC Ajout de inline aux operateurs de sortie
// 11/08/11 JPC Simplification : usage des macro et suppression methodes inutiles
// 25/03/12 JPC Changement de fonctionnement : reset, split, watch
//***************************************************************************



#include <sys/time.h> // Pour la compatibilité (non utile ?)
#include <sys/resource.h>
#include <iostream>
#include <sstream>
#include <string>

namespace IOTA {
	
    /// A class to measure time performances
    //NB: ne pas oublier de faire au moins un split() avant d'afficher le temps
	class Timer
	{
	public:
		Timer();			   ///< Des que l'objet est crée le temps demarre
        
        operator std::string(); ///< Transforme en chaine comme l'opérateur <<
        
		void reset();		   ///< Reinitialise le debut comptage
		const Timer& split();  ///< Temps intermédiaire depuis reset ou le dernier split
		const Timer& watch();  ///< Temps depuis reset
		
		unsigned long system() const; ///< Temps système en milli secondes
		unsigned long user  () const; ///< Temps utilisateur en milli secondes
		unsigned long real  () const; ///< Temps reel en milli secondes
		
		// Affichage du timer au format CPU 00:00:02.675 real 00:00:02.676
		inline friend std::ostream& operator << (std::ostream&, const Timer&);
		
	private:
		rusage usageStart;   ///< Usage au moment du reset
		rusage usageSplit;   ///< Usage au moment du split
		rusage usageWatch;    ///< Usage au moment du watch
		rusage usageSplitPrec; ///< Le split précédent
		
		
		// Pour le temps réel
		struct timeval timeStart;
		struct timeval timeSplit;
		struct timeval timeWatch;
		struct timeval timeSplitPrec;
		
		// Type de commande faite
		bool splitAsked;
		
	}; 
	
	
	/// A flux is a quantity at a given time
    /** Just a help in printing flux performances 
     */
	class flux {
	public:
		inline flux(long long,const Timer&);
		///< definition : un flux est une quantite de truc dans un temps donné
		
		inline friend std::ostream& operator <<(std::ostream&, const flux&);
		///< affichage du flux
		
	private:
		const Timer& t;
		long long q;
	};
	
	
	///////////////////////////////////////////////////////////////////
	
	inline 
	void Timer::reset(){
		splitAsked = false;
		getrusage(RUSAGE_SELF,&usageStart);
		gettimeofday(&timeStart,NULL);
		usageSplit = usageStart;
		usageSplitPrec = usageSplit;
		timeSplit  = timeStart;
		timeSplitPrec = timeSplit;
	}
	
	inline 
	const Timer& Timer::split() {
		splitAsked = true;
		// Conserve le split précédent
		usageSplitPrec = usageSplit;
		timeSplitPrec  = timeSplit;
		// Prend le temps intermediaire
		getrusage(RUSAGE_SELF,&usageSplit);
		gettimeofday(&timeSplit,NULL);
		return *this;
	}
	
	inline 
	const Timer&  Timer::watch() {
		splitAsked = false;
		getrusage(RUSAGE_SELF,&usageWatch);
		gettimeofday(&timeWatch,NULL);
        return *this;
	}
	
	
	
	inline
	Timer::Timer() : splitAsked(false) {
		// Pour initier des valeurs correctes
		reset();
	}
	
	/* La stucture timeval
	 struct timeval {
	 time_t      tv_sec;  secondes
	 suseconds_t tv_usec; microsecondes 
	 };
	 */
	
	inline
	unsigned long Timer::system() const {
		struct timeval res;
		// En fonction de la dernière commande
		if (splitAsked) {
			// Soustrait les deux temps
			timersub(&usageSplit.ru_stime,&usageSplitPrec.ru_stime,&res);
		} else {
			timersub(&usageSplit.ru_stime,&usageStart.ru_stime,&res);
		}
		// Rend le résultat en milli secondes
		return res.tv_sec*1000+res.tv_usec/1000;
	}
	
	inline
	unsigned long Timer::user() const {
		struct timeval res;
		// En fonction de la dernière commande
		if (splitAsked) {
			// Soustrait les deux temps
			timersub(&usageSplit.ru_utime,&usageSplitPrec.ru_utime,&res);
		} else {
			timersub(&usageSplit.ru_utime,&usageStart.ru_utime,&res);
		}
		// Rend le résultat en milli secondes
		return res.tv_sec*1000+res.tv_usec/1000;
	}
	
	inline
	unsigned long Timer::real() const {
		struct timeval res;
		// En fonction de la dernière commande
		if (splitAsked) {
			// Soustrait les deux temps
			timersub(&timeSplit,&timeSplitPrec,&res);
		} else {
			timersub(&timeSplit,&timeStart,&res);
		}
		// Rend le résultat en milli secondes
		return res.tv_sec*1000+res.tv_usec/1000;
	}
	
	
	inline
	std::ostream& operator << (std::ostream& out, const Timer& chrono) {
		long cpu = chrono.system()+chrono.user();   // cpu milli seconds
		long real = chrono.real(); // real milli seconds
		
		out << "CPU ";
		long cpuS = cpu / 1000;   // cpu seconds
		long h = cpuS/3600;
		long m = (cpuS % 3600) / 60;
		long s = cpuS % 60;
		long ms = cpu % 1000; // cpu remining millisec
		if (h <= 9)
			out<<'0';
		out<<h<<':';
		if (m <= 9 )
			out<<'0';
		out<<m<<':';
		if (s <= 9)
			out<<'0';
		out<<s<<'.';
		if (ms <= 99)
			out<<'0';
		if (ms <= 9)
			out<<'0';
		out<<ms;
		out << " real ";
		long realS = real / 1000; // real seconds
		h = realS / 3600;
		m = (realS % 3600) / 60;
		s = realS % 60;
		ms = real % 1000;
		if (h <= 9)
			out<<'0';
		out<<h<<':';
		if (m <= 9 )
			out<<'0';
		out<<m<<':';
		if (s <= 9)
			out<<'0';
		out<<s<<'.';
		if (ms <= 99)
			out<<'0';
		if (ms <= 9)
			out<<'0';
		out<<ms;
		return out;
	}
	
	
	inline flux::flux(long long quant,const Timer& time) : t(time), q(quant) {}
	
	inline
	std::ostream& operator <<(std::ostream& out, const flux& f) {
		long real = f.t.real() / 1000; // real seconds
		if ((real <= 0)|| (f.q<0)) {
			out<<"### /s";
		} else {
			long long val = f.q / real;
			if (val <1000) {
				out<<val<<" /s";
			} else if (val < 1000000) {
				out<<(val+500)/1000<<" K/s";
			} else if (val <1000000000) {
				out<<(val+500000)/1000000<<" M/s";
			} else {
				out<<(val+500000000)/1000000000<<" G/s";
			}
		}
		return out;
	}
    
    inline
    Timer::operator std::string() {
        std::stringstream out;
        out<<*this;
        return out.str();
    }
	
}

#endif
