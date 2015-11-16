var http = require('http');
var fs = require('fs');
var url = require('url');
var querystring = require('querystring');
var async = require('async');
var cheerio = require('cheerio');
var nodemailer = require('nodemailer');
var smtpTransport = require('nodemailer-smtp-transport');
var transporter = nodemailer.createTransport();
// var transporter = nodemailer.createTransport(smtpTransport(
//   {host: 'smtpout.secureserver.net',
//    secure: true,
//    port: 465,
//    auth: {
//      user: '',
//      pass: ''
//    }}));
var intervalLength = 28;        //in days
var mailFrom = 'phamp@math.upenn.edu';
var mailTo = process.env.NODE_ENV === 'test' ? 'phamp@math.upenn.edu' : ['phamp@math.upenn.edu', 'pemantle@math.upenn.edu', 'dmutz@asc.upenn.edu'].join(', ');

var toDateA, fromDateA, toDateB, fromDateB;
var readFile = false;
var file;
switch (process.argv.length) {
  case 2:
  // no arguments, last two weeks
  toDateB = new Date();         //start with exclusive
  fromDateB = new Date(toDateB - 24*intervalLength*60*60*1000);
  toDateA = new Date(fromDateB - 24*1*60*60*1000);
  fromDateA = new Date(fromDateB - 24*intervalLength*60*60*1000);
  toDateB = new Date(toDateB - 24*1*60*60*1000); // convert back to inclusive
  break;
  case 3:
  toDateB = new Date(process.argv[2]); //exlusive date
  fromDateB = new Date(toDateB - 24*intervalLength*60*60*1000);
  toDateA = new Date(fromDateB - 24*1*60*60*1000);
  fromDateA = new Date(fromDateB - 24*intervalLength*60*60*1000);
  toDateB = new Date(toDateB - 24*1*60*60*1000); // convert back to inclusive
  break;
  case 4:
  if (process.argv[2] === 'file') {
    readFile = true;
    file = process.argv[3];
  } else {
    console.error('Unknown arguments');
    process.exit(-1);
  }
  break;
  case 6:
  fromDateA = new Date(process.argv[2]);
  toDateA = new Date(process.argv[3]);
  fromDateB = new Date(process.argv[4]);
  toDateB = new Date(process.argv[5]);
  break;  
  default:
  console.error('Unknown arguments');
  process.exit(-1);
}

var body;
if (readFile === true) {
  var responseBody = fs.readFileSync(file, 'utf8');
  var $ = cheerio.load(responseBody);
  body = {
    "from-date-a": $('#from-date-a').text(),
    "to-date-a": $('#to-date-a').text(),
    "from-date-b": $('#from-date-b').text(),
    "to-date-b": $('#to-date-b').text()
  }  
  processResponse(responseBody);
} else {
  body = {
    "from-date-a": fromDateA.toISOString().slice(0,10),
    "to-date-a": toDateA.toISOString().slice(0,10),
    "from-date-b": fromDateB.toISOString().slice(0,10),
    "to-date-b": toDateB.toISOString().slice(0,10),
    "min-occurences": 40,         //hard code for now
    "percent-increase": 4.0
  }
  var postData = querystring.stringify(body);
  var requestOptions = {
    hostname: 'johnny.sas.upenn.edu',
    port: 80,
    path: '/~pemantle/cgi-bin/hot_list',
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Content-Length': postData.length
    }    
  }

  var responseBody = new String();
  var req = http.request(requestOptions, function(res) {
              res.on('data', function(chunk) { responseBody += chunk; });
              res.on('end', function() {
                processResponse(responseBody);
                fs.writeFileSync(body['from-date-a'] + '_' + body['to-date-b'] + '_hot_list.html', responseBody);
              });
            });
  req.write(postData);
  req.end();
}

function processResponse(responseBody) {
  var relativeUrl = 'http://johnny.sas.upenn.edu/~pemantle/cgi-bin/';
  var $ = cheerio.load(responseBody);  
  var hotListLink = $('#hot-list-file').attr('href');
  http.get(relativeUrl + hotListLink,
           function(res) {
             var hotListFile = new String();
             res.on('data', function(chunk) {
               hotListFile += chunk;
             });
             res.on('end', function() {
               var lines = hotListFile.split('\n');
               var header = lines[0].split(',');
               var data = lines
                          .slice(1)
                          .filter(function(l) {
                            return l.length;
                          })
                          .map(function(l) { return l.split(','); });
               // now build email
               var thStyle = 'border-bottom: 1px solid black; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';
               var style = 'text-align: center; padding-left: 1em; padding-right: 1em; padding-top: 0.25em; padding-bottom: 0.25em';               
               var htmlEmail = new String('<p>The top words are</p>')
               htmlEmail += '<table style="border-collapse: collapse"><thead><tr>\n';
               header.forEach(function(head) {
                 htmlEmail += '<th style="' + thStyle + '">' + head + '</th>\n'
               });
               htmlEmail += '</tr></thead><tbody>\n'
               data.forEach(function(r) {
                 htmlEmail += '<tr>\n';
                 r.forEach(function(d) {
                   htmlEmail += '<td style="' + style + '">' + d + '</td>\n';
                 });
                 htmlEmail += '</tr>\n';
               });
               htmlEmail += '</tbody></table>';
               htmlEmail += '<p>The hot list can be downloaded <a href="'+relativeUrl + hotListLink+'">here</a>.</p>'
               transporter.sendMail({
                 from: mailFrom,
                 to: mailTo,
                 subject: 'Hot list from ' + body['from-date-a'] + ' to ' + body['to-date-b'],
                 html: htmlEmail
               }, function(err, info) {
                    if (err) fs.writeFileSync('job_status_err.txt', JSON.stringify(err));
                    fs.writeFileSync('job_status_info.txt', JSON.stringify(info));
                  });
             });
           });
}