'use strict';

(function() {

enum CalibrationStatus {
  Calibrated = <any>'Calibrated',
  InProgress = <any>'InProgress',
  Error = <any>'Error',
}

class ViewerController {
  constructor($scope, socket) {
    this.$scope = $scope;
    this.socket = socket;
    this.connectedClients = [
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
      {
        id : 123,
        ip : '127.0.0.1',
        calibStatus : CalibrationStatus.Calibrated,
      },
      {
        id : 456,
        ip : '148.0.1.43',
        calibStatus : CalibrationStatus.Error,
      },
    ];
  }

  takePicture() {
    console.log('Trying to take a picture');
    this.socket.takePicture();
  }

  calibrate() {
    console.log('Trying to calibrate');
  }
}

angular.module('serverApp')
  .component('viewer', {
    templateUrl: 'app/viewer/viewer.html',
    controller: ViewerController
  });

})();
