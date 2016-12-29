'use strict';

angular.module('serverApp', [
  'serverApp.constants',
  'ngCookies',
  'ngResource',
  'ngSanitize',
  'ngFileSaver',
//  'btford.socket-io',
  'ui.router',
  'ui.bootstrap'
])
  .config(function($urlRouterProvider, $locationProvider) {
    $urlRouterProvider
      .otherwise('/');

    $locationProvider.html5Mode(true);
  });
