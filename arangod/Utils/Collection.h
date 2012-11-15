////////////////////////////////////////////////////////////////////////////////
/// @brief "safe" single-collection wrapper
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triAGENS GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2011-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_UTILS_COLLECTION_H
#define TRIAGENS_UTILS_COLLECTION_H 1

#include "Logger/Logger.h"
#include "Basics/StringUtils.h"
#include "ShapedJson/json-shaper.h"
#include "VocBase/primary-collection.h"
#include "VocBase/vocbase.h"

using namespace std;
using namespace triagens::basics;

namespace triagens {
  namespace arango {

// -----------------------------------------------------------------------------
// --SECTION--                                                  class Collection
// -----------------------------------------------------------------------------

    class Collection {

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Collection
////////////////////////////////////////////////////////////////////////////////

      private:
        Collection (const Collection&);
        Collection& operator= (const Collection&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the accessor
////////////////////////////////////////////////////////////////////////////////

        Collection (TRI_vocbase_t* const vocbase, 
                    const string& name,
                    const TRI_col_type_e type,
                    const bool create) : 
          _vocbase(vocbase), 
          _name(name),
          _type(type),
          _create(create),
          _collection(0) {

          assert(_vocbase);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the accessor
////////////////////////////////////////////////////////////////////////////////

        ~Collection () {
          release();
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief use the collection and initialise the accessor
////////////////////////////////////////////////////////////////////////////////

        int use () {
          if (_collection != 0) {
            // we already called use() before
            return TRI_ERROR_NO_ERROR;
          }

          if (_name.empty()) {
            // name is empty. cannot open the collection
            return TRI_set_errno(TRI_ERROR_ARANGO_COLLECTION_NOT_FOUND);
          }

          // open or create the collection
          if (isdigit(_name[0])) {
            TRI_voc_cid_t id = StringUtils::uint64(_name);

            _collection = TRI_LookupCollectionByIdVocBase(_vocbase, id);
          }
          else {
            if (_type == TRI_COL_TYPE_DOCUMENT) {
              _collection = TRI_FindDocumentCollectionByNameVocBase(_vocbase, _name.c_str(), _create);
            }
            else if (_type == TRI_COL_TYPE_EDGE) {
              _collection = TRI_FindEdgeCollectionByNameVocBase(_vocbase, _name.c_str(), _create);
            }
          }
 
          if (_collection == 0) {
            // collection not found
            return TRI_set_errno(TRI_ERROR_ARANGO_COLLECTION_NOT_FOUND);
          }

          int result = TRI_UseCollectionVocBase(_vocbase, const_cast<TRI_vocbase_col_s*>(_collection));

          if (TRI_ERROR_NO_ERROR != result) {
            _collection = 0;

            return TRI_set_errno(result);
          }

          LOGGER_TRACE << "using collection " << _name;
          assert(_collection->_collection != 0);

          return TRI_ERROR_NO_ERROR;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief release all locks
////////////////////////////////////////////////////////////////////////////////
        
        bool release () {
          if (_collection == 0) {
            return false;
          }

          LOGGER_TRACE << "releasing collection";
          TRI_ReleaseCollectionVocBase(_vocbase, _collection);
          _collection = 0;

          return true;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief check whether a collection is initialised
////////////////////////////////////////////////////////////////////////////////

        inline bool isValid () const {
          return (_collection != 0);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the underlying vocbase collection
////////////////////////////////////////////////////////////////////////////////

        inline TRI_vocbase_col_t* collection () {
          assert(_collection != 0);
          return _collection;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the underlying primary collection
////////////////////////////////////////////////////////////////////////////////

        inline TRI_primary_collection_t* primary () {
          assert(_collection->_collection != 0);
          return _collection->_collection;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the underlying collection
////////////////////////////////////////////////////////////////////////////////

        inline bool waitForSync () {
          assert(_collection->_collection != 0);
          return primary()->base._info._waitForSync;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the collection's shaper
////////////////////////////////////////////////////////////////////////////////
        
        inline TRI_shaper_t* shaper () {
          return primary()->_shaper;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the underlying collection's id
////////////////////////////////////////////////////////////////////////////////

        inline TRI_voc_cid_t cid () const {
          assert(_collection != 0);
          return _collection->_cid;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the underlying collection's name
////////////////////////////////////////////////////////////////////////////////

        const string& name () const {
          return _name;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the vocbase
////////////////////////////////////////////////////////////////////////////////

        const TRI_vocbase_t* const vocbase () const {
          return _vocbase;
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief the vocbase
////////////////////////////////////////////////////////////////////////////////

        TRI_vocbase_t* const _vocbase;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection name / id
////////////////////////////////////////////////////////////////////////////////

        const string _name;

////////////////////////////////////////////////////////////////////////////////
/// @brief type of collection
////////////////////////////////////////////////////////////////////////////////
        
        const TRI_col_type_e _type;

////////////////////////////////////////////////////////////////////////////////
/// @brief create flag for collection
////////////////////////////////////////////////////////////////////////////////

        const bool _create;

////////////////////////////////////////////////////////////////////////////////
/// @brief the underlying vocbase collection
////////////////////////////////////////////////////////////////////////////////

        TRI_vocbase_col_t* _collection;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

  }
}

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}\\)"
// End:
