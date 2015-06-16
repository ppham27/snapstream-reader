/*global graphData */
var width = 720, height = 720;
var maxSize = 50;
var minDistance = 2*maxSize;
var maxDistance = width - 2*maxSize;
var margin = maxSize;
var rightSidebar = d3.select("#graph")
                   .append("div")
                   .attr("id", "rightSidebar");
var svg = d3.select("#graph")
          .append("svg")
          .attr("width", width)
          .attr("height", height);
var drag = d3.behavior.drag()
           .origin(function(d) { return d; })
           .on("dragstart", dragstarted)
           .on("drag", dragged)
           .on("dragend", dragended);
var layoutSelector = rightSidebar.append("div")
                     .attr("id", "layout-selector");
layoutSelector.append("h2").text("Layouts");
layoutSelector.append("button")
.text("Spring Embed")
.on("click", springEmbedLayout);
layoutSelector.append("br");
layoutSelector.append("button")
.text("Random")
.on("click", randomLayout);
layoutSelector.append("br");
layoutSelector.append("button")
.text("Reset")
.on("click", circularLayout);
var timeSelector = rightSidebar.append("div")
                   .attr("id","time-selector");
timeSelector.append("h2").text("Time Period");
var timeSelectorForm = timeSelector.append("form")
                       .on("change", timeChange);
var info = rightSidebar.append("div")
           .attr("id", "info");
info.append("h2").text("Information");
info.append("p")
.text('This visualization shows the relationship between countries. Countries are considered to be related if they are frequently mentioned together on television. Mouseover nodes and links to see more information about the countries and how they are related. Nodes can be dragged. ');
info.append("p")
.html('Using <span style="color:#e41a1c">Spring Embed</span> may help you see the relationship more clearly. The algorithm tries to place countries that are frequently mentioned together close together. The algorithm works by placing a short spring between closely related countries and a long spring between unrelated countries. Then, the total potential energy is minimized by applying Newton\'s method iteratively. One may need to click <span style="color:#e41a1c">Random</span> and <span style="color:#e41a1c">Spring Embed</span> a few times before a desirable layout is found. <span style="color:#e41a1c">Reset</span> puts everything back in a circle.');
info.append("p")
.html('By selecting a different time period, one can see how the relationships between countries change over time.');
info.append("h2").text("Upload File");
var fileUploader = info.append("form").attr("method", "POST")
                   .attr("action",'cgi-bin/process_file')
                   .attr("enctype",'multipart/form-data');
fileUploader.append("span").text("Matrix:").attr("class", "file-label");
fileUploader.append("input")
.attr("type", "file")
.attr("name", "matrix_file")
.attr("accept", "text/plain");
fileUploader.append("br");
fileUploader.append("input")
.attr("type", "submit")
.attr("value", "Submit");

var nodeTip = d3.tip()
              .attr('class', 'tip')
              .direction('e')
              .html(function (d) {
                return d.name + '<br>'
                     + '<strong>Size:</strong> <span style="color:#e41a1c">' + d.size + '</span>';;
              });
var linkTip = d3.tip()
              .attr('class', 'tip')
              .offset(function() {
                return [this.getBBox().height/2, 120];
              })
              .html(function (d) {
                return '<span style="color:#e41a1c">' + graphData.nodes[d.source].name + '</span><br>'
                     + '<span style="color:#e41a1c">' + graphData.nodes[d.target].name + '</span><br>'
                     + '<strong>Distance:</strong> <span style="color:#e41a1c">' + d.distance + '</span>';
              });
svg.call(nodeTip);
svg.call(linkTip);

