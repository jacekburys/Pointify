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
      }
    };
  });
