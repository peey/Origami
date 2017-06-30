var g = new Graph();
fillGraph(g, 5);

var d3Graph = graphToD3(g);
var svgCanvas = d3.select("#svgTest00");
makeForceDirectedGraph(d3Graph, svgCanvas);

function fillGraph(graph, numNodes){
	var numEdges = numNodes*1.2;
	graph.clear();
	for(var i = 0; i < numNodes; i++) graph.newNode();
	for(var i = 0; i < graph.nodes.length; i++){
		// inverse relationship to scramble edges# with node#
		var first = parseInt(graph.nodes.length-1-i);
		var match;
		do{ match = Math.floor(Math.random()*graph.nodes.length);
		} while(match == first);
		graph.newEdge(first, match);
	}
	if(numEdges > numNodes){
		for(var i = 0; i < numEdges - numNodes; i++){
			var rand1 = Math.floor(Math.random()*numNodes);
			var rand2 = Math.floor(Math.random()*numNodes);
			graph.newEdge( rand1, rand2 );
		}
	}
	graph.clean();
	
	// fill DOM
	var nodeString = '';
	for(var i = 0; i < graph.nodes.length-1; i++){
		nodeString += i + ', ';
	}
	nodeString += graph.nodes.length-1;
	var edgeString = '';
	for(var i = 0; i < graph.edges.length-1; i++){
		edgeString += '[' + graph.edges[i].node[0] + ',' + graph.edges[i].node[1] + '], ';
	}
	edgeString += '[' + graph.edges[graph.edges.length-1].node[0] + ',' + graph.edges[graph.edges.length-1].node[1] + ']';
	var graphString = '{nodes: [' + nodeString + '],<br>&nbsp;edges: [ ' + edgeString + ' ]}';
	$("#spanGraphContents").html(graphString);
}