var graphData;
var link = svg.selectAll(".link.data");
var node = svg.selectAll(".node.data");
var nodeLabel = svg.selectAll(".node-label.data");
var urlQuery = {};
window.location.search.slice(1).split('&').forEach(function(keyValues) {
  var kvSplit = keyValues.split('=');  
  urlQuery[kvSplit[0]] = kvSplit[1];
});
var fileName = decodeURIComponent(urlQuery['filename'] || 'default.json');
d3.json(fileName, function(err, graph) {
  graphData = graph;
  // add times
  for (var i = 0; i < graphData.times.length; ++i) {
    var timeSelectorRadio = timeSelectorForm.append("input")
                            .attr("type", "radio")
                            .attr("name", "time")
                            .attr("value", i);
    if (i === 0) timeSelectorRadio.attr("checked", true);
    timeSelectorForm.append("span").attr("class", "time-label").text(graphData.times[i].name);
    if (i !== graphData.times.length - 1) timeSelectorForm.append("br");
  }
  if (graphData.times.length === 1) timeSelector.style("display", "none");
  
  // build link data
  var links = [];
  var maxLinkDistance = -1;
  var timeKey = getTimeKey();
  for (var i = 0; i < graph.nodes.length - 1; ++i) {
    for (var j = i + 1; j < graph.nodes.length; ++j) {
      links.push({source: i, target: j, distance: graphData.links[i][j][timeKey]});
      graphData.links[i][j].l = graphData.links[j][i].l = graphData.links[i][j][timeKey];
      if (graphData.links[i][j].l > maxLinkDistance) maxLinkDistance = graphData.links[i][j].l;
    }
  }
  // renormalize length
  graphData.links.forEach(function(d) {
    d.forEach(function(dd) {
      dd.l = minDistance + (maxDistance - minDistance)*dd.l/maxLinkDistance;
    });
  });
  link = link.data(links)
         .enter().append("line")
         .attr("class", "link data")
         .on('mouseover', linkTip.show)
         .on('mouseout', linkTip.hide);
  var maxNodeSize = d3.max(graph.nodes, function(d) { return d.size; })
  node = node.data(graph.nodes)
         .enter().append("circle")
         .attr("class", "node data")
         .attr("r", function(d) { return maxSize*Math.sqrt(d.size/maxNodeSize); })
         .on('mouseover', function(d) { 
           var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
           nodeTip.show(d, target.node());
         })
         .on('mouseout',  function(d) { 
           var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
           nodeTip.hide(d, target.node());
         })
         .call(drag);
  nodeLabel = nodeLabel.data(graph.nodes)
              .enter().append("text")
              .attr("class", "node-label data")
              .attr("text-anchor", "middle")
              .attr("dy", "5px")
              .text(function(d) { return d.symbol; })
              .on('mouseover', function(d) {
                var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
                nodeTip.show(d, target.node());
              })
              .on("mouseout", function(d) {
                var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
                nodeTip.hide(d, target.node());
              })
              .call(drag);
  circularLayout();
  draw();
});

// draw graph
function draw(duration) {
  duration = typeof duration !== 'undefined' ? duration : 0;
  link.transition().duration(duration)
  .attr("x1", function(d) { return graphData.nodes[d.source].x; })
  .attr("y1", function(d) { return graphData.nodes[d.source].y; })
  .attr("x2", function(d) { return graphData.nodes[d.target].x; })
  .attr("y2", function(d) { return graphData.nodes[d.target].y; });
  node.transition().duration(duration)
  .attr("cx", function(d) { return d.x; })
  .attr("cy", function(d) { return d.y; });
  nodeLabel.transition().duration(duration)
  .attr("x", function(d) { return d.x; })
  .attr("y", function(d) { return d.y; });
}


