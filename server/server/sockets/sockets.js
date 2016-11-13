export default function(io) {

  io.on('connection', function(socket) {
    console.log('a user connected');
    socket.on('new_frame', function(frame) {
      console.log('new frame');
      io.sockets.emit('viewer-pointcloud', frame);
    });
    socket.on('disconnect', function() {
      console.log('disconnected');
    });
    socket.on('viewer-takepicture', function() {
      console.log('Take Picture button pressed');
      io.sockets.emit('take_picture');
    });
  });

}
