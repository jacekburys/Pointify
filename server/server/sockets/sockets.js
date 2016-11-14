export default function(io) {

  io.on('connection', function(socket) {

    console.log('a user connected');
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
