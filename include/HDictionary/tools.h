/**
 \file tools.h
 \brief Miscelaneous calculus tools
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \note IOTA Information Retrieval System
 */

#ifndef tools_h
#define tools_h
#include <typeinfo> // typeid
#include <cstdio>
#include <cassert>
#include <vector>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <math.h>
#include <string>
#include <cstdio>

namespace IOTA {
    
	const double PI = 3.141592653589793238462643;
    
    /// \brief computes the square of a
	template <class T> T sq(T a);
    
    /// Minimum of two values
    template <class T> T min(const T& a,const T& b);
    
    /// Minimum of tree values
    template <class T> T min(const T& a,const T& b, const T& c);
    
    /// Minimum of four values
    template <class T> T min(const T& a,const T& b, const T& c, const T& d);
    
    /// Maximum of two values
    template <class T> T max(const T& a,const T& b);
    
    /// Maximum of tree values
    template <class T> T max(const T& a,const T& b,const T& c);
    
    /// absolute of two values
    template <class T> T abs(const T& a);
    
    /// Switch two values
    template<typename T> void switchValue(T& a,T& b);
    
    /// Find the index of the  maximum value of an array
	template<class T> void findMaxIndex(const T& array,size_t& max);
    
    /// Find the two index of the first two maximum values
	template<class T> void findMaxIndex(const T& array,size_t& max1, size_t& max2);
    
    /// Normalize the content of a vector
    /// Perform a linear normalisation between low and high value
	template <class T> void normalize(std::vector<T>& v,T low=0, T high=1);
    
    /// Transform any type that implement operator<< into a string
	template <class T> std::string to_string (const T& t);
    
    /// Ajoute la chaine src à dst, avec dst de taille max size
    void strcat(char * dst, const char * rc, size_t size);
    
    /// Supprime le caractère \n en fin de chaine
    void chomp(char* s);
    
    /// Supprime le suffix de la chaine s'il est trouvé
    void delSuffix(std::string& s,const char* suffix);
    
    /// Separe une ligne en plusieurs champs séparés par le caractère indiqué
    void split(std::vector<std::string>& fields,std::string line, char sep);
    
    /// Transforme un entier en chaine
    std::string itos(int i);
    
    // Transforme n'importe quoi en chaine
    template<typename T>
    std::string tos(const T& i);
    
    /// Recupère le pointeur sur le nom de fichier sans le path
    const char* skipPath(const char * s);
    
    /// Recupère un pointeur sur l'extension du nom du fichier
    const char* skipToExtension(const char * s);
    
    /// Transform any integer to a unique string
    /// Used to generate a lot of differents string for testing
    std::string genString(unsigned i);
    
    
    void writePercentage( long a, long b, bool interactif=true );
    // Affiche le pourcentage de a/b sur la mÍme ligne
    
    void writePercentage( long a, long b, const char*, long c, long d, bool interactif=true, long unsigned t=0);
    // Affiche le pourcentage de a/b le texte et le pourcentage de c/d sur la mÍme ligne
    // ou dans un fichier, affiche aussi le temp restant estime
    
    
    bool writePercentage( long a, long b, unsigned& pourcentage);
    
    
    // Test pour savoir si on peut encore lire quelque chose
    bool canReadFile(FILE*);
    
    /// Euclidian distance among 2 vector responding to size() and operator[]
    template<typename T1,typename T2>
    double dist2(const T1& v1, ///< First vector
                 const T2& v2  ///< Second vector
    );
    
    
    /// Square Euclidian distance among 2 vectors of the same size
    /// Warning: parameters are supposed to be correct
    template<typename T>
    double dist(const T* v1, ///< First vector
                const T* v2, ///< Second vector
                size_t size  ///< Size of both vectors
    );
    
    /// Square Euclidian distance among 2 vectors of the same size
    template<typename T>
    double dist(const T* v1,       ///< First vector
                std::vector<T>& v2 ///< Second vector
    );
    
    
    /// Fast Square Euclidian distance among 2 vectors of the same size
    /// Computation is stopped as soon as min distance is reached
    /// WARNING: when min is reach computed value is incomplete and should not be used
    template<typename T>
    double dist(const T* v1, ///< First vector
                const T* v2, ///< Second vector
                size_t size, ///< Size of both vectors
                double min   ///< Min distance
    );
    
