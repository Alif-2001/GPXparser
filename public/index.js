// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    var connection = 0;
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/start',   //The server endpoint we are connecting to
        
        success: function (data) {
            console.log(data);
            if(connection == 0){
                $('#statusBar6').empty();
                $('#statusBar6').append('<div class="alert alert-danger">Connect to DB for more options</div>');
                $('.hide').hide();
            }
            if(data.files.length == 0){
                $('.hide').hide();
                $('.hide1').hide();
                $('#statusBar6').empty();
                $('#statusBar6').append('<div class="alert alert-danger">Add Files to the server to access DB Comands</div>');
                $('#statusBar').append(createStatusBar('No files in the upload folder', 'alert-secondary'));
                console.log('No files in the upload folder');
                $('#fileTable').append('No files');
                return;
            }
            $('.hide1').show();
            let toAppend = ""; let toAppendDrop = "";
            for(var i = 0; i < data.files.length; i++){
                toAppend += '<tr>';
                toAppend += '<th scope="row"><a href="'+data.files[i].filename+'" download>'+data.files[i].filename+'</a></th>';
                toAppend += '<td>'+data.files[i].version+'</td>';
                toAppend += '<td>'+data.files[i].creator+'</td>';
                toAppend += '<td>'+data.files[i].numWaypoints+'</td>';
                toAppend += '<td>'+data.files[i].numRoutes+'</td>';
                toAppend += '<td>'+data.files[i].numTracks+'</td>';
                toAppend += '<td><form action=\'/delete/'+data.files[i].filename+'\' method=\'post\'><button type="submit" onclick="confirm(\'Are you sure you want to delete '+data.files[i].filename+'?\')" class="btn btn-danger">Delete</button></form></td>';
                toAppend += '</tr>';
                toAppendDrop += '<option>'+data.files[i].filename+'</option>';
            }
            $('#fileTable').append(toAppend);
            $('#multiSelect1').append(toAppendDrop);
            $('#multiSelect2').append(toAppendDrop);
            $('#statusBar').append(createStatusBar('All valid uploaded files read successfully!', 'alert-success'));
            console.log('All valid uploaded files read successfully');
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#statusBar').append(createStatusBar(error, 'alert-danger'));
            console.log(error); 
        }
    });
    
    $('#uploadBtn').submit(function(e){
        e.preventDefault();
    });

    $('#multiSelect1').change(function(){
        var filename = $('#multiSelect1').val();
        if(filename == 0){
            return;
        }
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/select1',
            data: {
                filename: filename,
            },
            success: function (data){
                console.log(data);
                if(Object.keys(data).length == 0){
                    $('#statusBar').append(createStatusBar('No components to show for '+data.filename, 'alert-secondary'));
                    console.log("No components for file "+data.filename);
                    $('#GPXTable').append('No Components');
                    return;
                }
                let toAppend = "";
                
                $('#GPXTable').empty();
                for(var i = 0; i < data.routes.length; i++){
                    let component = data.routes[i];

                    let more = "";
                    for(var j=0; j < component.other.length; j++){
                        more += Object.keys(component.other[j]);
                        more += ': ';
                        more += Object.values(component.other[j]);
                    }
                    if(more == ""){
                        more = "No other Data";
                    }

                    toAppend += '<tr>';
                    toAppend += '<th scope = "row">route'+i+'</th>';
                    toAppend += '<td>'+component.name+'</td>';
                    toAppend += '<td>'+component.numPoints+'</td>';
                    toAppend += '<td>'+component.len+'m</td>';
                    toAppend += '<td>'+component.loop+'</td>';
                    toAppend += '<td>';
                    toAppend += '<button type="button" class="1 btn btn-primary" onclick="alert(\''+more+'\')">Show other data</button>';
                    toAppend += '<button class="btn btn-primary" onclick="renamePrompt(\''+data.filename+'\',\''+component.name+'\')" >Rename</button>';
                    toAppend += '</td>';
                    toAppend += '</tr>';
                }
                for(var i = 0; i < data.tracks.length; i++){
                    let component = data.tracks[i];

                    let more = "";
                    for(var j=0; j < component.other.length; j++){
                        more += Object.keys(component.other[j]);
                        more += ': ';
                        more += Object.values(component.other[j]);
                    }
                    if(more == ""){
                        more = "No other Data";
                    }

                    toAppend += '<tr>';
                    toAppend += '<th scope = "row">track'+i+'</th>';
                    toAppend += '<td>'+component.name+'</td>';
                    toAppend += '<td>'+component.numPoints+' </td>';
                    toAppend += '<td>'+component.len+'m</td>';
                    toAppend += '<td>'+component.loop+'</td>';
                    toAppend += '<td>';
                    toAppend += '<button type="button" class="1 btn btn-primary" onclick="alert(\''+more+'\')" >Show other data</button>';
                    toAppend += '<button class="btn btn-primary" onclick="renamePrompt(\''+data.filename+'\',\''+component.name+'\')" >Rename</button>';
                    toAppend += '</td>';
                    toAppend += '</tr>';
                }
                if(toAppend == ""){
                    toAppend += 'No items';
                }
                $('#GPXTable').append(toAppend);
                $('#statusBar').append(createStatusBar(data.filename +'\'s components read successfully!', 'alert-success'));
                console.log(data.filename +'\'s components read successfully!');
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#statusBar').append(createStatusBar(error, 'alert-danger'));
                console.log(error); 
            }
        });
    });

    $('#createGPXform').submit(function(e){
        
        e.preventDefault();

        let ext = $('#GPXname').val().split('.').pop();
        if(ext != 'gpx'){
            $('#statusBar4').append(createStatusBar('Make sure the extension is ".gpx"', 'alert-danger'));
            return;
        }

        var filename = $('#GPXname').val();
        var version = '1.1';
        var creator = $('#GPXcreator').val();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'text',       //Data type - we will use JSON for almost everything 
            url: '/createGPX',
            data:{
                filename: filename,
                other : {
                    version: version,
                    creator: creator,
                },
            },
            success: function (data){
                $('#statusBar4').append(createStatusBar(data, 'alert-success'));
                console.log(data);
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#statusBar4').append(createStatusBar(error, 'alert-danger'));
                console.log(error); 
            }
        });
    });

    var waypoints = [];
    
    $('#addRoute').submit(function(e){
        var filename;
        e.preventDefault();
        if($('#multiSelect2').val() != 0){
            filename = $('#multiSelect2').val();
        }else{
            $('#statusBar3').append(createStatusBar('Invalid file select' , 'alert-danger'));
            return;
        }
        if(waypoints.length == 0){
            $('#statusBar3').append(createStatusBar('No inputs' , 'alert-danger'));
            return;
        }

        
        $.ajax({
            type: 'get',
            dataType: 'text',
            url: '/addroute',
            data: {
                filename: filename,
                name: $('#routeName').val(),
                waypoints: waypoints,
            },

            success: function (data){
                waypoints = [];
                $('#newPointTable').empty();
                if(data == '1'){
                    $('#statusBar3').append(createStatusBar('Added route to '+filename +' successfully!', 'alert-success'));
                    console.log('Added route to '+filename +' successfully!');
                }else{
                    $('#statusBar3').append(createStatusBar('Route could not be added to '+filename , 'alert-danger'));
                    console.log('Route could not be added to '+filename );
                }
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#statusBar3').append(createStatusBar(error, 'alert-danger'));
                console.log(error); 
            }

        });

    });

    $('#addRouteBtn').click(function(e){
        e.preventDefault();
        waypoints = [];
        $('#statusBar3').empty();
        $('#newPointTable').empty();
    });

    $('#addWaypoint').click(function(e){
        e.preventDefault();
        var latitude = $('#lat').val();
        var longitude = $('#lon').val();
        if(latitude != '' && longitude != '' && isNaN(longitude) == false && isNaN(latitude) == false){
            waypoints.push({lat: latitude, lon: longitude});
            $('#newPointTable').append('<tr><td>'+latitude+'</td><td>'+longitude+'</td></tr>');
        }
    });


    $('#findPath').submit(function(e){
        e.preventDefault();

        var lat1 = $('#lat1').val();
        var lat2 = $('#lat2').val();
        var lon1 = $('#lon1').val();
        var lon2 = $('#lon2').val();
        var acc = $('#acc').val();

        if(isNaN(lon2) != false || isNaN(lat2) != false || isNaN(lon1) != false || isNaN(lat1) != false || isNaN(acc) != false){
            $('#statusBar2').append(createStatusBar('Enter valid numbers!', 'alert-danger'));
            console.log('Invalid input');
            return;
        }
        /* console.log(lat1);
        console.log(lon1);
        console.log(lat2);
        console.log(lon2); */
        //console.log(acc);

        $.ajax({
            type: 'get',
            dataType: 'json',
            url:'/find',
            data: {
                point1: {
                    lat: lat1,
                    lon: lon1
                },
                point2: {
                    lat: lat2,
                    lon: lon2
                }, 
                acc: acc,
            },
            success: function (data){
                $('#findTable').empty();
                console.log(data);
                let toAppend = "";
                for(var i = 0; i < data.length; i++){
                    var file = data[i];
                    for(var j = 0; j < file.found[0].routes.length; j++){
                        var route = file.found[0].routes[j];
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">route'+j+'</th>';
                        toAppend += '<td>'+route.name+'</td>';
                        toAppend += '<td>'+route.numPoints+'</td>';
                        toAppend += '<td>'+route.len+'m</td>';
                        toAppend += '<td>'+route.loop+'</td>';
                        toAppend += '<td><a href="'+file.filename+'" download>'+file.filename+'</a></td>';
                        toAppend += '</tr>';
                    }
                    for(var j = 0; j < file.found[0].tracks.length; j++){
                        var track = file.found[0].tracks[j];
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">track'+j+'</th>';
                        toAppend += '<td>'+track.name+'</td>';
                        toAppend += '<td>'+track.numPoints+'</td>';
                        toAppend += '<td>'+track.len+'m</td>';
                        toAppend += '<td>'+track.loop+'</td>';
                        toAppend += '<td><a href="'+file.filename+'" download>'+file.filename+'</a></td>';
                        toAppend += '</tr>';
                    }
                }
                if(toAppend == ""){
                    $('#statusBar2').append(createStatusBar('No Components were found!', 'alert-secondary'));
                    toAppend += 'No items found.';
                }else{
                    $('#statusBar2').append(createStatusBar('Components found!', 'alert-sucess'));
                }
                
                $('#findTable').append(toAppend);
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#statusBar2').append(createStatusBar(error, 'alert-danger'));
                console.log(error); 
            }
        })
    }); 
    

    //DB functions
    $('#connectDB').submit(function(e){
        e.preventDefault();
        var userName = $('#Username').val();
        var password = $('#Password').val();
        var dbname = $('#DBname').val();

        $.ajax({
            type:'get',
            dataType:'json',
            url:'/connectDB',
            data:{
                username: userName,
                password: password,
                DBname: dbname,
            },
            success: function(data){
                console.log(data);
                if(data.error != ''){
                    $('#statusBar5').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    connection = 1;
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar('Database has '+data.files+' files, '+data.routes+' routes, and '+data.points+' points','alert-success'));
                    $('#autoClose').click();
                    $('.hide').show();
                }
            },
            fail: function(error){
                //statusbar5
                console.log(error);
            }
        })
    });


    $('#DBstore').click(function(e){
        console.log('store');
        $('#statusBar6').empty();
        $('#statusBar6').append(createStatusBar('Storing...','alert-info'));
        $('.hide').attr("disabled", true);
        $.ajax({
            type:'get',
            dataType: 'json',
            url:'/storeDB',
            success: function(data){
                console.log(data);
                if(data.error != ''){
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar('Database has '+data.files+' files, '+data.routes+' routes, and '+data.points+' points','alert-success'));
                    $('.hide').attr("disabled", false);
                }
            },
            fail: function(error){
                //statusbar5
                console.log(error);
            },
        });
    });

    $('#DBclear').click(function(e){
        console.log('clear');
        $.ajax({
            type:'get',
            dataType: 'json',
            url:'/clearDB',
            success: function(data){
                console.log(data);
                if(data.error != ''){
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar('Database has '+data.files+' files, '+data.routes+' routes, and '+data.points+' points','alert-success'));
                }
            },
            fail: function(error){
                //statusbar5
                console.log(error);
            },
        });
    });

    $('#DBstatus').click(function(e){
        console.log('Show status');
        $.ajax({
            type:'get',
            dataType: 'json',
            url: '/DBstatus',
            success: function(data){
                console.log(data);
                if(data.error != ''){
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar('Status: Database has '+data.files+' files, '+data.routes+' routes, and '+data.points+' points','alert-info'));
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });

    $('#displayAllRoutes').click(function(e){
        e.preventDefault();
        console.log('Display all routes');
        var sort = $('#multiSelect3').val();

        $.ajax({
            type:'get',
            dataType: 'json',
            url: '/DBallRoutes',
            data: {
                sort: sort,
            },
            success: function(data){
                console.log(data);
                var toAppend = '';
                if(data.error != ''){
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.routes.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.routes[i].id+'</th>';
                        toAppend += '<td>'+data.routes[i].name+'</td>';
                        toAppend += '<td>'+data.routes[i].len+'m</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }else{
                        $('#routeQueryTable').empty();
                        $('#routeQueryTable').append(toAppend);
                    }
                }

            },
            fail: function(error){
                console.log(error);
            }
        })
    });

    $('#multiSelect3').change(function(e){
        e.preventDefault();
        console.log('Display all routes');
        var sort = $('#multiSelect3').val();

        $.ajax({
            type:'get',
            dataType: 'json',
            url: '/DBallRoutes',
            data: {
                sort: sort,
            },
            success: function(data){
                console.log(data);
                var toAppend = '';
                if(data.error != ''){
                    $('#statusBar6').empty();
                    $('#statusBar6').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.routes.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.routes[i].id+'</th>';
                        toAppend += '<td>'+data.routes[i].name+'</td>';
                        toAppend += '<td>'+data.routes[i].len+'m</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }else{
                        $('#routeQueryTable').empty();
                        $('#routeQueryTable').append(toAppend);
                    }
                }
            },
            fail: function(error){
                console.log(error);
            }
        })
    });


    $('#displaySpecificRoute').submit(function(e){
        e.preventDefault();
        var fileID = $('#multiSelect4').val();
        var sort = $('#multiSelect5').val();

        if(fileID == 0){
            $('#statusBar7').append(createStatusBar('Please select a file!','alert-danger'));
            console.log('Invalid input');
            return;
        }
        $('#statusBar7').empty();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/DBspecRoutes',
            data:{
                fileID: fileID,
                sort: sort,
            },
            success: function(data){
                console.log(data);
                var toAppend = '';
                if(data.error != ''){
                    $('#statusBar7').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.routes.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.routes[i].id+'</th>';
                        toAppend += '<td>'+data.routes[i].name+'</td>';
                        toAppend += '<td>'+data.routes[i].len+'m</td>';
                        toAppend += '<td>'+data.routes[i].file+'m</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }
                    $('#routeSpecQueryTable').empty();
                    $('#routeSpecQueryTable').append(toAppend);
                }
            },
            fail: function(error){
                console.log(error);
            }
        });

    });

    $('#displaySpecificPoint').submit(function(e){
        e.preventDefault();
        var fileID = $('#multiSelect6').val();
        var sort = $('#multiSelect7').val();

        if(fileID == 0){
            $('#statusBar8').append(createStatusBar('Please select a file!','alert-danger'));
            console.log('Invalid input');
            return;
        }
        $('#statusBar8').empty();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/DBspecPoints',
            data:{
                fileID: fileID,
                sort: sort,
            },
            success: function(data){
                console.log(data);
                var toAppend = '';
                if(data.error != ''){
                    $('#statusBar8').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.points.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.points[i].index+'</th>';
                        toAppend += '<td>'+data.points[i].lat+'</td>';
                        toAppend += '<td>'+data.points[i].lon+'</td>';
                        toAppend += '<td>'+data.points[i].name+'</td>';
                        toAppend += '<td>'+data.points[i].Rname+'</td>';
                        toAppend += '<td>'+data.points[i].Rlen+'</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }
                    $('#pointSpecQueryTable').empty();
                    $('#pointSpecQueryTable').append(toAppend);
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });

    $('#displaySpecNRoute').submit(function(e){
        e.preventDefault();
        var fileID = $('#multiSelect8').val();
        var est = $('#multiSelect9').val();
        var sort = $('#multiSelect10').val();
        var size = $('#Nroutes').val();
        console.log(fileID);
        if(fileID == 0){
            $('#statusBar9').append(createStatusBar('Please select a file!','alert-danger'));
            console.log('Invalid input');
            return;
        }
        $('#statusBar9').empty();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/DBspecNRoutes',
            data:{
                fileID: fileID,
                sort: sort,
                est: est,
                size: size,
            },
            success: function(data){
                console.log(data);
                var toAppend = '';
                if(data.error != ''){
                    $('#statusBar9').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.routes.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.routes[i].id+'</th>';
                        toAppend += '<td>'+data.routes[i].name+'</td>';
                        toAppend += '<td>'+data.routes[i].len+'</td>';
                        toAppend += '<td>'+data.routes[i].Fname+'</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }
                    $('#routeSpecNQueryTable').empty();
                    $('#routeSpecNQueryTable').append(toAppend);
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });


    $('#getRoutes').click(function(e){
        setSelectRoutes();
    });

    $('#displayRoutePoints').submit(function(e){
        e.preventDefault();
        console.log('Display points from route');
        var routeID = $('#multiSelect11').val();
        
        if(routeID == 0){
            $('#statusBar10').append(createStatusBar('Please select a file!','alert-danger'));
            console.log('Invalid input');
            return;
        }
        $('#statusBar10').empty();
        $.ajax({
            type: 'get',
            url: '/DBRoutePoints',
            data: {
                routeID: routeID,
            },
            success: function(data){
                console.log(data);
                var toAppend ='';
                if(data.error != ''){
                    $('#statusBar10').append(createStatusBar(data.error,'alert-danger'));
                }else{
                    for(var i = 0; i < data.points.length; i++){
                        toAppend += '<tr>';
                        toAppend += '<th scope = "row">'+data.points[i].index+'</th>';
                        toAppend += '<td>'+data.points[i].lat+'</td>';
                        toAppend += '<td>'+data.points[i].lon+'</td>';
                        toAppend += '<td>'+data.points[i].name+'</td>';
                        toAppend += '</tr>';
                    }
                    if(toAppend == ''){
                        toAppend += 'no Items found';
                    }
                    $('#routePointsQueryTable').empty();
                    $('#routePointsQueryTable').append(toAppend);
                }
            },
            fail: function(e){
                console.log(error);
            }
        });
    });

    //Extra function
    function createStatusBar(message, alert){
        $('#statusBar').empty();
        $('#statusBar2').empty();
        $('#statusBar3').empty();
        $('#statusBar4').empty();
        $('#statusBar5').empty();
        $('#statusBar7').empty();
        $('#statusBar8').empty();
        $('#statusBar9').empty();
        $('#statusBar10').empty();
        let toAppend = '';
        if(message != ''){
            toAppend += '<div class="alert '+alert+'">'+message+'</div>';
        }
        return toAppend;
    }

    function setSelectRoutes(){
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getAllRoutesDB',
            success: function(data){
                console.log(data);
                if(data.error != ''){

                }else{
                    let toAppend = '';
                    toAppend += '<option value="0" selected>Choose</option>';
                    for(let i = 0; i < data.routes.length; i++){
                        toAppend += '<option value="'+data.routes[i].route_id+'">'+data.routes[i].name+'</option>';
                    }
                    $('#multiSelect11').empty();
                    $('#multiSelect11').append(toAppend);
                }
            },
            fail: function(error){
                console.log(error);
            }
        })
    }

    $('.select').click(function(e){
        setSelectFiles();
    });

    function setSelectFiles(){
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getAllFilesDB',
            success: function(data){
                console.log(data);
                if(data.error != ''){

                }else{
                    let toAppend = '';
                    toAppend += '<option value="0" selected>Choose</option>';
                    for(let i = 0; i < data.files.length; i++){
                        toAppend += '<option value="'+data.files[i].gpx_id+'">'+data.files[i].name+'</option>';
                    }
                    $('#multiSelect4').empty();$('#multiSelect6').empty();$('#multiSelect8').empty();
                    $('#multiSelect4').append(toAppend);
                    $('#multiSelect6').append(toAppend);
                    $('#multiSelect8').append(toAppend);
                }
            },
            fail: function(error){
                console.log(error);
            }
        })
    }

});

function renamePrompt(filename, routeName){
    var newName = prompt('Change '+routeName+' in '+filename+' to:');
    if(newName == null || newName == ""){
        //$('#statusBar').append(createStatusBar('Nothing was entered', 'alert-danger'));
        console.log('Nothing was entered');
    }else{
        $.ajax({
            type: 'get',
            dataType: 'text',
            url:'/rename',
            data:{
                filename: filename,
                prevName: routeName,
                newName: newName,
            },
            success: function (data){
                if(data == '1'){
                    //$('#statusBar').append(createStatusBar('Route: '+ routeName +' renamed to '+ newName +' successfully!', 'alert-success'));
                    console.log('Route '+ routeName +' renamed to '+ newName +' successfully!');
                }else{
                    //$('#statusBar').append(createStatusBar('Could not rename Route '+ routeName, 'alert-danger'));
                    console.log('Could not rename Route '+ routeName, 'alert-danger');
                }
            },
            fail: function(error){
                //$('#statusBar').append(createStatusBar(error, 'alert-danger'));
                console.log(error);
            }
        });
    }
}
