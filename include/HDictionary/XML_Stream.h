// ************************************************************************
//                      INTERFACE OF MODULE Stream
// ************************************************************************
/** 
 @file XML_Stream.h
 @brief Efficient XML Parser
 @author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 @copyright Laboratoire d'Informatique de Grenoble (LIG).
 @copyright Université de Grenoble II Pierre Mendès France (UPMF).
 @copyright Centre National de la Recherche Scienfifique (CNRS).
 
 @date 2002
 @note IOTA Information Retrieval System
 
 Module to parse in a very efficient manner XML output.
 In can be used also for XML output.
 To use this parser, one have to write a loop using XML::read()
 @attention For efficiency the parser read the input line by line.
 Hence, there is a maximal input buffer size.
 @section Usage
 The minimum usage consists on creating two objects, the stream
 and the item, then read all xml items of the input.
 Depending of the type, each XML elements should be treated.
 Notice that attributes are seen as separated XML elements.
 User has to keep track (ex: by a stack), on the XML tree structure.
 Stream does not provide anything to manage the XML structure, it is
 just a simple raw syntax parser oriented toward speed and efficiency.
 @code
 * Stream xml;
 * Stream::Item item;
 * head(xml,argc,argv,"explanation of the module");
 *
 * while (xml.read(item)) {
 *   switch (item.getType()) {
 *    case Stream::intag :
 *      ...
 *      xml.write(item);
 *      break;
 *    case Stream::outag :
 *      xml.write(item);
 *      ...
 *      break;
 *    case Stream::attribute :
 *      ...
 *      break;
 *    default :
 *      xml.write(item);
 *      break;
 *    }
 *  }
 @endcode 
 *************************************************************************/


#ifndef __XMLFILTER_H
#define __XMLFILTER_H

///For accessign large files greater than 2Gb
#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64
#include <stdio.h>

#include <assert.h>
/// for ctime un the header
#include <time.h>  
#include <stdlib.h> // Pour abort, atof
#include <string>
#include <string.h>  // pour strerror
#include <errno.h>

#include "error.h"

#include "tools.h"
using IOTA::tos;


using namespace std;

/// Enables fast XML input browsing

namespace XML
{
    class Stream;
    
    // Sort un header dans la sortie xml qui contient deux lignes
	// la première est le nom du module et son nom en clair (nomModule) avec l'auteur
	// la deuxième est le ligne de commande avec la date
	/// Ouput an XML header with module name, cammande line and date
	void head(Stream& xml, ///< XML output
			  int argc,       ///< Module standard argc value
			  const char * argv[],  ///< Standard ligne arguments 
			  const char* moduleName ///< Name of the module
			  );
	
	
    class Stream {
	public:
		/// The type of XML element that are recognized
		enum type {nothing,      ///< Empty
			notknown,     ///< unknown type
			word,         ///< text between two xml tags
			instruction,  ///< instruction tag in the format : <? instruction
			inComment,    ///< Start of an XML comment format <!--
			comment,      ///< text inside a comment
			empty,        ///< the end of and empty tag :  />, in this case item is the name of the tag like outag
			intag,        ///< a starting tag
			outag,        ///< and ending tag
			attribute,    ///< attribute in a tag
			dtd,          ///< attribute starting by  "<!" and not followed by --
			eol,          ///< end of line
			eolInTag,     ///< end of line inside a tag
			eolInComment, ///< end of line inside a comment
			eof,          ///< end of file
			error         ///< should not be used
		};
	public:
		/// Describe an XML item: a name, a type and a value for attributes
		/// The type refers to 
		class Item {
			friend class Stream;
		public:
			/// An empty Item
			Item() : n(NULL), v(NULL), t(error), bufferId(-1) {};
			/// An Item with its type and value
			Item(type _t, string s): n((unsigned char*)s.c_str()), v(NULL), t(_t), bufferId(-1) {}; 
			Item(unsigned char * _n, unsigned char * _v, type _t ): n(_n), v(_v), t(_t), bufferId(-1) {}; 
			
            type getType() const {return t;}
            
            /// Set only the name of the item
            void setName(const char* name) {n = (unsigned char*)name;}
			
			/// Get the name if the item in an byte format (for accents)
			unsigned char* getName() {return n;}
			/// Get the name if the Item in an byte format (for accents)
			const unsigned char* getName() const {return n;}
			/// Get the value of the Item
			unsigned char* getValue() {return v;}
			/// Get the value of the Item
			const unsigned char* getValue() const {return v;}
			/// Get the value as a C string
			char* getValueStr() {return (char*)v;}
			/// Get the value of the Item as a float
			/// WARNING POTENTIAL CONVERSION ERROR
			float getValueFloat() const {return atof((char*)v);}
			/// Get the value of the Item as an unsigned
			/// WARNING POTENTIAL CONVERSION ERROR
			// Must be treated by an abort assert or exception
			unsigned getValueUnsigned() const {return atol((char*)v);}
			/// Get the value of the Item as a string
			string getValueString() const {return string((char*)v);}
			/// Get the name of the Item
			char* c_str() {return (char*) n;}
			/// Get the name of the item
			const char* c_str() const {return (char*) n;}
			
			//@{
			/// Test between item and string. To simplify the writing of tag filter
			friend bool operator == (const Item&,const char*);
			friend bool operator == (const char*,const Item&);
			friend bool operator != (const Item&,const char*);
			friend bool operator != (const char*,const Item&);
			friend bool operator == (const Item&,const string);
			friend bool operator == (const string,const Item&);
			friend bool operator != (const Item&,const string);
			friend bool operator != (const string,const Item&);
			//@}
			