    /// Cosine between two vectors of dimention n
    template<typename T>
    double cosine(const T* v1, ///< First vector
                  const T* v2, ///< Second vector
                  size_t size ///< Size of both vectors
    );
    
	/// Fast chi2 distance among 2 vectors of the same size : d(x,y) = sum( (xi-yi)^2 / (xi+yi) ) / 2
    /// Computation is stopped as soon as min distance is reached.
    /// WARNING: when min is reach computed value is incomplete and should not be used
    template<typename T>
    double distchi2(const T* v1, ///< First vector
                    const T* v2, ///< Second vector
                    size_t size, ///< Size of both vectors
                    double min   ///< Min distance
    );
    
    /// Compute Hamming distance.
    /// \Warning: T should be an integer type, and only low BYTE is used
    template<typename T>
    double distHammingB(const T* v1, ///< First vector
                        const T* v2, ///< Second vector
                        size_t size, ///< Size of both vectors
                        double min   ///< Min distance
    );
    
    
    
    /// Triplet structure
    template<typename T>
    struct Triple {
        T first;
        T second;
        T third;
        Triple(const T& v1,const T& v2,const T& v3) : first(v1),second(v2),third(v3){}
    };
    
    /// Static computation of the number of bits of an integer (Meta Programming !)
    /// Usage : cout<<"BitSize 0xF: "<< Unsigned<0xF>::bitSize <<endl;
    template <unsigned int x>
    struct Unsigned {
        static const unsigned int bitSize = (0x1&x) + Unsigned<x/2>::bitSize;
        static const unsigned int lastBit = (0x1&x);
    };
    
    template <>
    struct Unsigned<0> {
        static const unsigned int bitSize = 0;
        static const unsigned int lastBit = 0;
    };
    
    
    
    ///Flux de sortie avec indentation
    /// Utilisation comme un stream :
    /// Ident indent;
    /// indent<<"Hello"<<endl;
    /// indent++;
    
    class Indent {
        unsigned _n;
        std::ostream *_os;
    public:
        // Indentation de zéro espaces sur std::out
        Indent() : _n(0), _os(&std::cout) {};
        // Indentation de n espaces sur std::out
        Indent(unsigned n) : _n(n), _os(&std::cout) {};
        // Indentation de n espaces sur un flux passé en paramètre
        Indent(unsigned n,std::ostream& os) : _n(n), _os(&os) {};
        // Indente plus
        unsigned operator ++(int) { return _n++;}
        // Indente moins
        unsigned operator --(int) { return _n--;}
        // Sort sur le flux de manière indentée
        template <typename T>
        std::ostream& operator<<(T data) {
            for (unsigned i=0;i<_n;i++) {
                _os->put(' ');
            }
            *_os << data;
            return *_os;
        }
        // Active une fonction manipulateur de flux sur le flux interne
        // Ex: pour le manipulateur endl()
        std::ostream& operator<< (std::ostream& (*pf)(std::ostream&)) { return pf(*_os);}
        
    };
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
	/// \brief computes the square of a
	template <class T>
    inline T sq(T a) { return a*a; }
    
    
    template <class T>
    inline
    T abs(const T& a) {
        return (a >= 0) ? a : -a;
    }
    
    
	/// Find the  index of the  maximum value
	template<class T>
    inline
    void findMaxIndex(const T& array,size_t& max) {
        assert(array.size() > 1);
        max = 0;
        for (unsigned i=1;i<array.size();i++) {
            if (array[i] > array[max]) {
                max = i;
            }
        }
    }
    
    
	/// Find the two index of the first two maximum values
	template<class T>
    inline
    void findMaxIndex(const T& array,size_t& max1, size_t& max2) {
        assert(array.size() > 2);
        if (array[0] > array[1]) {
            max1 = 0;
            max2 = 1;
        } else {
            max1 = 1;
            max2 = 0;
        }
        // Invariant : array[max1] >= array[max2] max1 != max2
        // et array[max2] >= oh[0..i[
        for (unsigned i=2;i<array.size();i++) {
            if (array[i] > array[max2]) {
                if (array[i] > array[max1]) {
                    max2 = max1;
                    max1 = i;
                } else {
                    max2 = i;
                }
            }
        }
    }
    
    
    /* min et max sont déjà definit dans la stl */
    
