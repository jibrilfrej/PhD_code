/**
 \file error.h
 \brief Global errors definitions and log files
 \author Jean-Pierre Chevallet (Jean-Pierre.Chevallet@imag.fr)
 \note IOTA Information Retrieval System
 \copyright Laboratoire d'Informatique de Grenoble.
 \copyright Université de Grenoble (Pierre Mendès France UPMF).
 \copyright Centre National de la Recherche Scienfifique (CNRS).
 \date 2012
 \ingroup System
 
 Using log stream
 ----------------
 
 A log stream is used for information output for user or for debug.
 All log stream are defined as MACRO, so are not ins the scope of IOTA
 namespace. The syntax is like stream (ex: cerr), but without end.
 Each log ouput is atmomical and can be used in threads.
 Ex:
 
 ~~~~~~~~~~~~~
 ERROR<<"the file is not open";
 ~~~~~~~~~~~~~
 
 This produces the output:
 
 ~~~~~~~~~~~~~
 # ERROR test_error test_error.cc main(213): the file is not open
 ~~~~~~~~~~~~~
 
 The name of the program, the name of the file, the name of the function
 and the line number are automatically added to the output.
 The stream accepts `const char*`, `string`, `int` and `Error`.
 For `Error` the string and the error code is output.
 (see [LogStream](@ref IOTA::LogStream)  for more details).
 
 There are several output forms:
 - BUG: for strong internal errors that must be investigated.
 All informations are avaible.
 - ERROR: mostly for user error with limited information.
 - WARNING: mostly for user, with limited information.
 - INFO: to follow the computation.
 - DEBUG: trace for debug that should not appears in normal situation.
 - DEBUG1: more details debug informations (level 1).
 - USAGE: special output for the 'usage' (or help) message in case of errors.
 
 The log is by default output on the standard error stream (cerr).
 On can use a file instead using:
 ~~~~~~~~~~~~~
 IOTA::setLogStream("MyProgram.log");
 ~~~~~~~~~~~~~
 
 \warning On can call IOTA::setLogStream() only once.
 
 
 
 */


#ifndef error_h
#define error_h

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <cstring>
#include <time.h>
#include <string>

#include <iostream>
using std::cout;
using std::cerr;
using std::hex;
using std::dec;

#include <sstream>
using std::ostringstream;

#include <ostream>
#include <fstream>

#include "Timer.h"

#include "tools.h"
using IOTA::strcat;
using IOTA::itos;
using IOTA::delSuffix;
using IOTA::skipPath;

/// Le nom du programe est utilisé dans les messages d'erreurs
/// Doit être définit par le programme principal, ou bien est
/// définit dans version.h.
/// ATTENTION: si le programme principal inclus version.h
/// cette variable globale est automatiquement définie.
#ifdef LIB 
extern const char* programName;
#elif !defined(version_h)
const char* programName= "IOTA";
#endif


/// Niveau de log
enum LogLevel {logBUG,logERROR,logWARNING,logINFO,logDEBUG,logDEBUG1};

/// Variable de niveau de log affiché
/// Changer cette variable pour changer les log affichés
// ATTENTION : si par malheur on redefinit la variable __LOGLEVEL__
// Alors les #define sont interprétes AVEC cette variable et cela peut même
// produite une erreur de compilations assez incomprehensible !
// IL NE FAUT PAS UTILISER DE VARIABLE GLOBALE : mais par exemple une variable
// statique qui est INVISIBLE de l'exterieur, ou il ne faut pas utiliser de #define
// car ils sont sensibles au contexte !!!
// Autre solution : utiliser une variable locale mais static
// Solution pour l'instant : changer de nom de initial de logLevel en __LOGLEVEL__
// NE PAS UTILISER CETTE VARIABLE DIRECTEMENT DANS LE CODE : utiliser les fonctions prévues
#ifdef LIB
extern LogLevel __LOGLEVEL__;
#else
LogLevel __LOGLEVEL__=logINFO; ///< Valeur de log par défaut : pas de message de débug.
#endif

// Liste de macro pour les divers messages de logs
// Technique : cree un objet temporaire, detruit à la fin de l'expression
// Ainsi la sortie est atomique pour les theads