		private:
			unsigned char* n;  // Nom de l'item
			unsigned char* v;  // valeur pour les attributs uniquement
			type t; // type de l'item
			long bufferId; // ID unique correspondant au No de buffer
			// ou les pointeurs n et v atterissent, pour detecter des incoherences
			// Chaque nouveux buffer porte un nouveau numero
			// Non utilisé pour l'instant
			// a utiliser lors de copie d'Items
		};
		
	public:
		/// XML fiter of input and output
		Stream(
			   /// XML Input, by default it is stdin
			   /// \pre The file i should be allready open
			   FILE* i=stdin, 
			   /// XML output, by default it is stdout
			   FILE* o=stdout,
			   char minC=' '    // caractere min pour la construction des mots
			   // La valeur 31 permet de récuperer une ligne
			   // au lieu d'un mot séparé par tout caractère inférieur
			   // ou égal à minC, par défaut l'espace (32)
			   ) :
		in(i), out(o),  posIn(bufferIn),  posOut(bufferOut), posOutMax(bufferOut+bSize-1),
		state(nothing), lastwriten(nothing), typeToClose(nothing),
		minChar(minC), fposbuffer(0), _skipedSpaces(0), lnNb(1), outWith(0), formatI("%i"), formatU("%u") {};
		
		/// Verify that the last tag is correctly closed
		~Stream();
		
		/// \brief Read the next XML item
		/// \return true if not at end of file
		bool read(Item&);
		
		/// \brief Read the next XML item, and indicate the spaces skiped befor
		bool read(Item&,int& skipedSpaces);
		
		/// \brief Next type to read
		// ATTENTION : NE MARCHE PAS AU DEBUT : faire start()
		type next() const {return state;}
        
        
        /// \brief Read XML header, return true if found
        /// If throughOut is true then output the xml header
		bool header(bool throughOut=false);
		
		/// \brief Skip all end of line and comment tags, count lineNb
		void skipEolComment();
        
		
		/// Return the current position in the input file
		off_t ftell(const Item&);
		// Donne la position dans le fichier
		// de l'élément lu
		// Attention ne fonctionne qu'avec un item
		// qui vient juste d'être lu par read
		
		static int errorNo; 
		static const char *errorMess;
		// Global, le No de l'erreur et message
		
		/// Positionne une erreur
		void setError(int eNb, 
					  const char* mess /// error message
					  );
		
		/// Try to recover from an XML syntax error
		void errorRecover();
		
		// Sorties
		
		/// \brief Force output to be done. 
		/// Usefull in interctive mode, and for communication with sockets
		/// but the communication is less efficient
		void flush();
		
		/// Output any string, automatically close the previous tag
		void write(const unsigned char*);
		/// Output any string, automatically close the previous tag
		void write(const char*);
		
		/// Output a word with a space before is necessary. Automatically close the previous tag
		void writeWord(const unsigned char*);
		/// Output a word with a space. Automatically close the previous tag
		void writeWord(const char*);
		
		
		void write(const Item&);
		// Gère correctement la fermeture de la balise
		// après les attributs, les espaces entre les mots
		// et les sauts de ligne en trop
		
		string toXml(const Item& item);
		
		/// Output a string
		friend Stream& operator <<(Stream&,const char*);
		/// Output a string of byte
		friend Stream& operator <<(Stream&,const unsigned char*);
		/// Output a char
		friend Stream& operator <<(Stream&,const char);
        /// Output an int
		friend Stream& operator <<(Stream&,int);
		friend Stream& operator <<(Stream&,unsigned);
		friend Stream& operator <<(Stream&,float);
		friend Stream& operator <<(Stream&,const Item&);
		friend Stream& operator <<(Stream&,string);
        
        /// Set with for int output
        void setw(int n);
        
		/// New line and flush the output
		void flushLine();
		
		// Acces au No de ligne
		// La numérotation démarre à 1
		// C'est 1 plus le nombre de \n
		unsigned lineNb();
		// Force une valeur au No de ligne
		void lineNb(unsigned);
        
		
	private:
		// Methodes a usage interne
		
		void start();
		// Demarre l'analyse du premier item
		
		void skipSpace();
		// saute tout caractère de séparation, conserve le nombre de caractres sauts
        
		void skipName();
		// Se positione juste apres un nom
		
		void closeType(type typeToWrite);
		// termine le type en cours 
		// pour une balise en cours d'ouverture
		// la ferme, pour un word, met un espace
		// En fonction du type de l'item en cours dans typeToClose
		// et en fonction du type de l'item à produire
		
		string closeTypeString(type CLtypeToWrite);
		// equivalent en chaine de carractere
		
		void seek(); 
		// recherche le début de quelque chose
        
        // regarde si une séquence de caractère est présente
        // dans le buffer à partir de la position courante posIn
        // IMPORTANT: ne modifie pas posIn
        bool test(const char*) const;
		
		void seekComment();
		// Recherche la suite ou la fin d'un commentaire : -->
		// ou la fin de ligne d'un commentaire
		
		void output(const char*);
		void output(const char);
		void output(const char,const char);
		void output(const unsigned char*);
		// sort la chaine ou le caractère dans le buffer de sortie
		// ne fait aucun traitement
		
		void fputBuffer();
		// Ecrit par fput le contenu du buffer et le vide
		
		bool isFirstCharName(const unsigned char);
		// True if char is the first char of a name
		
		char * readBufferIn();
		// Read a new line and feed the current input buffer
		
