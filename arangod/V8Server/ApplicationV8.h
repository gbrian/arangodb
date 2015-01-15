////////////////////////////////////////////////////////////////////////////////
/// @brief V8 engine configuration
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2011-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_V8SERVER_APPLICATION_V8_H
#define ARANGODB_V8SERVER_APPLICATION_V8_H 1

#include "Basics/Common.h"

#include "ApplicationServer/ApplicationFeature.h"

#include <v8.h>

#include "Basics/ConditionVariable.h"
#include "V8/JSLoader.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

struct TRI_server_s;
struct TRI_vocbase_s;

namespace triagens {
  namespace aql {
    class QueryRegistry;
  }

  namespace basics {
    class Thread;
  }

  namespace rest {
    class HttpRequest;
    class ApplicationDispatcher;
    class ApplicationScheduler;
  }

  namespace arango {

// -----------------------------------------------------------------------------
// --SECTION--                                        class GlobalContextMethods
// -----------------------------------------------------------------------------

    class GlobalContextMethods {

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief method type
////////////////////////////////////////////////////////////////////////////////

        enum MethodType {
          TYPE_UNKNOWN = 0,
          TYPE_RELOAD_ROUTING,
          TYPE_FLUSH_MODULE_CACHE,
          TYPE_RELOAD_AQL,
          TYPE_BOOTSTRAP_COORDINATOR
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief get a method type number from a type string
////////////////////////////////////////////////////////////////////////////////

