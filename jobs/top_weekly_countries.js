var http = require('http');
var fs = require('fs');
var querystring = require('querystring');
var async = require('async');
var cheerio = require('cheerio');
var nodemailer = require('nodemailer');
var transporter = nodemailer.createTransport()

var mailFrom = 'phamp@math.upenn.edu';
var mailTo = ['phamp@math.upenn.edu', 'pemantle@math.upenn.edu', 'dmutz@asc.upenn.edu'].join(', ');

// determine date range, from is inclusive, to is exclusive
var toDate, fromDate;
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
  fromDate = new Date(process.argv[2]);
  toDate = new Date(process.argv[3]);
  toDate = new Date(toDate - 24*1*60*60*1000); 
  break;
  // 2 arguments specifies range
  default:
  console.error('Unknown arguments');
  process.exit(-1);
}
var upperBoundDate = new Date(toDate + 24*60*60*1000);
var fromDateString = fromDate.toISOString().slice(0,10);
var toDateString = toDate.toISOString().slice(0,10);
var countries = fs.readFileSync('../resources/dictionary.csv', 'utf8');
var countriesDict = {};
countries = countries.split('\n').map(function(country) { 
              var splitLine = country.split(',');
              var searchTerm = splitLine[1];
              if (searchTerm) countriesDict[searchTerm] = {name: splitLine[0], symbol: splitLine[2]};
              return searchTerm;
            }).filter(function(country) { return !!country; });
var countriesString = countries.join('\n');
var characterDistance = 300;
var topFilter = 25;

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
}

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
                   countryCounts.sort(function(a, b) { return b.count - a.count; });
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
<th style="' + thStyle + '">Occurences</th>\n\
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
                 htmlEmail += 'See the visualization <a href="' + (relativeUrl + visualizationLink) + '">here</a>.';
                 htmlEmail += '</p>';
                 transporter.sendMail({
                   from: mailFrom,
                   to: mailTo,
                   subject: 'Top Countries from ' + fromDateString + ' to ' + toDateString,
                   html: htmlEmail
                 }, function(err, info) {
                      if (err) console.error(err); process.exit(-1);
                      console.log(info);
                    })
               });
}