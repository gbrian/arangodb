/*jslint indent: 2, nomen: true, maxlen: 120, sloppy: true, vars: true, white: true, plusplus: true */
/*global require, exports, ArangoClusterInfo, ArangoServerState*/

////////////////////////////////////////////////////////////////////////////////
/// @brief Pregel module. Offers all submodules of pregel.
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2014 triagens GmbH, Cologne, Germany
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
/// @author Florian Bartels, Michael Hackstein, Guido Schwab
/// @author Copyright 2011-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var db = require("internal").db;
var _ = require("underscore");
var arangodb = require("org/arangodb");
var ERRORS = arangodb.errors;
var ArangoError = arangodb.ArangoError;

exports.getServerName = function () {
  return ArangoServerState.id() || "localhost";
};

exports.genWorkCollectionName = function (executionNumber) {
  return "P_work_" + executionNumber;
};

exports.genMsgCollectionName = function (executionNumber) {
  return "P_messages_" + executionNumber;
};

exports.genGlobalCollectionName = function (executionNumber) {
  return "P_global_" + executionNumber;
};

exports.getWorkCollection = function (executionNumber) {
  return db._collection(exports.genWorkCollectionName(executionNumber));
};

exports.getMsgCollection = function (executionNumber) {
  return db._collection(exports.genMsgCollectionName(executionNumber));
};

exports.getOriginalCollection = function (id, executionNumber) {
  var mapping = exports.getGlobalCollection(executionNumber).document("map").map;
  var collectionName = exports.getResultCollection(id), originalCollectionName;

  _.each(mapping, function(value, key) {
    if (value.resultCollection === collectionName) {
      originalCollectionName = key;
    }
  });

  if (originalCollectionName === undefined) {
    return collectionName;
  }
  return originalCollectionName;
};

exports.getResultCollection = function (id) {
  return id.split('/')[0];
};

exports.getGlobalCollection = function (executionNumber) {
  return db._collection(exports.genGlobalCollectionName(executionNumber));
};

exports.getResponsibleShard = function (col, doc) {
  if (ArangoServerState.role() === "PRIMARY") {
    require("internal").print(col);
    var colId = ArangoClusterInfo.getCollectionInfo(db._name(), col).id;
    require("internal").print(colId);
    var res = ArangoClusterInfo.getResponsibleShard(colId, doc).shardId;
    require("internal").print(res);
    return res;
  }
  return col;
};

exports.getShardKeysForCollection = function (executionNumber, collection) {
  var globalCol = exports.getGlobalCollection(executionNumber);
  var map = globalCol.document("map").map;
  if (!map[collection]) {
    var err = new ArangoError();
    err.errorNum = ERRORS.ERROR_PREGEL_INVALID_TARGET_VERTEX.code;
    err.errorMessage = ERRORS.ERROR_PREGEL_INVALID_TARGET_VERTEX.message;
    throw err;
  }
  return map[collection].shardKeys;
};

exports.getResponsibleEdgeShards = function (executionNumber, vertex) {
  var doc = vertex._id;
  var globalCol = exports.getGlobalCollection(executionNumber);
  var col = doc.split("/");
  var map = globalCol.document("map").map;
  var example = {
    _from: doc
  };
  var result = [];
  if(map[col] !== undefined) {
    if (ArangoServerState.role() === "PRIMARY") {
      _.each(map, function (c, key) {
        if (c.type === 3) {
          result.push(exports.getResponsibleShard(key, example));
        }
      });
      return result;
    }
    _.each(map, function (c, key) {
      if (c.type === 3) {
        result.push(key);
      }
    });
    return result;
  }
  if (ArangoServerState.role() === "PRIMARY") {
    _.each(map, function (c, col) {
      if (c.type === 3) {
        result.push(exports.getResponsibleShard(col, example));
      }
    });
    return result;
  }
  _.each(map, function (c, col) {
    if (c.type === 3) {
      result.push(col);
    }
  });
  return result;
};

exports.Conductor = require("org/arangodb/pregel/conductor");
exports.Worker = require("org/arangodb/pregel/worker");
exports.Vertex = require("org/arangodb/pregel/vertex").Vertex;
exports.Edge = require("org/arangodb/pregel/edge").Edge;
exports.MessageQueue = require("org/arangodb/pregel/messagequeue").MessageQueue;