	template <class T>
    inline
    T min(const T& a,const T& b) {
        return (a < b) ? a : b;
    }
    
    template <class T>
    inline
    T min(const T& a,const T& b, const T& c) {
        return min(min(a,b),c);
    }
    
    template <class T>
    inline
    T min(const T& a,const T& b, const T& c, const T& d) {
        return min(min(min(a,b),c),d);
    }
    
	template <class T>
    inline
    T max(const T& a,const T& b) {
        return (a > b) ? a : b;
    }
    
    template <class T>
    inline
    T max(const T& a,const T& b,const T& c) {
        return max(max(a,b),c);
    }
    
    
	template<typename T>
    inline
    void switchValue(T& a,T& b) {
        T tmp;
        tmp=a;
        a=b;
        b=tmp;
    }
    
	/// Normalize the content of a vector
	template <class T>
    inline
    void normalize(std::vector<T>& v,T low, T high) {
        assert(v.size() > 0);
        // Recherche le max et le min
        T max = v[0];
        T min = max;
        for (size_t i=0;i<v.size();i++) {
            if (max < v[i]) {
                max = v[i];
            } else if (min > v[i]) {
                min = v[i];
            }
        }
        // Et normalise
        T ratio = max-min;
        for (size_t i=0;i<v.size();i++) {
            v[i] = (v[i]-min) / ratio;
        }
    }
    
