open Jest;
open Expect;
open Squarify;

/* Create an on-the-fly custom matcher for floating point comparison.
   Can be used anywhere in place of a number. Second argument is optional.
   Usage: `expect(2.001).toEqual(approx(2, 2))`
   https://jasmine.github.io/tutorials/your_first_suite#section-Custom_asymmetric_equality_tester
   */
let approx: float => float = [%raw
  expected => "{
  const precision = 2;
  const asymmetricMatch =
    actual => Math.abs(expected - actual) < Math.pow(10, -precision) / 2;

  // Note: also give a meaningful name to help in reading jest's diff if mismatched:
  Object.defineProperty(asymmetricMatch, 'name', {value: `approx(${expected})`});

  return {asymmetricMatch};
}"
];

let inputTree: tree =
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

test("getShorterside", () => {
  let actual = getShorterSide({x0: 0.0, y0: 0.0, x1: 6.0, y1: 4.0});
  expect(actual) |> toEqual(4.0);
});

test("sum", () => {
  let actual = sum([2.0, 1.0, 3.0]);
  expect(actual) |> toEqual(6.0);
});

describe("sumValuesInTree", () => {
  test("LeafNode", () => {
    let actual = sumValuesInTree(LeafNode({value: 3.0, label: "BAA"}));
    expect(actual) |> toEqual(3.0);
  });

  test("BranchNode", () => {
    let actual = sumValuesInTree(inputTree);
    expect(actual) |> toEqual(24.0);
  });
});

describe("generalizedMax", () => {
  test("Empty array", () => {
    let actual = generalizedMax([]);
    expect(actual) |> toEqual(None);
  });

  test("Non-empty array", () => {
    let actual = generalizedMax([1.0, 3.0, 2.0]);
    expect(actual) |> toEqual(Some(3.0));
  });
});

describe("max", () => {
  test("empty rest argument", () => {
    let actual = max(1.0, []);
    expect(actual) |> toEqual(1.0);
  });
  test("non-empty rest argument", () => {
    let actual = max(1.0, [2.0]);
    expect(actual) |> toEqual(2.0);
  });
});

test("getMaxAspectRatioInRow", () => {
  let actual = getMaxAspectRatioInRow(6.0, [6.0, 4.0], 4.0);
  expect(actual) |> toEqual(4.0);
});

test("fitRowIntoContainer", () => {
  let actual =
    fitRowIntoContainer(
      [
        (LeafNode({label: "A", value: 6.0}), 6.0),
        (LeafNode({label: "B", value: 6.0}), 6.0),
        (LeafNode({label: "C", value: 4.0}), 4.0),
      ],
      {x0: 0.0, y0: 0.0, x1: 6.0, y1: 4.0},
    );
  let nodesWithLayout = [
    (
      LeafNode({label: "C", value: 4.0}),
      {x0: 0.0, y0: approx(0.0), x1: 4.0, y1: approx(1.0)},
    ),
    (
      LeafNode({label: "B", value: 6.0}),
      {x0: 0.0, y0: approx(1.0), x1: 4.0, y1: approx(5.0 /. 2.0)},
    ),
    (
      LeafNode({label: "A", value: 6.0}),
      {x0: 0.0, y0: approx(5.0 /. 2.0), x1: 4.0, y1: 4.0},
    ),
  ];
  let remainingRectangle = {x0: 4.0, y0: 0.0, x1: 6.0, y1: 4.0};
  expect(actual) |> toEqual((nodesWithLayout, remainingRectangle));
});

test("squarify", () => {
  let nodeA = LeafNode({value: 6.0, label: "A"});
  let nodeB = LeafNode({value: 6.0, label: "B"});
  let nodeC = LeafNode({value: 4.0, label: "C"});
  let nodeD = LeafNode({value: 3.0, label: "D"});
  let nodeE = LeafNode({value: 2.0, label: "E"});
  let nodeF = LeafNode({value: 2.0, label: "F"});
  let nodeG = LeafNode({value: 1.0, label: "G"});

  let levelOneOfInputTree = [nodeA, nodeB, nodeC, nodeD, nodeE, nodeF, nodeG];
  let container = {x0: 0.0, y0: 0.0, x1: 6.0, y1: 4.0};

  let actual = squarify(levelOneOfInputTree, [], container, []);

  let rectangleA = {x0: 0.0, y0: 2.0, x1: 3.0, y1: 4.0};
  let rectangleB = {x0: 0.0, y0: 0.0, x1: 3.0, y1: 2.0};
  let rectangleC = {
    x0: 3.0,
    y0: approx(5.0 /. 3.0),
    x1: approx(33.0 /. 7.0),
    y1: 4.0,
  };
  let rectangleD = {
    x0: approx(33.0 /. 7.0),
    y0: approx(5.0 /. 3.0),
    x1: 6.0,
    y1: 4.0,
  };
  let rectangleE = {
    x0: 3.0,
    y0: 0.0,
    x1: approx(21.0 /. 5.0),
    y1: approx(5.0 /. 3.0),
  };
  let rectangleF = {
    x0: approx(21.0 /. 5.0),
    y0: 0.0,
    x1: approx(27.0 /. 5.0),
    y1: approx(5.0 /. 3.0),
  };
  let rectangleG = {
    x0: approx(27.0 /. 5.0),
    y0: approx(0.0),
    x1: 6.0,
    y1: approx(5.0 /. 3.0),
  };

  expect(actual)
  |> toEqual([
       (nodeG, rectangleG),
       (nodeF, rectangleF),
       (nodeE, rectangleE),
       (nodeD, rectangleD),
       (nodeC, rectangleC),
       (nodeB, rectangleB),
       (nodeA, rectangleA),
     ]);
});

test("traverse", () => {
  // Here are the rectangles in the final output, defined separately for
  // readability:
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

  let rectangleD = {
    x0: approx(33.0 /. 7.0),
    y0: approx(5.0 /. 3.0),
    x1: 6.0,
    y1: 4.0,
  };
  let rectangleE = {
    x0: 3.0,
    y0: 0.0,
    x1: approx(21.0 /. 5.0),
    y1: approx(5.0 /. 3.0),
  };
  let rectangleF = {
    x0: approx(21.0 /. 5.0),
    y0: 0.0,
    x1: approx(27.0 /. 5.0),
    y1: approx(5.0 /. 3.0),
  };
  let rectangleG = {
    x0: approx(27.0 /. 5.0),
    y0: approx(0.0),
    x1: 6.0,
    y1: approx(5.0 /. 3.0),
  };

  let actual = traverse(inputTree, {x0: 0.0, y0: 0.0, x1: 6.0, y1: 4.0});

  expect(actual)
  |> toEqual([
       {value: 1.0, label: "G", layout: rectangleG},
       {value: 2.0, label: "F", layout: rectangleF},
       {value: 2.0, label: "E", layout: rectangleE},
       {value: 3.0, label: "D", layout: rectangleD},
       {value: 4.0, label: "C", layout: rectangleC},
       {value: 2.0, label: "BB", layout: rectangleBB},
       {value: 1.0, label: "BAB", layout: rectangleBAB},
       {value: 3.0, label: "BAA", layout: rectangleBAA},
       {value: 6.0, label: "A", layout: rectangleA},
     ]);
});