#ifdef __ANDROID__
	///les macros de debug pour aller dans android logcat
	/// Pour afficher une erreur interne grave
	#define BUG IOTA::LogStream(ANDROID_LOG_ERROR, "B - ",__FILE__,__func__,__LINE__)

	/// Un message d'erreur utilisateur
	#define ERROR if(__LOGLEVEL__<logERROR);else IOTA::LogStream(ANDROID_LOG_ERROR, "E - ")

	/// Un warning
	#define WARNING if(__LOGLEVEL__<logWARNING);else IOTA::LogStream(ANDROID_LOG_INFO, "W - ")

	/// Une info utilisateur
	#define INFO if(__LOGLEVEL__<logINFO);else IOTA::LogStream(ANDROID_LOG_INFO, "I - ")

	/// Debug niveau 0
	#define DEBUG if(__LOGLEVEL__<logDEBUG);else IOTA::LogStream(ANDROID_LOG_DEBUG, "",__FILE__,__func__,__LINE__)

	/// Debug niveau 1
	#define DEBUG1 if(__LOGLEVEL__<logDEBUG1);else IOTA::LogStream(ANDROID_LOG_VERBOSE, "",__FILE__,__func__,__LINE__)

	/// Affichage de l'utilisation d'une commande
	#define USAGE IOTA::LogStream(ANDROID_LOG_INFO)
#else
	///les macros de debug par defaut
	/// Pour afficher une erreur interne grave
	#define BUG IOTA::LogStream("# ERROR ",__FILE__,__func__,__LINE__)

	/// Un message d'erreur utilisateur
	#define ERROR if(__LOGLEVEL__<logERROR);else IOTA::LogStream("* ERROR in ")

	/// Un warning
	#define WARNING if(__LOGLEVEL__<logWARNING);else IOTA::LogStream("! WARNING ")

	/// Une info utilisateur
	#define INFO if(__LOGLEVEL__<logINFO);else IOTA::LogStream("- ")

	/// Debug niveau 0
	#define DEBUG if(__LOGLEVEL__<logDEBUG);else IOTA::LogStream("0 ",__FILE__,__func__,__LINE__)

	/// Debug niveau 1
	#define DEBUG1 if(__LOGLEVEL__<logDEBUG1);else IOTA::LogStream("1 ",__FILE__,__func__,__LINE__)

	/// Affichage de l'utilisation d'une commande
	#define USAGE IOTA::LogStream()
#endif

/// Fichier de sortie du log, par défaut c'est la sortie standard d'erreur
/// Object de sortie pour les log dans un fichier
#ifdef LIB
extern std::ostream *logStream;  ///< Pointeur vers le flux actuel, en cours d'utilisation
extern std::ofstream logFStream; ///< flux de lof de type fichier
#else
std::ostream *logStream = &std::cerr;
std::ofstream logFStream; ///< Variable globale pour le flux de log de type fichier.
#endif



namespace IOTA {
    
    
    /// List of exiting errors
    enum ExitError {
        exitNoError    = 0, ///< Ok no error
        exitParamError = 1  ///< Wrong parameter
    };
    
    /// Modification de la sortie du log
    void setLogStream(const char*);
    void setLogStream(const std::string&);
    
    
    /// Modification du niveau de log
    void setLogLevel(int level);
    
    // Message d'erreur plus spécifique en clair
    // ATTENTION: n'est pas mis à jour systématiquement !
#ifdef LIB
    // une variable globale ne doit pas être dans le .o d'une une librairie
    // ou dans un .o autre que le programme principal
    // Sinon à l'édition de lien elle en pluseurs exemplaires.
    extern std::string errorMessage;
#else
    std::string errorMessage="(empty error message)";
#endif
    
    void _displayError(const char* file, const char* func, const char* line, const std::string& message);
    
    /// Liste des erreurs de fonctionnement : 32 bits.
    /// Format : Val|Objet|Action|Etat|Flags.
    /// Flags  : 4 bits
    /// Etat   : 8 bits
    /// Action : 8 bits
    /// Objet  : 8 bits
    /// Val    : 4 Bits
    /// Total  = 32 bits
    /// Dernier octet : 8 flags possibles.
    enum Error {
        