	/// Transform any type into a string
	template <class T>
    inline std::string to_string (const T& t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
    
    
	// Ajoute la chaine src à dst, avec dst de taille max size
	inline
    void strcat(char *  _dst, const char * _src, size_t size) {
        char* dst = _dst;
        size_t s = size;
        // recherche la fin
        while (*dst && s) {
            ++dst;
            --s;
        }
        // Plus de place
        if (*dst !=0 || s==0 ) return;
        // Recopie
        const char* src = _src;
        while(*src && s) {
            *dst++ = *src++;
            --s;
        }
        // place le zéro final
        if (s==0) {
            --dst;
        }
        *dst = 0;
    }
    
    
	// fonction qui transforme un entier en chaine
	inline
    std::string itos(int i)
    {
        //	char buffer[5];
        //	sprintf(buffer, "%d", i);
        //	return buffer;
        std::string str;
        std::stringstream inout;
        inout << i;
        inout >> str;
        return str;
    }
    
    // fonction qui transforme n'importe quoi en chaine
    template<typename T>
    inline
    std::string tos(const T& i)
    {
        std::stringstream inout;
        // Force l'ecriture sous un entier des caractères
        if (typeid(T) == typeid(unsigned char)) {
            inout << unsigned(i);
        } else if (typeid(T) == typeid(char)) {
            inout << int(i);
        } else {
            inout << i;
        }
        return inout.str();
    }
    
    
    
    // Recupère le pointeur sur le nom sans le path
    // dans la même chaine
    inline
    const char* skipPath(const char * s) {
        const char* l = s;
        // va au bout de la chaine
        while (*l) l++;
        // s'arrete au premier '/'
        while ((s<l) && (*l != '/')) l--;
        if (*l == '/') l++;
        return l;
    }
    
    // Recupère le pointeur sur l'extension
    // dans la même chaine, rend vide si non trouvé
    inline
    const char* skipToExtension(const char * s) {
        const char* l = s;
        // va au bout de la chaine
        while (*l) l++;
        // le bout de la chaine
        const char* last = l;
        // s'arrete au premier '.'
        while ((s<l) && (*l != '.')) l--;
        if (*l == '.')  {
            l++;
            return l;
        } else {
            // pas trouvé, retourne la chaine vide
            return last;
        }
        
    }
    
    
    /// Supprime le caractère \n en fin der chaine
    inline
    void chomp(char* s) {
        char* l = s;
        // va au bout de la chaine ou sue le caractère \n
        while ((*l!=0) && (*l != '\n')) l++;
        if (*l == '\n') {
            *l = 0;
        }
    }
    
    inline
    void split(std::vector<std::string>& fields,std::string line, char sep) {
        fields.clear();
        std::string curField;
        char lastChar=0;
        for (size_t i=0;i<line.size();i++) {
            if (line[i]==sep) {
                if (lastChar != sep) {
                    fields.push_back(curField);
                    curField = "";
                }
            } else {
                curField.push_back(line[i]);
            }
            lastChar = line[i];
        }
        // Si le dernier caractère n'est pas le séparateur alors
        // le dernier push_back n'a pas eu lieu
        if (lastChar != sep) {
            fields.push_back(curField);
        }
    }
    
    
    /// Transform an interger to a string
    /// Used to generate a lot of differents string for testing
    inline
    std::string genString(unsigned i) {
        std::string s;
        unsigned mod;
        char c;
        do {
            // produce a new character
            mod = i % 26;
            // produce a new character and add it
            c = char('a'+mod);
            s += c;
            i /= 26;
        }   while (i > 0);
        return s;
    }
    
    
    inline void writePercentage(long a, long b, bool interactif)
    {
        static long int prevPerc = -1;
        long perc;
        if (b < 100000)
            perc = a * 100 / b;
        else
            perc = a / (b / 100);
        
        if(interactif)
        {
            if( perc != prevPerc )
            {
                std::cout << "  " << perc << "%\xd"; std::cout.flush();
                prevPerc = perc;
            }
        }
    }
    
    inline long CalcPercentage(long A, long B) {
        if (B == 0)
            return -1;
        if (B < 100000)
            return  A * 100 / B;
        else
            return A / (B / 100);
    }
    
    inline void writePercentage(long A1, long A2, const char* text, long B1, long B2, bool interactif, long unsigned t)
    {
        //	long unsigned t_reste; // utilise pour calculer le temp restant avant la fin
        // du processus
        
        static long int prevPercA = -1;
        static long int prevPercB = -1;
        long percA;
        long percB;
        percA = CalcPercentage(A1,A2);
        percB = CalcPercentage(B1,B2);
        
        //	if(interactif)
        //	{
        if( (percA != prevPercA) || (percB != prevPercB) )
        {
            std::cout << "  " << percA <<"%"<<text<<percB  << "%\xd"; std::cout.flush();
            prevPercA = percA;
            prevPercB = percB;
        }
        //	}
        //	else
        //	{
        //		if( (percA != prevPercA) || (percB != prevPercB) )
        //		{
        //			t_reste = (((long unsigned)100 * t) / (long unsigned)(percA + 1))-t;
        //			fstream f("progress", ios::out|ios::trunc);
        //			f << percA << " " << percB << " " << t_reste;
        //			f.close();
        //			system("chmod 666 progress");
        //		}
        //	}
    }
    
    inline bool writePercentage(long a, long b, unsigned& pourcentage)
    {
        static long int prevPerc = -1;
        long perc;
        if (b < 100000)
            perc = a * 100 / b;
        else
            perc = a / (b / 100);
        
        if( perc != prevPerc ) {
            std::cout << "  " << perc << "%\xd"; std::cout.flush();
            prevPerc = perc;
            pourcentage = unsigned(perc);
            return true;
        }
        return false;
    }
    
    inline
    bool canReadFile(FILE* file) {
        int c = getc(file);
        ungetc(c,file);
        return feof(file) == 0;
    }
    
    template<typename T>
    inline
    double dist(const T* v1,const T* v2,size_t size)  {
        register double d=0.f;
        register const T* p1;
        register const T* p2;
        for (p1=v1,p2=v2; p1 <(v1+size);++p1,++p2) {
            d += sq(double(*p1) - double(*p2));
        }
        return d;
    }
    
    /// Euclidian distance among 2 vector responding to size() and operator[]
    template<typename T1,typename T2>
    inline
    double dist2(const T1& v1, ///< First vector
                 const T2& v2  ///< Second vector
    ) {
        assert(v1.size() == v2.size());
        register double d=0.f;
        for (size_t i=0;i< v1.size();i++) {
            d += sq(double(v1[i]) - double(v2[i]));
        }
        return sqrt(d);
    }
    
    template <typename T>
    inline
    double dist(const T* v1, const T* v2, size_t size, double min)  {
        register double d = 0.f;
        register const T* p1;
        register const T* p2;
        for(p1=v1,p2=v2 ; p1 < (v1+size) ; ++p1,++p2) {
            d += sq(double(*p1) - double(*p2));
            if (d > min) break;
        }
        return d;
    }
    
    template<typename T>
    inline
    double dist(const T* v1,        ///< First vector
                std::vector<T>& v2 ///< Second vector
    ) {
        return dist(v1,v2.data(),v2.size());
    }
    
    template<typename T>
    inline
    double cosine(const T* v1, ///< First vector
                  const T* v2, ///< Second vector
                  size_t size ///< Size of both vectors
    ) {
        register double innerProd = 0.0;
        register double norm1 = 0.0;
        register double norm2 = 0.0;
        for(size_t i=0;i<size;i++) {
            innerProd += double(*v1)*double(*v2);
            norm1 += double(*v1)*double(*v1);
            norm2 += double(*v2)*double(*v2);
            v1++;v2++;
        }
        //std::cerr<<innerProd<<' '<<norm1<<' '<<norm2<<endl;
        // Valeur des normes avant racice carre globale
        double norm = norm1*norm2;
        // post-vérification pour les valeurs limites
        if (norm == 0) {
            if (norm1 == 0 && norm2 == 0) {
                // Les deux nulles : correspondance maximum
                return 1.0;
            } else {
                // Une des deux nulle : différence maximimum
                return -1.0;
            }
        } else {
            // cas général
            return  innerProd/sqrt(norm);
        }
    }
    
    template <typename T>
    inline
    double distchi2(const T* v1, const T* v2, size_t size, double min)  {
        register double d = 0.f;
        register const T* p1;
        register const T* p2;
        for(p1=v1,p2=v2 ; p1 < (v1+size) ; ++p1,++p2) {
		    if (( double(*p1) + double(*p2)) == 0.0) continue;
            d += sq(double(*p1) - double(*p2)) / ( abs(double(*p1)) + abs(double(*p2)));
            if (d > min) break;
        }
        return d / 2.0;
    }
    
    // Comptage du nombre de bits
    inline
    int bitcount(unsigned int num) {
#if __GNUC__
        return __builtin_popcount(num);
#else
        int count = 0;
        static int nibblebits[] =
        {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
        for(; num != 0; num >>= 4)
            count += nibblebits[num & 0x0f];
        return count;
#endif
    }
    
    template<typename T>
    inline
    double distHammingB(const T* v1, ///< First vector
                        const T* v2, ///< Second vector
                        size_t size, ///< Size of both vectors
                        double min   ///< Min distance
    ) {
        register unsigned d = 0;
        register const T* p1;
        register const T* p2;
        for(p1=v1,p2=v2 ; p1 < (v1+size) ; ++p1,++p2) {
            // Calcule le XOR des deux bytes
            unsigned char v1 = 0xFF & int(*p1);
            unsigned char v2 = 0xFF & int(*p2);
            // Calcule le XOR
            unsigned char res = v1 ^ v2;
            // Compte de nombre de bits
            d += bitcount(res);
            if (double(d) > min) break;
        }
        return double(d);
    }
    
    
    
    inline
    void delSuffix(std::string& s,const char* suffix) {
        // calcule la taille du suffixe
        size_t sSize = 0;
        const char *p = suffix;
        while (*p++) sSize++;
        if (sSize != 0 && s.size() >= sSize) {
            // Recherche si le suffixe est présent
            p = suffix;
            // La position du suffixe dans la chaine
            size_t pos = s.size()-sSize;
            size_t i = pos;
            while (*p!=0 && s[i]==*p ) {
                i++; p++;
            }
            // Regarde si le suffixe est trouvé
            if (*p==0) {
                s.erase(pos);
            }
        }
    }
    
}

#endif
