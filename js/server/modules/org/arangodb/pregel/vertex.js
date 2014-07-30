/*jslint indent: 2, nomen: true, maxlen: 120, sloppy: true, vars: true, white: true, plusplus: true */
/*global require, exports*/

////////////////////////////////////////////////////////////////////////////////
/// @brief Pregel wrapper for vertices. Will add content of the vertex and all 
///   functions allowed within a pregel execution step.
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
/// @author Florian Bartels, Michael Hackstein, Heiko Kernbach
/// @author Copyright 2011-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var db = require("internal").db;
var pregel = require("org/arangodb/pregel");
var _ = require("underscore");
var edge = require("org/arangodb/pregel").Edge;

var Vertex = function (executionNumber, vertexId) {
  var self = this;
  this._executionNumber = executionNumber;
  this._result = {};

  //get attributes from original collection
  var collectionName = pregel.getOriginalCollection(vertexId);
  var collection = pregel.getResponsibleShard(collectionName, this);
  var data = db._document(vertexId);

  //write attributes to vertex
  _.each(data, function(val, key) {
    self[key] = val;
  });

};

Vertex.prototype._deactivate = function () {
  var resultCollectionName = pregel.getResultCollection(this._id, this._executionNumber);
  var resultCollection = pregel.getResponsibleShard(resultCollectionName, this);
  var myResDocId = resultCollection + "/" + this._key;

  //var doc = db._document(myResDocId);
  db[resultCollection].update(myResDocId, {"active": false});
};

Vertex.prototype._getResult = function () {
  var resultCollectionName = pregel.getResultCollection(this._id, this._executionNumber);
  var resultCollection = pregel.getResponsibleShard(resultCollectionName, this);
  var myResDocId = resultCollection + "/" + this._key;

  return db[resultCollection].document(myResDocId).result;
};

Vertex.prototype._delete = function () {
  var resultCollectionName = pregel.getResultCollection(this._id, this._executionNumber);
  var resultCollection = pregel.getResponsibleShard(resultCollectionName, this);
  var myResDocId = resultCollection + "/" + this._key;

  //var doc = db[resultCollection].document(myResDocId);
  db[resultCollection].update(myResDocId, {"deleted": true});
};

Vertex.prototype._getEdges = function () {
  var respEdges = pregel.getResponsibleEdgeShards(this._executionNumber, this);
  var self = this, edges = [];

  _.each(respEdges, function(collection) {
    require("console").log("collection ############## : " + collection);
    edges.push(new edge(self._executionNumber, db[collection].outEdges(self)));
  });

  return edges;
};

Vertex.prototype._save = function () {
  var resultCollectionName = pregel.getResultCollection(this._id, this._executionNumber);
  var resultCollection = pregel.getResponsibleShard(resultCollectionName, this);
  var myResDocId = resultCollection + "/" + this._key;

  //var doc = db[resultCollection].document(myResDocId);
  db[resultCollection].update(myResDocId, {"result": this._result});
};

exports.Vertex = Vertex;