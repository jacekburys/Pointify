export default function(io) {

  io.nextClientID = 0;
  io.on('connection', function(socket) {

    console.log('a user connected');
    if ('clientType' in socket.handshake.query) {
      if (socket.handshake.query.clientType !== 'viewer') {
        console.log('kinect client connected');
        var newClient = {
          calibStatus : false,
          ip : socket.request.connection.remoteAddress,
          id : io.nextClientID,
        };
        io.nextClientID += 1;
        io.sockets.emit('viewer-new-client', newClient);
      } else {
        console.log('viewer connected');
      }
    } 

    socket.on('disconnect', function() {
      console.log('disconnected');
    });

    // the Take Picture button on the frontend was pressed
    socket.on('viewer-takepicture', function() {
      console.log('Take Picture button pressed');
      io.sockets.emit('take_picture');
    });

    // the Calibrate button on the frontend was pressed
    socket.on('viewer-calibrate', function() {
      console.log('Calibrate button pressed');
      io.sockets.emit('calibrate');
    });

    // the client sent a frame
    socket.on('new_frame', function(frame) {
      console.log('new frame');
      io.sockets.emit('viewer-pointcloud', frame);
    });

    // a client sent a calibration status
    // this may be Success or Failure
    socket.on('calibration_status', function(stat) {
      console.log('got calibration status');
      console.log(stat);
    });
  });
}
