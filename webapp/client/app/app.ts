'use strict';

angular.module('serverApp', [
  'serverApp.constants',
  'ngCookies',
  'ngResource',
  'ngSanitize',
  'ngFileSaver',
  'ngMaterial',
  'ngAnimate',
  'ui.router',
  'ui.bootstrap'
])
.config(function($urlRouterProvider, $locationProvider, $qProvider, $mdThemingProvider) {
  $urlRouterProvider.otherwise('/');
  $locationProvider.html5Mode(true);
  $qProvider.errorOnUnhandledRejections(false);
  $mdThemingProvider
    .theme('default')
    .primaryPalette('grey')
    .accentPalette('red')
    .dark();
});