	private:
		FILE* in;
		FILE* out;
		static unsigned const bSize = 512*1024; // taille des buffers
		unsigned char bufferIn[bSize];
		unsigned char bufferOut[bSize];
		unsigned char* posIn; // Position du caractère à rendre
		unsigned char* posOut; // Position du caractère à écrire
		unsigned char* posOutMax; // Position max du dernier caractère du buffer
		unsigned char* currentTag; // Position of the string of the tag: only valide befor the end of the tag !!
		type state; // Etat de l'automate de lecture pour le prochain element a lire
		
		type lastwriten;   // type du dernier item lu
		type typeToClose;  // type de la balise en cours non fermée
		// pour un word il fau mettre un espace, pour les autres
		// balises il faut les fermer apres les attributs
		// utilisé par closeType
		
		// equivalent pour transformer le xml en string
		Stream::type CLtypeToClose;
		Stream::type CLlastwriten;
		
		
		char minChar;  // caractere ASCII borne inf des caractères
		// acceptés comme mot
		off_t fposbuffer;
		// Position dans le fichier du buffer
		// Sert à reperer ce qu'on a lu par rapport
		// au fichier pour un index
		int _skipedSpaces;
		
		// Pour gener un No de ligne  la lecture
		unsigned lnNb;
        
        // Formatage de sortie : taille fixe en caractères
        int outWith;
        std::string formatI; // Format des int
        std::string formatU; // Format des unsigned int
	};
	
	
    /// Recherche séquenciellement dans une table de chaine si l'item (tag) est présent.
    /// Utilisé avec une liste de tag dans la ligne de commande.
	bool goodTag(Stream::Item item,const char * stringTab[],size_t tabSize);
    
    
	
	///////////////// INLINE ZONE ////////////////////////////////////////
	
	// intialisation de l'erreur globale
	
	int Stream::errorNo = -1;
	const char *Stream::errorMess = "No error";
	
	inline
	Stream::~Stream() {
		flush();
	}
	
	inline
	void Stream::start() {
		// On doit être à l'état initial
		assert(state == nothing);
		currentTag=NULL;
		// Chargement du premier buffer
		if (readBufferIn() == NULL) {
			// File is empty
			WARNING<<"Warning from XML::Stream::start()";
			WARNING<<"Input is empty";
			state=eof;
			return;
		}
		// Repositionne pour l'invariant
		// test l'entrée dans une balise
		seek();
	}
	
	inline
	unsigned Stream::lineNb() { return lnNb;};
	
	
	inline
	void Stream::lineNb(unsigned ln) { lnNb=ln;};
	
	
	
	inline
	bool operator == (const Stream::Item& i,const char* s) {
		return strcmp((const char*)i.n,s) == 0;
	}
	
	inline
	bool operator == (const char* s,const Stream::Item& i) {
		return strcmp((const char*)i.n,s) == 0;
	}
	
	inline
	bool operator != (const Stream::Item& i,const char* s) {
		return strcmp((const char*)i.n,s) != 0;
	}
	
	inline
	bool operator != (const char* s,const Stream::Item& i) {
		return strcmp((const char*)i.n,s) != 0;
	}
	
	inline
	bool operator == (const Stream::Item& i,const string s) {
		return i == s.c_str();
	}
	
	inline
	bool operator == (const string s,const Stream::Item& i) {
		return s.c_str() == i;
	}
	
	inline
	bool operator != (const Stream::Item& i,const string s) {
		return i != s.c_str();
	}
	
	inline
	bool operator != (const string s,const Stream::Item& i) {
		return s.c_str() != i;
	}
	
	inline
	void Stream::fputBuffer() {
		assert(posOut<=posOutMax);
		*posOut = 0;
		fputs((char*)bufferOut,out);
		posOut = bufferOut;
	}
	
	inline
	void Stream::flush() {
		if (posOut != bufferOut) {
			// Buffer non vide
			*posOut = 0;
			fputs((char*)bufferOut,out);
			posOut = bufferOut;
		}
		fflush(out);
	}
	
	inline
	void Stream::closeType(type typeToWrite) {
		// En fonction du type à fermer
		switch (typeToClose) {
			case intag :
				if (typeToWrite == empty) {
					output("/>");
					typeToClose = nothing;
					
				} else {
					if ((typeToWrite != attribute) &&
						(typeToWrite != eolInTag)) {
						output('>');
						typeToClose = nothing;
					}
				}
				break;
			case inComment:
				if ((typeToWrite != comment) &&
					(typeToWrite != eolInComment)) {
					output("-->");
					typeToClose = nothing;
				}
				break;
			case instruction :
				if (typeToWrite != attribute) {
					output('?','>');
					typeToClose = nothing;
				}
				break;
			case word:
				// On ajoute un espace qu'entre deux mots
				if (typeToWrite == word) {
					output(' ');
				}
				typeToClose = nothing;
				break;
			case nothing:
			case notknown:
				break;
			default :
				BUG<<"### Stream internal error in closeType : typeToClose = "<<typeToClose;
				assert(false);
				// Impossible
				break;
		}
	}
	
	inline
	void Stream::write(const unsigned char* p) {
		// Ferme eventuellement la balise en cours
		closeType(notknown);
		output(p);
		typeToClose = notknown;
		lastwriten = notknown;
	}
	
	inline
	void Stream::writeWord(const unsigned char* p) {
		// Ferme eventuellement la balise en cours
		closeType(word);
		output(p);
		typeToClose = word;
		lastwriten = word;
	}
	
	inline
	void Stream::writeWord(const char* p) {
		writeWord((const unsigned char*)p);
	}
	