        ///////////////////////////////////////////////////////////
        /// Flags 0000000X : 4 bits = 4 flags
        /// Attributs possibles en même temps
        eFlagRead     = 0x1, ///< Lecture
        eFlagWrite    = 0x2, ///< Ecriture
        eFlagFormat   = 0x4, ///< Format incorrect
        eFlagTimeOut  = 0x8, ///< Time out pendant une opération
        
        ///////////////////////////////////////////////////////////
        /// Etat : 00000XX0 : 8 bits = 256 états
        /// Décrit l'état de l'objet qui pose problème
        /// Si Zero : pas d'état
        eEmpty    = 0x010, ///< Objet vide
        eExist    = 0x020, ///< Objet existe
        eNotExist = 0x030, ///< Objet n'existe pas
        eBroken   = 0x040, ///< Interruption
        ePartial  = 0x050, ///< Objet ou action incomplet
        eClosed   = 0x060, ///< Objet non ouvert (fermé)
        eOverflow = 0x070, ///< Dépassement de capacité (ex: tableau)
        eNull     = 0x080, ///< Valeur nulle non autorisée
        eValue    = 0x090, ///< Valeur incorrecte
        eType     = 0x0A0, ///< Type incorrect
        eId       = 0x0B0, ///< Identifiant incorrect
        eKey      = 0x0C0, ///< Clée incorrecte
        eEndianness = 0x0D0, ///< Endianness incorrect
        eEnd      = 0x0E0, ///< A la fin de la structure
        eSize     = 0x0F0, ///< Taille incorrecte
        eLocked   = 0x100, ///< Objet bloqué (locked)
        eNotLocked= 0x110, ///< Objet non bloqué
        eNotSpec  = 0xFF0, ///< Etat non détaillé
        
        
        ///////////////////////////////////////////////////////////
        /// Actions 000XX000 : 8 bits = 256 actions
        /// Decrit l'opération qui pose problème
        eCreate    = 0x01000, // Creation imposible
        eOpen      = 0x02000, // Ouverture impossible
        eClose     = 0x03000, // Fermeture impossible
        eRead      = 0x04000, // Lecture impossible
        eWrite     = 0x05000, // Ecriture impossible
        eTrace     = 0x06000, // Impossible de mettre en place une trace
        eConnect   = 0x07000, // Connection impossible
        eReadSize  = 0x08000, // Lecture de la taille impossible
        eWriteSize = 0x09000, // Ecriture de la taille impossible
        eRemove    = 0x0A000, // Suppression impossible
        eFork      = 0x0B000, // Le fork n'a pas marché
        eLock      = 0x0C000, // Le lock ou unlock n'a pas marché
        eInit      = 0x0D000, ///< l'initialisation n'a pas marchée
        eWait      = 0x0E000, ///< Le wait a échoué
        eSignal    = 0x0F000, ///< Le signal de synchro a echoué

        
        ///////////////////////////////////////////////////////////
        // Objet de l'erreur sur 7 bits  07F00000
        ///////////////////////////////////////////////////////////
        
        // Erreurs relatives aux fichiers
        eFile         = 0x0100000, // Erreur générale sur un fichier
        eFileHeader   = 0x0200000, // Header du fichier incorrect
        eFileSeek     = 0x0300000, // Seek impossible
        eFileFgetpos  = 0x0400000, // fgetpos impossible
        eFileFsetpos  = 0x0500000, // fsetpos impossible
        eFileItem     = 0x0600000, ///< An item of a file
        
        // Erreurs relatives à une memory map
        eMmap         = 0x1000000,
        
        // Erreurs en rapport avec les process
        eSignalComWaitFileBusy = 0x2000000, // le fichier est déjà utilisé pour un autre processus
        
        // Erreurs avec les sockets
        eSocket          = 0x4000000, // Sur les sockets
        eSocketService   = 0x4100000, // Service inconnu
        eSocketProtocol  = 0x4200000, // Protocole inconnu
        eSocketTransport = 0x4300000, // Transport incorrect
        eSocketHost      = 0x4400000, // Nom ou adresse IP incorrect
        eSocketBind      = 0x4500000, // Bind refusé
        eSocketAccept    = 0x4600000, // Accept refusé
        eSocketPoll      = 0x4700000, // Erreur dans un poll
        
        // Erreurs avec les outils de synchronisation
        eMutex           = 0x4800000, ///< Erreur avec un mutex pthread_mutex_t
        eCond            = 0x4900000, ///< Erreurs avec une variable condition pthread_cond_t
        
