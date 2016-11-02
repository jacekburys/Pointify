export default function(socketio) {

  socketio.on('connection', function(socket) {
    console.log('a user connected');
    socket.emit('hello');
    socket.on('new_frame', function(frame) {
      console.log('new frame');
      console.log(frame);
    });
    socket.on('disconnect', function() {
      console.log('disconnected');
    });
  });

}
