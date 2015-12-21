var width = 790;
var height = 480;
var margin = {top: 10, right: 10, bottom: 20, left: 60};
// graph
var controls = d3.select("#graph")
               .append("div").attr("id", "controls")
               .style("width", 960 + "px")
               .style("height", (500-height) + "px");
var legend = d3.select("#graph") 
             .append("div")
             .attr("id", "legend")
             .style("width", (960 - width) + "px")
             .style("height", height + "px")
             .style("display", "block")
             .style("float", "right");
var svg = d3.select("#graph")
          .append("svg")
          .attr("width", width)
          .attr("height", height)
          .style("display", "block");
svg.append("clipPath")
.attr("id", "clip")
.append("rect")
.attr("x", margin.left).attr("y", margin.top)
.attr("width", width - margin.left - margin.right)
.attr("height", height - margin.top - margin.bottom);
legend.append("h2").text("Legend");
legend = legend.append("ul");

// set up axes
var x = d3.time.scale().range([margin.left, width - margin.right]);
var y = d3.scale.linear().range([height - margin.bottom , margin.top]);
var xAxis = d3.svg.axis()
            .scale(x).orient("bottom")
            .innerTickSize(-height + margin.bottom + margin.top).outerTickSize(10).tickPadding(8)
            .tickFormat(d3.time.format("%b %d"));
var yAxis = d3.svg.axis()
            .innerTickSize(-width + margin.left + margin.right).outerTickSize(10).tickPadding(8)
            .scale(y).orient("left")
            .tickFormat(d3.format("%"));
svg.append("g")
.attr("class", "x axis")
.attr("transform", "translate(0," + (height - margin.bottom) + ")");
// d3.select(".x.axis")
// .append("text").attr("id", "x-axis-title")
// .attr("x", margin.left + (width - margin.right - margin.left)/2)
// .attr("y", 45).attr("text-anchor", "middle")
// .text("Date");
svg.append("g")
.attr("class", "y axis")
.attr("transform", "translate(" + margin.left + "," + 0 + ")");
var yAxisTitle = d3.select(".y.axis")
                 .append("text").attr("id", "y-axis-title")
                 .attr("x", -50)
                 .attr("y", margin.top + (height - margin.top - margin.left)/2)
                 .attr("text-anchor", "middle")
                 .attr("transform", "rotate(-90, -50," + (margin.top + (height - margin.top - margin.left)/2) + ")")
                 .text("Contexts");

// build controls
controls = controls.append("form");
var variableSelector = controls.append("fieldset");
variableSelector.append("label").text("Variable:")
.attr("for", "variable");
["Contexts", "Programs", "Total Matches"].forEach(function(d, idx) {
  var input = variableSelector.append("input")
              .attr("type", "radio")
              .attr("name", "variable")
              .attr("value", d)
              .on("change", function() {
                var variable = getVariable();
                var isPercent = getIsPercent();
                yAxisTitle.text(variable);
                var valueRange = calculateValue(data, variable, isPercent);
                drawAxes(minDate, maxDate, valueRange[0], valueRange[1], isPercent);
                draw();
              });
  if (idx === 0) input[0][0].checked = true;
  variableSelector.append("span").attr("class", "radio-label").text(d);
});
var isPercentSelector = controls.append("fieldset");
isPercentSelector.append("label").text("Percentage:")
.attr("for", "isPercent");
isPercentSelector.append("input")
.attr("type", "checkbox")
.attr("id", "isPercent")
.attr("name", "isPercent")
.on("change", function() {
  var variable = getVariable();
  var isPercent = getIsPercent();
  yAxisTitle.text(variable);
  var valueRange = calculateValue(data, variable, isPercent);
  drawAxes(minDate, maxDate, valueRange[0], valueRange[1], isPercent);
  draw();
})
.node().checked = true;
controls.append("button").attr("type", "button")
.text("Reset Zoom")
.on("click", brushed);
controls.append("span")
.attr("class", "info").html("Hold the <em>Control</em> key to enable tooltips.");

