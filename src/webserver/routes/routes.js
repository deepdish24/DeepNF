/** TODO: CHANGE DUMMY USER ID TO CURRENT USER ID
 * Get the current user id and render the home feed
 */
var getHome = function(req, res){

	//TODO: Change to current user
	//var current_user_id = "1";

	res.render("index.ejs");
}; 

var showBS = function(req, res){
	res.render("bs.ejs");
}

var printData = function(req, res){
	var fs = require('fs');
 
      fs.readFile('test.txt', 'utf8', function(err, contents) {
      	console.log("currently reading file");
        //console.log(contents);

        
        res.render("bs.ejs", {content_data: contents});
      });
 
    console.log('after calling readFile');

}

var getData = function(req,res) {
		var fs = require('fs');
 
      fs.readFile('test.txt', 'utf8', function(err, contents) {
      	console.log("currently reading file");
        //console.log(contents);

        
        res.send({content_data: contents});
      });
}

//Set routes
var routes = { 

		get_home: getHome,
		print_Data: printData,
		get_Data: getData,
};

module.exports = routes;