// various layouts
function springEmbedLayout() {
  var nodes = graphData.nodes;
  var links = graphData.links;
  var n = nodes.length;
  function distance(a, b) {
    return Math.sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
  }
  function energy() {
    var E = 0;
    for (var i = 0; i < n - 1; ++i) {
      for (var j = i+1; j < n; ++j) {
        var diff = distance(nodes[i], nodes[j]) - links[i][j].l;
        E += 0.5*links[i][j].k*diff*diff;
      }
    }
    return E;
  }
  function dEd(idx, f) {
    var res = 0;
    for (var i = 0; i < n; ++i) {
      if (i === idx) continue;
      res += links[idx][i].k*((f(nodes[idx]) - f(nodes[i])) - links[idx][i].l*(f(nodes[idx]) - f(nodes[i]))/distance(nodes[idx], nodes[i]));            
    }

    return res;
  }
  function dEdx(idx) {
    return dEd(idx, function(d) { return d.x; });
  }
  function dEdy(idx) {
    return dEd(idx, function(d) { return d.y; });
  }  
  function ddEdxdx(idx) {
    var res = 0;
    for (var i = 0; i < n; ++i) {
      if (i === idx) continue;
      var yDiff = nodes[idx].y-nodes[i].y;
      var dist = distance(nodes[idx], nodes[i]);
      res += links[idx][i].k*(1 - links[idx][i].l*yDiff*yDiff/(dist*dist*dist));
    }
    return res;
  }
  function ddEdydy(idx) {
    var res = 0;
    for (var i = 0; i < n; ++i) {
      if (i === idx) continue;
      var xDiff = nodes[idx].x-nodes[i].x;
      var dist = distance(nodes[idx], nodes[i]);
      res += links[idx][i].k*(1 - links[idx][i].l*xDiff*xDiff/(dist*dist*dist));
    }
    return res;
  }
  function ddEdxdy(idx) {
    var res = 0;
    for (var i = 0; i < n; ++i) {
      if (i === idx) continue;
      var xDiff = nodes[idx].x-nodes[i].x;
      var yDiff = nodes[idx].y-nodes[i].y;
      var dist = distance(nodes[idx], nodes[i]);
      res += links[idx][i].k*(links[idx][i].l*xDiff*yDiff/(dist*dist*dist));
    }
    return res;
  }
  function ddEdydx(idx) {
    return ddEdxdy(idx);
  }
  var E = energy();
  var deltaEs = [];
  for (var i = 0; i < n; ++i) deltaEs.push(E);
  var done = false;
  var counter = 0;
  while (!done) {
    var maxIdx = -1; 
    var maxEi = -1;
    for (var i = 0; i < n; ++i) {
      var dx = dEdx(i); 
      var dy = dEdy(i);
      var newEi = Math.sqrt(dx*dx + dy*dy);
      if (maxEi < newEi) { 
        maxEi = newEi;
        maxIdx = i;
      }
    }
    var deltaEi = maxEi;
    while (deltaEi > 0.000001) {
      // iterate with Newton's method      
      // [ a b ]
      // [ c d ]
      var a = ddEdxdx(maxIdx);
      var b = ddEdxdy(maxIdx);
      var c = ddEdydx(maxIdx);
      var d = ddEdydy(maxIdx);
      // inverted this becomes
      // [ d -b]
      // [ -c a]/(ad - bc)
      var deltaX = -(d*dEdx(maxIdx) - b*dEdy(maxIdx))/(a*d-b*c);
      var deltaY = -(-c*dEdx(maxIdx) + a*dEdy(maxIdx))/(a*d-b*c);
      nodes[maxIdx].x += deltaX;
      nodes[maxIdx].y += deltaY;
      var dx = dEdx(maxIdx); 
      var dy = dEdy(maxIdx);
      var newEi = Math.sqrt(dx*dx + dy*dy);
      deltaEi = maxEi - newEi;
      maxEi = newEi;
    }
    var newE = energy();
    deltaEs.shift();
    deltaEs.push(E - newE);
    E = newE;
    done = deltaEs.every(function(d) { return d <= 0.0001; });    
  }
  // recenter
  var cX = 0; 
  var cY = 0;
  for (var i = 0; i < n; ++i) {
    cX += nodes[i].x;
    cY += nodes[i].y;
  }
  cX /= n; 
  cY /= n;
  nodes.forEach(function(d) {
    d.x += width/2 - cX;
    d.y += height/2 - cY;
  });
  draw(1000);
  return E;
}
function randomLayout() {
  node.each(function(d) {
    d.x = margin + Math.random()*(width - 2*margin);
    d.y = margin + Math.random()*(height - 2*margin);
  });
  draw(1000);
}

function circularLayout() {
  node.each(function(d, i) {
    d.x = margin + (width-2*margin)/2*Math.cos(2*Math.PI*i/node[0].length) + (width-2*margin)/2;
    d.y = margin + (width-2*margin)/2*Math.sin(2*Math.PI*i/node[0].length) + (width-2*margin)/2;
  });
  draw(1000);
}


// drag handlers
function dragstarted(d) {
  d3.event.sourceEvent.stopPropagation();
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.hide(d, target.node());
}
function dragged(d) {
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.hide(d, target.node());
  d.x = d3.event.x;
  d.y = d3.event.y;
  draw();
}
function dragended(d) {
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.show(d, target.node());
}

function getTimeKey() {
  return graphData.times[parseInt(timeSelectorForm.selectAll("input")
                                  .filter(function() { return this.checked === true; })
                                  .node().value)].key;
}

function timeChange() {  
  var timeKey = getTimeKey();
  link.each(function(d) {
    d.distance = graphData.links[d.source][d.target][timeKey];
    graphData.links[d.source][d.target].l = graphData.links[d.target][d.source].l = graphData.links[d.source][d.target][timeKey];
  });
  graphData.links.forEach(function(d) {
    d.forEach(function(dd) {
      dd.l = minDistance + (maxDistance - minDistance)*dd.l/100;
    });
  });
  return springEmbedLayout();
}