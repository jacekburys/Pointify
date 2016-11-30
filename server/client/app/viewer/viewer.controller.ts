'use strict';

(function() {

class ViewerController {
  constructor($scope, socket) {
    this.$scope = $scope;
    this.socket = socket;
    this.scene = null;
    this.connectedClients = [];
    this.pointClouds = {};
    this.frameNumber = 0;
    this.pointCloudGeometries = {};
    this.material = new THREE.PointsMaterial({
      size: 1,
      vertexColors: THREE.VertexColors,
    });

    var _this = this;
    socket.ioSocket.on('viewer_calibration_status', function(stat) {
      console.log('got calibration status');
      var clientID = stat.clientID;
      var statusBool = stat.stat;
      console.log(_this.connectedClients);
      console.log(clientID);
      var index = _this.connectedClients.findIndex(function(client) {
        return client.clientID === clientID;
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
      console.log(_this.frameNumber);
      _this.frameNumber += 1;
      _this.renderPointCloud(frameObj);
    });
    socket.ioSocket.on('viewer_new_client', function(newClient) {
      console.log('new client connected');
      _this.connectedClients.push(newClient);
      //_this.latestPointCloud[newClient.clientID] = null;
      _this.$scope.$apply();
      console.log(_this.connectedClients);
    });
    socket.ioSocket.on('viewer_client_disconnect', function(clientID) {
      console.log('disconnect ' + clientID);
      var index = _this.connectedClients.findIndex(function(client) {
        return client.clientID = clientID;
      });
      if (index === -1) {
        return;
      }
      _this.connectedClients.splice(index, 1);
      _this.scene.remove(_this.pointClouds[clientID]);
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_on_connection', function(connectedClients) {
      console.log('viewer_on_connection');
      console.log(connectedClients);
      _this.connectedClients = connectedClients;
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
    console.log('trying to render frame');
    var clientID = frameObj.clientID;
    var isNew = false;
    if (!this.pointCloudGeometries[clientID]) {
      this.pointCloudGeometries[clientID] = new THREE.Geometry();
      isNew = true;
    }
    this.pointCloudGeometries[clientID].vertices = [];
    this.pointCloudGeometries[clientID].colors = [];

    var frame = frameObj.frame;

    if (frame.byteLength % 15 !== 0) {
      console.log('ERROR: byteLength % 15 != 0');
      return;
    }

    var dataView = new DataView(frame);
    var i = 0;
    var x, y, z, r, g, b;
    while (i < frame.byteLength) {
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
      this.pointCloudGeometries[clientID].vertices.push(new THREE.Vector3(x * 50, -y * 50, z * 50));
      this.pointCloudGeometries[clientID].colors.push(new THREE.Color(r / 255.0, g / 255.0, b / 255.0));
    }
    if (isNew) {
      var pointCloud = new THREE.Points(this.pointCloudGeometries[clientID], this.material);
      this.scene.add( pointCloud );
      this.pointClouds[clientID] = pointCloud;
    }

    this.pointCloudGeometries[clientID].verticesNeedUpdate = true;
    this.pointCloudGeometries[clientID].colorsNeedUpdate = true;
    this.pointCloudGeometries[clientID].dynamic = true;
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
      camera.position.z = 100;
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
