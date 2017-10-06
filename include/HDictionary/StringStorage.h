/**
 \file StringStorage.h
 \brief Efficient String Storage, no deletion allowed.
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \copyright Laboratoire d'Informatique de Grenoble.
 \copyright Université de Grenoble (Pierre Mendès France UPMF).
 \copyright Centre National de la Recherche Scienfifique (CNRS).
 \date 2007
 \note IOTA Information Retrieval System
 
 The StringStorage just keep all strings into one single compact
 memory space. Strings are copied into the memory space.
 When the initial memory block is full then an extention block is create.
 More extension bloks are created when more space is needed.
 There is no limitation except memory size.
 String can only being copied, no deletion is allowed.
 */


#ifndef StringStorage_h
#define StringStorage_h

#include <stdlib.h> // Pour abort
#include <vector>
using std::vector;
#include <cstdio>
#include <cassert>

#include "binFile.h"
using IOTA::writeHeader;
using IOTA::readHeader;
using IOTA::writeParam;
using IOTA::readParam;

#include "XML_Stream.h"

#include "error.h"

namespace IOTA {
    
    
    /** \brief Store a large set of strings into a common memory area
     \details
     */
    class StringStorage {
        
    public:
        
        /// Fix the increment when one block of memory is full
        StringStorage(unsigned initialCapacity, ///< Initial capacity of the storage
                      unsigned increment        ///< Size of block added when capacity overflow
                      );
        
        /// By default the increment is the same size of the initial capacity
        StringStorage(unsigned capacity);
        
        /// By default 1<<20 = 1 Mb for initial and 1<<18=512Kb for increment
        StringStorage();
        
        /// Release data space. Warning all string pointers becomes invalid.
        ~StringStorage();
        
        /// \brief Add a new string into the storage
        /// \return the address where the string is stored
        unsigned char* add(const unsigned char*p);
        
        /// \brief Add a new string into the storage
        /// \return the address where the string is stored
        char* add(const char*p);
        
        
        /// \brief Empty the storage structure
        /// \attention all existing pointers produces by add are after invalid
        void clear();
        
        /// output stat for debug
        void stat();
        
        /// Class for managing exception errors
        class Overflow {};
        
        /// Somme des taille des chaines de caractères
		size_t size() const;
        
        /// Occupation en octets de la somme des zones data
		size_t capacity() const;
        
        /// Save and load
        Error save(FILE*) const;
        Error load(FILE*);
        // loading after the header (after readHeader)
        Error load(FILE*,size_t);
        // Save in XML
        Error save(XML::Stream&);
        
        // Test un fichier
        Error check(const char*);
        
        /// Iterator const
        class const_iterator {
            friend class StringStorage;
            unsigned i; // No element courant
            unsigned blockId; // Block courant
            const char* str;        // Chaine courante du bloc courant
            StringStorage* const store;      // La matrice ou on itere
        public:
            // Itérateur vide, sert aussi à indiquer la fin
            const_iterator() : i(-1), blockId(-1), str(NULL), store(NULL) {};
            
            
            /// Access to current value
            const char* operator *() const {return str;}
            
            /// Next value
            const_iterator operator ++() {
                i++;
                if (i < store->nbString) {
                    str = store->next(blockId,str);
                    return *this;
                }
                str = NULL;
                return *this;
            }
            
            // Test de fin de boucle
            bool operator != (const const_iterator& it) const {
                // Inutile de tester autre chose !
                return it.str != this->str;
            }
        private:
            const_iterator(StringStorage* s) : i(0), blockId(0), str(s->data[0]), store(s) {};
        };
        
        const_iterator begin() {
            return const_iterator(this);
        }
        
        const_iterator end() {
            return const_iterator();
        }
        
    private:
        // Recherche la chaine suivante, NULL si c'est terminé
        // Met à jour le No de block où la chaine est trouvée
        // Pour l'iterateur
        const char* next(unsigned& blockId,const char* str);
        
        // Recherche la position du prochaine début de chaine ou NULL
        // Recherche uniquement dans le bloc indiqué
        // Pour next()
        const char* findNextString(unsigned blockId,const char* p) const;
        
        // Recherche la taille d'un bloc en partant de la fin
        // et cherchant le premier caractère non null
        // Utilisé pour sauvegarder juste les informations utiles de chaque bloc
        size_t blockSize(unsigned blockId) const;
        
        // Taille maximum d'un bloc
        size_t blockCapacity(unsigned blockId) const;
        
    private:
        // Nombre de chaines stockées
        unsigned nbString; // Utilisé pour la sauvegarde et l'iterateur
        // The set of all allocations
        vector<char*> data;
        typedef vector<char*>::size_type size_type;
        // The size of new storage zone
        unsigned initialCapacity; // first block size
        // initialCapacity is not const because it can change in case of load
        const unsigned extCapacity;  // other blocs size
        // zone de stockage des caractères des chaines
        char* str;      // current empty area
        char* strMax;   // current max adress
        
    };
    
