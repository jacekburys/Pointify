/* global io */
'use strict';

angular.module('serverApp')
  .factory('socket', function() {
    // socket.io now auto-configures its connection when we ommit a connection url
    //var ioSocket = io('', {
      // Send auth token on connection, you will need to DI the Auth service above
      // 'query': 'token=' + Auth.getToken()
    //});

    var ioSocket = io();

    //var socket = socketFactory({ioSocket});


    return {
      ioSocket,
      takePicture : function() {
        console.log('Trying to emit takepicture');
        ioSocket.emit('viewer-takepicture');
      },
      calibrate : function() {
        console.log('Trying to emit calibrate');
        ioSocket.emit('viewer-calibrate');
      }
    };
  });
