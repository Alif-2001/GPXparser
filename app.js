'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
  let ext = uploadFile.name.split('.').pop();

  let error = '';
  if(ext != 'gpx'){
    error = 'Invalid File!';
  }else if(checkIfPresent(uploadFile.name) == 1){
    error = 'File already present!';
  }

  if(error === ''){
    // Use the mv() method to place the file somewhere on your server
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
      if(err) {
        return res.status(500).send(err);
      }
  
      res.redirect('/');
    });
  }else{
    res.status(500).send(error);
  }
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//setup functions


var sharedLib = ffi.Library('./libgpxparser',{
  'GPXFileToJson': ['string',['string', 'string']],
  'GPXFileToData': ['string',['string', 'string']],
  'createGPX':['void', ['string','string']], 
  'addRouteToGPX': ['int',['string','string','string','string','int']],
  'GPXFileFind':['string',['string','string','double','double','double','double','double']],
  'renameRoute':['int',['string', 'string','string','string']],
  'validateGPXFile':['int',['string', 'string']],
});


//Start
app.get('/start', function(req , res){
  const fs = require('fs');
  const dir = './uploads/';
  let files = fs.readdirSync(dir);
  let result = {
    files : [],
  };
  for(const file of files){
    if(!validateGpx(file)){
      console.log('deleted '+file);
      fs.unlink(dir+file,(err)=>{
        if(err){
          console.log(err);
        }
      });
    }
  }
  files = fs.readdirSync(dir);
  for (const file of files) {
    let input = sharedLib.GPXFileToJson(dir+file, "gpx.xsd");
    let json = JSON.parse(input);
    if(Object.entries(json).length != 0){
      json["filename"] = file;
      result.files.push(json);
    }
  }
  res.send(result);
});

//dropdown1
app.get('/select1', function(req, res){
    let filename = req.query.filename;
    const dir = './uploads/';
    let result = sharedLib.GPXFileToData(dir+filename, "gpx.xsd");
    result = result.replace('\n','');
    let json = JSON.parse(result);
    json["filename"] = filename;
    res.send(json);
});

//createGPX
app.get('/createGPX', function(req, res){
    const dir = './uploads/';
    console.log(req.query.filename);
    if(checkIfPresent(req.query.filename) == 1){
      console.log('file already exists');
      res.send('File '+ req.query.filename +' already exists');
    }else{
      sharedLib.createGPX(dir+req.query.filename, JSON.stringify(req.query.other));
      res.send('File '+ req.query.filename +' created');
    }
});

//Addroute
app.get('/addroute', function(req,res){
    const dir = './uploads/';

    var result = sharedLib.addRouteToGPX(dir+req.query.filename, "gpx.xsd", '{"name":'+req.query.name+'}',JSON.stringify(req.query.waypoints),req.query.waypoints.length);
    if(result == 1){
      res.send('1');
    }else{
      res.send('0');
    }
});

