// initialize, include routes to access them 
var express = require('express');
var path = require('path');
var routes = require('./routes/routes.js');
var app = express();
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(express.static(path.join(__dirname, 'public')))
//<link href="/bootstrap/css/bootstrap.min.css" type="text/css" rel="stylesheet">
// Get the roots
app.get('/', routes.get_home);
app.get('/printData', routes.print_Data);
app.get('/getData', routes.get_Data);
/*app.get('/getAllUser', routes.get_all_user);
app.get('/getPost', routes.get_post);
app.get('/showLike', routes.get_like);
app.get('/getUser', routes.get_user);
app.get('/getUserFriend', routes.get_user_friend);
app.get('/getUserStatus', routes.get_status);
app.get('/showComment', routes.get_comment);
app.get('/getCommentData', routes.get_comment_data);

app.get('/getFriendPost', routes.get_friend_post);
app.get('/getFriendsFriend', routes.get_friends_friend);
app.get('/getFriendProfile', routes.get_friend_profile);

app.post('/addLikes', routes.post_like);
app.post('/postComment', routes.post_comment);
app.post('/postStatus', routes.post_status);

app.get('/getUserName', routes.get_user_name);

app.get('/search', routes.get_search);*/

//run server
app.listen(8080);
console.log('Server running on port 8080. Now open http://localhost:8080/ in your browser!');