        // Erreur utilisateur sur un paramètre
        eParam           = 0x5000000,
        
        // Erreur dans le code
        eNotImplemented  = 0x5100000,
        
        ///////////////////////////////////////////////////////////
        // Flag suplementaire 1 bits  08000000
        ///////////////////////////////////////////////////////////

        /// Utilisé par ThreadPool pour indiquer une tâche terminée
        eFlagDone        = 0x8000000,
        
        ///////////////////////////////////////////////////////////
        // Valeurs pour différencer certains cas X0000000
        ///////////////////////////////////////////////////////////
        eVal01 = 0x10000000,
        eVal02 = 0x20000000,
        eVal03 = 0x30000000,
        eVal04 = 0x40000000,
        eVal05 = 0x50000000,
        eVal06 = 0x60000000,
        eVal07 = 0x70000000,
        eVal08 = 0x80000000,
        eVal09 = 0x90000000,
        eVal10 = 0xA0000000,
        eVal11 = 0xB0000000,
        eVal12 = 0xC0000000,
        eVal13 = 0xD0000000,
        eVal14 = 0xE0000000,
        eVal15 = 0xF0000000,
        
        // Cas spécial
        // Comme il n'est pas correct de déclarer plusieurs
        // fois la même constante, il n'y a qu'une constante nulle
        // Les champs nulls sont considéré comme "sans infos" pour leurs champ
        eNoError = 0,
    };
    
    
    // Produit une chaine de description de l'erreur
    const char* toString(Error e);
    
    // Zone des raccourcis
    // Attention : ces definitions ne sont pas dans la portée de Error
    // Importer Error par  using IOTA::Error; ne suffit pas à les rendre visible
    // Il faut donc les inclure séparément. Ex: using IOTA::eFileOpen;
    
    // IMPORTANT : il faut combiner les erreurs avec un ou '|'
    // jamais avec un '+' sinon on peut dénaturer l'erreur par des retenues
    
    // Raccourcis d'erreur de fichiers avec flags et/ou opération
    
    /// Erreur à l'ouverture du fichier
    const Error eFileOpen     = Error(eFile|eOpen);
    /// Erreur à l'ouverture du fichier en lecture/ecriture
    const Error eFileOpenRW   = Error(eFile|eOpen|eFlagRead|eFlagWrite);
    const Error eFileOpenR    = Error(eFile|eOpen|eFlagRead);
    const Error eFileOpenW    = Error(eFile|eOpen|eFlagWrite);
    /// Erreur à la création du fichier
    const Error eFileCreate   = Error(eFile|eCreate);
    /// Erreur à la fermeture du fichier
    const Error eFileClose    = Error(eFile|eClose);
    /// Erreur fichier vide
    const Error eFileEmpty    = Error(eFile|eEmpty);
    /// Erreur à la lecture du fichier
    const Error eFileRead     = Error(eFile|eRead);
    /// Erreur à l'écriture du fichier
    const Error eFileWrite    = Error(eFile|eWrite);
    /// Erreur de format du fichier
    const Error eFileFormat   = Error(eFile|eFlagFormat);
    /// Fichier non trouvé
    const Error eFileNotFound = Error(eFile|eNotExist);
    
    
    // Raccourcis d'erreur de sockets avec flags et/ou opération
    
