# reason-squarify

This is an implementation of the squarified tree map algorithm in [ReasonML](https://reasonml.github.io/).

The main code is in [this file](src/Squarify.re).
All functions have corresponding [unit tests](__tests__/Squarify_test.re).
Here is an [interactive notebook](https://sketch.sh/s/jJbKS9daIU3PuUe9swkPh3/) so you can play and experiment with the code.
It contains a lot of the unit test cases as examples of how to use the functions.

Please check out two articles I wrote ([one](https://www.huy.dev/squarified-tree-map-reasonml-part-1-2019-03/) and [two](https://www.huy.dev/squarified-tree-map-reasonml-part-2-2019-05/)) to accompany this code.
I previously implemented a TypeScript version [here](https://github.com/huy-nguyen/squarify).

## Setup

- `yarn` or `npm install`.
- To build: `yarn build` or `npm run build`.
- To build in watch mode: `yarn start` or `npm run start`.
- To test, run `yarn test` or `npm test`.
- To test in watch mode, run `yarn test:watch` or `npm run test:watch`.

All functions have corresponding unit tests in the [test file](__tests__/Squarify_test.re).

## Usage

```reasonml
let input: tree =
  BranchNode([
    LeafNode({value: 6.0, label: "A"}),
    BranchNode([
      BranchNode([
        LeafNode({value: 3.0, label: "BAA"}),
        LeafNode({value: 1.0, label: "BAB"}),
      ]),
      LeafNode({value: 2.0, label: "BB"}),
    ]),
    LeafNode({value: 4.0, label: "C"}),
    LeafNode({value: 3.0, label: "D"}),
    LeafNode({value: 2.0, label: "E"}),
    LeafNode({value: 2.0, label: "F"}),
    LeafNode({value: 1.0, label: "G"}),
  ]);

let container = {x0: 0.0, y0: 0.0, x1: 6.0, y1: 4.0};

// The module contains the `traverse` function, which traverse the tree
// structure to perform layout:
let result = traverse(input, container);

// The output looks like this:
[
  {value: 1.0, label: "G", layout: rectangleG},
  {value: 2.0, label: "F", layout: rectangleF},
  {value: 2.0, label: "E", layout: rectangleE},
  {value: 3.0, label: "D", layout: rectangleD},
  {value: 4.0, label: "C", layout: rectangleC},
  {value: 2.0, label: "BB", layout: rectangleBB},
  {value: 1.0, label: "BAB", layout: rectangleBAB},
  {value: 3.0, label: "BAA", layout: rectangleBAA},
  {value: 6.0, label: "A", layout: rectangleA},
];
// where the rectangles look like this:
let rectangleA = {x0: 0.0, y0: 2.0, x1: 3.0, y1: 4.0};
let rectangleBAA = {x0: 0.0, y0: 0.5, x1: 2.0, y1: 2.0};
let rectangleBAB = {x0: 0.0, y0: 0.0, x1: 2.0, y1: 0.5};
let rectangleBB = {x0: 2.0, y0: 0.0, x1: 3.0, y1: 2.0};
let rectangleC = {
  x0: 3.0,
  y0: approx(5.0 /. 3.0),
  x1: approx(33.0 /. 7.0),
  y1: 4.0,
};
// ... and so on
```


# Editor
If you use `vscode`, Press `Windows + Shift + B` it will build automatically
