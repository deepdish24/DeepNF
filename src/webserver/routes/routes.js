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

        
        //res.render("bs.ejs", {content_data: contents});
      });
 
    console.log('after calling readFile');

}

var fs = require('fs');



function readFiles(dirname, onFileContent, onError) {

  fs.readdir(dirname, function(err, filenames) {
  	//return filenames.length;
    if (err) {
      onError(err);
      return;
    }
    var data = {};
    filenames.forEach(function(filename) {
    	
      fs.readFile(dirname + filename, 'utf-8', function(err, content) {
        if (err) {
          onError(err);
          return;
        }
            var contents_array = content.split('\n');
        	var i = 0
        	var content_object = {};
        	for (var key in contents_array){
        	
        		var data_array = contents_array[key].split(',');
        		content_object[i] = {time:data_array[0], packetID:data_array[1], function:data_array[2], action:data_array[3]};
        		i = i+1;
        		//console.log(content_object);
        	}
        	data[filename] = content_object;
        	//console.log(Object.keys(data).length);
        if (Object.keys(data).length == filenames.length) {
        	//console.log(data);
        	onFileContent(data);
        }
        
        //onFileContent(filename, content);
      });
    });
  });
}

function getNumFiles(dirname){
	fs.readdir(dirname, function(err, filenames) {
		console.log(filenames.length);
  	return filenames.length;
  });
}

function getFiles() {

}

var showMachine1 = function(req, res) {
	var dirname = './outputs/machine1/';
	var data = {};
	readFiles(dirname, function(callback) {

  		data = callback;
  		console.log(data);
  		//console.log(numFiles);

  		res.render("machine1.ejs", {content_data: data});
  		//return data;
	}, function(err) {
  		throw err;
	});
	//var request2 = fetch('/articles.json');

	//console.log(request1);
	var data = {};
	
	//console.log(data);

	
	


//console.log(data); 

  	/*fs.readdir(dirname, function(err, filenames) {
    	if (err) {
      		return;
    	}


    	for (var a = 0; a < filenames.length; a++) {
      		fs.readFile(dirname + filenames[a], 'utf-8', function(err, contents) {
        		if (err) {
        			console.log(err);
          			return;
        		}
        		
        		//console.log(contents);
        	var contents_array = contents.split('\n');
        	var i = 0
        	var content_object = {};
        	for (var key in contents_array){
        	
        		var data_array = contents_array[key].split(',');
        		content_object[i] = {time:data_array[0], packetID:data_array[1], function:data_array[2], action:data_array[3]};
        		i = i+1;
        		//console.log(content_object);
        	}
        	data[filenames[a]] = content_object;
        	console.log(data);
        	
        	
      });
      		console.log(data);
    };
console.log(data);
  	res.render("machine1.ejs", {content_data: data});
  });*/
  	

/*	 fs.readFile('test.txt', 'utf8', function(err, contents) {
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
      });*/
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
