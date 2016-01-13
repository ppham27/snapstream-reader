/*global countries */
var http = require('http');
var fs = require('fs');
var url = require('url');
var querystring = require('querystring');
var async = require('async');
var cheerio = require('cheerio');
var nodemailer = require('nodemailer');
var d3 = require('d3');
var transporter = nodemailer.createTransport();

var mailFrom = 'phamp@math.upenn.edu';
var mailTo = process.env.NODE_ENV === 'test' ? 'phamp@math.upenn.edu' : ['phamp@math.upenn.edu', 'pemantle@math.upenn.edu', 'dmutz@asc.upenn.edu'].join(', ');
var storageFolder = 'daily_2016_republicans';

// determine date range, from is inclusive, to is exclusive
var toDate, fromDate;
var readFile = false;
var writeFile = false;
var file;
switch (process.argv.length) {
  case 2:
  // no arguments means since May 1, 2015 minus 6 days
  toDate = new Date();
  toDate.setDate(toDate.getDate() - 1); // server takes inclusive range
  fromDate = new Date("2015-05-01");
  fromDate.setDate(fromDate.getDate() - 6);
  writeFile = true;
  break;
  case 3:
  // 1 arguments specifies to date
  toDate = new Date(process.argv[2]);
  toDate.setDate(toDate.getDate() - 1); // server takes inclusive range
  fromDate = new Date("2015-05-01");
  fromDate.setDate(fromDate.getDate() - 6);
  writeFile = true;
  break;
  case 4:
  if (process.argv[2] === 'file') {
    readFile = true;
    file = process.argv[3];
  } else {
    fromDate = new Date(process.argv[2]);
    toDate = new Date(process.argv[3]);
    toDate = new Date(toDate - 24*1*60*60*1000); 
  }
  break;  
  // 2 arguments specifies range
  default:
  console.error('Unknown arguments');
  process.exit(-1);
}

var candidates = fs.readFileSync('../resources/dictionary.csv', 'utf8');
var candidatesDict = {};
candidates = candidates.split('\n').map(function(candidate) { 
               var splitLine = candidate.split(',');
               var searchTerm = splitLine[1];
               if (searchTerm && splitLine[3] === "2016 Republican Primary") candidatesDict[splitLine[0]] = {searchTerm: splitLine[1], symbol: splitLine[2]};
               return splitLine[3] === "2016 Republican Primary" ? searchTerm : undefined;
             }).filter(function(candidate) { return !!candidate; });

var upperBoundDate, fromDateString, toDateString;
if (readFile) {
  var responseBody = fs.readFileSync(file, 'utf8');
  var $ = cheerio.load(responseBody);  
  fromDate = new Date($('#from-date').text());
  fromDate.setDate(fromDate.getDate() + 6);
  toDate = new Date($('#to-date').text());
  upperBoundDate = new Date(toDate);
  upperBoundDate.setDate(upperBoundDate.getDate() + 1);
  fromDateString = fromDate.toISOString().slice(0,10);
  toDateString = toDate.toISOString().slice(0,10);
  processResponse(responseBody);
} else {
  upperBoundDate = new Date(toDate);
  upperBoundDate.setDate(upperBoundDate.getDate() + 1);
  var shiftedFromDate = new Date(fromDate);
  shiftedFromDate.setDate(shiftedFromDate.getDate() - 6);
  fromDateString = fromDate.toISOString().slice(0,10);
  toDateString = toDate.toISOString().slice(0,10);
  var candidatesString = candidates.join('\n');
  var body = {"from-date": shiftedFromDate.toISOString().slice(0,10),
              "to-date": toDateString,
              "search-strings": candidatesString,
              "num-excerpts": 10,
              "excerpt-size": 200};
  var postData = querystring.stringify(body);
  var requestOptions = {
    hostname: 'johnny.sas.upenn.edu',
    port: 80,
    path: '/~pemantle/cgi-bin/multiple_search',
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Content-Length': postData.length
    }    
  };
  var responseBody = new String();
  var req = http.request(requestOptions, function(res) {
              res.on('data', function(chunk) {
                responseBody += chunk;
              });
              res.on('end', function() {
                processResponse(responseBody);              
                fs.writeFileSync(upperBoundDate.toISOString().slice(0, 10) + '_2016_republicans.html', responseBody);
              });
            });
  req.write(postData);
  req.end();
}

