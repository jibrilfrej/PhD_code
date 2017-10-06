// ************************************************************************
//                      INTERFACE OF MODULE HDictionary
// ************************************************************************
/**
 \file HDictionary.h
 \brief A template dictionnary usign a hash map for access
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \date 2008
 \note XIOTA Information Retrieval System

 */

#ifndef HDictionary_h
#define HDictionary_h
#include <algorithm>
#include <vector>
#include <string>
using std::vector;


#include "StringStorage.h"
// WARNING: hash_map is depecated, soon use unordered_map
// hash_map is defined in a special name space: __gnu_cxx
//#include <ext/hash_map>
//using namespace __gnu_cxx;
// END OF WARNING
//#include <hash_map.h>
#include <unordered_map>

namespace IOTA {

    /// A template dictionary using hash map for access
    template <class T> class HDictionary {
        public :
        /// Build the storage structure
        HDictionary(unsigned initialSize=200000 ///< Minimum initial size
                    );

        /// Delete the structure. All string pointers become invalid
        ~HDictionary();

        /// Find if an entry exist.
        /// return true if found
        bool find(const char*) const;

        /// Direct access to one entry of the dictionary
        /// If the entry does not exist, it is added with the initial T() value
        T& access(const char* w,  ///< The word to found
                  unsigned& pos,   ///< The absolute position in the dictionnary
                  bool& found     ///< True if w exists already
                  );

        /// same as access but without position information
        T& access(const char* w,  ///< The word to found
                  bool& found     ///< True if exists already
                  );

        /// Same as access but without found information
        T& operator[](const char* ///< The word to found
                      );


        // Entry of the dictionary
        struct Entry {
            const char* word;
            T data;
            explicit Entry(const char* s,const T& d): word(s),data(d) {};
            /// Operator used for sortData() : the smalest first
            friend bool operator <(const Entry& a,const Entry& b) {return a.data < b.data;};
        };

        /// Direct reading access to a full directory entry
        /// Only reading is allowed because word must not me changed
        /// In case of direct data modification access use data()
        const Entry& operator[](unsigned ///< absolute position in the dictionary
                                ) const;

        /// Direct modification access to data at an obsolute position
        T& data(unsigned);

        /// Empty the dictionary
        void clear();

        /// Return the number of entry in the structure
        unsigned size() const;

        /// Sort the dictionnary on the data
        /// \attention A rehash is then mandatory if further access usign strings
        void sortData();


        /// Trivial Iterator
        class iterator {
            friend class HDictionary<T>;
        public:
            /// Defaut constructor
            iterator();

            /// Next element
            iterator operator ++();

            /// Access to current entry
            Entry operator *() ;

            /// Access to an atribute of the current entry
            Entry* operator ->() ;

            /// Test two iterators
            bool operator ==(iterator);

            /// Mainly for testing the end
            bool operator !=(iterator);

        private:
            // Only begin() is allowed to use this constructor
            iterator( HDictionary<T>*,unsigned _i);
            unsigned i; // Current position
            HDictionary<T>* hDico; //
        };

        /// Start an iterator from the begining of the dictionary
        iterator begin () ;
        
        /// Indicate the end of the dictionary
        iterator end () ;




        private :
        // Locale storage of all strings
        StringStorage stringStorage;

        // Storage of all entries
        vector<Entry> dico;

        // Function to compare two strings
        struct eqstr
        {
            bool operator()(const char* s1, const char* s2) const
            {
			//cout<<s1<<";"<<s2<<" = "<< strcmp(s1, s2) <<endl;
			return strcmp(s1, s2) == 0;}
        };

		struct localHash{

			size_t operator()(const char* s) const
			{

                std::hash<std::string> my_hash;
				return my_hash(std::string(s));

			}


		};

        // Hash_map to a quick access to dictionnary
        typedef unordered_map<const char*, unsigned, localHash, eqstr> Hmap;
        Hmap hcode;

    };

    //////////////// INLINE ////////////////////////////////////////////

    template <class T> inline
    HDictionary<T>::HDictionary(unsigned initialSize) :
    stringStorage(initialSize),
    hcode(initialSize){
        // Set the initial capacity of idf
        dico.reserve(initialSize);
    };

    template <class T> inline
    HDictionary<T>::~HDictionary() {}


    template <class T> inline
    bool HDictionary<T>::find(const char* w) const {
        return hcode.find(w) != hcode.end();
    }

    template <class T> inline
    T& HDictionary<T>::access(const char* w,unsigned& _pos,bool& found) {
        // The hash map is used to a direct access to the dictionary vector
        const char* s;
        unsigned pos; // Location of the entry found
        typename HDictionary<T>::Hmap::iterator iter;
		cout<<w<<endl;
        iter = hcode.find(w);

        if (iter == hcode.end()) {
			            // the word is not found and must be added
            // first into the string storage space
            found=false;
            s = stringStorage.add(w);
            // Then into the hash map with string from the storage
            // The new id is the current max value
            pos=dico.size();
            hcode[s]=pos;
            // push the new value
            dico.push_back(Entry(s,T()));
            _pos = pos;
            return dico[pos].data;

        } else {
			// The hash indicate the string is found
            found=true;
            pos = iter->second;
            _pos = pos;
            return dico[pos].data;
        }
    }

    template <class T> inline
    T& HDictionary<T>::access(const char* w,bool& found) {
        unsigned pos; // Location of the entry found
        return access(w,pos,found);
    }


    template <class T> inline
    T& HDictionary<T>::operator[](const char* w) {
        bool unused;
        return access(w,unused);
    }

    template <class T> inline
    typename HDictionary<T>::Entry const & HDictionary<T>::operator[](unsigned i) const {
        return dico[i];
    }

    template <class T> inline
    T& HDictionary<T>::data(unsigned i) {
        return dico[i].data;
    }

    template <class T> inline
    unsigned HDictionary<T>::size() const{
        return dico.size();
    }

    template <class T> inline
    void HDictionary<T>::sortData() {
        sort(dico.begin(),dico.end());
    }


    template <class T> inline
    void HDictionary<T>::HDictionary::clear() {
        dico.clear();
        stringStorage.clear();
        hcode.clear();
    }

    /////////////////// Iterator ////////////////////////////
    template <class T> inline
    HDictionary<T>::iterator::
    iterator() : i(0), hDico(NULL) {};

    template <class T> inline
    HDictionary<T>::iterator::
    iterator(HDictionary<T>* d, unsigned _i) : i(_i), hDico(d) {};

    template <class T> inline
    typename HDictionary<T>::iterator HDictionary<T>::begin()  {
        return iterator(this,0);
    };

    template <class T> inline
    typename HDictionary<T>::iterator HDictionary<T>::end()  {
        return iterator(this,dico.size());
    };

    template <class T> inline
    bool HDictionary<T>::iterator::operator != (iterator iter) {
        assert(this->hDico == iter.hDico);
        return this->i != iter.i;
    }

    template <class T> inline
    bool HDictionary<T>::iterator::operator == (iterator iter) {
        assert(this->hDico == iter.hDico);
        return this->i == iter.i;
    }

    template <class T> inline
    typename HDictionary<T>::iterator
    HDictionary<T>::iterator::operator ++ () {
        ++i;
        return *this;
    }


    template <class T> inline
    typename HDictionary<T>::Entry
    HDictionary<T>::iterator::operator * ()  {
        return (hDico->dico)[i];
    }

    template <class T> inline
    typename  HDictionary<T>::Entry*
    HDictionary<T>::iterator::operator -> ()  {
        return &(hDico->dico)[i];
    }

}


#endif