    /// Création de socket impossible
    const Error eSocketCreate    = Error(eSocket|eCreate);
    // Ouverture de socket impossible
    const Error eSocketOpen      = Error(eSocket|eOpen);
    // Impossible de lire sur un socket
    const Error eSocketRead      = Error(eSocket|eRead);
    // Impossible de lire la taille d'une donnée sur un socket
    const Error eSocketReadSize  = Error(eSocket|eRead);
    // Impossible d'ecrire sur un socket
    const Error eSocketWrite     = Error(eSocket|eWrite);
    // Impossible d'ecrire de la taille d'une donnée sur un socket
    const Error eSocketWriteSize = Error(eSocket|eWriteSize);
    // Time out avec le socket
    const Error eSocketTimeOut   = Error(eSocket|eFlagTimeOut);
    // Connection refusée
    const Error eSocketConnect   = Error(eSocket|eConnect);
    // Liaison interrompue pendant une écriture
    const Error eSocketWBroken   = Error(eSocket|eWrite|eBroken);
    // Liaison interrompue pendant une électire
    const Error eSocketRBroken   = Error(eSocket|eRead|eBroken);
    // Socket fermée ecriture impossible
    const Error eSocketRClosed   = Error(eSocket|eRead|eClosed);
    // Socket fermée lecture impossible
    const Error eSocketWClosed   = Error(eSocket|eWrite|eClosed);
    // Time out pendant une écriture
    const Error eSocketWTimeOut  = Error(eSocket|eWrite|eFlagTimeOut);
    // Ecriture incomplete
    const Error eSocketWPartial  = Error(eSocket|eWrite|ePartial);
    // Lecture incomplete
    const Error eSocketRPartial  = Error(eSocket|eRead|ePartial);
    // Time out pendant une lecture
    const Error eSocketRTimeOut  = Error(eSocket|eRead|eFlagTimeOut);
    // Impossible de mettre en place une trace
    const Error eSocketTrace     = Error(eSocket|eTrace);
    /// Accept a été interrompu
    const Error eSocketAcceptBroken = Error(eSocketAccept|eBroken);
    /// Time out en attendant un close de l'autre (waitClose())
    const Error eSocketCloseTimeOut = Error(eSocket|eClose|eFlagTimeOut);
    
    /////////////////////////////////////////////////////////////////////////
    // Classes de gestion des erreurs
    
    // Exception racine de toute erreur
    // Encapsule le code d'erreur en un objet pour l'utiliser avec
    // des exceptions
    class CError {
    public:
        Error error; // Code de l'erreur
        CError(Error e) : error(e) {}
    };
    
    
    // Exception erreur en provenance du système d'exploitation
    class SystemError : public CError{
    public:
        int no; // le numero de l'erreur de type errno
        SystemError(Error e, int n) : CError(e),no(n) {}
        // voir le détail dans  /usr/include/asm/errno.h
        // Voir perror et sys_errlist pour afficher le message en clair
    };
    
    // Un parametre n'est pas correct : doit être remonté niveau utilisateur
    class ParamError : public CError {
    public:
        std::string param;
        std::string value;
        ParamError(std::string p, std::string v) : CError(eParam),param(p),value(v) {};
    };
    
    
    /////////////////////////////////////////////////////////////////////////
    
    // Pour éviter de devoir mettre Error() autour d'une combinaison
    inline
    Error operator|(Error error,Error flag) {
        return Error(int(error)|int(flag));
    }
    
    // ATTENTION: pour corriger le fait qu'on utilise + à la place de |
    // pour combinner les erreurs
    inline
    Error operator+(Error error,Error flag) {
        return Error(int(error)|int(flag));
    }
    
    /// Retourne le code l'objet de l'erreur
    inline
    Error objectError(Error e) {
        return Error(e & 0x07F00000);
    }
    
    /// Retourne les flags du code d'erreur
    inline
    Error flagError(Error e) {
        return Error(e & 0x0800000F);
    }
    
    /// Retourne la partie opération
    inline
    Error actionError(Error e) {
        return Error(e & 0x000FF000);
    }
    
    /// Retourne la partie état
    inline
    Error stateError(Error e) {
        return Error(e & 0x00000FF0);
    }
    
