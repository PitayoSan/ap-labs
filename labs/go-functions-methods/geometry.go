// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
)

// Point declaration and functions
type Point struct{ x, y float64 }

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

// Path declaration and functions
// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

//Helper functions
func getRandomFloatInRange(min float64, max float64) float64 {
	return (rand.Float64() * (max - min)) + min
}

func generateRandomPoints(sides int) []Point {
	var path = make([]Point, sides)
	for index := range path {
		newPoint := Point {
			x: getRandomFloatInRange(-100, 100),
			y: getRandomFloatInRange(-100, 100),
		}
		path[index] = newPoint
	}
	return path
}

func onSegment(p, q, r Point) bool {
	if ((q.X() <= math.Max(p.X(), r.X())) &&
		(q.X() >= math.Min(p.X(), r.X())) &&
		(q.Y() <= math.Max(p.Y(), r.Y())) &&
		(q.Y() >= math.Min(p.Y(), r.Y()))) {
		return true
	}
	return false
}

func pointsOrientation(p, q, r Point) int {
	// Orientation:
	// 0: collinear points
	// 1: clockwise points
	// 2: counterclockwise

	orientation := ((q.Y() - p.Y()) * (r.X() - q.X())) - ((q.X() - p.X()) * (r.Y() - q.Y()))

	if orientation > 0 {
		return 1
	} else if orientation < 0 {
		return 2
	} else {
		return 0
	}
}

func pointIntersect(p1, q1, p2, q2 Point) bool {
	o1 := pointsOrientation(p1, q1, p2)
	o2 := pointsOrientation(p1, q1, q2)
	o3 := pointsOrientation(p2, q2, p1)
	o4 := pointsOrientation(p2, q2, q1)

	if (o1 != o2) && (o3 != o4) {
		return true
	}

	if (o1 == 0) && onSegment(p1, p2, q1) {
		return true
	}

	if (o2 == 0) && onSegment(p1, q2, q1) {
		return true
	}

	if (o3 == 0) && onSegment(p2, p1, q2) {
		return true
	}

	if (o4 == 0) && onSegment(p2, q1, q2) {
		return true
	}

	return false
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage:\n$ go run geometry.go <sides>")
		os.Exit(0)
	}

	sides, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}

	if sides < 3 {
		fmt.Println("Figure must have 3 or more sides")
		os.Exit(0)
	}

	var path Path = generateRandomPoints(sides)
	perimeter := path.Distance()

	resultSummary := "  - "
	output := fmt.Sprintf("- Generating a [%d] sides figure\n- Figure's vertices\n", sides)
	for index, value := range path {
		newVertex := fmt.Sprintf("  - ( %f, %f)\n", value.X(), value.Y())
		output += newVertex
		newPerimeter := value.Distance(path[(index + 1) % len(path)])
		resultSummary += fmt.Sprintf("%f ", newPerimeter)
		if index < len(path) - 1 {
			resultSummary += "+ "
		}
	}
	resultSummary += fmt.Sprintf("= %f", perimeter)

	output += "- Figure's Perimeter\n"
	output += resultSummary
	fmt.Println(output)
}
