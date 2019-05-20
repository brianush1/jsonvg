# JsonVG
JsonVG (Json Vector Graphics) is a vector graphics format based on JSON. It always starts with the magic number "jsonvg(0)", followed by a Document object encoded in JSON. The specification is given in terms of TypeScript interfaces and comments explaining the various attributes.

```ts

interface Document {
	/** The width of the document in pixels */
	width: number;
	
	/** The height of the document in pixels */
	height: number;
	
	/** The shapes that make up the document */
	content: Shape[];
}

/** 4 points representing a cubic bezier curve or 2 points representing a line */
type BezierCurve = [Point, Point, Point, Point] | [Point, Point];

/** A point in the form [x, y] where x and y are the position of the point in pixels, from the top-left going right and down */
type Point = [number, number];

interface Shape {
	/** A list of segments that make up the shape */
	segments: BezierCurve[];
	
	/** Specifies the fill style of the shape */
	fill: FillStyle;
	
	/** Specifies the stroke style of the shape */
	stroke: StrokeStyle;
}

/** A color given in form [R, G, B, A], where all values range from 0.0 to 1.0, and alpha represents opacity */
type Color = [number, number, number, number];

interface FillStyle {
	/** The type of fill */
	type: "solid" | "none";
	
	/** If solid, the color of the fill */
	color?: Color = [0, 0, 0, 1];
}

interface StrokeStyle {
	/** The type of stroke */
	type: "solid" | "none";
	
	/** If solid, the location of the stroke */
	location?: "inner" | "edge" | "outer" = "edge";
	
	/**
	 * If solid, the way that line segments will be joined
	 * Note that two line segments will be joined if the end point of one is equivalent to the start point of the other
	 */
	join?: "miter" | "round" | "bevel" = "miter";
	
	/**
	 * If solid, the way that line segments will be capped
	 */
	cap?: "sharp" | "round" = "sharp";
	
	/** If solid, the width of the stroke */
	weight?: number = 1;
	
	/** If solid, the color of the stroke */
	color?: Color = [0, 0, 0, 1];
}

```

## Example:

```js
jsonvg(0){
	"width": 100,
	"height": 100,
	"content": [
		{
			"segments": [
				[[0, 0], [0, 100]],
				[[0, 100], [100, 100]],
				[[100, 100], [100, 0], [50, 50], [0, 0]]
			],
			"fill": {
				"type": "solid",
				"color": [0.752, 0.432, 0.192, 1.0]
			},
			"stroke": {
				"type": "solid",
				"location": "inner",
				"join": "round",
				"weight": 4,
				"color": [0.0, 0.0, 0.0, 1.0]
			}
		}
	]
}
```