// set up paths
var liner = d3.svg.line()
            .x(function(d) { return x(d.Date); })
            .y(function(d) { return y(d._value); });
var strokeColor = d3.scale.category10();
// point tip
var pointTip = d3.tip()
               .attr('class', 'tip')
               .direction('e')
               .html(function (d) {
                 var percentFormatter = d3.format('%');
                 var html = d.Term + ', ' + d.day;
                 ["Contexts", "Programs", "Total Matches"].forEach(function(dd) {
                   html += '<br><strong>'+dd+':</strong> <span style="color:#e41a1c">';
                   html += d[dd] + ' (' + percentFormatter(d[dd]/dailyTotals[d.day][dd]) + ')</span>';
                 });
                 return html;
               });

// set up zoom brush
var brush = d3.svg.brush()
            .x(x).y(y)
            .on("brushend", brushed);
window.addEventListener("keydown", function(e) {
  if (e.keyCode === 17) {
    var brushGroup = document.querySelector('g.brush');
    if (brushGroup) brushGroup.parentNode.removeChild(brushGroup);
  }
});
window.addEventListener("keyup", function(e) {
  if (e.keyCode === 17) svg.append("g").attr("class", "brush").call(brush);
});

var data, minDate, maxDate;     //global data variables
var dailyTotals = {};
d3.csv("default-time-series.csv", function(err, rawData) {  
  rawData.forEach(function(d) {
    if (dailyTotals[d.Date] === undefined) {
      dailyTotals[d.Date] = {"Programs": 0, "Contexts": 0, "Total Matches": 0};
    }
    d.day = d.Date;
    d.Programs = parseInt(d.Programs);
    d.Contexts = parseInt(d.Contexts);
    d["Total Matches"] = parseInt(d["Total Matches"]);
    dailyTotals[d.Date].Programs += d.Programs;
    dailyTotals[d.Date].Contexts += d.Contexts;
    dailyTotals[d.Date]["Total Matches"] += d["Total Matches"];
  });
  data = d3.nest()
         .key(function(d) { return d.Term; })  
         .entries(rawData);
  data.forEach(function(d) {
    d.values.forEach(function(d) {
      d.Date = d3.time.format("%Y-%m-%d").parse(d.Date);
    })
    d.values.sort(function(a, b) { return a.Date - b.Date; });
  });
  data.sort(function(a, b) {
    var aSum = d3.sum(a.values.slice(-3), function(d) { return d.Contexts; });
    var bSum = d3.sum(b.values.slice(-3), function(d) { return d.Contexts; });
    return bSum - aSum;
  });
  data = data.slice(0, 10);
  strokeColor.domain(data.map(function(d) { return d.key; }));                     
  // set up legend
  legend.selectAll("li")
  .data(data, function(d) { return d.key; })
  .enter()
  .append("li")
  .each(function(d) {
    var w = 40;
    var h = 30;
    var svg = d3.select(this).append("svg")
              .attr("width", 960-width)
              .attr("height", 30);
    svg.append("line")
    .attr("x1", 0).attr("y1", h/2)
    .attr("x2", w).attr("y2", h/2)
    .style("stroke", strokeColor(d.key));
    svg.append("circle")
    .attr("cx", w/2).attr("cy", h/2).attr("r", 2.5)
    .style("fill", strokeColor(d.key));
    svg.append("text").text(d.key)
    .attr("x", w + 5).attr("y", h/2).attr("dy", "5px");
    svg.on("mouseover", function() {
      d3.selectAll("path.data")
      .filter(function(dd) {
        return dd.key === d.key;
      }).transition().duration(100)
      .style("stroke-width", 3);
      d3.selectAll("circle.data")
      .filter(function(dd) {
        return dd.Term === d.key;
      }).transition().duration(100)
      .attr("r", 5);
    })
    .on("mouseout", function() {
      d3.selectAll("path.data")
      .filter(function(dd) {
        return dd.key === d.key;
      }).transition().duration(100)
      .style("stroke-width", 1);
      d3.selectAll("circle.data")
      .filter(function(dd) {
        return dd.Term === d.key;
      }).transition().duration(100)
      .attr("r", 2.5);
    });
  });


  minDate = d3.min(data[0].values, function(d) { return d.Date; });
  maxDate = d3.max(data[0].values, function(d) { return d.Date; });
  var minValue = d3.min(data.map(function(d) {
                          return d3.min(d.values, function(dd) { return dd.Contexts; });
                        }));
  var maxValue = d3.min(data.map(function(d) {
                          return d3.max(d.values, function(dd) { return dd.Contexts; });
                        }));
  var valueRange = calculateValue(data, getVariable(), getIsPercent());
  drawAxes(minDate, maxDate, valueRange[0], valueRange[1], getIsPercent());
  var paths = svg.selectAll("path.data")
              .data(data, function(d) { return d.key; })
              .enter().append("path")
              .attr("clip-path", "url(#clip)")
              .attr("class", "data")         
              .style("fill", "none")
              .style("stroke", function(d) { return strokeColor(d.key); }); 
  svg.selectAll("circle.data")
  .data([].concat.apply([], data.map(function(d) { return d.values; })), 
        function(d) { return d.Date.toJSON() + "|" + d.Term; })
  .enter().append("circle")
  .attr("clip-path", "url(#clip)")
  .attr("class", "data")
  .attr("r", 2.5)
  .style("fill", function(d) { return strokeColor(d.Term); })
  .style("stroke", "none")
  .on("mouseover", pointTip.show)
  .on("mouseout", pointTip.hide);
  draw();
  svg.append("g").attr("class", "brush").call(brush);
  svg.call(pointTip);
});


