'use strict';

(function() {

class ViewerController {

  constructor($scope) {
  }
}

angular.module('serverApp')
  .component('viewer', {
    templateUrl: 'app/viewer/viewer.html',
    controller: ViewerController
  });

})();
