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
    this.scene = null;
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
    ];

    var _this = this;
    socket.ioSocket.on('viewer-pointcloud', function(frame) {
      console.log('got frame from server');
      _this.renderPointCloud(frame);
    });
    socket.ioSocket.on('viewer-new-client', function(newClient) {
      console.log('new client connected');
      _this.connectedClients.push(newClient);
      console.log(_this.connectedClients);
    });
  }

  takePicture() {
    console.log('Trying to take a picture');
    this.socket.takePicture();
  }

  calibrate() {
    console.log('Trying to calibrate');
    this.socket.calibrate();
  }

  renderPointCloud(frame) {
    //this.scene.children.forEach(function(object){
    //    this.scene.remove(object);
    //});
    console.log('trying to render frame');
    var material = new THREE.PointsMaterial({
      size: 1,
      vertexColors: THREE.VertexColors,
    });
    var geometry = new THREE.Geometry();
    var x, y, z, r, g, b;
    var i = 0;
    if (frame.length % 6 !== 0) {
      console.log('ERROR');
      return;
    }
    while (i < frame.length) {
      x = frame[i];
      i++;
      y = frame[i];
      i++;
      z = frame[i];
      i++;
      r = frame[i];
      i++;
      g = frame[i];
      i++;
      b = frame[i];
      i++;
      geometry.vertices.push(new THREE.Vector3(x * 100, y * 100, z * 100));
      geometry.colors.push(new THREE.Color(r / 255.0, g / 255.0, b / 255.0));
    }
    var pointCloud = new THREE.Points(geometry, material);

    this.scene.add( pointCloud );
  }

  runThree() {
    var camera, scene, renderer, controls;
    (init.bind(this))();
    animate();

    function init() {
      var width = document.getElementById('viewer').clientWidth;
      var height = window.innerHeight;
      console.log(width, height);
      camera = new THREE.PerspectiveCamera( 70, width / height, 1, 1000 );
      camera.position.z = -10;
      camera.up.set(0, 0, 1);

      scene = new THREE.Scene();
      var material = new THREE.PointsMaterial({
        size: 1,
        vertexColors: THREE.VertexColors,
      });
      var geometry = new THREE.Geometry();
      var pointCloud = new THREE.Points(geometry, material);

      scene.add( pointCloud );
      renderer = new THREE.WebGLRenderer();
      renderer.setPixelRatio( window.devicePixelRatio );
      renderer.setSize( width, height );

      controls = new THREE.OrbitControls(camera, renderer.domElement);

      var axisHelper = new THREE.AxisHelper(5);
      scene.add(axisHelper);

      document.getElementById('viewer').appendChild( renderer.domElement );
      window.addEventListener( 'resize', onWindowResize, false );

      this.scene = scene;
    }

    function onWindowResize() {
      var width = document.getElementById('viewer').clientWidth;
      var height = window.innerHeight;
      camera.aspect = width / height;
      camera.updateProjectionMatrix();
      renderer.setSize( width, height );
    }

    function animate() {
      requestAnimationFrame( animate );
      renderer.render( scene, camera );
    }
  }

}

angular.module('serverApp')
  .component('viewer', {
    templateUrl: 'app/viewer/viewer.html',
    controller: ViewerController
  });

})();
