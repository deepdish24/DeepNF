var express = require('express');
var path = require('path');
var routes = require('./routes/routes.js');
var app = express();
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(express.static(path.join(__dirname, 'public')))
app.get('/', routes.get_home); // Home page
app.get('/about', routes.get_about); // About page
app.get('/machine1', routes.show_machine1); // Machine 1
app.get('/machine2', routes.show_machine2); // Machine 2
app.get('/machine3', routes.show_machine3); // Machine 3
app.get('/visualization', routes.show_visualization);
app.listen(8080);
console.log('Server running on port 8080. Now open http://localhost:8080/ in your browser!');
