export default function(io) {

  io.connectedClients = [];
  io.nextClientID = 0;
  io.viewerSocket = null;
  io.socketToID = {};
  io.on('connection', function(socket) {

    socket.clientID = io.nextClientID;

    console.log('a user connected');
    if ('clientType' in socket.handshake.query) {
      // TODO : make the c++ client send its type
      console.log('viewer connected');
      // viewers have negative ID
      socket.clientID = -1*io.nextClientID;
      io.viewerSocket = socket;
      io.sockets.emit('viewer_on_connection', io.connectedClients);
    } else {
      //if ('clientID' in socket.handshake.query) {
        // c++ client connected
        //io.socketToID[socket] = socket.handshake.query.clientID;
      //}
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
      console.log(io.connectedClients);
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
      console.log('removing');
      console.log(index);
      io.connectedClients.splice(index, 1);
    });

    // the Take Picture button on the frontend was pressed
    socket.on('viewer_take_picture', function() {
      console.log('Take Picture button pressed');
      io.sockets.emit('take_picture');
    });

    // the Start Streaming button on the frontend was pressed
    socket.on('viewer_start_streaming', function() {
      console.log('Start Streaming button pressed');
      io.sockets.emit('start_streaming');
    });

    // the Calibrate button on the frontend was pressed
    socket.on('viewer_calibrate', function() {
      console.log('Calibrate button pressed');
      io.sockets.emit('calibrate');
    });

    // the client sent a frame
    socket.on('new_frame', function(frame) {
      console.log('new frame');
      var frameObj = {
        frame : frame,
        clientID : socket.clientID,
      };
      //io.sockets.emit('viewer_pointcloud', frameObj);
      io.viewerSocket.emit('viewer_pointcloud', frameObj);
    });

    // a client sent a calibration status
    // this may be Success or Failure
    socket.on('calibration_status', function(stat) {
      console.log('got calibration status');
      var obj = {
        clientID : socket.clientID,
        stat : stat,
      };
      io.sockets.emit('viewer_calibration_status', obj);
    });
  });
}
