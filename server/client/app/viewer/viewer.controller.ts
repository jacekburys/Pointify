'use strict';

(function() {

class ViewerController {
  constructor($scope) {
    this.connectedClients = [
      {
        id : 123,
      },
      {
        id : 456,
      },
    ];

  }
}

angular.module('serverApp')
  .component('viewer', {
    templateUrl: 'app/viewer/viewer.html',
    controller: ViewerController
  });

})();
