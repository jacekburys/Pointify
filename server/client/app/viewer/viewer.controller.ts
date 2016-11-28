'use strict';

(function() {

class ViewerController {
  constructor($scope, socket) {
    this.$scope = $scope;
    this.socket = socket;
    this.scene = null;
    this.connectedClients = [
    ];
    this.latestPointCloud = {};

    var _this = this;
    socket.ioSocket.on('viewer_calibration_status', function(stat) {
      console.log('got calibration status');
      var clientID = stat.clientID;
      var statusBool = stat.stat;
      console.log(_this.connectedClients);
      console.log(clientID);
      var index = _this.connectedClients.findIndex(function(client) {
        return client.clientID = clientID;
      });
      if (index === -1) {
        console.log('client for status update not found');
        return;
      }
      if (statusBool) {
        _this.connectedClients[index].calibStatus = 'Success';
      } else {
        _this.connectedClients[index].calibStatus = 'Error';
      }
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_pointcloud', function(frameObj) {
      console.log('got frame from server');
      console.log(frameObj);
      _this.renderPointCloud(frameObj);
    });
    socket.ioSocket.on('viewer_new_client', function(newClient) {
      console.log('new client connected');
      _this.connectedClients.push(newClient);
      _this.latestPointCloud[newClient.clientID] = null;
      _this.$scope.$apply();
      console.log(_this.connectedClients);
    });
    socket.ioSocket.on('viewer_client_disconnect', function(clientID) {
      console.log('disconnect ' + clientID);
      var index = _this.connectedClients.findIndex(function(client) {
        return client.id = clientID;
      });
      if (index === -1) {
        return;
      }
      _this.connectedClients.splice(index, 1);
      _this.$scope.$apply();
    });
  }

  takePicture() {
    console.log('Trying to take a picture');
    this.socket.takePicture();
  }

  startStreaming() {
    console.log('Trying to start streaming');
    this.socket.startStreaming();
  }

  calibrate() {
    console.log('Trying to calibrate');
    this.socket.calibrate();
  }

  renderPointCloud(frameObj) {
    //this.scene.children.forEach(function(object){
    //    this.scene.remove(object);
    //});
    console.log('trying to render frame');
    var material = new THREE.PointsMaterial({
      size: 0.5,
      vertexColors: THREE.VertexColors,
    });
    var geometry = new THREE.Geometry();
    var x, y, z, r, g, b;
    var i = 0;
    var frame = frameObj.frame;

    console.log('new string frame');
    if (frame.byteLength % 15 !== 0) {
      console.log('ERROR');
      return;
    }

    var dataView = new DataView(frame);

    while (i < frame.byteLength) {
      /*
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
      */

      r = dataView.getUint8(i);
      i++;

      g = dataView.getUint8(i);
      i++;

      b = dataView.getUint8(i);
      i++;

      x = dataView.getFloat32(i, true);
      i += 4;

      y = dataView.getFloat32(i, true);
      i += 4;

      z = dataView.getFloat32(i, true);
      i += 4;

      geometry.vertices.push(new THREE.Vector3(x * 50, -y * 50, z * 50));
      geometry.colors.push(new THREE.Color(r / 255.0, g / 255.0, b / 255.0));
    }
    var pointCloud = new THREE.Points(geometry, material);

    var clientID = frameObj.clientID;

    if (this.latestPointCloud[clientID]) {
      console.log('latest not null');
      this.scene.remove(this.latestPointCloud[clientID]);
      console.log('removed for ' + clientID);
    } else {
      console.log('latest null');
    }
    this.scene.add( pointCloud );
    this.latestPointCloud[clientID] = pointCloud;
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
      camera.up.set(0, 1, 0);

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
