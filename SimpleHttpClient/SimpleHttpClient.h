////////////////////////////////////////////////////////////////////////////////
/// @brief simple http client
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
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
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2009, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_SIMPLE_HTTP_CLIENT_SIMPLE_HTTP_CLIENT_H
#define TRIAGENS_SIMPLE_HTTP_CLIENT_SIMPLE_HTTP_CLIENT_H 1

#include <Basics/Common.h>

#include <netdb.h>

#include "Basics/StringBuffer.h"

namespace triagens {
  namespace httpclient {
      
      class SimpleHttpResult;
    
    ////////////////////////////////////////////////////////////////////////////////
    /// @brief simple http client
    ////////////////////////////////////////////////////////////////////////////////

    class SimpleHttpClient {
    private:
      enum { READBUFFER_SIZE = 8096 };
      
      SimpleHttpClient (SimpleHttpClient const&);
      SimpleHttpClient& operator= (SimpleHttpClient const&);

    public:

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief supported request methods
      ////////////////////////////////////////////////////////////////////////////////

      enum http_method {
        GET, POST, PUT, DELETE, HEAD
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief state of the connection
      ////////////////////////////////////////////////////////////////////////////////

      enum request_state {
        IN_CONNECT,
        IN_WRITE,
        IN_READ_HEADER,
        IN_READ_BODY,
        IN_READ_CHUNKED_HEADER,
        IN_READ_CHUNKED_BODY,
        FINISHED,
        DEAD
      };      
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief constructs a new http client
      ///
      /// @param string hostport            server hostname and port
      /// @param double connectTimeout      timeout in seconds for the tcp connect 
      /// @param size_t connectRetries      maximum number of request retries
      /// @param SimpleLogger* logger       a logger object
      ///
      ////////////////////////////////////////////////////////////////////////////////

      SimpleHttpClient (string const& hostname,
                      int port,
                      double requestTimeout,
                      double connectTimeout, 
                      size_t connectRetries);

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief destructs a http client
      ////////////////////////////////////////////////////////////////////////////////

      virtual ~SimpleHttpClient ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief make a http request
      ///        The caller has to delete the result object
      ///
      /// @param int method                           http method
      /// @param string location                      request location
      /// @param char* body                           request body
      /// @param size_t bodyLength                    body length
      /// @param map<string, string> headerFields     header fields
      ///
      /// @return SimpleHttpResult          the request result
      ////////////////////////////////////////////////////////////////////////////////

      SimpleHttpResult* request (int method, 
                           const string& location,
                           const char* body, 
                           size_t bodyLength,
                           const map<string, string>& headerFields);

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns true if the socket is connected
      ////////////////////////////////////////////////////////////////////////////////      

      bool isConnected () {
          return _isConnected;
      } 
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns the hostname of the remote server
      ////////////////////////////////////////////////////////////////////////////////      

      const string& getHostname () {
          return _hostname;
      }
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns the port of the remote server
      ////////////////////////////////////////////////////////////////////////////////      

      int getPort () {
          return _port;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns the port of the remote server
      ////////////////////////////////////////////////////////////////////////////////      
     
      const string& getErrorMessage () {
          return _errorMessage;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief sets username and password 
      ///
      /// @param string prefix              prefix for sending username and password
      /// @param string username            username
      /// @param string password            password
      ///
      /// @return void
      ////////////////////////////////////////////////////////////////////////////////
      
      void setUserNamePassword (const string& prefix, 
                                const string& username, 
                                const string& password);

    private:
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief get the Result
      ///        The caller has to delete the result object
      ///
      /// @return SimpleHttpResult          the request result
      ////////////////////////////////////////////////////////////////////////////////

      SimpleHttpResult* getResult ();
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief set the request
      ///
      /// @param int method                           request method
      /// @param string location                      request uri
      /// @param char* body                           request body
      /// @param size_t bodyLenght                    size of body
      /// @param map<string, string> headerFields     list of header fields
      ////////////////////////////////////////////////////////////////////////////////
      
      void setRequest (int method,
                       const string& location,
                       const char* body, 
                       size_t bodyLength,
                       const map<string, string>& headerFields);
      
      void handleConnect ();
      void handleWrite (double timeout);
      void handleRead (double timeout);
      
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns true if the request is in progress
      ////////////////////////////////////////////////////////////////////////////////      

      bool isWorking () {
        return  _state < FINISHED;
      }   
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns true if the socket is connected
      ////////////////////////////////////////////////////////////////////////////////

      bool checkConnect ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief close connection
      ////////////////////////////////////////////////////////////////////////////////

      bool close ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief write data to socket
      ////////////////////////////////////////////////////////////////////////////////

      bool write ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief read th answer
      ////////////////////////////////////////////////////////////////////////////////

      bool read ();
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief read the http header
      ////////////////////////////////////////////////////////////////////////////////
      
      bool readHeader ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief read the http body by content length
      ////////////////////////////////////////////////////////////////////////////////
      
      bool readBody ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief read the chunk size
      ////////////////////////////////////////////////////////////////////////////////
      
      bool readChunkedHeader ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief read the net chunk
      ////////////////////////////////////////////////////////////////////////////////
      
      bool readChunkedBody ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief connect the socket
      ////////////////////////////////////////////////////////////////////////////////

      bool connectSocket (struct addrinfo *aip);

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief returns true if the socket is readable
      ////////////////////////////////////////////////////////////////////////////////

      bool readable ();
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief sets non-blocking mode for a socket
      ////////////////////////////////////////////////////////////////////////////////

      bool setNonBlocking (socket_t fd);

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief sets close-on-exit for a socket
      ////////////////////////////////////////////////////////////////////////////////

      bool setCloseOnExec (socket_t fd);
      

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief reset connection
      ////////////////////////////////////////////////////////////////////////////////

      void reset ();

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief ckeck socket
      ////////////////////////////////////////////////////////////////////////////////

      bool checkSocket ();
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief get timestamp
      ///
      /// @return double          time in seconds
      ////////////////////////////////////////////////////////////////////////////////
      
      double now ();                  
      
    private:
      string _hostname;          // the hostname
      int _port;                 // the port     
      double _requestTimeout;
      double _connectTimeout;
      size_t _connectRetries;

      socket_t _socket;         

      // read and write buffer
      triagens::basics::StringBuffer _writeBuffer;
      triagens::basics::StringBuffer _readBuffer;
                  
      request_state _state;
      
      size_t _written;
      
      uint32_t _nextChunkedSize;

      bool _isConnected;
            
      SimpleHttpResult* _result;
      
      double _lastConnectTime;
      size_t _numConnectRetries;
      
      string _errorMessage;
      
      std::vector< std::pair<std::string, std::string> >_pathToBasicAuth;

    };
  }
}

#endif