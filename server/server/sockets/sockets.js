export default function(io) {

  var Recording = require('./recording.model.js');
  var FRAME_RATE_CAP = 5;

  io.requestInterval = 1000.0 / FRAME_RATE_CAP;

  io.connectedClients = [];
  io.nextClientID = 1;
  io.viewerSocket = null;

  io.streaming = false;
  io.recording = false;
  io.frameBuffer = [];
  io.recordingBuffer = [];
  io.recordingFrameTimes = [];

  var d = new Date();
  io.lastFrameTime = d.getTime();
  io.frameRate = 0;
  io.lastRequestTime = 0;

  function requestStreamingFrames() {
    if (io.streaming) {
      var t = (new Date()).getTime();
      if (t - io.lastRequestTime < io.requestInterval) {
        var d = io.requestInterval - (t - io.lastRequestTime);
        setTimeout(function(){
          var t2 = (new Date()).getTime();
          io.lastRequestTime = t2;
          io.sockets.emit('take_picture');
        }, d);
      } else {
        io.lastRequestTime = t;
        io.sockets.emit('take_picture');
      }
    }
  }

  function updateFrameRate() {
    var d = new Date();
    var curr = d.getTime() / 1000;
    var prev = io.lastFrameTime;
    io.frameRate = 1.0 / (curr - prev);
    io.lastFrameTime = curr;
    io.viewerSocket.emit('viewer_frame_rate', io.frameRate);
  }

  function updateRecording() {
    if (io.recording) {
      io.recordingBuffer.push(io.frameBuffer);
      var d = new Date();
      io.recordingFrametimes.push(d.getTime());
    }
  }

  io.on('connection', function(socket) {

    socket.clientID = io.nextClientID;

    console.log('a user connected');
    if ('clientType' in socket.handshake.query) {
      // TODO : make the c++ client send its type
      console.log('viewer connected');
      // viewers have negative ID
      socket.clientID = -1*io.nextClientID;
      io.viewerSocket = socket;
      var onConnectionObj = {
        connectedClients : io.connectedClients,
        streaming : io.streaming
      };
      io.sockets.emit('viewer_on_connection', onConnectionObj);
    } else {
      console.log('kinect client connected');
      var newClient = {
        calibStatus : 'Not calibrated',
        ip : socket.request.connection.remoteAddress,
        clientID : io.nextClientID,
      };
      io.nextClientID += 1;
      io.sockets.emit('viewer_new_client', newClient);
      newClient.calibStatus = '???';
      io.connectedClients.push(newClient);
    } 

    socket.on('disconnect', function() {
      console.log('disconnected');
      if (socket.clientID < 0) {
        return;
      }
      io.sockets.emit('viewer_client_disconnect', socket.clientID);
      var index = io.connectedClients.findIndex(function(client) {
        return client.clientID === socket.clientID;
      });
      if (index === -1) {
        console.log('client for status update not found');
        return;
      }
      io.connectedClients.splice(index, 1);
    });

    // the Take Picture button on the frontend was pressed
    socket.on('viewer_take_picture', function() {
      io.sockets.emit('take_picture');
    });

    // the Start Streaming button on the frontend was pressed
    socket.on('viewer_start_streaming', function() {
      io.sockets.emit('start_streaming');
      io.streaming = true;
      requestStreamingFrames();
    });

    // the Stop Streaming button on the frontend was pressed
    socket.on('viewer_stop_streaming', function() {
      io.streaming = false;
      io.sockets.emit('stop_streaming');
    });

    // the Start Recording button on the frontend was pressed
    socket.on('viewer_start_recording', function() {
      io.recording = true;
      // TODO : possible issue with not recording first frame
      // TODO : recording implementation
    });

    // the Stop Recording button on the frontend was pressed
    socket.on('viewer_stop_recording', function() {
      io.recording = false;
    });

    // the current recording should be saved
    socket.on('viewer_save_recording', function() {
      // TODO
      var recording = new Recording({
        name : 'recording',
        frames : io.recordingBuffer,
        frameTimes : io.recordingFrameTimes,
      });
      recording.save(function(err) {
        if (err) {
          console.log(err);
          return;
        }
        console.log('recording saved');
      });
    });

    // the current recording should be discarded 
    socket.on('viewer_discard_recording', function() {
      io.recordingBuffer = [];
    });

    // got streaming frame
    socket.on('new_frame', function(frame) {
      var frameObj = {
        frame : frame,
        clientID : socket.clientID,
      };
      io.frameBuffer.push(frameObj);
      if (io.frameBuffer.length === io.connectedClients.length) {
        // sent the frames from frameBuffer to the viewer
        io.viewerSocket.emit('viewer_pointcloud', io.frameBuffer);
        // clear the buffer and request new frame
        updateRecording();
        io.frameBuffer = [];
        requestStreamingFrames();
        updateFrameRate();
      }
    });

    // the Calibrate button on the frontend was pressed
    socket.on('viewer_calibrate', function() {
      io.sockets.emit('calibrate');
    });

    // a client sent a calibration status
    // this may be Success or Failure
    socket.on('calibration_status', function(stat) {
      var index = io.connectedClients.findIndex(function(client) {
        return client.clientID === socket.clientID;
      });
      if (stat) {
        io.connectedClients[index].calibStatus = 'Success';
      } else {
        io.connectedClients[index].calibStatus = 'Not calibrated';
      }
      var obj = {
        clientID : socket.clientID,
        stat : stat,
      };
      io.sockets.emit('viewer_calibration_status', obj);
    });

    socket.on('number_of_markers', function(num) {
      //console.log(num);
      var numObj = {
        numberOfMarkers : num,
        clientID : socket.clientID
      };
      io.viewerSocket.emit('viewer_number_of_markers', numObj);
    });
  });
}