    /// Retourne la partie valeur sur un entier
    inline
    unsigned valueError(Error e) {
        return unsigned((e & 0xF0000000))>>28;
    }
    
    
    /// Transform error to a string
    inline
    const char* toString(Error e) {
        const size_t size = 256;
        // La même chaine pour tous les appels
        static char errorMessage[size];
        // Traite le type d'erreur
        switch (objectError(e)) {
            case eNoError:
                strncpy(errorMessage,"no error",size);
                break;
                
                // Erreur sur les fichiers
            case eFile:
                strncpy(errorMessage,"file",size);
                break;
            case eFileHeader:
                strncpy(errorMessage,"file header",size);
                break;
            case eFileSeek:
                strncpy(errorMessage,"file seek",size);
                break;
            case eFileFgetpos:
                strncpy(errorMessage,"file fgetpos",size);
                break;
            case eFileFsetpos:
                strncpy(errorMessage,"file fsetpos",size);
                break;
            case eFileItem:
                strncpy(errorMessage,"item of file",size);
                break;
                
                // Erreur sur les sockets
            case eSocket:
                strncpy(errorMessage,"socket",size);
                break;
            case eSocketService:
                strncpy(errorMessage,"socket service",size);
                break;
            case eSocketProtocol:
                strncpy(errorMessage,"socket protocol",size);
                break;
            case eSocketTransport:
                strncpy(errorMessage,"socket transport",size);
                break;
            case eSocketHost:
                strncpy(errorMessage,"socket host",size);
                break;
            case eSocketBind:
                strncpy(errorMessage,"socket bind",size);
                break;
            case eSocketAccept:
                strncpy(errorMessage,"socket accept",size);
                break;
            case eSocketPoll:
                strncpy(errorMessage,"socket poll",size);
                break;
                
                // Erreurs sur les outils de synchronisation
            case eMutex:
                strncpy(errorMessage,"mutex",size);
                break;
            case eCond:
                strncpy(errorMessage,"cond",size);
                break;
                
                // Sur les memory map
            case eMmap:
                strncpy(errorMessage,"mmap",size);
                break;
            case eSignalComWaitFileBusy:
                strncpy(errorMessage,"File busy for SignalComWait",size);
                break;
            case eNotImplemented:
                strncpy(errorMessage,"not implemented",size);
                break;
            default:
                strncpy(errorMessage,"unknown error",size);
                break;
        }
        
        // Ajoute l'état
        switch (stateError(e)) {
            case 0:
                // Pas d'état
                break;
            case eEmpty:
                strcat(errorMessage," empty",size);
                break;
            case eExist:
                strcat(errorMessage," data/object exist",size);
                break;
            case eNotExist:
                strcat(errorMessage," do not exist or not found",size);
                break;
            case ePartial:
                strcat(errorMessage," partial (incomplete)",size);
                break;
            case eClosed:
                strcat(errorMessage," closed",size);
                break;
            case eBroken:
                strcat(errorMessage," broken (interrupted)",size);
                break;
            case eNull:
                strcat(errorMessage," null value",size);
                break;
            case eValue:
                strcat(errorMessage," incorrect value",size);
                break;
            case eType:
                strcat(errorMessage," incorrect type",size);
                break;
            case eKey:
                strcat(errorMessage," incorrect key",size);
                break;
            case eEndianness:
                strcat(errorMessage," incorrect Endianness",size);
                break;
            case eEnd:
                strcat(errorMessage," at the end",size);
                break;
            case eId:
                strcat(errorMessage," incorrect identifier",size);
                break;
            case eNotSpec:
                strcat(errorMessage," (state not specified)",size);
                break;
            case eOverflow:
                strcat(errorMessage," overflow",size);
                break;
            case eSize:
                strcat(errorMessage," incorrect size",size);
                break;
            case eLocked:
                strcat(errorMessage," is locked",size);
                break;
            case eNotLocked:
                strcat(errorMessage," is not locked",size);
                break;
            default:
                strcat(errorMessage," #unknown state#",size);
                break;
        }
        
        // Ajoute l'opération
        switch (actionError(e)) {
            case eCreate:
                strcat(errorMessage," when create",size);
                break;
            case eOpen:
                strcat(errorMessage," when open",size);
                break;
            case eClose:
                strcat(errorMessage," when close",size);
                break;
            case eRead:
                strcat(errorMessage," when read",size);
                break;
            case eWrite:
                strcat(errorMessage," when write",size);
                break;
            case eReadSize:
                strcat(errorMessage," when read size",size);
                break;
            case eWriteSize:
                strcat(errorMessage," when write size",size);
                break;
            case eTrace:
                strcat(errorMessage," when trace",size);
                break;
            case eConnect:
                strcat(errorMessage," when connect",size);
                break;
            case eRemove:
                strcat(errorMessage," when remove",size);
                break;
            case eFork:
                strcat(errorMessage," when fork",size);
                break;
            case eLock:
                strcat(errorMessage," when lock or unlock",size);
                break;
            case eInit:
                strcat(errorMessage," when init",size);
                break;
            case eWait:
                strcat(errorMessage," when wait",size);
                break;
            case 0:
                // Si pas d'opération, pas de message
                break;
            default:
                strcat(errorMessage," unknown operation",size);
                break;
        }
        // Ajoute les flags
        if (flagError(e)) {
            if ((e & (eFlagRead|eFlagWrite)) == (eFlagRead|eFlagWrite)) { strcat(errorMessage," for reading and writing",size); }
            else {
                if (e & eFlagRead) strcat(errorMessage," for reading",size);
                if (e & eFlagWrite) strcat(errorMessage," for writing",size);
            }
            if (e & eFlagFormat) strcat(errorMessage," wrong format",size);
            if (e & eFlagTimeOut) strcat(errorMessage," time out",size);
            if (e & eFlagDone) strcat(errorMessage," done",size);
        }
        
        // Ajoute la valeur
        if (valueError(e) != 0) {
            strcat(errorMessage," (",size);
            strcat(errorMessage,itos(valueError(e)).c_str(),size);
            strcat(errorMessage,")",size);
        }
        return errorMessage;
    }
    