    ///////////// INLINE ////////////////////////////////////////////////
    
    inline
    StringStorage::StringStorage() :
    nbString(0), data(),initialCapacity(1<<20),extCapacity(1<<18),
    str(new char[initialCapacity]),strMax(str+initialCapacity)
    {// Allocate the initial storage
        data.push_back(str);
        // Store the initial pointer
    }
    
    inline
    StringStorage::StringStorage(unsigned ic) :
    nbString(0), data(),initialCapacity(ic),extCapacity(ic),
    str(new char[ic]),strMax(str+ic)
    {// Allocate the initial storage
        data.push_back(str);
        // Store the initial pointer
    }
    
    
    
    inline
    StringStorage::StringStorage(unsigned ic,unsigned c) :
    nbString(0), data(),initialCapacity(ic),extCapacity(c),
    str(new char[ic]),strMax(str+ic)
    {// Allocate the initial storage
        data.push_back(str);
        // Store the initial pointer
    }
    
    inline
    StringStorage::~StringStorage() {
        // Delete all allocated storage
        for(size_type i=0;i<data.size();i++) {
            delete [] data[i];
        }
    }
    
    // Ajout d'une chaine dans l'espace de stockage
    inline
    char* StringStorage::add(const char*p) {
        register const char* local = p;
        register char* oldStr = str;
        char* res=str;
        while((*local!=0)&&(str<strMax)) {
            *str++ = *local++;
        }
        // Check overflow
        if(str>=strMax) {
            // Attention : ce ne peux pas être le debut du bloc
            // dans ce cas c'est une erreur : la chaine est plus grande que le bloc !
            assert(oldStr != data.back());
            // Empty the rest of the block
            // Indispensable à la recherche des chaines par l'itérateur
            while(oldStr < strMax) {*oldStr++=0;}
            // Do a new allocation
            str = new char[extCapacity];
            strMax = str + extCapacity;
            data.push_back(str);
            // now try a add again
            return this->add(p);
        }
        // add end of string
        // This is possible because str<strMax
        // The new position of str may be incorrect
        *str++=0;
        nbString++;
        return res;
    }
    
    inline
    unsigned char* StringStorage::add(const unsigned char*p) {
        return (unsigned char*) this->add((const char*)p);
    }
    
    
    
    inline
    void StringStorage::clear() {
        // reset the first storage
        str = data[0];
        strMax = str + initialCapacity;
        // Delete all other allocated storage
        for(size_type i=1;i<data.size();i++) {
            delete [] data[i];
        }
        // IMPORTANT: reset the data vector
        data.clear();
        data.push_back(str);
        nbString=0;
    }
    
    inline
    size_t StringStorage::blockCapacity(unsigned blockId) const {
        return blockId ? extCapacity : initialCapacity;
    }
    
    inline
    size_t StringStorage::blockSize(unsigned blockId) const {
        assert(blockId < data.size());
        // Si c'est le dernier bloc
        if (blockId == data.size()-1) {
            // Ce bloc n'est pas plein
            return str-data[blockId];
        }
        // Cas général des blocs remplis
        // Recherche de la position finale exacte
        // Positionne sur la fin du bloc
        size_t cap = blockCapacity(blockId);
        const char* start = data[blockId];
        assert(cap > 0);
        const char* p = data[blockId] + cap -1 ;
        assert(*p == 0);
        // Recherche la première valeur non nulle
        while ((p >= start) && (*p == 0)) {--p;}
        assert(*p != 0);
        // on ajoute 1 pour le zéro, et un pour de placer apres la chaine
        // Comme les chaines sont non vide, la taille mini est 2 char
        return p - start + 2;
    }
    
    inline
    size_t StringStorage::capacity() const {
        size_t s = 0;
        for (unsigned i=0; i<data.size(); ++i) {
            s += blockCapacity(i);
        }
        
        return s;
    }
    
    inline
    size_t StringStorage::size() const {
        size_t s = 0;
        for (unsigned i=0; i<data.size(); ++i) {
            s += blockSize(i);
        }
        
        return s;
    }
    
    
    
    inline
    Error StringStorage::save(FILE* file) const {
        // Sauve l'entête
        Error e = writeHeader(file,"string",size());
        if (e != eNoError) return e;
        
        e = writeParam(file,nbString);
        if (e != eNoError) return e;
        
        // Parcours de tous les blocs à sauver
        for (unsigned i=0; i< data.size(); ++i) {
            size_t bSize = blockSize(i);
            if (fwrite(data[i],sizeof(char),bSize,file) != bSize) {
                return eFileWrite;
            }
        }
        return eNoError;
    }
    
