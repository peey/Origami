//
//  planarGraph.cpp
//
//
//  Created by Robby on 7/13/16.
//
//

#include "planarGraph.h"

//////////////////////////////////////////////////////////////////
//#include "algorithms2d.h"
bool onSegment(Vertex a, Vertex point, Vertex b);
int orientation(Vertex p, Vertex q, Vertex r);
bool doIntersect(Vertex p1, Vertex q1, Vertex p2, Vertex q2);
//////////////////////////////////////////////////////////////////


PlanarGraph::PlanarGraph(){
	
}


void PlanarGraph::cleanup(){
	int i = 0;
	while(i < vertices.size()-1){
		int j = i+1;
		while(j < vertices.size()){
			bool didRemove = false;
			// do the points overlap?
			if ( hVerticesEqual(vertices[i], vertices[j], EPSILON) ){
				didRemove = mergeVertices(i, j);
			}
			// only iterate forward if we didn't remove an element
			//   if we did, it basically iterated forward for us, repeat the same 'j'
			// this is also possible because we know that j is always greater than i
			if(!didRemove)
				j++;
		}
		i++;
	}
}


bool PlanarGraph::isValid(){
	invalidEdgeCrossings();
}


// quick and easy, use a square bounding box
bool PlanarGraph::hVerticesEqual(Vertex v1, Vertex v2, float epsilon){
	return (v1.x - epsilon < v2.x && v1.x + epsilon > v2.x &&
			v1.y - epsilon < v2.y && v1.y + epsilon > v2.y);
}
bool PlanarGraph::hVerticesEqual(float x1, float y1, float x2, float y2, float epsilon){
	return (x1 - epsilon < x2 && x1 + epsilon > x2 &&
			y1 - epsilon < y2 && y1 + epsilon > y2);
}



vector<unsigned int> PlanarGraph::edgesIntersectingEdges(){
	vector<unsigned int> invalidEdges;
	for(int i = 0; i < edges.size() - 1; i++){
		for(int j = i+1; j < edges.size(); j++){
			bool one = !edgeAdjacent(i, j);
			bool two = doIntersect(vertices[ edges[i].v1 ],
								   vertices[ edges[i].v2 ],
								   vertices[ edges[j].v1 ],
								   vertices[ edges[j].v2 ]);
			if(one && two){
				//                printf("LISTEN TO LAST ONE (%d:%d):\n   +(%f, %f) (%f, %f)\n   +(%f, %f) (%f, %f)\n", one, two,
				//                       vertices[ edges[i].v1 ].x, vertices[ edges[i].v1 ].y,
				//                       vertices[ edges[i].v2 ].x, vertices[ edges[i].v2 ].y,
				//                       vertices[ edges[j].v1 ].x, vertices[ edges[j].v1 ].y,
				//                       vertices[ edges[j].v2 ].x, vertices[ edges[j].v2 ].y);
				// true: edges i and j overlap
				invalidEdges.push_back(i);
				invalidEdges.push_back(j);
			}
		}
	}
	return invalidEdges;
}

bool PlanarGraph::invalidEdgeCrossings(){
	float SCALE = 100.0;
	for(int i = 0; i < edges.size(); i++){
		for(int j = 0; j < edges.size(); j++){
			if(i != j){
				Vertex vertex[4];
				vertex[0] = vertices[ edges[i].v1 ];
				vertex[1] = vertices[ edges[i].v2 ];
				vertex[2] = vertices[ edges[j].v1 ];
				vertex[3] = vertices[ edges[j].v2 ];
				for(int k = 0; k < 4; k++){
					vertex[k].x *= SCALE;
					vertex[k].y *= SCALE;
					vertex[k].z *= SCALE;
				}
				if(doIntersect(vertex[0], vertex[1], vertex[2], vertex[3]))
					return true;
			}
		}
	}
	return false;
}