    /// Stream for log that should not be used directly but using MACRO
    struct LogStream {
        ostringstream buff; /// Buffer local au stream
        std::ostream& out;  /// Sortie du stream cerr par défaut
        bool fmt;           /// si vrai, place des espaces et saute une ligne à chauque chevrons.
        int level;			/// niveau du log, utilise seulement par android

		/// Affichage complet ANDROID
		LogStream(
				  int __LOGLEVEL__,		  /// niveau du log, pour android
				  const char* header, /// Header du message
				  const char* file,   /// Nom du fichier ou se trouve le message
				  const char* func,   /// Nom de la fonction ou se trouve le message
				  int line            /// No de la ligne du message
				  );

		/// Affichage partiel ADNROID
		LogStream(
				  int __LOGLEVEL__,
				  const char* header  /// Header du message
				  );

		/// Constructeur réservé à USAGE ANDROID
		LogStream(
				  int __LOGLEVEL__
				  );

        /// Affichage complet
        LogStream(const char* header, /// Header du message
                  const char* file,   /// Nom du fichier ou se trouve le message
                  const char* func,   /// Nom de la fonction ou se trouve le message
                  int line            /// No de la ligne du message
                  );
        
        /// Affichage partiel
        LogStream(const char* header  /// Header du message
                  );
        
        /// Constructeur réservé à USAGE
        LogStream();
        
        /// Sortie d'une chaine C
        LogStream& operator<<(const char* s);
        LogStream& operator<<(const unsigned char* s);
        
        
        /// Sortie d'une chaine
        LogStream& operator<<(const std::string& s);
        
        /// Sortie d'un entier
        LogStream& operator<<(int i);
        LogStream& operator<<(long int i);
        LogStream& operator<<(long long int i);
        LogStream& operator<<(unsigned int i);
        LogStream& operator<<(long unsigned int i);
        LogStream& operator<<(long long unsigned int i);
        
        /// Sortie d'un flotant
        LogStream& operator<<(float i);
        LogStream& operator<<(double i);

        /// Sortie d'un caractère
        LogStream& operator<<(char c);

        /// Sortie d'une addresse
        LogStream& operator<<(void* p);

        /// Sortie d'un code d'erreur et son message
        LogStream& operator<<(Error error);
        
        /// Sortie d'une durée du timer
        LogStream& operator<<(const IOTA::Timer& t);
        
        /// Manipulateurs de flux comme hex, endl
        LogStream& operator<<(std::ios_base& (*)(std::ios_base&));
        
        /// Autre types
        template<typename T>
        LogStream& operator<<(const T& v);
        
        /// Affichage atomique
        ~LogStream();
    };
    
#ifndef LIB
    
    //ANDROID (niveau de log)
    inline
	LogStream::LogStream(int __LOGLEVEL__, const char* header,const char* file,const char* func,int line)
	: buff(),out(*logStream),fmt(false) {
		buff<<header<<programName<<' '<<skipPath(file)<<' '<<func<<'('<<line<<"): ";
		level = __LOGLEVEL__;
	};

    //ANDROID (niveau de log)
	inline
	LogStream::LogStream(int __LOGLEVEL__, const char* header)
	: buff(),out(*logStream) , fmt(false) {
		buff<<header<<programName<<": ";
		level = __LOGLEVEL__;
	};

