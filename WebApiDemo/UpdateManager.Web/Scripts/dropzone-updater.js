if (!String.prototype.format) {
    String.prototype.format = function () {
        var args = arguments;
        return this.replace(/{(\d+)}/g, function (match, number) {
            return typeof args[number] != 'undefined'
              ? args[number]
              : match
            ;
        });
    };
}


function bytesToSize(bytes, precision) {
    var kilobyte = 1024;
    var megabyte = kilobyte * 1024;
    var gigabyte = megabyte * 1024;
    var terabyte = gigabyte * 1024;

    if ((bytes >= 0) && (bytes < kilobyte)) {
        return bytes + ' B';

    } else if ((bytes >= kilobyte) && (bytes < megabyte)) {
        return (bytes / kilobyte).toFixed(precision) + ' KB';

    } else if ((bytes >= megabyte) && (bytes < gigabyte)) {
        return (bytes / megabyte).toFixed(precision) + ' MB';

    } else if ((bytes >= gigabyte) && (bytes < terabyte)) {
        return (bytes / gigabyte).toFixed(precision) + ' GB';

    } else if (bytes >= terabyte) {
        return (bytes / terabyte).toFixed(precision) + ' TB';

    } else {
        return bytes + ' B';
    }
}

var files = new Array();
var fileInfos = new Array();

Dropzone.options.dropzoneForm = {
    init: function () {        
        this.on("addedfile", function(file) {
            //alert("Added file.");
            files.push(file);

            var fileInfo = {};            
            var $currentFileTypes = $("#types").clone();
            $currentFileTypes.css("display", "");
            $currentFileTypes.wrap('<select></select>');
            $currentFileTypes.addClass("selectpicker");
            $currentFileTypes.attr("id", "type{0}".format(files.length - 1));

            var $currentFileRoles = $("#roles").clone();
            $currentFileRoles.css("display", "");
            $currentFileRoles.wrap('<select></select>');
            $currentFileRoles.addClass("selectpicker");
            $currentFileRoles.attr("multiple", "");
            $currentFileRoles.attr("id", "role{0}".format(files.length - 1));

            var fileName = "<td>{0}</td>".format(file.name);
            fileInfo.Name = file.name;

            var updateType = "<td>{0}</td>".format($currentFileTypes.parent().html());

            var roles = "<td>{0}</td>".format($currentFileRoles.parent().html());

            var fileType = "<td>{0}</td>".format(file.type);

            var fileSize = "<td>{0}</td>".format(bytesToSize(file.size));
            fileInfo.Size = file.size;

            /*
               0 - Name
               1 - Update Type
               2 - Roles
               3 - Type               
               4 - Size
            */
            fileInfos.push(fileInfo);
            var $row = "<tr>{0}{1}{2}{3}{4}</tr>".format(fileName, updateType, roles, fileType, fileSize);
            $($row).appendTo(".table > tbody");
            $("<li>" + file.name + "</li>").appendTo(".all");

            $('.selectpicker').selectpicker();
        });

        this.on("complete", function (data) {
            var res = eval('(' + data.xhr.responseText + ')');
            console.log(res.Message);            
        });
    }
};

$(".btn-primary").click(function() {    
    for (var i = 0; i < files.length; ++i) {
        var fileInfo = {};

        var $currentFileTypes = $("#types").clone();
        $currentFileTypes.css("display", "");
        $currentFileTypes.wrap('<select></select>');
        $currentFileTypes.addClass("selectpicker");
        $currentFileTypes.attr("id", "type{0}".format(i));

        var $currentFileRoles = $("#roles").clone();
        $currentFileRoles.css("display", "");
        $currentFileRoles.wrap('<select></select>');
        $currentFileRoles.addClass("selectpicker");
        $currentFileRoles.attr("multiple", "");
        $currentFileRoles.attr("id", "role{0}".format(i));

        var fileName = "<td>{0}</td>".format(files[i].name);
        fileInfo.Name = files[i].name; 

        var updateType = "<td>{0}</td>".format($currentFileTypes.parent().html());

        var roles = "<td>{0}</td>".format($currentFileRoles.parent().html());

        var fileType = "<td>{0}</td>".format(files[i].type);

        var fileSize = "<td>{0}</td>".format(bytesToSize(files[i].size));
        fileInfo.Size = files[i].size;

        var fileDimensions = "<td>n/a</td>";
        if (files[i].width && files[i].height) {
            fileDimensions = "<td>{0}x{1}</td>".format(files[i].width, files[i].height);
        }

        /*
           0 - Name
           1 - Update Type
           2 - Roles
           3 - Type
           4 - Dimensions
           5 - Size
        */
        fileInfos.push(fileInfo);
        var $row = "<tr>{0}{1}{2}{3}{4}{5}</tr>".format(fileName, updateType, roles, fileType, fileDimensions, fileSize);
        $($row).appendTo(".table > tbody");
    }

    $('.selectpicker').selectpicker();
});

$("#generatePkg").click(function () {
    var highCount = 0;
    var mediumCount = 0;
    var lowCount = 0;
    for (var i = 0; i < fileInfos.length; ++i) {
        //alert($("button[data-id='role{0}'] > div.filter-option".format(i)).html());
        fileInfos[i].Type = $("button[data-id='type{0}'] > div.filter-option".format(i)).html();
        fileInfos[i].Roles = $("button[data-id='role{0}'] > div.filter-option".format(i)).html();
        //alert(fileInfos[i].Type);
        if (fileInfos[i].Type == "HIGH") {            
            $("<li id=\"{0}\" class=\"list-group-item\">{1}</li>".format(fileInfos[i].Name, fileInfos[i].Name)).appendTo("#high");
            highCount++;
        }
        else if (fileInfos[i].Type == "MEDIUM") {
            $("<li id=\"{0}\" class=\"list-group-item\">{1}</li>".format(fileInfos[i].Name, fileInfos[i].Name)).appendTo("#medium");
            mediumCount++;
        }
        else {
            $("<li id=\"{0}\" class=\"list-group-item\">{1}</li>".format(fileInfos[i].Name, fileInfos[i].Name)).appendTo("#low");
            lowCount++;
        }

        // Updating the badges
        //
        $("#high-badge").text(highCount);
        $("#medium-badge").text(mediumCount);
        $("#low-badge").text(lowCount);
    }

    $("#Files").val(JSON.stringify(fileInfos));
    var options = {};
    $(".table").hide("blind", options, 1500, callback);
});

function callback() {    
    $("#loader").removeAttr("style");

    setTimeout(function() {        
        $("#loader").hide("bounce", {}, 1000, function(){
            $("#high-pkg").show("slide", {}, 500, function() {
                $("#medium-pkg").show("slide", {}, 500, function () {
                    $("#low-pkg").show("slide", {}, 500);
                });
            });
        });
    }, 3000);
};