        static MethodType getType (std::string const& type) {
          if (type == "reloadRouting") {
            return TYPE_RELOAD_ROUTING;
          }
          if (type == "flushModuleCache") {
            return TYPE_FLUSH_MODULE_CACHE;
          }
          if (type == "reloadAql") {
            return TYPE_RELOAD_AQL;
          }
          if (type == "bootstrapCoordinator") {
            return TYPE_BOOTSTRAP_COORDINATOR;
          }

          return TYPE_UNKNOWN;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get name for a method
////////////////////////////////////////////////////////////////////////////////

        static std::string const getName (MethodType type) {
          switch (type) {
            case TYPE_RELOAD_ROUTING:
              return "reloadRouting";
            case TYPE_FLUSH_MODULE_CACHE:
              return "flushModuleCache";
            case TYPE_RELOAD_AQL:
              return "reloadAql";
            case TYPE_BOOTSTRAP_COORDINATOR:
              return "bootstrapCoordinator";
            case TYPE_UNKNOWN:
            default:
              return "unknown";
          }
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get code for a method
////////////////////////////////////////////////////////////////////////////////

        static std::string const getCode (MethodType type) {
          switch (type) {
            case TYPE_RELOAD_ROUTING:
              return CodeReloadRouting;
            case TYPE_FLUSH_MODULE_CACHE:
              return CodeFlushModuleCache;
            case TYPE_RELOAD_AQL:
              return CodeReloadAql;
            case TYPE_BOOTSTRAP_COORDINATOR:
              return CodeBootstrapCoordinator;
            case TYPE_UNKNOWN:
            default:
              return "";
          }
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief static strings with the code for each method
////////////////////////////////////////////////////////////////////////////////

        static std::string const CodeReloadRouting;
        static std::string const CodeFlushModuleCache;
        static std::string const CodeReloadAql;
        static std::string const CodeBootstrapCoordinator;
    };

// -----------------------------------------------------------------------------
// --SECTION--                                               class ApplicationV8
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief application simple user and session management feature
////////////////////////////////////////////////////////////////////////////////

    class ApplicationV8 : public rest::ApplicationFeature {
      private:
        ApplicationV8 (ApplicationV8 const&) = delete;
        ApplicationV8& operator= (ApplicationV8 const&) = delete;

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 isolate and context
////////////////////////////////////////////////////////////////////////////////

        struct V8Context {

////////////////////////////////////////////////////////////////////////////////
/// @brief name
////////////////////////////////////////////////////////////////////////////////

          std::string _name;

////////////////////////////////////////////////////////////////////////////////
/// @brief identifier
////////////////////////////////////////////////////////////////////////////////

          size_t _id;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 context
////////////////////////////////////////////////////////////////////////////////

          v8::Persistent<v8::Context> _context;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 isolate, do not put an underscore in front, because then a lot
/// of macros stop working!
////////////////////////////////////////////////////////////////////////////////

          v8::Isolate* isolate;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 locker
////////////////////////////////////////////////////////////////////////////////

          v8::Locker* _locker;

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a global method
///
/// Caller must hold the _contextCondition.
////////////////////////////////////////////////////////////////////////////////

          bool addGlobalContextMethod (std::string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief executes all global methods
///
/// Caller must hold the _contextCondition.
////////////////////////////////////////////////////////////////////////////////

          void handleGlobalContextMethods ();

////////////////////////////////////////////////////////////////////////////////
/// @brief executes the cancelation cleanup
////////////////////////////////////////////////////////////////////////////////

          void handleCancelationCleanup ();

////////////////////////////////////////////////////////////////////////////////
/// @brief mutex to protect _globalMethods
////////////////////////////////////////////////////////////////////////////////

          basics::Mutex _globalMethodsLock;

////////////////////////////////////////////////////////////////////////////////
/// @brief open global methods
////////////////////////////////////////////////////////////////////////////////

          std::vector<GlobalContextMethods::MethodType> _globalMethods;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of executions since last GC of the context
////////////////////////////////////////////////////////////////////////////////

          size_t _numExecutions;

////////////////////////////////////////////////////////////////////////////////
/// @brief timestamp of last GC for the context
////////////////////////////////////////////////////////////////////////////////

          double _lastGcStamp;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the context has dead (ex-v8 wrapped) objects
////////////////////////////////////////////////////////////////////////////////

          bool _hasDeadObjects;
        };

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ApplicationV8 (struct TRI_server_s*,
                       triagens::aql::QueryRegistry*,
                       rest::ApplicationScheduler*,
                       rest::ApplicationDispatcher*);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ApplicationV8 ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief return the app-path
////////////////////////////////////////////////////////////////////////////////

        const std::string& appPath () const {
          return _appPath;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the dev-app-path
////////////////////////////////////////////////////////////////////////////////

        const std::string& devAppPath () const {
          return _devAppPath;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the concurrency
////////////////////////////////////////////////////////////////////////////////

        void setConcurrency (size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the database
////////////////////////////////////////////////////////////////////////////////

        void setVocbase (struct TRI_vocbase_s*);

////////////////////////////////////////////////////////////////////////////////
/// @brief enters an context
////////////////////////////////////////////////////////////////////////////////

        V8Context* enterContext (std::string const& name,
                                 TRI_vocbase_s*,
                                 bool initialise,
                                 bool useDatabase);

////////////////////////////////////////////////////////////////////////////////
/// @brief exists an context
////////////////////////////////////////////////////////////////////////////////

        void exitContext (V8Context*);

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a global context function to be executed asap
////////////////////////////////////////////////////////////////////////////////

        bool addGlobalContextMethod (std::string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief runs the garbage collection
////////////////////////////////////////////////////////////////////////////////

        void collectGarbage ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disables actions
////////////////////////////////////////////////////////////////////////////////

        void disableActions ();

////////////////////////////////////////////////////////////////////////////////
/// @brief enables development mode
////////////////////////////////////////////////////////////////////////////////

        void enableDevelopmentMode ();

////////////////////////////////////////////////////////////////////////////////
/// @brief defines a boolean variable
////////////////////////////////////////////////////////////////////////////////

        void defineBoolean (const std::string& name, bool value) {
          _definedBooleans[name] = value;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief defines a double constant
////////////////////////////////////////////////////////////////////////////////

        void defineDouble (const std::string& name, double value) {
          _definedDoubles[name] = value;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief upgrades the database
////////////////////////////////////////////////////////////////////////////////

        void upgradeDatabase (bool skip, bool perform);

////////////////////////////////////////////////////////////////////////////////
/// @brief runs the version check
////////////////////////////////////////////////////////////////////////////////

        void versionCheck ();

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares the server
////////////////////////////////////////////////////////////////////////////////

        void prepareServer ();

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares named contexts
////////////////////////////////////////////////////////////////////////////////

        bool prepareNamedContexts (const std::string& name,
                                   size_t concurrency,
                                   const std::string& worker);

// -----------------------------------------------------------------------------
// --SECTION--                                        ApplicationFeature methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void setupOptions (std::map<std::string, basics::ProgramOptionsDescription>&);

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool prepare ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool prepare2 ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool start ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void close ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void stop ();

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief determine which of the free contexts should be picked for the GC
////////////////////////////////////////////////////////////////////////////////

        V8Context* pickFreeContextForGc ();

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares a V8 instance
////////////////////////////////////////////////////////////////////////////////

        bool prepareV8Instance (const std::string& name, size_t i, bool useActions);

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares the V8 server
////////////////////////////////////////////////////////////////////////////////

        void prepareV8Server (const std::string& name, size_t i, const std::string& startupFile);

////////////////////////////////////////////////////////////////////////////////
/// @brief shuts down a V8 instance
////////////////////////////////////////////////////////////////////////////////

        void shutdownV8Instance (const std::string& name, size_t i);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief server object
////////////////////////////////////////////////////////////////////////////////

        struct TRI_server_s* _server;

////////////////////////////////////////////////////////////////////////////////
/// @brief query registry object
////////////////////////////////////////////////////////////////////////////////

        triagens::aql::QueryRegistry* _queryRegistry;

////////////////////////////////////////////////////////////////////////////////
/// @brief path to the directory containing the startup scripts
/// 
/// `--javascript.startup-directory directory`
///
/// Specifies the *directory* path to the JavaScript files used for
/// bootstraping.
////////////////////////////////////////////////////////////////////////////////

        std::string _startupPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of application directories
/// `--javascript.app-path directory`
///
/// Specifies the *directory* path where the applications are located.
/// Multiple paths can be specified separated with commas.
////////////////////////////////////////////////////////////////////////////////

        std::string _appPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of application directories
/// `--javascript.dev-app-path directory`
///
/// Specifies the `directory` path where the development applications are
/// located. Multiple paths can be specified separated with commas. Never use
/// this option for production.
////////////////////////////////////////////////////////////////////////////////

        std::string _devAppPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief use actions
////////////////////////////////////////////////////////////////////////////////

        bool _useActions;

////////////////////////////////////////////////////////////////////////////////
/// @brief enables development mode
////////////////////////////////////////////////////////////////////////////////

        bool _developmentMode;

////////////////////////////////////////////////////////////////////////////////
/// @brief enables frontend development mode
////////////////////////////////////////////////////////////////////////////////

        bool _frontendDevelopmentMode;

////////////////////////////////////////////////////////////////////////////////
/// @brief JavaScript garbage collection interval (each x requests)
/// @startDocuBlock jsStartupGcInterval
/// `--javascript.gc-interval interval`
///
/// Specifies the interval (approximately in number of requests) that the
/// garbage collection for JavaScript objects will be run in each thread.
/// @endDocuBlock
////////////////////////////////////////////////////////////////////////////////

        uint64_t _gcInterval;

////////////////////////////////////////////////////////////////////////////////
/// @brief JavaScript garbage collection frequency (each x seconds)
/// @startDocuBlock jsGcFrequency
/// `--javascript.gc-frequency frequency`
///
/// Specifies the frequency (in seconds) for the automatic garbage collection of
/// JavaScript objects. This setting is useful to have the garbage collection
/// still work in periods with no or little numbers of requests.
/// @endDocuBlock
////////////////////////////////////////////////////////////////////////////////

        double _gcFrequency;

////////////////////////////////////////////////////////////////////////////////
/// @brief optional arguments to pass to v8
/// @startDocuBlock jsV8Options
/// `--javascript.v8-options options`
///
/// Optional arguments to pass to the V8 Javascript engine. The V8 engine will
/// run with default settings unless explicit options are specified using this
/// option. The options passed will be forwarded to the V8 engine which will
/// parse them on its own. Passing invalid options may result in an error being
/// printed on stderr and the option being ignored.
///
/// Options need to be passed in one string, with V8 option names being prefixed
/// with double dashes. Multiple options need to be separated by whitespace.
/// To get a list of all available V8 options, you can use
/// the value *"--help"* as follows:
/// ```
/// --javascript.v8-options "--help"
/// ```
///
/// Another example of specific V8 options being set at startup:
/// 
/// ```
/// --javascript.v8-options "--harmony --log"
/// ```
///
/// Names and features or usable options depend on the version of V8 being used,
/// and might change in the future if a different version of V8 is being used
/// in ArangoDB. Not all options offered by V8 might be sensible to use in the
/// context of ArangoDB. Use the specific options only if you are sure that
/// they are not harmful for the regular database operation.
/// @endDocuBlock
////////////////////////////////////////////////////////////////////////////////

        std::string _v8Options;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 startup loader
////////////////////////////////////////////////////////////////////////////////

        JSLoader _startupLoader;

////////////////////////////////////////////////////////////////////////////////
/// @brief system database
////////////////////////////////////////////////////////////////////////////////

        struct TRI_vocbase_s* _vocbase;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of instances to create
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, size_t> _nrInstances;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, V8Context**> _contexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts queue lock
////////////////////////////////////////////////////////////////////////////////

        basics::ConditionVariable _contextCondition;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, std::vector<V8Context*>> _freeContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, std::vector<V8Context*>> _dirtyContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 busy contexts
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, std::set<V8Context*>> _busyContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief shutdown in progress
////////////////////////////////////////////////////////////////////////////////

        volatile sig_atomic_t _stopping;

////////////////////////////////////////////////////////////////////////////////
/// @brief garbage collection thread
////////////////////////////////////////////////////////////////////////////////

        basics::Thread* _gcThread;

////////////////////////////////////////////////////////////////////////////////
/// @brief scheduler
////////////////////////////////////////////////////////////////////////////////

        rest::ApplicationScheduler* _scheduler;

////////////////////////////////////////////////////////////////////////////////
/// @brief dispatcher
////////////////////////////////////////////////////////////////////////////////

        rest::ApplicationDispatcher* _dispatcher;

////////////////////////////////////////////////////////////////////////////////
/// @brief boolean to be defined
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, bool> _definedBooleans;

////////////////////////////////////////////////////////////////////////////////
/// @brief global doubles
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, double> _definedDoubles;

////////////////////////////////////////////////////////////////////////////////
/// @brief startup file
////////////////////////////////////////////////////////////////////////////////

        std::string _startupFile;

////////////////////////////////////////////////////////////////////////////////
/// @brief indicates whether gc thread is done
////////////////////////////////////////////////////////////////////////////////
          
        volatile bool _gcFinished;
    };
  }
}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
