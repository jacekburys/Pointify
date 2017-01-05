'use strict';

(function() {

class ViewerController {
  constructor($scope, socket, FileSaver, Blob, ply, $mdDialog) {
    this.$scope = $scope;
    this.socket = socket;
    this.FileSaver = FileSaver;
    this.Blob = Blob;
    this.ply = ply;
    this.$mdDialog = $mdDialog;
    this.streaming = false;
    this.scene = null;
    this.connectedClients = [];
    this.pointCloud = null;
    this.frameNumber = 0;
    this.frameRate = 0;
    this.rendering = false;
    this.pointCloudGeometry = null;
    this.material = new THREE.PointsMaterial({
      size: 0.3,
      vertexColors: THREE.VertexColors,
    });

    var _this = this;
    socket.ioSocket.on('viewer_calibration_status', function(stat) {
      var clientID = stat.clientID;
      var statusBool = stat.stat;
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
    socket.ioSocket.on('viewer_number_of_markers', function(numObj) {
      var num = numObj.numberOfMarkers;
      var clientID = numObj.clientID;
      //console.log('num of markers: ', num);
      var index = _this.connectedClients.findIndex(function(client) {
        return client.clientID === clientID;
      });
      if (index === -1) {
        console.log('client for status update not found');
        return;
      }
      if (_this.connectedClients[index].calibStatus === 'Success') {
        return;
      }
      if (num === 0) {
        _this.connectedClients[index].calibStatus = 'Marker not detected';
      } else if (num === 1) {
        _this.connectedClients[index].calibStatus = 'Marker detected';
      } else {
        _this.connectedClients[index].calibStatus = 'Marker detected';
        //_this.connectedClients[index].calibStatus = 'Multiple markers';
      }
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_pointcloud', function(frameArr) {
      _this.frameNumber += 1;
      _this.renderPointCloud(frameArr);
    });
    socket.ioSocket.on('viewer_new_client', function(newClient) {
      _this.connectedClients.push(newClient);
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_client_disconnect', function(clientID) {
      var index = _this.connectedClients.findIndex(function(client) {
        return client.clientID = clientID;
      });
      if (index === -1) {
        return;
      }
      _this.connectedClients.splice(index, 1);
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_on_connection', function(onConnectionObj) {
      _this.connectedClients = onConnectionObj.connectedClients;
      _this.streaming = onConnectionObj.streaming;
      _this.$scope.$apply();
    });
    socket.ioSocket.on('viewer_frame_rate', function(frameRate) {
      if (!_this.streaming) {
        return;
      }
      _this.frameRate = frameRate.toFixed(2);
      _this.$scope.$apply();
    });
  }

  takePicture() {
    console.log('Trying to take a picture');
    this.socket.takePicture();
  }

  toggleStreaming() {
    if (this.streaming) {
      this.stopStreaming();
    } else {
      this.startStreaming();
    }
  }

  startStreaming() {
    this.streaming = true;
    this.socket.startStreaming();
  }

  stopStreaming() {
    this.streaming = false;
    this.socket.stopStreaming();
    this.frameRate = 0;
  }

  calibrate() {
    this.socket.calibrate();
  }

  renderPointCloud(frameArr) {
    if (this.rendering) {
      return;
    }
    this.rendering = true;
    console.log('trying to render frame');
    var isNew = false;
    if (!this.pointCloudGeometry) {
      this.pointCloudGeometry = new THREE.Geometry();
      isNew = true;
    }
    this.pointCloudGeometry.vertices = [];
    this.pointCloudGeometry.colors = [];

    for (var ind = 0; ind < frameArr.length; ind++) {

      var frame = frameArr[ind].frame;

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
        this.pointCloudGeometry.vertices.push(new THREE.Vector3(x * 50, -y * 50, z * 50));
        this.pointCloudGeometry.colors.push(new THREE.Color(r / 255.0, g / 255.0, b / 255.0));
      }
    }

    if (isNew) {
      var pointCloud = new THREE.Points(this.pointCloudGeometry, this.material);
      this.scene.add( pointCloud );
      this.pointCloud = pointCloud;
    }

    this.pointCloudGeometry.verticesNeedUpdate = true;
    this.pointCloudGeometry.colorsNeedUpdate = true;
    this.pointCloudGeometry.dynamic = true;
    this.rendering = false;
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

  saveAsPLY() {
    if (!this.pointCloudGeometry) {
      // TODO : handle this error
      console.log('no pointCloudGeometry');
      return;
    }

    var confirm = this.$mdDialog.prompt()
      .title('File name?')
      .textContent('Type name of your point cloud file')
      .placeholder('pointcloud')
      .ariaLabel('PointCloud')
      .initialValue('pointcloud')
      //.targetEvent(ev)
      .ok('Save')
      .cancel('Cancel');

    this.$mdDialog.show(confirm).then(function(result) {
      console.log('ok');
      console.log('trying to save ply');
      var vertices = this.pointCloudGeometry.vertices;
      /var colors = this.pointCloudGeometry.colors;
      var plyText = this.ply.toPly(vertices, colors);
      var data = new this.Blob([plyText], {type: 'application/ply' });
      this.FileSaver.saveAs(data, result + '.ply');
    }.bind(this), function() {
      console.log('cancel');
    });
  }
}

angular.module('serverApp')
  .component('viewer', {
    templateUrl: 'app/viewer/viewer.html',
    controller: ViewerController
  });

})();