bool PlanarGraph::edgeIsValid(unsigned int edgeIndex){
	float SCALE = 100.0;
	for(int i = 0; i < edges.size(); i++){
		if(edgeIndex != i){
			Vertex vertex[4];
			vertex[0] = vertices[ edges[edgeIndex].v1 ];
			vertex[1] = vertices[ edges[edgeIndex].v2 ];
			vertex[2] = vertices[ edges[i].v1 ];
			vertex[3] = vertices[ edges[i].v2 ];
			for(int j = 0; j < 4; j++){
				vertex[j].x *= SCALE;
				vertex[j].y *= SCALE;
				vertex[j].z *= SCALE;
			}
			if(doIntersect(vertex[0], vertex[1], vertex[2], vertex[3]))
				return false;
		}
	}
	return true;
}



void PlanarGraph::findAndReplaceInstancesEdge(int *newVertexIndexMapping){
	for(int i = 0; i < edges.size(); i++){
		if(newVertexIndexMapping[ edges[i].v1 ] != -1){
			edges[i].v1 = newVertexIndexMapping[ edges[i].v1 ];
		}
		if(newVertexIndexMapping[ edges[i].v2 ] != -1){
			edges[i].v2 = newVertexIndexMapping[ edges[i].v2 ];
		}
	}
}

bool PlanarGraph::getVertexIndexAt(float x, float y, unsigned int *index){
	for(int i = 0; i < vertices.size(); i++){
		if(hVerticesEqual(vertices[i].x, vertices[i].y, x, y, USER_SETTING_GET_VERTEX_RANGE)){
			*index = i;
			return true;
		}
	}
	return false;
}



bool PlanarGraph::vertexLiesOnEdge(unsigned int vIndex, Vertex *intersect){
	Vertex v = vertices[vIndex];
	return vertexLiesOnEdge(v, intersect);
}

bool PlanarGraph::vertexLiesOnEdge(Vertex v, Vertex *intersect){
	// including a margin of error, bounding area around vertex
	
	// first check if point lies on end points
	for(int i = 0; i < vertices.size(); i++){
		if( hVerticesEqual(vertices[i], v, EPSILON) ){
			intersect->x = vertices[i].x;
			intersect->y = vertices[i].y;
			intersect->z = vertices[i].z;
			return true;
		}
	}
	
	for(int i = 0; i < edges.size(); i++){
		Vertex a = vertices[ edges[i].v1 ];
		Vertex b = vertices[ edges[i].v2 ];
		float crossproduct = (v.y - a.y) * (b.x - a.x) - (v.x - a.x) * (b.y - a.y);
		if(fabs(crossproduct) < EPSILON){
			// cross product is essentially zero, point lies along the (infinite) line
			// now check if it is between the two points
			float dotproduct = (v.x - a.x) * (b.x - a.x) + (v.y - a.y) * (b.y - a.y);
			// dot product must be between 0 and the squared length of the line segment
			if(dotproduct > 0){
				float lengthSquared = powf(b.x - a.x, 2) + powf(b.y - a.y, 2);
				if(dotproduct < lengthSquared){
					//TODO: intersection
					//                    intersect =
					return true;
				}
			}
		}
	}
	return false;
}

vector<unsigned int> PlanarGraph::connectedVertexIndices(unsigned int vIndex){
	vector<unsigned int> indices;
	// iterate over all edges
	for(int i = 0; i < edges.size(); i++){
		// if we find our index, add the vertex on the other end of the edge
		if(this->edges[i].v1 == vIndex)
			indices.push_back(this->edges[i].v2);
		if(this->edges[i].v2 == vIndex)
			indices.push_back(this->edges[i].v1);
	}
	return indices;
}

vector<unsigned int> PlanarGraph::connectingEdgeIndices(unsigned int vIndex){
	vector<unsigned int> indices;
	// iterate over all edges
	for(int i = 0; i < edges.size(); i++){
		// if we find our vertex, add the edge
		if(this->edges[i].v1 == vIndex || this->edges[i].v2 == vIndex)
			indices.push_back(i);
	}
	return indices;
}

