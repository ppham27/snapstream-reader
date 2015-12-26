/*global countries */
var http = require('http');
var fs = require('fs');
var url = require('url');
var querystring = require('querystring');
var async = require('async');
var cheerio = require('cheerio');
var nodemailer = require('nodemailer');
var transporter = nodemailer.createTransport();

var mailFrom = 'phamp@math.upenn.edu';
var mailTo = process.env.NODE_ENV === 'test' ? 'phamp@math.upenn.edu' : ['phamp@math.upenn.edu', 'pemantle@math.upenn.edu', 'dmutz@asc.upenn.edu'].join(', ');

// determine date range, from is inclusive, to is exclusive
var toDate, fromDate;
var readFile = false;
var file;
switch (process.argv.length) {
  case 2:
  // no arguments means last 7 days
  toDate = new Date();
  fromDate = new Date(toDate - 24*7*60*60*1000);
  toDate = new Date(toDate - 24*1*60*60*1000); // server takes inclusive range
  break;
  case 3:
  // 1 arguments specifies to date
  toDate = new Date(process.argv[2]);
  fromDate = new Date(toDate - 24*7*60*60*1000);
  toDate = new Date(toDate - 24*1*60*60*1000);
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

var countries = fs.readFileSync('../resources/dictionary.csv', 'utf8');
var countriesDict = {};
countries = countries.split('\n').map(function(country) { 
              var splitLine = country.split(',');
              var searchTerm = splitLine[1];
              if (searchTerm && splitLine[3] === "Country") countriesDict[searchTerm] = {name: splitLine[0], symbol: splitLine[2]};
              return splitLine[3] === "Country" ? searchTerm : undefined;
            }).filter(function(country) { return !!country; });

var upperBoundDate, fromDateString, toDateString;
if (readFile === true) {
  var responseBody = fs.readFileSync(file, 'utf8');
  var $ = cheerio.load(responseBody);  
  fromDate = new Date($('#from-date').text());
  toDate = new Date($('#to-date').text());
  upperBoundDate = new Date(toDate + 24*60*60*1000);
  fromDateString = fromDate.toISOString().slice(0,10);
  toDateString = toDate.toISOString().slice(0,10);
  processResponse(responseBody);
} else {
  upperBoundDate = new Date(toDate + 24*60*60*1000);
  fromDateString = fromDate.toISOString().slice(0,10);
  toDateString = toDate.toISOString().slice(0,10);
  var characterDistance = 300;
  var topFilter = 25;
  var countriesString = countries.join('\n');
  var body = {"from-date": fromDateString,
              "to-date": toDateString,
              "search-strings": countriesString,
              "distance": characterDistance,
              "top-filter": topFilter};
  var postData = querystring.stringify(body);
  var requestOptions = {
    hostname: 'johnny.sas.upenn.edu',
    port: 80,
    path: '/~pemantle/cgi-bin/pair_search',
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
                fs.writeFileSync(upperBoundDate.toISOString().slice(0, 10) + '.html', responseBody);
              });
            });    

  req.write(postData);
  req.end();
}

function processResponse(body) {
  var relativeUrl = 'http://johnny.sas.upenn.edu/~pemantle/cgi-bin/';
  var $ = cheerio.load(body);

  var matrixLink = $('#matrix').attr('href');
  var visualizationLink = $('#visualization-link').attr('href');
  async.series([
    function(done) {
      var countryCounts = [];
      http.get(relativeUrl + matrixLink,
               function(res) {
                 var matrixFile = new String();
                 res.on('data', function(chunk) {
                   matrixFile += chunk;
                 });
                 res.on('end', function() {
                   var lines = matrixFile.split('\n');
                   lines.splice(0, 1);
                   lines.splice(lines.length - 1, 1);
                   lines.forEach(function(line, idx) {
                     var splitLine = line.split(',');
                     var searchPattern = splitLine.splice(0, 1)[0];
                     var count = parseInt(splitLine[idx]);   
                     countryCounts.push({name: countriesDict[searchPattern].name,
                                         searchPattern: searchPattern,
                                         symbol: countriesDict[searchPattern].symbol,
                                         count: count})
                   });
                   countryCounts.sort(function(a, b) { 
                     if (b.count - a.count !== 0) {
                       return b.count - a.count; 
                     } else if (a.searchPattern < b.searchPattern) {
                       return -1;
                     } else if (a.searchPattern > b.searchPattern) {
                       return 1;
                     } else {
                       return 0;
                     }
                   });
                   done(null, countryCounts);
                 });             
               })
    }],
               function(err, res) {
                 var thStyle = 'border-bottom: 1px solid black; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';
                 var htmlEmail = new String('<p>The top countries are <table style="border-collapse: collapse"><thead><tr>\n\
<th style="' + thStyle + '">Rank</th>\
<th style="' + thStyle + '">Country</th>\
<th style="' + thStyle + '">Search Pattern</th>\
<th style="' + thStyle + '">Symbol</th>\
<th style="' + thStyle + '">Occurring Contexts</th>\n\
</tr></thead><tbody>');
                 res[0].slice(0,25).forEach(function(country, idx) {
                   var style = 'text-align: center; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';
                   htmlEmail += '\n<tr>\
<td style="' + style + '">' + (idx + 1) + '</td>\n\
<td style="' + style + '">' + country.name + '</td>\n\
<td style="' + style + '">' + country.searchPattern + '</td>\n\
<td style="' + style + '">' + country.symbol + '</td>\n\
<td style="' + style + '">' + country.count + '</td>\n\
</tr>'
                 });                 
                 htmlEmail += '</tbody></table></p>';                 
                 htmlEmail += '\n<p>';
                 var visualizationFilePath = querystring.parse(url.parse(relativeUrl + visualizationLink).query).filename;
                 if (fs.existsSync('../html/tmp/daily_country') && fs.existsSync('../html/' + visualizationFilePath)) {
                   fs.createReadStream('../html/' + visualizationFilePath)
                   .pipe(fs.createWriteStream('../html/tmp/daily_country' + visualizationFilePath.substr(3))); // rm tmp part of file path
                   visualizationLink = visualizationLink.replace('filename=tmp%2F','filename=tmp%2Fdaily_country%2F');
                 }
                 htmlEmail += 'See the visualization <a href="' + (relativeUrl + visualizationLink) + '">here</a>.';
                 htmlEmail += '</p>';
                 transporter.sendMail({
                   from: mailFrom,
                   to: mailTo,
                   subject: 'Top Countries from ' + fromDateString + ' to ' + toDateString,
                   html: htmlEmail
                 }, function(err, info) {
                      if (err) fs.writeFileSync('job_status_err.txt', JSON.stringify(err));
                      fs.writeFileSync('job_status_info.txt', JSON.stringify(info));
                    });
               });
}