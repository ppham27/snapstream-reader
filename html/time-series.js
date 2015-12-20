var width = 960;
var height = 500;
// graph
d3.select("#graph")
.append("svg")
.attr("width", width)
.attr("height", height);

var data;
d3.csv("default-time-series.csv", function(err, rawData) {  
  data = d3.nest()
         .key(function(d) { return d.Term; })  
         .entries(rawData);
  data.forEach(function(d) {
    d.values.forEach(function(d) {
      d.Contexts = parseInt(d.Contexts);
      d["Total Matches"] = parseInt(d["Total Matches"]);
      d.Programs = parseInt(d.Programs);
      d.Date = d3.time.format('%Y-%m-%d').parse(d.Date);
    })
    d.values.sort(function(a, b) { return a.Date - b.Date; });
  });
  data.sort(function(a, b) {
    console.log()
    var aSum = d3.sum(a.values.slice(-3), function(d) { return d.Contexts; });
    var bSum = d3.sum(b.values.slice(-3), function(d) { return d.Contexts; });
    return bSum - aSum;
  });
  var minDate = d3.min(data[0].values, function(d) { return d.Date; });
  var maxDate = d3.max(data[0].values, function(d) { return d.Date; });
});
