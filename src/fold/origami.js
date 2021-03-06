/**
 * Each of these should return an array of Edges
 * 
 * Each of the axioms create full-page crease lines
 *  ending at the boundary; in non-convex paper, this
 *  could result in multiple edges
 */

// "re:boundaries_vertices" = [[5,3,9,7,6,8,1,2]];
// "re:faces_matrix" = [[1,0,0,1,0,0]];

import * as Geom from "../../lib/geometry";
import * as Graph from "./graph";
import * as PlanarGraph from "./planargraph";
import { apply_diff } from "./diff";

export function crease_folded(graph, point, vector, face_index) {
	// if face isn't set, it will be determined by whichever face
	// is directly underneath point. or if none, index 0.
	if (face_index == null) {
		// todo, detect face under point
		face_index = 0;
		// let faces = Array.from(chopReflect.svg.childNodes)
		// 	.filter(el => el.getAttribute('id') == 'faces')
		// 	.shift();
		// faces.childNodes[face_index].setAttribute("class", "face");
		// face_index = found;
	}
	let primaryLine = Geom.Line(point, vector);
	let coloring = Graph.face_coloring(graph, face_index);
	PlanarGraph.make_faces_matrix_inv(graph, face_index)
		.map(m => primaryLine.transform(m))
		.reverse()
		.forEach((line, reverse_i, arr) => {
			let i = arr.length - 1 - reverse_i;
			PlanarGraph.split_convex_polygon(graph, i, line.point, line.vector, coloring[i] ? "M" : "V");
		});
}

export function crease_line(graph, point, vector) {
	// let boundary = Graph.get_boundary_vertices(graph);
	// let poly = boundary.map(v => graph.vertices_coords[v]);
	// let edge_map = Array.from(Array(graph.edges_vertices.length)).map(_=>0);
	let new_edges = [];
	let arr = Array.from(Array(graph.faces_vertices.length)).map((_,i)=>i).reverse();
	arr.forEach(i => {
		let diff = PlanarGraph.split_convex_polygon(graph, i, point, vector);
		if (diff.edges != null && diff.edges.new != null) {
			// a new crease line was added
			let newEdgeIndex = diff.edges.new[0].index;
			new_edges = new_edges.map(edge => 
				edge += (diff.edges.map[edge] == null ? 0 : diff.edges.map[edge])
			);
			new_edges.push(newEdgeIndex);
		}
	});
	return new_edges;
}

export function crease_ray(graph, point, vector) {
	let new_edges = [];
	let arr = Array.from(Array(graph.faces_vertices.length)).map((_,i)=>i).reverse();
	arr.forEach(i => {
		let diff = PlanarGraph.split_convex_polygon(graph, i, point, vector);
		if (diff.edges != null && diff.edges.new != null) {
			// a new crease line was added
			let newEdgeIndex = diff.edges.new[0].index;
			new_edges = new_edges.map(edge =>
				edge += (diff.edges.map[edge] == null ? 0 : diff.edges.map[edge])
			);
			new_edges.push(newEdgeIndex);
		}
	});
	return new_edges;
}

export function axiom1(graph, pointA, pointB) { // n-dimension
	let line = Geom.core.origami.axiom1(pointA, pointB);
	return crease_line(graph, line[0], line[1]);
}
export function axiom2(graph, pointA, pointB) {
	let line = Geom.core.origami.axiom2(pointA, pointB);
	return crease_line(graph, line[0], line[1]);
}
export function axiom3(graph, pointA, vectorA, pointB, vectorB) {
	let lines = Geom.core.origami.axiom3(pointA, vectorA, pointB, vectorB);
	// return lines.map(line => crease_line(graph, line[0], line[1]))
	// 	.reduce((a,b) => a.concat(b), []);
	return crease_line(graph, lines[0][0], lines[0][1]);
}
export function axiom4(graph, pointA, vectorA, pointB) {
	let line = Geom.core.origami.axiom4(pointA, vectorA, pointB);
	return crease_line(graph, line[0], line[1]);
}
export function axiom5(graph, pointA, vectorA, pointB, pointC) {
	let line = Geom.core.origami.axiom5(pointA, vectorA, pointB, pointC);
	return crease_line(graph, line[0], line[1]);
}
export function axiom6(graph, pointA, vectorA, pointB, vectorB, pointC, pointD) {
	let line = Geom.core.origami.axiom6(pointA, vectorA, pointB, vectorB, pointC, pointD);
	return crease_line(graph, line[0], line[1]);
}
export function axiom7(graph, pointA, vectorA, pointB, vectorB, pointC) {
	let line = Geom.core.origami.axiom7(pointA, vectorA, pointB, vectorB, pointC);
	return crease_line(graph, line[0], line[1]);
}

export function creaseRay(graph, point, vector) {
	let ray = Geom.core.Ray(point, vector);
	graph.faces_vertices.forEach(face => {
		let points = face.map(v => graph.vertices_coords[v]);
		Geom.core.intersection.clip_ray_in_convex_poly(_points, point, vector);
	})
	return crease_line(graph, line[0], line[1]);
}


export function fold_without_layering(fold, face) {
	if (face == null) { face = 0; }
	let faces_matrix = PlanarGraph.make_faces_matrix(fold, face);
	let vertex_in_face = fold.vertices_coords.map((v,i) => {
		for(var f = 0; f < fold.faces_vertices.length; f++){
			if(fold.faces_vertices[f].includes(i)){ return f; }
		}
	});
	let new_vertices_coords_cp = fold.vertices_coords.map((point,i) =>
		Geom.core.algebra.multiply_vector2_matrix2(point, faces_matrix[vertex_in_face[i]]).map((n) => 
			Geom.core.clean_number(n)
		)
	)
	fold.frame_classes = ["foldedState"];
	fold.vertices_coords = new_vertices_coords_cp;
	return fold;
}