//FindPath
app.get('/find', function(req, res){
    let data = req.query;
    const fs = require('fs');
    const dir = './uploads/';
    const files = fs.readdirSync(dir);
    let result = [];
    for(const file of files) {
      let input = sharedLib.GPXFileFind(dir+file, "gpx.xsd", data.point1.lat, data.point1.lon, data.point2.lat, data.point2.lon, data.acc);
      let fresult = {
        filename: file,
        found: [],
      };
      fresult.found.push(JSON.parse(input));
      result.push(fresult);
    };
    res.send(result);
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

//Delete file
app.post('/delete/:name', function(req, res){
  var filePath = './uploads/'+ req.params.name;
  const fs = require('fs');
  fs.unlink(filePath,(err)=>{
    if(err){
      console.log(err);
      return;
    }
  });
  res.redirect('/');
});

//rename
app.get('/rename', function(req, res){
    const dir = './uploads/';
    var result = sharedLib.renameRoute(dir+req.query.filename, "gpx.xsd", req.query.prevName, req.query.newName);
    if(result == 1){
      res.send('1');
    }else{
      res.send('0');
    }
});

//MYSQL commands

const mysql = require('mysql2/promise');
const { connect } = require('http2');
const { get } = require('http');

var host;
var user;
var pass;
var db;

app.get('/connectDB',async function(req, res){
    user = req.query.username;
    pass = req.query.password;
    db = req.query.DBname;
    host = 'dursley.socs.uoguelph.ca';

    var result = {
      error : '',
      files : 0,
      routes: 0,
      points: 0,
    }

    let connection;

    try{

      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
    
      await connection.execute('CREATE TABLE IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL, primary key(gpx_id))');
      await connection.execute('CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, primary key(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)');
      await connection.execute('CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, primary key(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)');

      let id = await connection.execute('SELECT MAX(gpx_id) FROM FILE');
      result.files = id[0][0]['MAX(gpx_id)'] == null ? 0 : id[0][0]['MAX(gpx_id)'];
      id = await connection.execute('SELECT MAX(route_id) FROM ROUTE');
      result.routes = id[0][0]['MAX(route_id)'] == null ? 0 : id[0][0]['MAX(route_id)'];
      id = await connection.execute('SELECT MAX(point_id) FROM POINT');
      result.points = id[0][0]['MAX(point_id)'] == null ? 0 : id[0][0]['MAX(point_id)'];
      
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }

    res.send(result);
});

app.get('/storeDB', async function(req, res){
    const fs = require('fs');
    const dir = './uploads/';
    const files = fs.readdirSync(dir);
    let fileDetails = [];
    let fileNames = [];
    var result = {
      error : '',
      files : 0,
      routes: 0,
      points: 0,
    }
    for (const file of files) {
      let input = sharedLib.GPXFileToJson(dir+file, "gpx.xsd");
      fileNames.push(file);
      let json = JSON.parse(input);
      if(Object.entries(json).length != 0){
        json["filename"] = file;
        fileDetails.push(json);
      }
    }

    let connection;
    try{
      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
      //Clear the tables
      await connection.execute('SET FOREIGN_KEY_CHECKS = 0');
      await connection.execute('TRUNCATE TABLE FILE');
      await connection.execute('TRUNCATE TABLE ROUTE');
      await connection.execute('TRUNCATE TABLE POINT');
      await connection.execute('SET FOREIGN_KEY_CHECKS = 1');
      let gpxId = 0;
      let routeId = 0;
      for(let i = 0; i < fileDetails.length ;i++){
        await connection.execute('INSERT INTO FILE VALUES (null, "'+fileDetails[i].filename+'",'+fileDetails[i].version+',"'+fileDetails[i].creator+'")');
        gpxId++;
        const dir = './uploads/';
        let routeDetails = sharedLib.GPXFileToData(dir+fileDetails[i].filename, "gpx.xsd");
        routeDetails = routeDetails.replace('\n','');
        routeDetails = JSON.parse(routeDetails);
        let rCount = 0;
        for( let j=0; j < routeDetails.routes.length; j++){
          let name = (routeDetails.routes[j].name == '')? 'Unamed '+(rCount++) : routeDetails.routes[j].name;
          await connection.execute('INSERT INTO ROUTE VALUES(null, "'+ name +'", '+routeDetails.routes[j].len+', '+gpxId+')');
          routeId++;
          for(let k=0; k < routeDetails.routes[j].points.length; k++){
            var ptname = (routeDetails.routes[j].points[k].name == '')? 'NULL' : '\"'+routeDetails.routes[j].points[k].name+'\"';
            await connection.execute('INSERT INTO POINT VALUES(null, '+k+', '+ routeDetails.routes[j].points[k].lat +','+ routeDetails.routes[j].points[k].lon +', '+ptname+', '+ routeId +')');
          }
        }
      }
      let id = await connection.execute('SELECT MAX(gpx_id) FROM FILE');
      result.files = id[0][0]['MAX(gpx_id)'] == null ? 0 : id[0][0]['MAX(gpx_id)'];
      id = await connection.execute('SELECT MAX(route_id) FROM ROUTE');
      result.routes = id[0][0]['MAX(route_id)'] == null ? 0 : id[0][0]['MAX(route_id)'];
      id = await connection.execute('SELECT MAX(point_id) FROM POINT');
      result.points = id[0][0]['MAX(point_id)'] == null ? 0 : id[0][0]['MAX(point_id)'];
  
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }

    res.send(result);
});

app.get('/clearDB', async function(req, res){
    let connection;
    var result = {
      error : '',
      files : 0,
      routes: 0,
      points: 0,
    }
    try{
      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
      await connection.execute('SET FOREIGN_KEY_CHECKS = 0');
      await connection.execute('TRUNCATE TABLE FILE');
      await connection.execute('TRUNCATE TABLE ROUTE');
      await connection.execute('TRUNCATE TABLE POINT');
      await connection.execute('SET FOREIGN_KEY_CHECKS = 1');
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }

    res.send(result);
});

app.get('/DBstatus', async function(req, res){
    let connection;
    var result = {
      error : '',
      files : 0,
      routes: 0,
      points: 0,
    }
    try{
      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
      let id = await connection.execute('SELECT MAX(gpx_id) FROM FILE');
      result.files = id[0][0]['MAX(gpx_id)'] == null ? 0 : id[0][0]['MAX(gpx_id)'];
      id = await connection.execute('SELECT MAX(route_id) FROM ROUTE');
      result.routes = id[0][0]['MAX(route_id)'] == null ? 0 : id[0][0]['MAX(route_id)'];
      id = await connection.execute('SELECT MAX(point_id) FROM POINT');
      result.points = id[0][0]['MAX(point_id)'] == null ? 0 : id[0][0]['MAX(point_id)'];
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }

    res.send(result);
});

app.get('/DBallRoutes', async function(req, res){
    var result = {
      error : '',
      routes : [],
    };
    let connection;
    try{
      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
      console.log(req.query.sort);
      let [rows, fields] = await connection.execute('SELECT * FROM ROUTE ORDER BY '+req.query.sort+'');
      let r = {};
      for(let row of rows){
        r['name'] = row.route_name;
        r['len'] = row.route_len;
        r['id'] = row.route_id;
        result.routes.push(r);
        r = {};
      }
      
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }
    res.send(result);
});

app.get('/DBspecRoutes', async function(req, res){
    console.log(req.query);
    var result = {
      error : '',
      routes : [],
    };
    let connection;
    try{
      connection = await mysql.createConnection({
        host: host,
        user: user,
        password: pass,
        database: db,
      });
      let [rows, fields] = await connection.execute('SELECT * FROM ROUTE INNER JOIN FILE ON ROUTE.gpx_id = FILE.gpx_id WHERE FILE.gpx_id = '+req.query.fileID+' ORDER BY '+req.query.sort+'');
      let r = {};
      for(let row of rows){
        r['name'] = row.route_name;
        r['len'] = row.route_len;
        r['id'] = row.route_id;
        r['file'] = row.file_name;
        result.routes.push(r);
        r = {};
      }
      
    }catch(e){
      console.log(e);
      result.error = e.message;
    }finally{
      if (connection && connection.end) connection.end();
    }
    res.send(result);
});

app.get('/DBspecPoints', async function(req, res){
  console.log(req.query);
  var result = {
    error : '',
    points : [],
  };
  let connection;
  try{
    connection = await mysql.createConnection({
      host: host,
      user: user,
      password: pass,
      database: db,
    });
    let [rows, fields] = await connection.execute('SELECT * FROM ROUTE INNER JOIN FILE ON FILE.gpx_id = ROUTE.gpx_id WHERE FILE.gpx_id = '+req.query.fileID+' ORDER BY '+req.query.sort+'');
    let r = {};
    for(let row of rows){
      let [rows2, fields2] = await connection.execute('SELECT * FROM POINT INNER JOIN ROUTE ON POINT.route_id = ROUTE.route_id WHERE ROUTE.route_id = '+row.route_id+'');
      for(let row2 of rows2){
        r['index'] = row2.point_index;
        r['lon'] = row2.longitude;
        r['lat'] = row2.latitude;
        r['name'] = row2.point_name;
        r['Rname'] = row2.route_name;
        r['Rlen'] = row2.route_len;
        result.points.push(r);
        r = {};
      }
    }
    
  }catch(e){
    console.log(e);
    result.error = e.message;
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(result);
});

app.get('/DBspecNRoutes', async function(req, res){
  console.log(req.query);
  var result = {
    error : '',
    routes : [],
  };
  let connection;
  try{
    connection = await mysql.createConnection({
      host: host,
      user: user,
      password: pass,
      database: db,
    });
    let est = (req.query.est == 'longest')? 'DESC' : 'ASC';
    let [rows, fields] = await connection.execute('(SELECT * FROM ROUTE INNER JOIN FILE ON ROUTE.gpx_id = FILE.gpx_id WHERE FILE.gpx_id = '+req.query.fileID+' ORDER BY route_len '+est+' LIMIT '+req.query.size+') ORDER BY '+req.query.sort+'');
    let r = {};
    for(let row of rows){
      r['id'] = row.route_id;
      r['name'] = row.route_name;
      r['len'] = row.route_len;
      r['Fname'] = row.file_name;
      result.routes.push(r);
      r = {};
    }
    
  }catch(e){
    console.log(e);
    result.error = e.message;
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(result);
});

app.get('/DBRoutePoints', async function(req, res){
  var result = {
    error : '',
    points : [],
  };
  let connection;
  try{
    connection = await mysql.createConnection({
      host: host,
      user: user,
      password: pass,
      database: db,
    });
    let [rows, fields] = await connection.execute('SELECT * FROM POINT INNER JOIN ROUTE ON ROUTE.route_id = POINT.route_id WHERE POINT.route_id = '+req.query.routeID+'');
    let r = {};
    var i = 0;
    for(let row of rows){
      r['index'] = row.point_index;
      r['lat'] = row.latitude;
      r['lon'] = row.longitude;
      r['name'] = (row.point_name == null)? 'Unamed' : row.point_name;
      result.points.push(r);
      r = {};
      i++;
    }
    
  }catch(e){
    console.log(e);
    result.error = e.message;
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(result);
});

app.get('/getAllRoutesDB', async function(req, res){
  var result = {
    error : '',
    routes : [],
  };
  let connection;
  try{
    connection = await mysql.createConnection({
      host: host,
      user: user,
      password: pass,
      database: db,
    });
    let [rows, fields] = await connection.execute('SELECT route_name, route_id FROM ROUTE');
    let r = {};
    var i = 0;
    for(let row of rows){
      r['name'] = (row.route_name == null)? 'Unamed'+i : row.route_name;
      r['route_id'] = row.route_id;
      result.routes.push(r);
      r = {};
      i++;
    }
    
  }catch(e){
    console.log(e);
    result.error = e.message;
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(result);
});

app.get('/getAllFilesDB', async function(req, res){
  var result = {
    error : '',
    files : [],
  };
  let connection;
  try{
    connection = await mysql.createConnection({
      host: host,
      user: user,
      password: pass,
      database: db,
    });
    let [rows, fields] = await connection.execute('SELECT file_name, gpx_id FROM FILE');
    let r = {};
    var i = 0;
    for(let row of rows){
      r['name'] = (row.file_name == null)? 'Unamed'+i : row.file_name;
      r['gpx_id'] = row.gpx_id;
      result.files.push(r);
      r = {};
      i++;
    }
    
  }catch(e){
    console.log(e);
    result.error = e.message;
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(result);
});

//helpers

function checkIfPresent(filename){
  const fs = require('fs');
  const dir = './uploads/';
  const files = fs.readdirSync(dir);
  console.log(files);
  return ((files.includes(filename))?  1 : 0);
}

function validateGpx(filename){
  const dir = './uploads/';
  let check = sharedLib.validateGPXFile(dir+filename, "gpx.xsd");
  return check;
}
