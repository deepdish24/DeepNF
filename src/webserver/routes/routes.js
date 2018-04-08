var fs = require('fs');
// Get the home page ('/')
var getHome = function(req, res){
	res.render("home.ejs");
}; 

// Get about page
var getAbout = function(req, res) {
	res.render("about.ejs");
}

// Function to read all text files inside a given directory. Assuming the text file has
// (time,packetID,function,action) format, this function construct a javascript object
function readFiles(dirname, onFileContent, onError) {
  fs.readdir(dirname, function(err, filenames) {
    if (err) {
      onError(err);
      return;
    }
    var data = {}; // object containing all text files results
    if (filenames.length == 0) {
    	onFileContent(undefined);
    }
    // Read all files in the directory
    filenames.forEach(function(filename) {
      fs.readFile(dirname + filename, 'utf-8', function(err, content) {
        if (err) {
          onError(err);
          return;
        }
       	  var contents_array = content.split('\n'); // Split text files by new line
          var i = 0; // Check if all files have been processed
          var content_object = {}; // object containing results for 1 text file
          for (var key in contents_array){
        	var data_array = contents_array[key].split(','); // Get each field
        	content_object[i] = {time:data_array[0], packetID:data_array[1], function:data_array[2], action:data_array[3]};
        	i = i+1;
          }
          data[filename] = content_object; // Insert the textfile into the array we are passing on
        
          // If every text file has been processed, send the object to call back.
          if (Object.keys(data).length == filenames.length) {
        	onFileContent(data);
          }
      });
    });
  });
}

// Send machine 1's log data to the webpage
var showMachine1 = function(req, res) {
	var dirname = './outputs/machine1/'; // directory path 
	var data = {};
	readFiles(dirname, function(callback) {
  	  data = callback;
      res.render("machine1.ejs", {content_data: data});
	}, function(err) {
  		throw err;
	});
}

// Send machine 2's log data to the webpage
var showMachine2 = function(req, res) {
	var dirname = './outputs/machine2/'; // directory path 
	var data = {};
	readFiles(dirname, function(callback) {
  	  data = callback;
      res.render("machine2.ejs", {content_data: data});
	}, function(err) {
  		throw err;
	});
}

// Send machine 2's log data to the webpage
var showMachine3 = function(req, res) {
	var dirname = './outputs/machine3/'; // directory path 
	var data = {};
	readFiles(dirname, function(callback) {
  	  data = callback;

      res.render("machine3.ejs", {content_data: data});
	}, function(err) {
  		throw err;
	});
}


//Set routes
var routes = { 

		get_home: getHome,
		get_about: getAbout,
		show_machine1: showMachine1,
		show_machine2: showMachine2,
		show_machine3: showMachine3,
		//get_data_machine1: getDataMachine1
};

module.exports = routes;