	inline
	void Stream::output(const unsigned char* p) {
        assert(p != NULL);
		while (*p!=0) {
			if (posOut >= posOutMax) {
				fputBuffer();
			}
			*posOut++ = *p++;
		}
	}
	
	
	inline
	void Stream::output(const char* p) {
		while (*p!=0) {
			if (posOut >= posOutMax) {
				fputBuffer();
			}
			*posOut++ = *p++;
		}
	}
	
	inline
	void Stream::output(const char p) {
		if (posOut >= posOutMax) {
			fputBuffer();
		}
		*posOut++ = p;
	}
	
	inline
	void Stream::output(const char p1, const char p2) {
		if (posOut+1 >= posOutMax) {
			fputBuffer();
		}
		*posOut++ = p1;
		*posOut++ = p2;
	}
	
	inline
	void Stream::write(const char* p) {
		write((const unsigned char*)p);
	}
	
	inline
	void Stream::write(const Item& item) {
		// Ferme eventuellement la balise en cours
		closeType(item.getType());
		// Sortie de l'item
		switch (item.getType()) {
			case instruction :
				output('<','?');
				output(item.c_str());
				typeToClose = instruction;
				break;
			case inComment :
				output("<!--");
				output(item.c_str());
				typeToClose = inComment;
				break;
			case comment :
				output(item.c_str());
				typeToClose = inComment;
				break;
			case eolInComment:
				output('\n');
				typeToClose = inComment;
				break;   
			case dtd :
				output('<','!');
				output(item.c_str());
				output('>');
				typeToClose = nothing;
				break;
			case outag :
				output('<','/');
				output(item.c_str());
				output('>');
				typeToClose = nothing;
				break;
			case intag:
				output('<');
				output(item.c_str()); 
				typeToClose = intag;
				break;
			case empty :
				// fait dans closeType
				break;
			case word:
				output(item.c_str());
				typeToClose = word;
				break;
			case attribute :
				output(' ');
				output(item.c_str());  
				output('=','"');
				output(item.getValue());  
				output('"');
				//typeToClose is still the same intag or instruction
				break;
			case eolInTag:
				output('\n');
				typeToClose = intag;
				break;
			case eol:
				if (lastwriten != eol) {
					output('\n');
				}
				typeToClose = nothing;
				break;
			case error :    
				output("\n### XML ERROR : ");
				if (item.getValue()!= NULL) {
					output(item.getValue());
				}
				if (item.c_str() != NULL) {
					output(" AT ");
					output(item.c_str());
				}  
				output("\n");
				flush();
				break;
			default:
				BUG<<"Stream internal error in write item. Type = "
				<<item.getType();
				abort();
				break;
		}
		lastwriten = item.getType();
	}
	
	inline
	Stream& operator <<(Stream& out,const char* s) {
		out.write(s);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,string s) {
		out.write(s.c_str());
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,const unsigned char* s) {
		out.write(s);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,const char c) {
		out.output(c);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,const Stream::Item& item) {
		out.write(item);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,int i) {
		char buffer[32];
		sprintf(buffer,out.formatI.c_str(),i);
		out.write(buffer);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,unsigned u) {
		char buffer[32];
		sprintf(buffer,out.formatU.c_str(),u);
		out.write(buffer);
		return out;
	}
	
	inline
	Stream& operator <<(Stream& out,float f) {
		char buffer[32];
		sprintf(buffer,"%.10g",f);
		out.write(buffer);
		return out;
	}
	
	inline
	void  Stream::flushLine() {
		closeType(eol);
		output('\n');
		flush();
	} 
	
    inline
    bool Stream::test(const char* w) const {
        const unsigned char* p = posIn;
        while (*p && *w && (*p == *w)) {
            p++;w++;
        }
        // *p==0 || *w==0 || (*p != *w)
        return (*p == 0) || (*w == 0);
    }
    
	inline  
	void Stream::skipName() {
		// Definition plus stricte que la norme
		// Accepte les catactres au dessus de 128, par exemple pour de l'UTF-8
		// dans les noms de balises ou d'attributs
		while ((*posIn != 0) && 
			   (((*posIn>='a') && (*posIn<='z')) ||
				((*posIn>='A') && (*posIn<='Z')) ||
				((*posIn>='0') && (*posIn<='9')) ||
				(*posIn=='.') ||
				(*posIn=='-') ||
				(*posIn=='_') ||
				(*posIn==':') ||
				(*posIn>=128)))
			++posIn;
	}
	
	
	inline  
	void Stream::skipSpace() {
		_skipedSpaces=0;
		while ((*posIn != 0) && (*posIn<=' ') && (*posIn!='\n')) {			
			++posIn;++_skipedSpaces;
		}
		if (*posIn==0) {
			// Normalement impossible : toute ligne doit se terminer par un retour chariot
			// car lecture par fgets
			// sauf 1) en cas de fichier qui se termine sans fin de ligne
			//      2) ou si le caractre 00 est trouv
			//      3) le buffer sature et on n'a pas pu mettre toute la ligne dedans
			// en effet, tout buffer se termine par une fin de ligne a cause de fgets de read
			if (feof(in)) {
				// A la fin du fichier donc terminaison sans retour chariot : Ok
				// le fichier se termine sans fin de ligne
				state = eof;
				return;
			} else {
				// on n'est pas a la fin du fichier : donc problme
				// Deux cas : le caractre 00 ou dbordement de buffer
				if (posIn < bufferIn+bSize -1) {
					// 00 n'est pas le dernier caractère du buffer
					// donc c'est un caractère !
					BUG<<"### ERROR IN Stream::skipSpace";
					BUG<<"### FOUND 00 CHARACTER";
					if (fposbuffer != -1) {
						BUG<<" AT "<<fposbuffer+(posIn-bufferIn);
					} 
					BUG<<"### buffer position="<<(unsigned long)(posIn-bufferIn);
					// Affiche le buffer
					for (int i=0;i<posIn-bufferIn;i++) {
						BUG<<bufferIn[i];
					}
					abort();
				} else if (posIn == bufferIn+bSize -1) {
					// dernier caractère du buffer : donc buffer saturé
					BUG<<"### INPUT ERROR IN Stream::skipSpace";
					BUG<<"### INPUT BUFFER OVERFLOW : TRY INCREASE BUFFER SIZE";
					BUG<<"### CURRENT MAX BUFFER SIZE = "<<bSize;
					BUG<<"### CURRENT BUFFER CONTENT : ";
					// Replace les espaces dans le buffer pour l'afficher correctement
					for (posIn=bufferIn;posIn<bufferIn+bSize-1;++posIn) {
						if (*posIn == 0) 
							*posIn= ' ';
					}
					BUG<<bufferIn;
					abort();
				} else {
					// Cas impossible hors borne du buffer
					BUG<<"### INTERNAL ERROR IN Stream::skipSpace";
					BUG<<"### POINTER OUT OF BOUND : SHOULD NEVER HAPPEND !";
					BUG<<"### BUG TO BE CORRECTED : TOO BAD ...";
					abort();
				}
			}
		}
	}
    