    inline
    Error StringStorage::save(XML::Stream& xml) {
        xml<<"<?xml version=\"1.0\">\n";
        xml<<"<string nb=\""<<nbString<<"\" />\n";
        for (const_iterator iter=begin();iter != end(); ++iter) {
            xml<<*iter<<"\n";
        }
        xml<<"</string>\n";
        return eNoError;
    }
    
    
    inline
    Error StringStorage::load(FILE* file,size_t size) {
        // Vide la structure
        this->clear();
        // Charge le paramètre
        Error e = readParam(file,nbString);
        if (e != eNoError) return e;
        
        if ((size==0) || (nbString==0)) { return eFileEmpty;}
        
        // Regarde si le premier et unique block est assez grand
        if (size > initialCapacity) {
            // Non, il faut allouer à nouveau un autre bloc plus grand
            delete[] data[0];
            initialCapacity = size;
            data[0] = new char[initialCapacity];
            str = data[0];
            strMax = str+initialCapacity;
        }
        // Chargement en une seule fois
        size_t r = fread(str,1,size,file);
        if (r!= size) {
            return eFileRead|ePartial;
        }
        // place la zone libre
        str = data[0]+size;
        return eNoError;
    }
    
    
    
    inline
    Error StringStorage::load(FILE* file) {
        // Charge l'entete
        size_t size=0;
        time_t time;
        string format;
        Error e = readHeader(file,format,time,size);
        if (e != eNoError) return e;
        if (format != string("string")) {
            return eFileRead|eFlagFormat|eType;
        }
        return load(file,size);
    }
                    
    inline
    Error StringStorage::check(const char* fileName) {
        FILE* file = fopen(fileName,"r");
        if (file == NULL) { fclose(file); return eFileOpen; }
        // Charge l'entete
        size_t size=0;
        time_t time;
        string format;
        Error e = readHeader(file,format,time,size);
        if (e != eNoError) { fclose(file); return e; }
        if (format != string("string")) {
            fclose(file);
            return eFileRead|eFlagFormat|eType;
        }
        // Lit le paramètre
        unsigned nb;
        e = readParam(file,nb);
        if (e != eNoError) {fclose(file);  return e;}
        // Test le nombre d'octets et le nombre de chaines
        int c = ' ';
        unsigned nbZero = 0;
        size_t nbByte = 0;
        while ((c = getc(file)) != EOF) {
            nbByte++;
            if (c == 0) {nbZero++;}
        }
        if (nbByte != size) {
            fclose(file);
            BUG<<"File "<<fileName<<" incorrect size="<<nbByte<<" expecting: "<<size;
            return eFileRead|ePartial|eVal01;
        }
        if (nbZero != nb) {
            fclose(file);
            BUG<<"File "<<fileName<<" incorrect string number="<<nbZero<<" expecting "<<nb;
            return eFileRead|ePartial|eVal01;
        }
        fclose(file);
        return eNoError;
    }
                    
    
    
    
    inline
    void StringStorage::stat() {
        INFO<<"Occupation="<<initialCapacity<<" + "<<extCapacity<<"*"<<data.size()-1
        <<"="<<initialCapacity+extCapacity*(data.size()-1);
        INFO<<"Last block size="<<str-data.back();
    }
    
    // Cherche le prochain début de chaine dans le bloc indiqué
    inline
    const char* StringStorage::findNextString(unsigned blockId,const char* p) const {
        assert(blockId < data.size());
        // p pointe sur un caractère non nul
        assert(*p != 0);
        // Calcul la limite du bloc blocId
        char* blockLimit;
        if (blockId == 0) {
            blockLimit = data[0] + initialCapacity;
        } else {
            blockLimit = data[blockId] + extCapacity;
        }
        // Recherche la prochaine valeur nulle
        register const char* local = p;
        while((local<blockLimit)&&(*local!=0)) {local++;}
        // A trouve la chaine ?
        if (local >= blockLimit) {
            // Fin de block : Rien trouvé
            // NB : c'est une erreur
            return NULL;
        }
        // Tente de trouver le premier caractère non nul
        while((local<blockLimit)&&(*local==0)) {local++;}
        if (local >= blockLimit) {
            return NULL;
        }
        if (*local!=0) {
            // Trouvé
            return local;
        }
        return NULL;
    }
    
    
    inline
    const char* StringStorage::next(unsigned& blockId,const char* p) {
        // Recherche la prochaine valeur non nulle du bloc courant
        const char * local = findNextString(blockId,p);
        if (local != NULL) {
            return local;
        }
        // essaye de passer au bloc suivant
        blockId++;
        if (blockId < data.size()) {
            return data[blockId];
        } else {
            return NULL;
        }
    }
    
    
}
#endif