	//ANDROID (niveau de log)
	inline
	LogStream::LogStream(int __LOGLEVEL__)
	: buff(),out(*logStream) , fmt(true) {
		buff<<"Usage: "<<programName;
		level = __LOGLEVEL__;
	};

    inline
    LogStream::LogStream(const char* header,const char* file,const char* func,int line)
    : buff(),out(*logStream),fmt(false) {
        buff<<header<<programName<<' '<<skipPath(file)<<' '<<func<<'('<<line<<"): ";
    };
    
    inline
    LogStream::LogStream(const char* header)
    : buff(),out(*logStream) , fmt(false) {
        buff<<header<<programName<<": ";
    };
    
    inline
    LogStream::LogStream()
    : buff(),out(*logStream) , fmt(true) {
        buff<<"Usage: "<<programName;
    };
    
    inline
    LogStream& LogStream::operator<<(int i) {
        buff<<i;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(long int i) {
        buff<<i;return *this;
    }

    inline
    LogStream& LogStream::operator<<(long long int i) {
        buff<<i;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(unsigned int i) {
        buff<<i;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(long unsigned int i) {
        buff<<i;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(long long unsigned int i) {
        buff<<i;return *this;
    }

    inline
    LogStream& LogStream::operator<<(float i) {
        buff<<i;return *this;
    }

    inline
    LogStream& LogStream::operator<<(double i) {
        buff<<i;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(char c) {
        buff<<c;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(void *i) {
        buff<<hex<<"0x"<<i;
        return *this;
    }

    
    inline
    LogStream& LogStream::operator<<(const char* s) {
        if (fmt) buff<<"  ";
        buff<<std::string(s);if(fmt) buff<<'\n';return *this;
    };
    
    inline
    LogStream& LogStream::operator<<(const std::string& s) {
        if (fmt) buff<<"  ";
        buff<<s;
        if(fmt) buff<<'\n';
        return *this;
    };
    
    template<typename T>
    inline
    LogStream& LogStream::operator<<(const T& v) {
        buff<<v;return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(const unsigned char* fileName) {
        return this->operator<<((const char*)fileName);
    }
    
    inline
    LogStream::~LogStream() {
        if (!fmt) buff<<"\n";
#ifdef __ANDROID__
        __android_log_print(level, "IOTA", buff.str().c_str());
#else
        out<<buff.str(); // Action atomique pour les threads
        // cf. www.gnu.org/software/libc/manual/html_node/Streams-and-Threads.html
        out.flush();
#endif
    };
    
    inline
    LogStream& LogStream::operator<<(Error error) {
        buff<<toString(error)<<" (0x"<<hex<<error<<")"<<dec;
        return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(const IOTA::Timer& t) {
        buff<<t;
        return *this;
    }
    
    inline
    LogStream& LogStream::operator<<(std::ios_base& (*streamManip)(std::ios_base&)) {
        buff<<streamManip;
        return *this;
    }
    
    
    void setLogLevel(int level) {
        if (level > logDEBUG1) {
            WARNING<<"__LOGLEVEL__ '"<<level<<"' incorrect. Set to max.";
            __LOGLEVEL__ = logDEBUG1;
        } else {
            __LOGLEVEL__ = LogLevel(level);
        }
    }
    
    
    
    void setLogStream(const char* fileName) {
        // Verifie que le fichier de log n'est pas déjà ouvert
        if (logFStream.is_open()) {
            BUG<<"Log file alrealy open, cannot open '"<<fileName<<"'";
        }
        // Tente d'ouvrir le fichier en ajout
        logFStream.open(fileName, std::ofstream::out | std::ofstream::app);
        // Vérifie que le flux a pu être ouvert
        if (logFStream.fail()) {
            // Erreur d'ouverture, on reporte l'erreur
            ERROR<<"Cannot open log file '"<<fileName<<"'";
        } else {
            // on a pu ouvrir le fichier qui prend la place de cerr
            logStream = &logFStream;
            // On ajoute au stream la date d'ouverture
            const size_t length=256;
            char date[length];
            time_t t = time(NULL);
            strftime(date,length,"%y%m%d:%H%M%S",localtime(&t));
            INFO<<"Starting log file "<<date;
            
        }
        
    }
    
    inline
    void setLogStream(const std::string& s) {
        setLogStream(s.c_str());
    }
    
    
    
    
#endif
    
    
}

#endif
