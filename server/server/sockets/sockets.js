export default function(socketio) {

  socketio.on('connection', function(socket) {
    console.log('a user connected');
    socket.emit('hello');
    socket.on('disconnect', function() {
      console.log('disconnected');
    });
  });

}
