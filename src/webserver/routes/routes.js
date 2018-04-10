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
  fs.readdir(dirname, function(err, folders) {
    if (err) {
      onError(err);
      return;
    }
    var data = {}; // object containing all text files results
    if (folders.length == 0) {
    	onFileContent(undefined);
    }
    //console.log(folders);
    // Read all files in the directory
    folders.forEach(function(folder) {
      //console.log(folder);
      fs.readdir(dirname + folder, function(err, filenames) {
        if (err) {
          onError(err);
          return;
        }
        filenames.forEach(function(filename) {
          fs.readFile(dirname + folder + '/' + filename, 'utf-8', function(err, content){
            var contents_array = content.split('\n'); // Split text files by new line
            var i = 0; // Check if all files have been processed
            var content_object = {}; // object containing results for 1 text file
            for (var key in contents_array){
              var data_array = contents_array[key].split(','); // Get each field
              content_object[i] = {time:data_array[0], packetID:data_array[1], function:data_array[2], action:data_array[3]};
              i = i+1;
            }
            data[folder] = content_object;
            if (Object.keys(data).length == folders.length) {
              onFileContent(data);
            }
          });
          //console.log(data);
        });
        //console.log(filename);
       	 /* var contents_array = content.split('\n'); // Split text files by new line
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
          }*/
      });
    });
  });
}

// Send machine 1's log data to the webpage
var showMachine1 = function(req, res) {
	var dirname = '/home/ubuntu/DeepNF/build/log/';//'/home/nets212/DeepNF/src/webserver/log/machine1/';////' // directory path 
	var data = {};
	readFiles(dirname, function(callback) {
  	  data = callback;
      console.log(data);
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

// Function to read all text files inside a given directory. Assuming the text file has
// (time,packetID,function,action) format, this function construct a javascript object
function readGraph(dirname, onFileContent, onError) {
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
/*
          var contents_array = content.split('\n'); // Split text files by new line
          var i = 0; // Check if all files have been processed
          var content_object = {}; // object containing results for 1 text file
          for (var key in contents_array){
          var data_array = contents_array[key].split(','); // Get each field
          content_object[i] = {id:data_array[0], label:data_array[1], function:data_array[2], action:data_array[3]};
          i = i+1;
          }*/
        
          data[filename] = content; // Insert the textfile into the array we are passing on
        
          // If every text file has been processed, send the object to call back.
          if (Object.keys(data).length == filenames.length) {
          onFileContent(data);
          }
      });
    });
  });
}

var showVisualization = function (req, res){
  var dirname = './outputs/graph/'; // directory path 
  var data = {};
  readGraph(dirname, function(callback) {
      data = callback;
      var nodes = data['nodes.txt'];
      var edges = data['edges.txt'];
      console.log(nodes);
      res.render("visualization.ejs", {node_data: nodes, edge_data: edges});
  }, function(err) {
      throw err;
  });
}


//Set routes
var routes = { 

		//get_home: getHome,
		//get_about: getAbout,
		show_machine1: showMachine1,
		//show_machine2: showMachine2,
		//show_machine3: showMachine3,
    show_visualization: showVisualization,
		//get_data_machine1: getDataMachine1
};

module.exports = routes;
