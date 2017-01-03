/* global io */
'use strict';

angular.module('serverApp')
  .factory('recordingService', function($http) {
    return {
      getRecordingNamesAndIds : function(cb) {
        $http.get('api/recordings').then(function(res) {
          console.log('got response from api/recordings');
          console.log(res);
          cb(res);
        });
      },
      deleteRecording : function(id, cb) {
        $http.delete('api/recordings/' + id).then(function(res) {
          console.log('got response from api/recordings DELETE');
          console.log(res);
          cb(res);
        });
      },
      getRecording : function(id, cb) {
        $http.get('api/recordings/' + id).then(function(res) {
          console.log('got response from api/recordings GET');
          console.log(res);
          cb(res);
        });
      }
    };
  });
