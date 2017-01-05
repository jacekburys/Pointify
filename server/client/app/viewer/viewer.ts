'use strict';

angular.module('serverApp')
  .config(function($stateProvider) {
    $stateProvider
      .state('viewer', {
        url: '/',
        template: '<viewer></viewer>'
      });
  });