vector<unsigned int> PlanarGraph::connectingVertexIndicesSortedRadially(unsigned int vIndex){
	vector<unsigned int> connectedVertices = connectedVertexIndices(vIndex);
	vector<float> globalAngleValues;  // calculated from global 0deg line
	// we have to query the global angle of each segment
	// so we can locally sort each clockwise or counter clockwise
	vector<float> sortedGlobalAngleValues;
	for(int i = 0; i < connectedVertices.size(); i++){
		float angle = atan2(this->vertices[connectedVertices[i]].y - this->vertices[vIndex].y,
							this->vertices[connectedVertices[i]].x - this->vertices[vIndex].x);
		globalAngleValues.push_back( angle );
		sortedGlobalAngleValues.push_back( angle );
	}
	sort(sortedGlobalAngleValues.begin(), sortedGlobalAngleValues.begin()+connectedVertices.size());
	// now each edge'd sprout angle is sorted from -pi to pi
	vector<unsigned int> connectedVertexIndicesSorted;
	for(int i = 0; i < connectedVertices.size(); i++)
		for(int j = 0; j < connectedVertices.size(); j++)
			if(sortedGlobalAngleValues[i] == globalAngleValues[j])
				connectedVertexIndicesSorted.push_back(connectedVertices[j]);
	return connectedVertexIndicesSorted;
}

vector<float> PlanarGraph::connectingVertexInteriorAngles(unsigned int vIndex, vector<unsigned int> connectedVertexIndicesSorted){
	vector<float> anglesBetweenVertices;
	vector<float> anglesOfVertices;
	for(int i = 0; i < connectedVertexIndicesSorted.size(); i++){
		float angle = atan2(this->vertices[connectedVertexIndicesSorted[i]].y - this->vertices[vIndex].y,
							this->vertices[connectedVertexIndicesSorted[i]].x - this->vertices[vIndex].x);
		anglesOfVertices.push_back(angle);
	}
	for(int i = 0; i < anglesOfVertices.size(); i++){
		// when it's the wrap around value (i==3) add 2pi to the angle it's subtracted from
		float diff = anglesOfVertices[(i+1)%anglesOfVertices.size()]
		+ (M_PI*2 * (i==3))
		- anglesOfVertices[i%anglesOfVertices.size()];
		anglesBetweenVertices.push_back( diff );
	}
	return anglesBetweenVertices;
}

void PlanarGraph::rotateEdge(int index, int indexOrigin, float angle){
	float distance = sqrt(powf( this->vertices[indexOrigin].y - this->vertices[index].y ,2)
						  +powf( this->vertices[indexOrigin].x - this->vertices[index].x ,2));
	float currentAngle = atan2(this->vertices[index].y, this->vertices[index].x);
	this->vertices[index].x = distance*cosf(currentAngle + angle);
	this->vertices[index].y = distance*sinf(currentAngle + angle);
}




/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//
//                            2D ALGORITHMS
//

// if points are all collinear
// checks if point q lies on line segment 'ab'
bool onSegment(Vertex a, Vertex point, Vertex b){
	if (point.x <= max(a.x, b.x) && point.x >= min(a.x, b.x) &&
		point.y <= max(a.y, b.y) && point.y >= min(a.y, b.y))
		return true;
	return false;
}


// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Vertex p, Vertex q, Vertex r){
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (fabs(val) <= .00001) return 0;  // collinear
	return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Vertex p1, Vertex q1, Vertex p2, Vertex q2){
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);
	
	// General case
	if (o1 != o2 && o3 != o4){
		// 0 1 0 2
		// 0 2 0 1
		//        printf("general %d %d %d %d\n", o1, o2, o3, o4);
		return true;
	}
	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) {
		//        printf("one\n");
		return true;
	}
	
	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)){
		//        printf("two\n");
		return true;
	}
	
	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)){
		//        printf("three\n");
		return true;
	}
	
	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)){
		//        printf("four\n");
		return true;
	}
	
	return false; // Doesn't fall in any of the above cases
}