	inline 
	bool Stream::isFirstCharName(const unsigned char c) {
		// Very simplfified version
		// See http://www.w3.org/TR/2004/REC-xml-20040204/#NT-Letter
		return c >= 0x0041;  // au dessus de 'A'
	}
	
	inline 
	void Stream::seek() {
		skipSpace();
		if (state==eof) {
			return;
		}	
		if (*posIn=='\n') {
			// Il faut lire des lignes entières 
			// sinon il y a risque de coupure du buffer
			// le buffer doit etre vide a la fin d'une ligne
			++posIn;
			if (*posIn != 0) {
				// problème, le buffer n'est pas vide (impossible)
				BUG<<"### BUFFER INCONSISTENCY in Stream::seek";
				BUG<<bufferIn;
				abort();
			}
			// Incrémente le comptage du nombre de lignes
			++lnNb;
			// etat normal : fin de ligne
			state=eol;
			return;
		}
		
		// le caractère courant n'est pas un espace et est non nul 
		// Regarde si on débute un autre tag
		if(*posIn=='<') {
			++posIn;
			// Regarde si c'est un tag de fin ou d'instruction
			switch (*posIn) {
				case 0 :
					state = nothing;
					BUG<<"### ERROR IN XMLFinter::seek ; null car found in seeking a tag";
					abort();
				case '/' :
					state=outag;
					++posIn;
					skipSpace();
					break;
				case '?' :
					state=instruction;
					++posIn;
					skipSpace();
					break;
				case '!' :
					// Un commentaire ou un élément de DTD
					++posIn;
					if (*posIn=='-') {
						// vérifie l'autre '-'
						++posIn;
						if (*posIn=='-') {
							++posIn;
							state=inComment;
						} else {
							state = dtd;
						}
					} else {
						// DTD non traité en détail
						state=dtd;
					}
					break;
				default :
					// Le nom d'un tag doit débuter par une lettre
					if (isFirstCharName(*posIn)) {
						state = intag;
					} else {
						// a < not followed by a name
						state = error;
						setError(7,"a tag name must start with a character after <");
					}
					break;
			}
		} else {
			//On est sur le début d'un mot
			state = word;
		}
	}
	
	inline
	void Stream::seekComment() {
		// Recherche caractères de fin de commentaire : -->
		// ou la suite du commentaire ou eol
		// Passe le commentaire en lui même
		// sans détailler les espaces
		char c1,c2;
		c1=0;c2=0;
		while ((*posIn != 0) &&
			   (*posIn != '\n') &&
			   (c1 != '-' || c2!= '-' || *posIn!='>') &&
			   (*posIn != '>')) {
			c1=c2;c2=*posIn;++posIn;
		}
		switch (*posIn) {
			case '\n':
				// found and eof inside a comment
				state = eolInComment;
				// Incrmente le nombre de ligne
				++lnNb;
				// Place le zero de fin de chaine
				*posIn=0;
				break;
			case 0:
				setError(1,"End of file or buffer to small, comment not closed by \"-->\"");
				state = error;
				break;
			case '>':
				if ((c1=='-')&&(c2=='-')) {
					// fin correcte de balise
					// place la fin de chaine juste avant
					*(posIn-2)=0;
					// Saut le caractère >
					++posIn;
					//et repart sur une recherche normale de l'état suivant
					seek();
				} else {
					// Fin de balise incorrecte
					setError(10,"comment closed by '>' not by '-->'");
					// Saute ce caractere traite pour pouvoir poursuivre
					// par errorRecover(), finit la chaine par zero
					// pour pouvoir afficher quand même le bon commentaire
					*posIn=0;
					++posIn;
					state = error;
				}
				break;
			default:
				// impossible
				assert(false);
		}
	}
	
	inline
	void Stream::errorRecover() {
		if (*posIn != 0)
			seek();
		else
			state=nothing;
	}
	
	
	inline
	off_t Stream::ftell(const Item& item) {
		//BUG<<"fposbuffer="<<fposbuffer;
		if (fposbuffer == -1) {
			BUG<<"### WARNING IN Stream::ftell";
			BUG<<"### ftell not possible into a pipe or fifo";
		}
		switch(item.t) {
			case intag :
				// Il faut se positionner sur le <
				return fposbuffer+(item.n-bufferIn-1);
			default :
				return fposbuffer+(item.n-bufferIn);
		}
	}
	