function processResponse(body) {
  var relativeUrl = 'http://johnny.sas.upenn.edu/~pemantle/cgi-bin/';
  var $ = cheerio.load(body);
  var dataLink = $('#long-data').attr('href');
  var visualizationLink = $('#visualization').attr('href');
  // first grab data, then send email
  async.waterfall([
    function(done) {
      var candidateCounts = [];
      http.get(relativeUrl + dataLink,
               function(res) {
                 var matrixFile = new String();
                 res.on('data', function(chunk) {
                   matrixFile += chunk;
                 });
                 res.on('end', function() {
                   var lines = matrixFile.split('\n');
                   lines.splice(0, 1); //remove header?
                   lines.forEach(function(line, idx) {
                     var splitLine = line.split(',');
                     var count = parseInt(splitLine[idx]);   
                     candidateCounts.push({Date: splitLine[0],
                                           Candidate: splitLine[1],
                                           "Search Term": candidatesDict[splitLine[1]].searchTerm,
                                           Contexts: parseInt(splitLine[2]),
                                           Programs: parseInt(splitLine[3]),
                                           Matches: parseInt(splitLine[4])})
                   });
                   done(null, candidateCounts);
                 });             
               });
    },
    function(candidateCounts, done) {
      // get counts for last 7 days, sort by latest date
      var dates = new Set();
      candidateCounts.forEach(function(d) { dates.add(d.Date); })
      var datesArray = [];
      dates.forEach(function(date) { datesArray.push(date); });
      datesArray.sort();
      datesArray = datesArray.slice(-7);
      var candidateStats = {};
      candidateCounts.forEach(function(d) {
        if (datesArray.indexOf(d.Date) >= 0) {
          if (candidateStats[d.Candidate] === undefined) candidateStats[d.Candidate] = {};
          candidateStats[d.Candidate][d.Date] = d.Contexts;
        }
      });
      var candidateData = [];
      Object.keys(candidateStats).forEach(function(candidate) {
        var d = {Candidate: candidate,
                 "Search Term": candidatesDict[candidate].searchTerm};
        datesArray.forEach(function(date) {
          d[date] = candidateStats[candidate][date];
        });
        candidateData.push(d);
      });
      var lastDate = datesArray[datesArray.length - 1];
      candidateData.sort(function(a, b) {
        return b[lastDate] - a[lastDate]; //sort in descending order
      });      
      done(null, candidateData, datesArray);
    },
    function(candidateData, datesArray, done) {
      // print table out
      var thStyle = 'border-bottom: 1px solid black; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';
      var htmlEmail = '<p>The number of matching contexts for 2016 Republican Candidates are <table style="border-collapse: collapse"><thead><tr>\n\
<th style="' + thStyle + '">Candidate</th>\n\
<th style="' + thStyle + '">Search Term</th>\n';
      datesArray.forEach(function(date) {
        htmlEmail += '<th style="' + thStyle + '">' + date + '</th>\n';
      });
      htmlEmail += '</tr></thead><tbody>\n';
      candidateData.forEach(function(d, idx) {
        var style = 'text-align: center; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';
        htmlEmail += '\n<tr>\
<td style="' + style + '">' + d.Candidate + '</td>\n\
<td style="' + style + '">' + d["Search Term"] + '</td>\n';
        datesArray.forEach(function(date) {
          htmlEmail += '<td style="' + style + '">' + d[date] + '</td>\n';
        });
        htmlEmail += '</tr>';
      });
      htmlEmail += '</tbody></table></p>\n';
      var dataFilePath = querystring.parse(url.parse(relativeUrl + visualizationLink).query).filename;
      var query = {title: '2016 Republican Candidates from ' + fromDateString + ' to ' + toDateString,
                   filename: dataFilePath};            
      if (writeFile && fs.existsSync('../html/tmp/' + storageFolder) && fs.existsSync('../html/' + dataFilePath)) {
        fs.createReadStream('../html/' + dataFilePath)
        .pipe(fs.createWriteStream('../html/tmp/' + storageFolder + '/' + upperBoundDate.toISOString().slice(0, 10) + '.csv')); // rm tmp part of file path
        query.filename = 'tmp/' + storageFolder + '/' + upperBoundDate.toISOString().slice(0, 10) + '.csv';
        fs.writeFileSync('../html/tmp/' + storageFolder + '/current.js',
                         'if (initialized === false) initializeData(' + JSON.stringify(d3.csv.parse(fs.readFileSync('../html/' + dataFilePath, 'utf8'))) + ');');
      }
      htmlEmail += '<p>See the visualization <a href="' + (relativeUrl + '../time-series.html?' + querystring.stringify(query)) + '">here</a>.</p>';
      transporter.sendMail({
        from: mailFrom,
        to: mailTo,
        subject: '2016 Republican Candidates from ' + fromDateString + ' to ' + toDateString,
        html: htmlEmail
      }, function(err, info) {
           if (err) fs.writeFileSync('job_status_err.txt', JSON.stringify(err));
           fs.writeFileSync('job_status_info.txt', JSON.stringify(info));
         });
    }]);  
}