var express = require('express');
var path = require('path');
var routes = require('./routes/routes.js');
var app = express();
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(express.static(path.join(__dirname, 'public')))
app.get('/', routes.show_machine1); // Home page
//app.get('/about', routes.get_about); // About page
//app.get('/machine1', routes.show_machine1); // Machine 1
//app.get('https://ec2-13-58-167-87.us-east-2.compute.amazonaws.com', routes.show_machine2); // Machine 2
//app.get('ec2-18-188-160-168.us-east-2.compute.amazonaws.com', routes.show_machine3); // Machine 3
app.get('/receiver', routes.show_receiver);
app.get('/visualization', routes.show_visualization);
app.listen(80);
console.log('Server running on port 8080. Now open http://localhost:8080/ in your browser!');