function calculateValue(data, variable, isPercent) {
  var minValue = Number.MAX_VALUE;
  var maxValue = 0;
  data.forEach(function(d) {
    d.values.forEach(function(dd) {
      var value = dd[variable];
      if (isPercent) value /= dailyTotals[dd.day][variable];
      if (minValue > value) minValue = value;
      if (maxValue < value) maxValue = value;
      dd._value = value;
    });
  });
  return [minValue, maxValue];
}

function drawAxes(minDate, maxDate, minValue, maxValue, percent) {
  x.domain([minDate, maxDate]);
  y.domain([minValue, maxValue]); 
  if (percent === true) {
    yAxis.tickFormat(d3.format("%"));
  } else {
    yAxis.tickFormat(d3.format("d"));
  }
  d3.select("g.x.axis").transition().duration(1000).call(xAxis);  
  d3.select("g.y.axis").transition().duration(1000).call(yAxis);  
}

function draw() {
  svg.selectAll("path.data")
  .transition().duration(1000)
  .attr("d", function(d) { return liner(d.values); })
  svg.selectAll("circle.data")
  .transition().duration(1000)
  .attr("cx", function(d) { return x(d.Date); })
  .attr("cy", function(d) { return y(d._value); });
}

function getVariable() {
  var inputs = Array.prototype.slice.call(document.querySelectorAll('input[name="variable"]'));
  return inputs.filter(function(input) { return input.checked; })[0].value;

}

function getIsPercent() {
  return document.getElementById("isPercent").checked;
}

function brushed() {
  if (brush.empty()) {
    var variable = getVariable();
    var isPercent = getIsPercent();
    yAxisTitle.text(variable);
    var valueRange = calculateValue(data, variable, isPercent);
    drawAxes(minDate, maxDate, valueRange[0], valueRange[1], isPercent);
    draw(); 
  } else {
    var variable = getVariable();
    var isPercent = getIsPercent();
    yAxisTitle.text(variable);
    var valueRange = calculateValue(data, variable, isPercent);
    var extent = brush.extent();
    d3.selectAll("g.brush").call(brush.clear());
    drawAxes(extent[0][0], extent[1][0], extent[0][1], extent[1][1], isPercent);
    draw();     
  }  
}
