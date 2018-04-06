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

var showMachine1 = function(req, res) {
		var fs = require('fs');
	var content_object = {};
	 fs.readFile('test.txt', 'utf8', function(err, contents) {
      	console.log("currently reading file");
        

        var contents_array = contents.split('\n');
        var i = 0
        for (var key in contents_array){
        	
        	var data_array = contents_array[key].split(',');
        	content_object[i] = {time:data_array[0], packetID:data_array[1], function:data_array[2], action:data_array[3]};
        	i = i+1;
        	//console.log(content_object);
        }
        
        res.render("machine1.ejs", {content_data: content_object});
      });
}

/*var getDataMachine1 = function(req, res) {
	var fs = require('fs');
	var content_object = {};
	 fs.readFile('test.txt', 'utf8', function(err, contents) {
      	console.log("currently reading file");
        console.log(contents);

        var contents_array = contents.split('\n');
        for (var i = 0; i < contents_array.length(); i++) {
        	var data_array = contents_array[i].split(',');
        	content_object[i] = {time:data_array[0], function_id:data_array[1], function:data_array[2], action:data_array[3]};
        }
        
        res.({content_data: content_object});
      });
}
*/
//Set routes
var routes = { 

		get_home: getHome,
		print_Data: printData,
		//get_Data: getData,
		show_machine1: showMachine1,
		//get_data_machine1: getDataMachine1
};

module.exports = routes;