	// Read a new line and feed the current input buffer
	inline 
	char *  Stream::readBufferIn() {
		fposbuffer = ftello(in);
		if (fposbuffer == -1) {
			switch (errno) {
				case ESPIPE : // it is a pipe so not possible to ftell, it is NOT an error
					break;
				default :
					BUG<<"### ERROR IN Stream::readBufferIn";
					BUG<<"### ftell error "<<errno<<" : "<<strerror(errno);
					abort();
					break;
			}
		}
		posIn = bufferIn;
		return fgets((char*)bufferIn,bSize,in);
	}
	
	inline
	bool Stream::read(Item& item,int& skipedSpaces) {
		// return the skiped spaces before reading
		skipedSpaces = _skipedSpaces;
		// Reset the space counter
		_skipedSpaces =0;
		return read(item);
	}
    
    
    inline
	bool Stream::header(bool throughOut) {
		// Il faut d'abord savoir si on a démmaré
		// la lecture pour connaitre l'état du prochain
		// item à lire
		if (state == nothing) {
			// lecture non démarrée
			start();
		}
        Item item;
        Item attrib;
        // Saute les lignes vides
        while (state == eol) {
           read(item); 
        }
        if (state == instruction) {
            // Format <?
            // Normalement c'est uniquement en minuscule
            // Test sans faire de lecture !
            if (test("xml") || test("XML")) {
                // On peut lire le nom de l'instruction
                read(item);
                if (throughOut) write(item);
                // Passe tous les attributs
                // CHANGER ICI SI ON VEUT RECUPERER LES ATTRIBUTS DE XML
                while (state == attribute) {
                    read(attrib);
                    if (throughOut) write(attrib);
                }
                // Saute les lignes vides
                while (state == eol) {
                    read(item); 
                    if (throughOut) write(item);
                }
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
	}
    
	
	inline
	void Stream::skipEolComment() {
		// Il faut d'abord savoir si on a démmaré
		// la lecture pour connaitre l'état du prochain
		// item à lire
		if (state == nothing) {
			// lecture non démarrée
			start();
		}
		Item item;
		while (state == eol || 
			   state == eolInTag ||
			   state == eolInComment ||
			   state == comment ||
			   state == inComment) {
			read(item);
		}
	}
	
	inline
	bool Stream::read(Item& item) {
		// Invariant : 
		// *posIn est sur le premier caractère d'un mot ou 0
		// state indique ce qui a été trouvé
		unsigned char* posEnd; // Temporaire, désigne la fin de la chaine
		char coteValue; // le caractère de cote de valeur, " ou ' ou rien
		assert(posIn>=bufferIn);
		if (state == eof) {
			return false;
		}
		// clean value
		item.v = NULL;
		// L'invariant est valide
		switch (state) {
			case nothing :
				// etat initial uniquement
				currentTag=NULL;
				// Chargement du premier buffer
				if (readBufferIn() == NULL) {
					// File is empty
					BUG<<"### Warning from Stream::read";
					BUG<<"### Input is empty";
					state=eof;
					item.n = NULL;
					item.t = eof;
					return false;
				}
				// Repositionne pour l'invariant
				// test l'entrée dans une balise
				seek();
				// Relance la lecture: analyser must have one step ahead
				assert(state != nothing);
				return read(item);
				break;
			case eol :
				item.n = NULL;
				item.t = eol;
				// Fin de la ligne et du buffer
				// Il faut recharger le buffer d'une ligne
				if (readBufferIn() == NULL) {
					// plus rien a lire
					state=eof;
				} else {
					// Cherche le prochain etat
					// test l'entrée dans une balise
					seek();
				}
				break;
			case word :
				item.n = posIn;
				item.t = word;
				// Passe le mot pour trouver la fin
				// délimitée par un caractère espace ou le début d'une balise
				while ((*posIn>minChar) && (*posIn!='<'))
					++posIn;
				// Repere la position de fin
				posEnd=posIn;
				// Recherche le type suivant 
				seek();
				// Position la fin de chaine
				*posEnd=0;
				break;
			case inComment :
				item.n = posIn;
				item.t = inComment;
				// Recherche l'etat suivant : la suite du commentaire
				// ou la fin donc autre chose
				seekComment();
				break;
			case comment:
				item.n = posIn;
				item.t = comment;
				// Recherche la fin ou la suite du commentaire
				seekComment();
				break;
			case eolInComment:
				item.n = NULL;
				item.t = eolInComment;
				// Il faut recharger le buffer
				if (readBufferIn() == NULL) {
					// Fin du fichier
					state = eof;
				} else {
					// il faut deternier l'etat suivant
					// sachant qu'on est dans un commentaire
					switch(*posIn) {
						case '\n':
							++lnNb;
							state=eolInComment;
							break;
						case '-':
							if((*(posIn+1)==' ')&&(*(posIn+2)=='>')) {
								// fin d'un commentaire, seek normal
								posIn +=2;
								seek();
							} else {
								state=comment;
							}
						default:
							state=comment;
					}
				}
				break;
			case dtd :
				item.n = posIn;
				item.t = dtd;
				// Recherche caractère de fin de dtd : >
				while((*posIn != 0) &&  (*posIn!='>')) {
					++posIn;
				}
				if (*posIn!=0) {
					*posIn=0;
					++posIn;
					seek();
				} else {
					setError(2,"dtd not closed by \">\"");
					state = error;
				}
				break;
			case instruction :
				item.n = posIn;
				item.t = instruction;
				// Saute le nom de l'instruction
				skipName();
				// Conserve la position de fin de chaine
				posEnd=posIn;
				// Traite le caractère de délimitation
				if (*posIn==0) {
					// Balise non fermée sur le buffer
					setError(14,"instruction not closed by \">\"");
					item.t = error;
					state = error;
					break;
				}
				skipSpace();
				// Test fin de tag '?>' ou les attributs 
				if (*posIn=='?') {
					++posIn;
					if (*posIn=='>') {
						// Fin balise
						++posIn;
						// Recherche la prochaine info
						seek();
					} else {
						// fermeture balise incorrecte
						setError(15,"instruction not closed by \"?>\"");
						state = error;
					}
				} else {
					// On est sur des attributs
					state=attribute;
				}
				// Fin du traitement, marquage de la fin de chaine trouvée
				*posEnd=0;
				break;
			case empty :
				// currentTag should be valid
				assert(currentTag != NULL);
				item.n = currentTag;
				item.t = empty;
				// On doit être sur un >
				if (*posIn!='>') {
					setError(3,"missing \">\" in empty tag");
					state = error;
					break;
				}
				// currentTag no more valid
				currentTag=NULL;
				++posIn;
				seek();
				break;
			case intag :
				currentTag=posIn;
				// WARNING: this value is only valid during tag because of possible buffer change
				// used ONLY for empty tag
				item.n = posIn;
				item.t = intag;
				// Saute le nom du tag
				skipName();
				// Conserve la position de fin de chaine
				posEnd=posIn;
				skipSpace();
				// Test fin de tag
				switch (*posIn) {
					case '/' :
						++posIn;
						// Pointe sur le > final
						state=empty;
						break;
					case '>' :
						++posIn;
						// Cherche l'état suivant
						seek();
						break;
					case '\n':
						// une fin de ligne apres le nom du tag avant un attribut ou la fermeture
						++lnNb;
						state = eolInTag;
						break;
					default :
						// C'est un attribut
						state=attribute;
						break;
				}
				// Caractère de fin de chaine pour le nom
				*posEnd=0;
				break;
			case eolInTag:
				item.n = NULL;
				item.t = eolInTag;
				// Il faut recharger le buffer
				if (readBufferIn() == NULL) {
					// Fin du fichier
					state = eof;
				} else {
					// il faut determiner le nouvel état 
					skipSpace();
					// Test fin de tag
					switch (*posIn) {
						case '/' :
							++posIn;
							// Pointe sur le > final
							state=empty;
							break;
						case '>' :
							++posIn;
							// Cherche l'état suivant
							seek();
							break;
						case '\n':
							// encore une fin de ligne dans le tag
							++lnNb;
							state = eolInTag;
							break;
						default :
							// C'est un attribut
							state=attribute;
							break;
					}
				}
				break;
			case outag :
				// currentTag no more valid
				currentTag=NULL;
				item.n = posIn;
				item.t = outag;
				skipName();
				// Conserve la position de fin de nom
				posEnd=posIn;
				skipSpace();
				if (*posIn!='>') {
					setError(4,"missing \">\" in end tag");
					state=error;
				} else {
					// Debute-on un autre tag ?
					++posIn;
					seek();
				}
				// Marque la fin du nom
				*posEnd=0;
				break;
			case attribute :
				item.n = posIn;
				item.t = attribute;
				// Passe pour trouver le nom de l'attribut
				skipName();
				posEnd=posIn;
				skipSpace();
				// Recherche du '='
				if (*posIn == '\n') {
					++lnNb;
					setError(8,"Sorry : end of line '\\n' not allowed before a '=' of a tag attribute");
					item.t=error;
					state=error;
					break;
				}
				if (*posIn != '=') {
					setError(5,"no = found after attribute");
					item.t = error;
					state=error;
					break;
				}
				++posIn;
				// Place la fin de chaine du nom
				*posEnd=0;
				skipSpace();
				if (*posIn == '\n') {
					++lnNb;
					setError(9,"Sorry : end of line '\\n' not allowed after a '=' of a tag attribute");
					item.t=error;
					state=error;
					break;
				}
				// Recherche de la valeur
				// Normalement doit dbuter par " ou par ' (c'est dans la norme)
				// mais accepte " et ' et rien du tout
				if ((*posIn == '"') || (*posIn == '\'')) {
					coteValue = *posIn;
					++posIn;
				} else {
					// C'est une erreur
					setError(11,"Attribute must start with a single or double quote");
					item.t=error;
					state=error;
					break;
				}
				item.v=posIn;
				// Recherche de la fin de la valeur, le new line est interdit
				while (*posIn !=0 && *posIn!='\n' && *posIn!=coteValue) {
					++posIn;
				}
				// Note la position d'arret
				posEnd=posIn;
				// Si c'est " ou ' alors c'est traité
				if (*posIn==coteValue) {
					++posIn;
				} else {
					// cas d'erreurs
					if (*posIn == '\n') {
						++lnNb;
						setError(12,"Sorry: newline not allowed in attribute value (but allowed in XML standart)");
						item.t=error;
						state=error;
						break;
					}
					if (*posIn == 0) {
						setError(13,"no quote found to end the attribute value, null char found instead");
						item.t=error;
						state=error;
						break;
					}        
				}
				// Recherche l'tat suivant
				skipSpace();
				// Test fin de tag
				switch (*posIn) {
					case '/' :
						++posIn;
						// Pointe sur le > final
						state=empty;
						break;
					case '>' :
						++posIn;
						// Cherche l'état suivant
						seek();
						break;
					case '?' :
						// fin des parametres d'une instruction
						++posIn;
						if(*posIn!='>') {
							setError(6,"found '?' without '>'");
							state=error;
							break;
						}
						++posIn;
						seek();
						break;
					case '\n':
						// Un retour chariot dans un tag apres un attribut
						++lnNb;
						state=eolInTag;
						break;
					default :
						// C'est un attribut
						state=attribute;
						break;
				}
				// Marque la fin de la chaine
				*posEnd=0;
				break;
			case error :
				// An error has been found, no item to build
				item = Item();
				break;
			default :
				break;
		}
		return true;
	}
	
	inline
	void head(Stream& xml,int argc, const char * argv[], const char* nomModule) {
		xml<<"<!-- ";
		// Sort la commande
		for (int i=0;i<argc;i++) {
			xml<<argv[i]<<" ";
		}
		xml<<"@ ";
		time_t time_v = time(NULL);
		char * time_asc = ctime(&time_v);
		// Suppression du retour chariot à la fin
		time_asc[strlen(time_asc)-1] = 0;
		xml<<time_asc;
		// Sort un message en clair
		xml<<" ("<<nomModule<<") -->";
		xml.flushLine();
	}
	
    /// \deprecated OBSOLETE 130703 Il ne faut plus utiliser cette version
    /// Utiliser la version avec moins d'arguments, et plus générale
	inline
	bool goodTag(Stream::Item item,int in,int out, const char * argv[]) {
		if ((in==0) || (out==0)) 
			return false;
		for(int i=in;i<out;i++) {
			if (item == argv[i])
				return true;
		}
		return false;
	}
    
    /// Recherche séqunciellement dans une table de chaine si l'item (tag) est présent
    inline
	bool goodTag(Stream::Item item,const char * stringTab[],size_t tabSize) {
		if (tabSize == 0)
			return false;
		for(size_t i=0;i<tabSize;i++) {
			if (item == stringTab[i])
				return true;
		}
		return false;
	}

	
	
	/***********************************************************************
	 @name closeType
	 @param CLtypeToWrite Stream::type
	 @return string
	 
	 retourne la chaine correspondante pour la fermeture du type xml passe
	 en parametre.
	 ***********************************************************************/
	inline
	string Stream::closeTypeString(Stream::type CLtypeToWrite) {
		// En fonction du type à fermer
		string out="";
		switch (CLtypeToClose) {
			case Stream::intag :
				if (CLtypeToWrite == Stream::empty) {
					out+="/>";
					CLtypeToClose = Stream::nothing;
					
				} else {
					if (CLtypeToWrite != Stream::attribute) {
						out+='>';
						CLtypeToClose = Stream::nothing;
					}
				}
				break;
			case Stream::instruction :
				if (CLtypeToWrite != Stream::attribute) {
					out+='?';
					out+='>';
					CLtypeToClose = Stream::nothing;
				}
				break;
			case Stream::word:
				// On ajoute un espace qu'entre deux mots
				if (CLtypeToWrite == Stream::word) {
					out+=' ';
				}
				CLtypeToClose = Stream::nothing;
				break;
			default:
				// rien a faire ?
				break;
		}
		return out;
	}
	
	/***********************************************************************
	 @name toXml
	 @param item Stream::item
	 @return string
	 
	 retourne la chaine correspondante a l'item pass en parametre.
	 ***********************************************************************/
	inline
	string Stream::toXml(const Stream::Item& item) {
		// Ferme eventuellement la balise en cours
		string out="";
		out+=closeTypeString(item.getType());
		// Sortie de l'item
		switch (item.getType()) {
			case Stream::instruction :
				out+='<';out+='?';
				out+=item.c_str();
				CLtypeToClose = Stream::instruction;
				break;
			case Stream::comment :
				out+="<!--";
				out+=item.c_str();
				out+="-->";
				CLtypeToClose = Stream::nothing;
				break;
			case Stream::dtd :
				out+='<';out+='!';
				out+=item.c_str();
				out+='>';
				CLtypeToClose = Stream::nothing;
				break;
			case Stream::outag :
				out+='<';out+='/';
				out+=item.c_str();
				out+='>';
				CLtypeToClose = Stream::nothing;
				break;
			case Stream::intag:
				out+='<';
				out+=item.c_str();
				CLtypeToClose = Stream::intag;
				break;
			case Stream::empty :
				// fait dans closeType
				break;
			case Stream::word:
				out+=item.c_str();
				CLtypeToClose = Stream::word;
				break;
			case Stream::attribute :
				out+=' ';
				out+=item.c_str();
				out+='=';out+='"';
				out+=(char *)item.getValue();
				out+='"';
				break;
			case Stream::eol:
				if (CLlastwriten!= Stream::eol)
					out+='\n';
				break;
			case Stream::error :
				out+="\n### XML ERROR : ";
				out+=(char *)item.getValue();
				out+=" AT ";
				out+=item.c_str();
				out+="\n";
				break;
			default:
				break;
		}
		CLlastwriten = item.getType();
		return out;
	}
	
	inline
	void Stream::setError(int eNo, const char* mess) {
		errorNo = eNo;
		errorMess = mess;
	}
    
    inline
    void Stream::setw(int n) {
        outWith = n;
        // Créee le format avec une taille width
        formatI = std::string("%")+tos(n)+ std::string("i");
        formatU = std::string("%")+tos(n)+ std::string("u");
    }

	
}
#endif


