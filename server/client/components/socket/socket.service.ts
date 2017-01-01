/* global io */
'use strict';

angular.module('serverApp')
  .factory('socket', function() {
    // socket.io now auto-configures its connection when we ommit a connection url
    //var ioSocket = io('', {
      // Send auth token on connection, you will need to DI the Auth service above
      // 'query': 'token=' + Auth.getToken()
    //});

    var ioSocket = io({ query : 'clientType=viewer'});

    //var socket = socketFactory({ioSocket});


    return {
      ioSocket,
      takePicture : function() {
        console.log('Trying to emit takepicture');
        ioSocket.emit('viewer_take_picture');
      },
      startStreaming : function() {
        console.log('Trying to emit start_streaming');
        ioSocket.emit('viewer_start_streaming');
      },
      stopStreaming : function() {
        console.log('Trying to emit stop_streaming');
        ioSocket.emit('viewer_stop_streaming');
      },
      startRecording: function() {
        console.log('Trying to emit start_recording');
        ioSocket.emit('viewer_start_recording');
      },
      stopRecording: function() {
        console.log('Trying to emit stop_recording');
        ioSocket.emit('viewer_stop_recording');
      },
      calibrate : function() {
        console.log('Trying to emit calibrate');
        ioSocket.emit('viewer_calibrate');
      }
    };
  });
